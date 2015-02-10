ifeq ($(filter-out aries castor leo scorpion sirius,$(TARGET_DEVICE)),)

LOCAL_PATH := $(call my-dir)

ifeq ($(BUILD_KERNEL),)
INSTALLED_KERNEL_TARGET ?= $(PRODUCT_OUT)/kernel

DTB_FILES := $(shell find -L $(ANDROID_BUILD_TOP)/device/sony/$(TARGET_DEVICE)/dtbs -name "*.dtb")
DTBS := $(foreach dtb,$(DTB_FILES),$(PRODUCT_OUT)/dtbs/$(notdir $(dtb)))
$(DTBS) : $(DTB_FILES) | $(ACP)
	$(hide) @mkdir -p $(PRODUCT_OUT)/dtbs
	$(transform-prebuilt-to-target)

file := $(INSTALLED_KERNEL_TARGET)
ALL_PREBUILT += $(file)
$(file) : $(TARGET_PREBUILT_KERNEL) $(DTBS) | $(ACP)
	$(transform-prebuilt-to-target)

ALL_PREBUILT += $(INSTALLED_KERNEL_TARGET)
endif

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
