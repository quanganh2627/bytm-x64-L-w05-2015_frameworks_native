/*
* Copyright (c) 2009-2011 Intel Corporation.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef _OMX_VPP_H_
#define _OMX_VPP_H_

#include <OMX_Types.h>
#include <OMX_Index.h>

namespace android {
/**
 * Enumeration of possible image filter types
 */
typedef enum OMX_INTEL_IMAGEFILTERTYPE {
    OMX_INTEL_ImageFilterDenoise            = 0x00000001, /**< extension for Intel Denoise */
    OMX_INTEL_ImageFilterDeinterlace        = 0x00000002, /**< extension for Intel de-interlace */
    OMX_INTEL_ImageFilterSharpness          = 0x00000004, /**< extension for Intel sharpness */
    OMX_INTEL_ImageFilterScale              = 0x00000008, /**< extension for Intel Scaling*/
    OMX_INTEL_ImageFilterColorBalance       = 0x00000010, /**< extension for Intel Colorbalance*/
    OMX_INTEL_ImageFilter3P                 = 0x00000020, /**< extension for 3P */
} OMX_INTEL_IMAGEFILTERTYPE;

/**
 * Enumeration of possible configure index for video processing
 */
typedef enum  _OMX_INTEL_VPP_INDEXTYPE {
    /* Vendor specific area for storing indices */
    OMX_INTEL_IndexConfigVPPStart
        = ((OMX_INDEXTYPE)OMX_IndexVendorStartUnused + 0xA0000), /**< reference: OMX_CONFIG_VPPStart */

    OMX_INTEL_IndexConfigFilterType,              /**< reference: OMX_INTEL_CONFIG_FILTERTYPE */
    OMX_INTEL_IndexConfigDenoiseLevel,            /**< reference: OMX_INTEL_CONFIG_DENOISETYPE */
    OMX_INTEL_IndexConfigDeinterlaceLevel,        /**< reference: OMX_INTEL_CONFIG_DEINTERLACETYPE */
    OMX_INTEL_IndexConfigColorBalanceLevel,       /**< reference: OMX_INTEL_CONFIG_COLORBALANCETYPE */
    OMX_INTEL_IndexConfigSharpnessLevel,          /**< reference: OMX_INTEL_CONFIG_SHARPNESSTYPE */
    OMX_INTEL_IndexConfigScaleLevel,              /**< reference: OMX_INTEL_CONFIG_SCALARTYPE */
    OMX_INTEL_IndexConfigIntel3PLevel,            /**< reference: OMX_INTEL_CONFIG_INTEL3PTYPE */
} OMX_INTEL_VPP_INDEX;

/**
 * Image filter configuration
 *
 * STRUCT MEMBERS:
 *  nSize        : Size of the structure in bytes
 *  nVersion     : OMX specification version information
 *  nFilterType : VPP filter type enumeration
 */
typedef struct OMX_INTEL_CONFIG_FILTERTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nFilterType;
} OMX_INTEL_CONFIG_FILTERTYPE;

/**
 * Denoise level type
 * Level setting for Denoise filter.
 *
 * STRUCT MEMBERS:
 *  nSize       : Size of the structure in bytes
 *  nVersion    : OMX specification version information
 *  nLevel      : nLevel hinting processing level. Range of values is 0 to 64 and stepping is 1 for every level.
 *                0 causes no change to the image.  Increased values cause increased processing to occur, with 64 applying maximum processing.
 *
 * if application doesn’t setConfig with this setting, the OMX VPP will use default value
 */
typedef struct OMX_INTEL_CONFIG_DENOISETYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nLevel;
} OMX_INTEL_CONFIG_DENOISETYPE;

/**
 * List of deinterlace types for pre/post processing
 *
 * ENUMS:
 *  None        : No de-interlace
 *  Bob         : Bob de-interlace
 *  ADI         : Advanced de-interlace.
 */
typedef enum OMX_INTEL_DEINTERLACETYPE {
    OMX_INTEL_DeinterlaceNone,
    OMX_INTEL_DeinterlaceBob,
    OMX_INTEL_DeinterlaceADI
} OMX_INTEL_DEINTERLACETYPE;

/**
 * Deinterlace level type
 * Level setting for Deinterlace filter.
 *
 * STRUCT MEMBERS:
 *  nSize       : Size of the structure in bytes
 *  nVersion    : OMX specification version information
 *  eDeinterlace: eDeinterlace hinting processing type defined in OMX_INTEL_DEINTERLACETYPE.
 */
typedef struct OMX_INTEL_CONFIG_DEINTERLACETYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_INTEL_DEINTERLACETYPE eDeinterlace;
} OMX_INTEL_CONFIG_DEINTERLACETYPE;


/**
 * Sharpness level type
 *  Level setting for sharpness filter.
 *
 * STRUCT MEMBERS:
 *  nSize       : Size of the structure in bytes
 *  nVersion    : OMX specification version information
 *  nLevel      : nLevel hinting processing level. Range of values is 0 to 64 and stepping is 1 for every level.
 *                0 causes no change to the image.  Increased values cause increased processing to occur, with 64 applying maximum processing.
 *
 *  if application doesn’t setConfig with this setting, the OMX VPP will use default value
 */
typedef struct OMX_INTEL_CONFIG_SHARPNESSTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nLevel;
} OMX_INTEL_CONFIG_SHARPNESSTYPE;


/**
 * List of scalar types for pre/post processing
 *
 * ENUMS:
 *  None        : No scaling
 *  Bilinear    : Bilinear scalar
 *  AVS         : Advanced Video Scalar
 */
typedef enum OMX_INTEL_SCALARTYPE {
    OMX_INTEL_ScalarNone,
    OMX_INTEL_ScalarBilinear,
    OMX_INTEL_ScalarAvs
} OMX_INTEL_SCALARTYPE;

/**
 * Sharpness level type
 * Level setting for scale filter.
 *
 * STRUCT MEMBERS:
 *  nSize       : Size of the structure in bytes
 *  nVersion    : OMX specification version information
 *  eScalar     : eScalar hinting processing type defined in OMX_INTEL_SCALARTYPE
 *
 *  if application doesn’t setConfig with this setting, the OMX VPP will use default value
 */
typedef struct OMX_INTEL_CONFIG_SCALARTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_INTEL_SCALARTYPE eScalar;
} OMX_INTEL_CONFIG_SCALARTYPE;


/**
 * ColorBalance level type
 * Level setting for colorbalance filter.
 *
 * STRUCT MEMBERS:
 *  nSize       : Size of the structure in bytes
 *  nVersion    : OMX specification version information
 *  nLevel      : nLevel hinting processing level. Range of values is 0 to 64 and stepping is 1 for every level.
 *                0 causes no change to the image.  Increased values cause increased processing to occur, with 64 applying maximum processing.
 *
 *  if application doesn’t setConfig with this setting, the OMX VPP will use default value
 */
typedef struct OMX_INTEL_CONFIG_COLORBALANCETYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nLevel;
} OMX_INTEL_CONFIG_COLORBALANCETYPE;


/**
 * Intel 3P level type
 * Level setting for Intel 3P filter.
 *
 * STRUCT MEMBERS:
 *  nSize       : Size of the structure in bytes
 *  nVersion    : OMX specification version information
 *  fFrameRate  : frame rate of the clip
 *  nHasEncoder : indicate whether there is encoder context
 *
 *  if application doesn’t setConfig with this setting, the OMX VPP will use default value
 */
typedef struct OMX_INTEL_CONFIG_INTEL3PTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nHasEncoder;
    OMX_U32 nFrameRate;
} OMX_INTEL_CONFIG_INTEL3PTYPE;

/**
 * Intel extension for Video post processing event
 *
 * OMX_INTEL_EventIntelUseColorFormat:    recommend OMX client to use recommend color format.
 *                                          OMX VPP can detect the usage of vpp output.
 *                                          if skip 3P happen, no need to use YUY2 anymore because of power saving. Because NV12 is better
 *
 * OMX_INTEL_EventIntelSkipVpp:           if no need any 3P workload, recommend upper layer to skip VPP. It render decode output directly
 * OMX_INTEL_EventIntelResumeVpp:         if OMX VPP detects 3P can be applied, then it sends this event to upper layer.
 */
/** @ingroup comp */
typedef enum OMX_INTEL_EVENTTYPE {
    OMX_INTEL_EventIntelUseColorFormat    = 0x7F000001,   /**< recommend upper layer to pass buffer with expected color format */
    OMX_INTEL_EventIntelSkipVpp           = 0x7F000002,   /**< recommend upper layer to skip VPP */
    OMX_INTEL_EventIntelResumeVpp         = 0x7F000003    /**< recommend upper layer to resume VPP */
} OMX_INTEL_EVENTTYPE;

} //namespace android
#endif// _OMX_VPP_H_
