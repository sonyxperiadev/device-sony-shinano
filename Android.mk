ifeq ($(filter-out aries castor leo scorpion sirius,$(TARGET_DEVICE)),)

LOCAL_PATH := $(call my-dir)

ifeq ($(BUILD_KERNEL),)
INSTALLED_KERNEL_TARGET ?= $(PRODUCT_OUT)/kernel

DTB_DIR := $(ANDROID_BUILD_TOP)/device/sony/$(TARGET_DEVICE)/dtbs
DTB_FILES := $(shell find -L $(DTB_DIR) -name "*.dtb")
DTB_OUT_DIR := $(PRODUCT_OUT)/dtbs
DTBS := $(foreach dtb,$(DTB_FILES),$(DTB_OUT_DIR)/$(notdir $(dtb)))
$(DTB_OUT_DIR)/%.dtb : $(DTB_DIR)/%.dtb | $(ACP)
	$(hide) @mkdir -p $(DTB_OUT_DIR)
	$(transform-prebuilt-to-target)

file := $(INSTALLED_KERNEL_TARGET)
ALL_PREBUILT += $(file)
$(file) : $(TARGET_PREBUILT_KERNEL) $(DTBS) | $(ACP)
	$(transform-prebuilt-to-target)

ALL_PREBUILT += $(INSTALLED_KERNEL_TARGET)
endif

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
