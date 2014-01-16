LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	IGraphicBufferConsumer.cpp \
	IConsumerListener.cpp \
	BitTube.cpp \
	BufferItemConsumer.cpp \
	BufferQueue.cpp \
	ConsumerBase.cpp \
	CpuConsumer.cpp \
	DisplayEventReceiver.cpp \
	GLConsumer.cpp \
	GraphicBufferAlloc.cpp \
	GuiConfig.cpp \
	IDisplayEventConnection.cpp \
	IGraphicBufferAlloc.cpp \
	IGraphicBufferProducer.cpp \
	ISensorEventConnection.cpp \
	ISensorServer.cpp \
	ISurfaceComposer.cpp \
	ISurfaceComposerClient.cpp \
	LayerState.cpp \
	Sensor.cpp \
	SensorEventQueue.cpp \
	SensorManager.cpp \
	Surface.cpp \
	SurfaceControl.cpp \
	SurfaceComposerClient.cpp \
	SyncFeatures.cpp \

LOCAL_SHARED_LIBRARIES := \
	libbinder \
	libcutils \
	libEGL \
	libGLESv2 \
	libsync \
	libui \
	libutils \
	liblog

 LOCAL_C_INCLUDES += \
        $(TOP)/frameworks/native/include/media/openmax

ifeq ($(INTEL_FEATURE_ARKHAM),true)
LOCAL_CFLAGS += -DINTEL_FEATURE_ARKHAM
endif

LOCAL_MODULE:= libgui

ifeq ($(TARGET_BOARD_PLATFORM), tegra)
	LOCAL_CFLAGS += -DDONT_USE_FENCE_SYNC
endif
ifeq ($(TARGET_BOARD_PLATFORM), tegra3)
	LOCAL_CFLAGS += -DDONT_USE_FENCE_SYNC
endif

ifeq ($(TARGET_BOARD_PLATFORM), clovertrail)
	LOCAL_CFLAGS += -DUSE_NATIVE_FENCE_SYNC
	LOCAL_CFLAGS += -DUSE_WAIT_SYNC
endif
ifeq ($(TARGET_BOARD_PLATFORM),baytrail)
        LOCAL_CFLAGS += -DUSE_GEN_HW
        LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/ufo
endif

# for VPP support on MRFLD only
ifeq ($(TARGET_HAS_VPP),true)
    LOCAL_CFLAGS += -DGFX_BUF_EXT
endif

include $(BUILD_SHARED_LIBRARY)

ifeq (,$(ONE_SHOT_MAKEFILE))
include $(call first-makefiles-under,$(LOCAL_PATH))
endif
