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

#ifndef DITHERER_H
#define DITHERER_H

#include <GLES/gl.h>

namespace android {

class Ditherer
{
public:
    Ditherer();
    void apply(int32_t destinationX0, int32_t destinationY0, int32_t destinationX1, int32_t destinationY1);
    ~Ditherer();

private:
    GLuint m_patternTextureId;
};

}

#endif // #ifndef DITHERER_H
