
#ifndef ANDROID_PAUSE_STRATEGY_H
#define ANDROID_PAUSE_STRATEGY_H

#include <utils/RefBase.h>
#include <utils/Log.h>
#include <utils/KeyedVector.h>



namespace android {

class DisplayDevice;
class SurfaceFlinger;
class Layer;

class PauseStrategy {

public:

    PauseStrategy(const sp<SurfaceFlinger>&sf);

    //-------------------------------------------------------------
    //public API to handle pause/resume
    //-------------------------------------------------------------
    void setPauseExternalDisplay(bool status);

    // MUST be called before skipProjectionSetting was called
    // could be called at the beginning of handleTransaction
    // MUST be called in SF main thread
    void handleExternalDisplayPauseLocked();

    // called during handleTransctions related with displayTransaction
    bool skipProjectionSetting(int deviceType);

    // called during rebuildLayerStack
    bool skipBuildLayerList(int deviceType);

    // called during setupHWComposer
    // force HWC skip the layer. all the layer during animation will set as skip to HWC
    // but and only the first frame will be composite; all the other frames composition will be bypassed
    bool forceHwcSkip(int deviceType);

    // called during doComposition to check if need to skip composition on external display
    bool skipComposition();

    void resetBypassFlag();

    //--------------------------------------------------
    //public API for disabling rotation animation
    //--------------------------------------------------

    //@params: shouldDisableAnimationRotaion
    //true -  disable animation rotation start
    //false - disable animation rotation end

    void disableAnimationRotation(bool shouldDisableAnimationRotaion);
    //void needDisableAnimationRotationImmediately(bool shouldDisableAnimationRotaion);
    //void checkIfRotationFinished(Vector< sp<Layer> >&currentLayers);

    // --------------------------------------------------
    // to support a legacy feature used to set HWC_ROATION_IN_PROGRESS, called during setUpHWComposer
    // --------------------------------------------------
    bool isRotationFinished() {return !mDisableRotationAnimationOnExternalPending;}


private:
    void setPauseExternalDisplayImmediately(bool status);
    void updateBypassCount();

private:

    sp<SurfaceFlinger> mFlinger;

    const static int PRESERVED_FRAME_COUNT = 1;

    // read/Write in binder thread, read in main thread on handletransactionLocked
    // protected with SurfaceFlinger::mStateLock
    bool mPauseExternalDisplayPending;

    // access only write only in main thread, no sync needed
    // when mBypassCount <= PRESERVED_FRAME_COUNT, frame will be force to FB
    // when mBypassCount > PRESERVED_FRAME_COUNT, frame will be skipped
    int mBypassCount;

    // Following 2 attributions accessed only from main thread. no sync needed
    bool mPauseExternalDisplay;
    bool mProjectionSettingPending ;

    // flag indicating when there is a request to disable animation rotation pending
    // protected with SurfaceFlinger::mStateLock
    bool mDisableRotationAnimationOnExternalPending;



};

}

#endif
