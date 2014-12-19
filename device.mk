# Copyright (C) 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

SOMC_PLATFORM := shinano

DEVICE_PACKAGE_OVERLAYS += \
    device/sony/shinano/overlay

SONY_ROOT = device/sony/shinano/rootdir
PRODUCT_COPY_FILES += \
    $(SONY_ROOT)/init.recovery.shinano.rc:root/init.recovery.shinano.rc \
    $(SONY_ROOT)/init.shinano.rc:root/init.shinano.rc \
    $(SONY_ROOT)/init.shinano.usb.rc:root/init.shinano.usb.rc \
    $(SONY_ROOT)/init.shinano.pwr.rc:root/init.shinano.pwr.rc \
    $(SONY_ROOT)/fstab.shinano:root/fstab.shinano \
    $(SONY_ROOT)/ueventd.shinano.rc:root/ueventd.shinano.rc \
    $(SONY_ROOT)/system/usr/idc/max1187x_touchscreen_0.idc:system/usr/idc/max1187x_touchscreen_0.idc \
    $(SONY_ROOT)/system/usr/idc/clearpad.idc:system/usr/idc/clearpad.idc \
    $(SONY_ROOT)/system/etc/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf \
    $(SONY_ROOT)/system/etc/wifi/wpa_supplicant_overlay.conf:system/etc/wifi/wpa_supplicant_overlay.conf \
    $(SONY_ROOT)/system/etc/wifi/p2p_supplicant_overlay.conf:system/etc/wifi/p2p_supplicant_overlay.conf \
    $(SONY_ROOT)/system/etc/media_codecs.xml:system/etc/media_codecs.xml \
    $(SONY_ROOT)/system/etc/media_profiles.xml:system/etc/media_profiles.xml \
    $(SONY_ROOT)/system/etc/audio_policy.conf:system/etc/audio_policy.conf

PRODUCT_COPY_FILES += \
    $(SONY_ROOT)/system/etc/tfa98xx/coldboot.patch:/system/etc/tfa98xx/coldboot.patch \
    $(SONY_ROOT)/system/etc/tfa98xx/TFA9890.patch:/system/etc/tfa98xx/TFA9890.patch \
    $(SONY_ROOT)/system/etc/tfa98xx/TFA9890_top.config:/system/etc/tfa98xx/TFA9890_top.config \
    $(SONY_ROOT)/system/etc/tfa98xx/TFA9890_btm.config:/system/etc/tfa98xx/TFA9890_btm.config \
    $(SONY_ROOT)/system/etc/tfa98xx/btm.speaker:/system/etc/tfa98xx/btm.speaker \
    $(SONY_ROOT)/system/etc/tfa98xx/top.speaker:/system/etc/tfa98xx/top.speaker \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeaker_top.preset:/system/etc/tfa98xx/HiFiSpeaker_top.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeaker_btm.preset:/system/etc/tfa98xx/HiFiSpeaker_btm.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerRing_top.preset:/system/etc/tfa98xx/HiFiSpeakerRing_top.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerRing_btm.preset:/system/etc/tfa98xx/HiFiSpeakerRing_btm.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerSforce_top.preset:/system/etc/tfa98xx/HiFiSpeakerSforce_top.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerSforce_btm.preset:/system/etc/tfa98xx/HiFiSpeakerSforce_btm.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/VoiceCallSpeaker_top.preset:/system/etc/tfa98xx/VoiceCallSpeaker_top.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/VoiceCallSpeaker_btm.preset:/system/etc/tfa98xx/VoiceCallSpeaker_btm.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/FMSpeaker_top.preset:/system/etc/tfa98xx/FMSpeaker_top.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/FMSpeaker_btm.preset:/system/etc/tfa98xx/FMSpeaker_btm.preset \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeaker_top.eq:/system/etc/tfa98xx/HiFiSpeaker_top.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeaker_btm.eq:/system/etc/tfa98xx/HiFiSpeaker_btm.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerRing_top.eq:/system/etc/tfa98xx/HiFiSpeakerRing_top.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerRing_btm.eq:/system/etc/tfa98xx/HiFiSpeakerRing_btm.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerSforce_top.eq:/system/etc/tfa98xx/HiFiSpeakerSforce_top.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/HiFiSpeakerSforce_btm.eq:/system/etc/tfa98xx/HiFiSpeakerSforce_btm.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/VoiceCallSpeaker_top.eq:/system/etc/tfa98xx/VoiceCallSpeaker_top.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/VoiceCallSpeaker_btm.eq:/system/etc/tfa98xx/VoiceCallSpeaker_btm.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/FMSpeaker_top.eq:/system/etc/tfa98xx/FMSpeaker_top.eq \
    $(SONY_ROOT)/system/etc/tfa98xx/FMSpeaker_btm.eq:/system/etc/tfa98xx/FMSpeaker_btm.eq

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

# NFC
PRODUCT_COPY_FILES += \
    $(SONY_ROOT)/system/etc/libnfc-nxp.conf:system/etc/libnfc-nxp.conf \
    $(SONY_ROOT)/system/etc/nfcee_access.xml:system/etc/nfcee_access.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml

#Audio
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.primary.msm8974 \
    audio.r_submix.default \
    audio.usb.default \
    libaudio-resampler \
    libacdbloader \
    libacdbmapper \
    libaudcal \
    libaudioalsa \
    libdiag

# for audio.primary.msm8974
PRODUCT_PACKAGES += \
    libtinyalsa \
    libtinycompress \
    libaudioroute

#GFX
PRODUCT_PACKAGES += \
    gralloc.msm8974 \
    hwcomposer.msm8974 \
    memtrack.msm8974 \
    libgenlock \
    libqdutils \
    libqdMetaData

PRODUCT_PACKAGES += \
    lights.shinano

# NFC packages
PRODUCT_PACKAGES += \
    com.android.nfc_extras \
    NfcNci \
    Tag \
    nfc_nci.msm8974

#GPS
PRODUCT_PACKAGES += \
    libloc_api_v02 \
    libloc_adapter \
    libloc_eng \
    libgps.utils \
    gps.msm8974

#WLAN
PRODUCT_PACKAGES += \
    p2p_supplicant.conf \
    gsm_domains.conf \
    hostapd \
    libwpa_client \
    wpa_supplicant \
    wpa_supplicant.conf

PRODUCT_PACKAGES += \
    libmiscta \
    libta \
    tad_static \
    ta_qmi_service

PRODUCT_PACKAGES += \
    thermanager \
    bt_addr

PRODUCT_PACKAGES += \
    rmt_storage

#Charger
PRODUCT_PACKAGES += \
    charger \
    charger_res_images \

PRODUCT_PACKAGES += \
    librs_jni \
    com.android.future.usb.accessory

PRODUCT_PACKAGES += \
    Dialer \
    Email \
    Exchange2 \
    InCallUI \
    Launcher3

# Filesystem management tools
PRODUCT_PACKAGES += \
    e2fsck

PRODUCT_TAGS += dalvik.gc.type-precise

# APN list
PRODUCT_COPY_FILES += device/sample/etc/apns-full-conf.xml:system/etc/apns-conf.xml

PRODUCT_PROPERTY_OVERRIDES += \
    keyguard.no_require_sim=true

