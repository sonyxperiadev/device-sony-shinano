LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	liblog libutils libcutils

LOCAL_SRC_FILES := \
	tfa9890.c \
	audio_amplifier.c

LOCAL_MODULE := libaudioamp

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
        liblog libutils libcutils

LOCAL_SRC_FILES := \
        tfa9890.c \
        tfa9890_amp.c

LOCAL_MODULE := tfa9890_amp

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
