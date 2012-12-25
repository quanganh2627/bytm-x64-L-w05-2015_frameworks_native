LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main_surfaceflinger.cpp 

LOCAL_SHARED_LIBRARIES := \
	libsurfaceflinger \
	libbinder \
	libutils

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../services/surfaceflinger

ifeq ($(TARGET_HAS_MULTIPLE_DISPLAY),true)
    LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)
    LOCAL_SHARED_LIBRARIES += libmultidisplay
    LOCAL_CFLAGS += -DTARGET_HAS_MULTIPLE_DISPLAY
endif


LOCAL_MODULE:= surfaceflinger

include $(BUILD_EXECUTABLE)
