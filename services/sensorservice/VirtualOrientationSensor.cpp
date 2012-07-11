/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <math.h>
#include <sys/types.h>

#include <utils/Errors.h>

#include <hardware/sensors.h>

#include "VirtualOrientationSensor.h"
#include "SensorDevice.h"

namespace android {
// ---------------------------------------------------------------------------
#define DegToRad                0.0174533
#define RadToDeg                57.29578

#define PI                      3.1415926535897932f
#ifndef GRAVITY_EARTH
#define GRAVITY_EARTH           9.80665f
#endif
#define TwoPI                   2 * M_PI
#define GRAVITY_SENSITIVITY     0.017f
#define GRAVITY_PARAM           0.166713f /*(GRAVITY_EARTH*GRAVITY_SENSITIVITY)*/
#define COMPASS_THRESHOLD               12.0f

static float iheading = -1;
static float ipitch = 0;
static float iroll = 0;

template <typename T>
static inline T clamp(T v)
{
    return v < 0 ? 0 : v;
}

VirtualOrientationSensor::VirtualOrientationSensor(sensor_t const* list, size_t count)
    : mSensorDevice(SensorDevice::getInstance()),
      mALowPass(M_SQRT1_2, 1.5f),
      mAX(mALowPass), mAY(mALowPass), mAZ(mALowPass),
      mMLowPass(M_SQRT1_2, 1.5f),
      mMX(mMLowPass), mMY(mMLowPass), mMZ(mMLowPass)
{
    for (size_t i=0 ; i<count ; i++) {
        if (list[i].type == SENSOR_TYPE_ACCELEROMETER) {
            mAcc = Sensor(list + i);
        }
        if (list[i].type == SENSOR_TYPE_MAGNETIC_FIELD) {
            mMag = Sensor(list + i);
        }
    }
    memset(mMagData, 0, sizeof(mMagData));
}

static int AccelRangeCheck(double value)
{
    if (abs(value) <= 1)
        return 1;
    return 0;
}

static double ASIN(double X)
{
    double demon = sqrt(-X * X +1);
    return atan2(X,demon);
}

static double ATAN2(double Y,double X)
{
    return atan2(Y,X);
}

bool VirtualOrientationSensor::process(sensors_event_t* outEvent,
        const sensors_event_t& event)
{
    const static double NS2S = 1.0 / 1000000000.0;

    if (event.type == SENSOR_TYPE_MAGNETIC_FIELD) {
        const double now = event.timestamp * NS2S;
        if (mMagTime == 0) {
            mMagData[0] = mMX.init(event.magnetic.x);
            mMagData[1] = mMY.init(event.magnetic.y);
            mMagData[2] = mMZ.init(event.magnetic.z);
        } else {
            double dT = now - mMagTime;
            mMLowPass.setSamplingPeriod(dT);
            mMagData[0] = mMX(event.magnetic.x);
            mMagData[1] = mMY(event.magnetic.y);
            mMagData[2] = mMZ(event.magnetic.z);
        }
        mMagTime = now;

        // CalculateHeading
        double Head = -1;
        double Head2 = 0;

        int magForward = mMagData[0];
        int magLeft = mMagData[1];
        int magUp = mMagData[2];

        double angle_F = -iroll * PI / 180;
        double angle_L = -ipitch * PI / 180;

        // Generate Sine and Cosine components
        double C_a_F = cos(angle_F);
        double S_a_F = sin(angle_F);

        double C_a_L = cos(angle_L);
        double S_a_L = sin(angle_L);

        // Generate the tilt compensated magnetic horizontal components
        //HC1 = C_a_F * magForward + S_a_F * S_a_L * magLeft - S_a_F * C_a_L * magUp;
        double HC1 = C_a_F * magForward + S_a_F * S_a_L * magLeft + S_a_F * C_a_L * magUp;
        double HC2 = C_a_L * magLeft - S_a_L * magUp;

        // Calculate the Heading in units of radians.
        Head = -ATAN2(HC1, HC2);
        Head2 = ATAN2(-magLeft, -magForward)*RadToDeg;

        Head2 = Head2<0 ? (Head2 +360) : Head2;
        //Range scale it to 0 to 360 degrees
        if (Head < 0)
            Head += TwoPI;

        Head *= RadToDeg;
        iheading = (float)Head;
    } else if (event.type == SENSOR_TYPE_ACCELEROMETER) {
        const double now = event.timestamp * NS2S;
        float Ax, Ay, Az;
        if (mAccTime == 0) {
            Ax = mAX.init(event.acceleration.x);
            Ay = mAY.init(event.acceleration.y);
            Az = mAZ.init(event.acceleration.z);
        } else {
            double dT = now - mAccTime;
            mALowPass.setSamplingPeriod(dT);
            Ax = mAX(event.acceleration.x);
            Ay = mAY(event.acceleration.y);
            Az = mAZ(event.acceleration.z);
        }
        mAccTime = now;

        // CalculateAngle
        float tmp = 0;
        float x = Ax, y = Ay, z = Az;

        // pitch
        ipitch = (y == 0) ? ((z>0)?90:-90) : atan((z/y)*(x*x+y*y+z*z)/(y*y+z*z))*180/PI;
        if (z <= 0) {
            if (y >= 0)
                ipitch = ipitch - 90;
            else
                ipitch = ipitch + 90;
        } else {
            if (y >= 0)
                ipitch = ipitch - 90;
            else
                ipitch = ipitch + 90;
        }
        ipitch = ((int)(ipitch*10 + 0.5))/10.0;

        //roll
        iroll = (x == 0) ? ((z>0)?90:-90) : atan((z/x)*(x*x+y*y+z*z)/(x*x+z*z))*180/PI;
        if (z <= 0) {
            if (x > 0)
                iroll = - iroll - 90;
            else
                iroll = 90 - iroll;
        } else {
            if (x <= 0)
                iroll = 90 + iroll;
            else
                iroll = iroll - 90;
        }
        iroll = -((int)(iroll*10 + 0.5))/10.0;

        *outEvent = event;
        outEvent->data[0] = (float)iheading;
        outEvent->data[1] = (float)ipitch;
        outEvent->data[2] = (float)iroll;
        outEvent->sensor = '_ori';
        outEvent->type = SENSOR_TYPE_ORIENTATION;
        outEvent->orientation.status = event.magnetic.status;

        if (mMagTime != 0)
            return true;
    }
    return false;
}

status_t VirtualOrientationSensor::activate(void* ident, bool enabled) {
    mSensorDevice.activate(this, mAcc.getHandle(), enabled);
    mSensorDevice.activate(this, mMag.getHandle(), enabled);
    if (enabled) {
        mMagTime = 0;
        mAccTime = 0;
    }
    return NO_ERROR;
}

status_t VirtualOrientationSensor::setDelay(void* ident, int handle, int64_t ns) {
    mSensorDevice.setDelay(this, mAcc.getHandle(), ns);
    mSensorDevice.setDelay(this, mMag.getHandle(), ns);
    return NO_ERROR;
}

Sensor VirtualOrientationSensor::getSensor() const {
    sensor_t hwSensor;
    hwSensor.name       = "Virtual Orientation Sensor";
    hwSensor.vendor     = "Honeywell Inc.";
    hwSensor.version    = 1;
    hwSensor.handle     = '_ori';
    hwSensor.type       = SENSOR_TYPE_ORIENTATION;
    hwSensor.maxRange   = 360.0f;
    hwSensor.resolution = 1.0f;
    hwSensor.power      = mAcc.getPowerUsage() + mMag.getPowerUsage();
    hwSensor.minDelay   = mAcc.getMinDelay();
    Sensor sensor(&hwSensor);
    return sensor;
}

// ---------------------------------------------------------------------------
}; // namespace android

