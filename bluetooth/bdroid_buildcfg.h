/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

#if !defined(OS_GENERIC)
#include <cutils/properties.h>
#include <string.h>

static inline const char* getBTDefaultName()
{
    char device[PROPERTY_VALUE_MAX];
    property_get("ro.boot.hardware", device, "");

    if (!strcmp("sirius", device)) {
        return "Xperia Z2";
    } else if (!strcmp("castor", device)) {
        return "Xperia Z2 Tablet";
    } else if (!strcmp("castor_windy", device)) {
        return "Xperia Z2 Tablet";
    } else if (!strcmp("leo", device)) {
        return "Xperia Z3";
    } else if (!strcmp("aries", device)) {
        return "Xperia Z3 Compact";
    } else if (!strcmp("scorpion", device)) {
        return "Xperia Z3 Tablet";
    } else if (!strcmp("scorpion_windy", device)) {
        return "Xperia Z3 Tablet";
    }

    return "Xperia";
}

#define BTM_DEF_LOCAL_NAME getBTDefaultName()
#endif // OS_GENERIC

#define HCILP_INCLUDED FALSE

/* #define BTA_AV_CO_CP_SCMS_T   TRUE */
#define SDP_AVRCP_1_5   FALSE

#endif
