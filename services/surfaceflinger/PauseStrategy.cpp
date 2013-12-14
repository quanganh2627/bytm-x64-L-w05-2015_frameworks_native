
#include "PauseStrategy.h"
#include <cutils/log.h>
#include <cutils/properties.h>
#include "SurfaceFlinger.h"
#include "DisplayDevice.h"
#include "Layer.h"



using namespace android;

#define DEBUG 0


PauseStrategy::PauseStrategy(const sp<SurfaceFlinger> &sf):
        mFlinger(sf),
        mPauseExternalDisplayPending(false),
        mBypassCount(0),
        mPauseExternalDisplay(false),
        mProjectionSettingPending(false),
        mDisableRotationAnimationOnExternalPending(false)
{

}


/* @params: status when set to true, external display will be paused on PREVIOUS frame on
 *          next refresh cycle;
 *          when set to false, if previous status is true, it will resume on next
 *          refresh cycle.
 *
 * This function is called from thread other main/surfaceFlinger thread; and the status of
 * mPauseExternalDisplayPending will be picked up in handleTransction in main thread. See
 * handleExternalDisplayPauseLocked().
 *
*/

void PauseStrategy::setPauseExternalDisplay(bool status)
{
    if (DEBUG) ALOGD("ORT: set setPauseExternalDisplay to %d",status);
    mPauseExternalDisplayPending = status;
    // when playing video, there is no transaction needed to be handled, we use this API
    // trigger the update of mPauseExternalDisplay
    mFlinger->setPausetTransaction();
}


/**
 * This function is called from the main/surfaceFlinger thread
 */
void PauseStrategy::setPauseExternalDisplayImmediately(bool status)
{
    setPauseExternalDisplay(status);
    handleExternalDisplayPauseLocked();
}



/*
 * all the following functions MUST be called in the sf main thread.
 *
 */
void PauseStrategy::handleExternalDisplayPauseLocked() {

    // pause/resume request already handled. return
    if (mPauseExternalDisplayPending == mPauseExternalDisplay) {
        return;
    }

    // whenever pause/resume status change, we will invalidate the visible region
    // to force a redraw
    mFlinger->invalidateVisibleRegion();

    // for disable rotation animation
    mPauseExternalDisplay = mPauseExternalDisplayPending;
    if (DEBUG) ALOGD("ORT: set mBypassCompositionOnExternal to %d",mPauseExternalDisplay);

    // animation finished.
    if (mPauseExternalDisplayPending == false) {
        mBypassCount = 0;
        // rectify the projection setting we have skipped on frame 1
        // MUST be called on main thread
        if (mProjectionSettingPending ) {
            mFlinger->rectifyProjectionSetting();
            mProjectionSettingPending = false;
        }
    }
}

// keep the first frame
bool PauseStrategy::skipComposition() {
    // increase the count
    updateBypassCount();

    if (DEBUG)  ALOGD("ORT: should skip skipComposition  byPass %d count %d, mProjectionSettingPending = %d",
            mPauseExternalDisplay, mBypassCount,mProjectionSettingPending);
    bool result = mPauseExternalDisplay && mBypassCount > PRESERVED_FRAME_COUNT;

    if (mPauseExternalDisplay && mBypassCount <= PRESERVED_FRAME_COUNT) {
        if (DEBUG) ALOGD("ORT: preserver this frame, force it to composite to FB");
    }

    return result;
}

bool PauseStrategy::forceHwcSkip(int deviceType) {
    bool ret= mPauseExternalDisplay && deviceType >= DisplayDevice::DISPLAY_EXTERNAL;
    if (DEBUG) ALOGD("ORT: forceHwcSkip %d",ret);
    return ret;
}

bool PauseStrategy::skipBuildLayerList(int deviceType) {

    if (DEBUG) ALOGD("ORT: should skip build layerList setting byPass %d count %d",
            mPauseExternalDisplay,mBypassCount);

    bool result = mPauseExternalDisplay
            && deviceType >= DisplayDevice::DISPLAY_EXTERNAL;

    // if we are in the pause status, we have to invalidate the visible region
    // so can walk to path to skipBuildLayerList  forceHwcSkip
    // if (result == true) {
       // mFlinger->invalidateVisibleRegion();
    // }

    return result;
}

bool PauseStrategy::skipProjectionSetting(int deviceType) {
    if (DEBUG) ALOGD("ORT: skipProjectionSetting mPauseExternalDisplay %d mBypassCount %d",
            mPauseExternalDisplay,mBypassCount);

    // Transaction always happened before REFRESH Mesage
    bool shouldSkip =  mPauseExternalDisplay == true
            && deviceType >= DisplayDevice::DISPLAY_EXTERNAL;

    if (shouldSkip) {
        ALOGD("ORT: DPS pending the projection change");
        mProjectionSettingPending = true;
    } else {
        ALOGD("ORT: DPS allow this projection change");
    }

	return shouldSkip;
}

void PauseStrategy::updateBypassCount() {
    if (mPauseExternalDisplay) {
        mBypassCount++;
    }
}


void PauseStrategy::resetBypassFlag() {
    mPauseExternalDisplayPending = false;
}


//---------------------------------------------------------------------------------------------------
// help function to implement disable rotation animation feature
//----------------------------------------------------------------------------------------------------
/*
 *  This function will be called binder thread, within SurfaceFlinger::setTransactionState.
*/

void PauseStrategy::disableAnimationRotation(bool shouldDisableAnimationRotaion){

    if (DEBUG) ALOGD("ORT: animation status to %s",shouldDisableAnimationRotaion ? "in-progress":"finished");

    // If already paused and it is not because of wanting to disable rotation animation
    // ignore the request. Handle the case: pause external display , and rotate the device
    if (mPauseExternalDisplayPending && !mDisableRotationAnimationOnExternalPending){
        ALOGW("external display already paused");
        return;
    }

    // set the flag only when request toggles
    if (shouldDisableAnimationRotaion != mDisableRotationAnimationOnExternalPending) {
        mDisableRotationAnimationOnExternalPending = shouldDisableAnimationRotaion;
        setPauseExternalDisplay(shouldDisableAnimationRotaion);
    }
}
#if 0
void PauseStrategy::needDisableAnimationRotationImmediately(bool shouldDisableAnimationRotaion){

    disableAnimationRotation(shouldDisableAnimationRotaion);
    handleExternalDisplayPauseLocked();
}

// called in rebuildLayerList to check if animation finished
void PauseStrategy::checkIfRotationFinished(Vector< sp<Layer> >&currentLayers){

    //if (mDisableRotationAnimationOnExternalPending)
    {
       bool animaitonStillinProgress = false;
       const char *s1="BlackSurface";
       const char *s2="ScreenshotSurface";
       const size_t count = currentLayers.size();
       for (size_t i=0 ; i<count ; i++) {
            const sp<Layer>& layer(currentLayers[i]);
            //ALOGD("ORT:layer name 2 %s" , layer->getName().string());
            if (layer->getName().find(s1,0) != -1 ||
                layer->getName().find(s2,0) != -1){
                animaitonStillinProgress = true;
            }
       }

       if(!animaitonStillinProgress) {
           if (DEBUG) ALOGD("ORT: animation finished detected");
           needDisableAnimationRotationImmediately(false);
       }
   }
}
#endif

