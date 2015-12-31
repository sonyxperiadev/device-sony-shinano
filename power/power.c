/*
 * Copyright (C) 2016 Adam Farden
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

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LOG_TAG "Simple PowerHAL"
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

int sysfs_write(char *path, char *s)
{
    char buf[80];
    int len;
    int ret = 0;
    int fd = open(path, O_WRONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return -1 ;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);

        ret = -1;
    }

    close(fd);

    return ret;
}

static void power_init(__attribute__((unused)) struct power_module *module)
{
    ALOGI("Simple PowerHAL is alive!.");
}

enum {
    PROFILE_POWER_SAVE = 0,
    PROFILE_BALANCED,
    PROFILE_HIGH_PERFORMANCE
};

static int current_power_profile = PROFILE_BALANCED;

static void set_power_profile(int profile) {

    if (profile == current_power_profile)
        return;

    switch (profile) {
        case PROFILE_POWER_SAVE:
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay", "40000 1100000:80000 1700000:40000");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load", "90");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/hispeed_freq", "960000");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/min_sample_time", "20000");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/target_loads", "85 1200000:90 1800000:70");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_rate", "30000");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_slack", "20000");
            break;

        case PROFILE_BALANCED:
        case PROFILE_HIGH_PERFORMANCE:
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay", "20000 1400000:40000 1700000:20000");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load", "90");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/hispeed_freq", "1190400");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/min_sample_time", "40000");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/target_loads", "85 1500000:90 1800000:70");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_rate", "30000");
            sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/timer_slack", "20000");
            break;
    }

    current_power_profile = profile;
}

static int low_power_mode = 0;

static void power_hint(__attribute__((unused)) struct power_module *module,
                       power_hint_t hint, void *data)
{
    switch (hint) {
        case POWER_HINT_VSYNC:
            break;

        case POWER_HINT_INTERACTION:
            // When touching the screen, pressing buttons etc.
            if (data) {
                ALOGI("Interacting with device, boost speeds for 80000uS");
                sysfs_write("/sys/devices/system/cpu/cpufreq/interactive/boostpulse", "1");
            }
            break;

        case POWER_HINT_LOW_POWER:
            // When we want to save battery.
            if (data) {
                ALOGI("Low power mode enabled.");
                low_power_mode = 1;
                set_power_profile(PROFILE_POWER_SAVE);
            } else {
                ALOGI("Low power mode disabled.");
                low_power_mode = 0;
                set_power_profile(PROFILE_BALANCED);
            }
            break;

        default:
            break;
    }
}

static void set_interactive(__attribute__((unused)) struct power_module *module,
                            int on)
{
    // set interactive means change governor, cpufreqs etc
    // for when device is awake and ready to be used.

    if (!on) {
        ALOGI("Device is asleep.");
        set_power_profile(PROFILE_POWER_SAVE);
    } else {
        ALOGI("Device is awake.");
        if (!low_power_mode) {
            set_power_profile(PROFILE_BALANCED);
        }
    }
}

void set_feature(__attribute__((unused)) struct power_module *module,
                 feature_t feature, int state)
{
#ifdef TAP_TO_WAKE_NODE
    if (feature == POWER_FEATURE_DOUBLE_TAP_TO_WAKE) {
            ALOGI("Double tap to wake is %s.", state ? "enabled" : "disabled");
#ifdef TAP_TO_WAKE_STRING
            sysfs_write(TAP_TO_WAKE_NODE, state ? "enabled" : "disabled");
#else
            sysfs_write(TAP_TO_WAKE_NODE, state ? "1" : "0");
#endif
        return;
    }
#endif
}

static struct hw_module_methods_t power_module_methods = {
    .open = NULL,
};

struct power_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = POWER_MODULE_API_VERSION_0_3,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = POWER_HARDWARE_MODULE_ID,
        .name = "Simple Power HAL",
        .author = "Adam Farden",
        .methods = &power_module_methods,
    },

    .init = power_init,
    .powerHint = power_hint,
    .setInteractive = set_interactive,
    .setFeature = set_feature,
};
