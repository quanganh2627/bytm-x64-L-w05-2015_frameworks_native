/*===================== begin_copyright_notice ==================================

INTEL CONFIDENTIAL
Copyright 2013
Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents related to the
source code ("Material") are owned by Intel Corporation or its suppliers or
licensors. Title to the Material remains with Intel Corporation or its suppliers
and licensors. The Material contains trade secrets and proprietary and confidential
information of Intel or its suppliers and licensors. The Material is protected by
worldwide copyright and trade secret laws and treaty provisions. No part of the
Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express
written permission.

No license under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or delivery
of the Materials, either expressly, by implication, inducement, estoppel
or otherwise. Any license under such intellectual property rights must be
express and approved by Intel in writing.

======================= end_copyright_notice ==================================*/

#include "Ditherer.h"
#include <cutils/log.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

namespace android {

/** \brief Check the OpenGL error status */

void checkGLError(const char* operation)
{
    GLint error = glGetError();
    if (error != GL_NO_ERROR)
    {
        ALOGE("Error 0x%x on %s\n", error, operation);
    }
}

static unsigned int colourFor(int value)
{
    return ( ((value+2) & 0x3) <<16) |(((value+3) & 0x3)<<8) | value;
}

Ditherer::Ditherer()
{
    // Create the colour texture
    glGenTextures(1, &m_patternTextureId);
    checkGLError("glGenTextures");

    // Set the format and size
    glBindTexture(GL_TEXTURE_2D, m_patternTextureId);
    checkGLError("glBindTexture");

    unsigned int pattern[16] = {
        colourFor(0), colourFor(3), colourFor(1), colourFor(2),
        colourFor(1), colourFor(0), colourFor(2), colourFor(3),
        colourFor(2), colourFor(1), colourFor(3), colourFor(0),
        colourFor(3), colourFor(2), colourFor(0), colourFor(1) };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, pattern);
    checkGLError("glTexImage2D");

    // Set the filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    checkGLError("glTexParameteri");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGLError("glTexParameteri");

    // Set the wrapping mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    checkGLError("glTexParameteri");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    checkGLError("glTexParameteri");
}

void Ditherer::apply(int32_t destinationX0, int32_t destinationY0, int32_t destinationX1, int32_t destinationY1)
{
    // Enable blending
    glEnable(GL_BLEND);
    checkGLError("glEnable");

    // The source factor is 1 and the destination factor is also 1,
    // as we only want to add the dithering pattern to previous frame buffer
    // contents

    glBlendFunc(GL_ONE, GL_ONE);
    checkGLError("glBlendFunc");

    // Explicitly set the texture environment mode to "replace"
    glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    checkGLError("glTexEnvx");

    // Enable texturing
    glBindTexture(GL_TEXTURE_2D, m_patternTextureId);
    checkGLError("glBindTexture");

    glEnable(GL_TEXTURE_2D);
    checkGLError("glEnable");

    // Order the X and Y bounds
    GLfloat minX, maxX, minY, maxY;

    if (destinationY0 > destinationY1)
    {
        maxY = (GLfloat)destinationY0;
        minY = (GLfloat)destinationY1;
    }
    else
    {
        maxY = (GLfloat)destinationY1;
        minY = (GLfloat)destinationY0;
    }

    if (destinationX0 > destinationX1)
    {
        maxX = (GLfloat)destinationX0;
        minX = (GLfloat)destinationX1;
    }
    else
    {
        maxX = (GLfloat)destinationX1;
        minX = (GLfloat)destinationX0;
    }

    // Anticlockwise-defined quad with:
    // - X in [0, width-1] and positive to the right in screen space
    // - Y in [0, height-1] and positive upwards in screen space
    const GLfloat positions[8] =
    {
        minX, maxY,   // Left-top
        minX, minY,   // Left-bottom
        maxX, minY,   // Right-bottom
        maxX, maxY,   // Right-top
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    checkGLError("glEnableClientState");

    glVertexPointer(2, GL_FLOAT, 0, positions);
    checkGLError("glVertexPointer");

    GLfloat u1 = (destinationX1 - destinationX0) / 4.f;
    GLfloat v1 = (destinationY1 - destinationY0) / 4.f;

    // Texture coordinates in [0,1] range
    const GLfloat texCoords[8] =
    {
        0.f, v1,   // Left-top
        0.f, 0.f,  // Left-bottom
        u1,  0.f,  // Right-bottom
        u1,  v1,   // Right-top
    };

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    checkGLError("glEnableClientState");

    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    checkGLError("glTexCoordPointer");

    // Draw the quad (fan)
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    checkGLError("glDrawArrays");

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    checkGLError("glDisableClientState");

    glDisable(GL_BLEND);
    checkGLError("glDisable");
}

Ditherer::~Ditherer()
{
    // Destroy the colour texture
    glDeleteTextures(1, &m_patternTextureId);
    checkGLError("glGenTextures");
}

}
