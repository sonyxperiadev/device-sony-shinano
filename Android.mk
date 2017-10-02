ifeq (shinano,$(PRODUCT_PLATFORM))

$(shell mkdir -p $(PRODUCT_OUT)/system/vendor/firmware)
$(shell pushd $(PRODUCT_OUT)/system/vendor/firmware > /dev/null && ln -s /vendor/etc/firmware/libpn547_fw.so libpn547_fw.so && popd > /dev/null)

LOCAL_PATH := $(call my-dir)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
