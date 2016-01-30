/*
 * Copyright (C) 2015 The CyanogenMod Project
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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define LOG_TAG "Simple PowerHAL"
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

#include "power.h"

#define CPUFREQ_PATH "/sys/devices/system/cpu/cpu0/cpufreq/"
#define INTERACTIVE_PATH "/sys/devices/system/cpu/cpufreq/interactive/"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static int boostpulse_fd = -1;

// static int current_power_profile = -1;
// Force Balanced profile; AOSP does not have profiles like CM.
static int current_power_profile = 1;
static int requested_power_profile = -1;
static int is_low_power_mode = 0;
static int is_8974ac = -1;

static int is_target_8974ac()
{
    int fd;
    char buf[10] = {0};

    if (is_8974ac >= 0)
        return is_8974ac;

    fd = open("/sys/devices/soc0/soc_id", O_RDONLY);
    if (fd >= 0) {
        if (read(fd, buf, sizeof(buf) - 1) == -1) {
            ALOGW("%s: Unable to read soc_id", __func__);
            is_8974ac = 0;
        } else {
            int soc_id = atoi(buf);
            if (soc_id == 194) {
            is_8974ac = 1;
            }
        }
    }
    close(fd);
    return is_8974ac;
}

static int sysfs_write_str(char *path, char *s)
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

static int sysfs_write_int(char *path, int value)
{
    char buf[80];
    snprintf(buf, 80, "%d", value);
    return sysfs_write_str(path, buf);
}

static int is_profile_valid(int profile)
{
    return profile >= 0 && profile < PROFILE_MAX;
}

static void power_init(__attribute__((unused)) struct power_module *module)
{
    ALOGI("Simple PowerHAL is alive!.");
}

static int boostpulse_open()
{
    pthread_mutex_lock(&lock);
    if (boostpulse_fd < 0) {
        boostpulse_fd = open(INTERACTIVE_PATH "boostpulse", O_WRONLY);
    }
    pthread_mutex_unlock(&lock);

    return boostpulse_fd;
}

static void set_power_profile(int profile)
{
    if (!is_profile_valid(profile)) {
        ALOGE("%s: unknown profile: %d", __func__, profile);
        return;
    }

    if (is_low_power_mode) {
        /* Let's assume we get a valid profile */
        requested_power_profile = profile;
        ALOGD("%s: low power mode enabled, ignoring profile change request", __func__);
        return;
    }

    if (profile == current_power_profile)
        return;

    ALOGD("%s: setting profile %d", __func__, profile);

    sysfs_write_int(INTERACTIVE_PATH "boost",
                    profiles[profile].boost);
    sysfs_write_int(INTERACTIVE_PATH "boostpulse_duration",
                    profiles[profile].boostpulse_duration);
    sysfs_write_int(INTERACTIVE_PATH "go_hispeed_load",
                    profiles[profile].go_hispeed_load);
    sysfs_write_int(INTERACTIVE_PATH "hispeed_freq",
                    profiles[profile].hispeed_freq);
    sysfs_write_int(INTERACTIVE_PATH "io_is_busy",
                    profiles[profile].io_is_busy);
    sysfs_write_int(INTERACTIVE_PATH "min_sample_time",
                    profiles[profile].min_sample_time);
    sysfs_write_str(INTERACTIVE_PATH "target_loads",
                    profiles[profile].target_loads);
    sysfs_write_int(CPUFREQ_PATH "scaling_max_freq", is_target_8974ac() ?
                    profiles[profile].scaling_max_freq_ac : profiles[profile].scaling_max_freq_ac);

    current_power_profile = profile;
}

static void set_low_power_mode(int on)
{
    if (on == is_low_power_mode)
        return;

    ALOGD("%s: state=%d", __func__, on);

    if (on) {
        requested_power_profile = current_power_profile;
        set_power_profile(PROFILE_POWER_SAVE);
        is_low_power_mode = 1;
    } else {
        is_low_power_mode = 0;
        set_power_profile(requested_power_profile);
    }
}

static void power_hint(__attribute__((unused)) struct power_module *module,
                       power_hint_t hint, void *data)
{
    char buf[80];
    int len;

    switch (hint) {

        case POWER_HINT_INTERACTION:
            if (!is_profile_valid(current_power_profile)) {
                ALOGD("%s: no power profile selected yet", __func__);
                return;
            }

            if (!profiles[current_power_profile].boostpulse_duration)
                return;

            if (boostpulse_open() >= 0) {
                snprintf(buf, sizeof(buf), "%d", 1);
                len = write(boostpulse_fd, &buf, sizeof(buf));
                if (len < 0) {
                    strerror_r(errno, buf, sizeof(buf));
                    ALOGE("Error writing to boostpulse: %s\n", buf);

                    pthread_mutex_lock(&lock);
                    close(boostpulse_fd);
                    boostpulse_fd = -1;
                    pthread_mutex_unlock(&lock);
                }
            }
            break;

        case POWER_HINT_LOW_POWER:
            pthread_mutex_lock(&lock);
            set_low_power_mode(data ? 1 : 0);
            pthread_mutex_unlock(&lock);
            break;

        default:
            break;
    }
}

static void set_interactive(__attribute__((unused)) struct power_module *module,
                            int on)
{
    if (!is_profile_valid(current_power_profile)) {
        ALOGD("%s: no power profile selected yet", __func__);
        return;
    }

    if (on) {
        sysfs_write_int(INTERACTIVE_PATH "hispeed_freq",
                        profiles[current_power_profile].hispeed_freq);
        sysfs_write_int(INTERACTIVE_PATH "go_hispeed_load",
                        profiles[current_power_profile].go_hispeed_load);
        sysfs_write_str(INTERACTIVE_PATH "target_loads",
                        profiles[current_power_profile].target_loads);
    } else {
        sysfs_write_int(INTERACTIVE_PATH "hispeed_freq",
                        profiles[current_power_profile].hispeed_freq_off);
        sysfs_write_int(INTERACTIVE_PATH "go_hispeed_load",
                        profiles[current_power_profile].go_hispeed_load_off);
        sysfs_write_str(INTERACTIVE_PATH "target_loads",
                        profiles[current_power_profile].target_loads_off);
    }
}

static struct hw_module_methods_t power_module_methods = {
    .open = NULL,
};

struct power_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = POWER_MODULE_API_VERSION_0_2,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = POWER_HARDWARE_MODULE_ID,
        .name = "Simple Power HAL",
        .author = "Adam Farden",
        .methods = &power_module_methods,
    },

    .init = power_init,
    .powerHint = power_hint,
    .setInteractive = set_interactive,
};
