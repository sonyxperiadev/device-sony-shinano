/*
 * Copyright (C) 2015 The CyanogenMod Project
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

enum {
    PROFILE_POWER_SAVE = 0,
    PROFILE_BALANCED,
    PROFILE_HIGH_PERFORMANCE,
    PROFILE_MAX
};

typedef struct governor_settings {
    int is_interactive;
    int boost;
    int boostpulse_duration;
    int go_hispeed_load;
    int go_hispeed_load_off;
    int hispeed_freq;
    int hispeed_freq_off;
    int io_is_busy;
    int min_sample_time;
    char *target_loads;
    char *target_loads_off;
    int scaling_max_freq_ab;
    int scaling_max_freq_ac;
} power_profile;

static power_profile profiles[PROFILE_MAX] = {
    [PROFILE_POWER_SAVE] = {
        .boost = 0,
        .boostpulse_duration = 0,
        .go_hispeed_load = 90,
        .go_hispeed_load_off = 90,
        .hispeed_freq = 729600,
        .hispeed_freq_off = 729600,
        .io_is_busy = 0,
        .min_sample_time = 40000,
        .target_loads = "80 998400:90 1190400:99",
        .target_loads_off = "95 1190400:99",
        .scaling_max_freq_ab = 1497600,
        .scaling_max_freq_ac = 1497600,
    },
    [PROFILE_BALANCED] = {
        .boost = 0,
        .boostpulse_duration = 60000,
        .go_hispeed_load = 50,
        .go_hispeed_load_off = 90,
        .hispeed_freq = 1190400,
        .hispeed_freq_off = 729600,
        .io_is_busy = 1,
        .min_sample_time = 40000,
        .target_loads = "85 1500000:90 1800000:70",
        .target_loads_off = "95 1190400:99",
        .scaling_max_freq_ab = 2265600,
        .scaling_max_freq_ac = 2457600,
    },
    [PROFILE_HIGH_PERFORMANCE] = {
        .boost = 1,
        .boostpulse_duration = 0,
        .go_hispeed_load = 50,
        .go_hispeed_load_off = 50,
        .hispeed_freq = 1190400,
        .hispeed_freq_off = 1190400,
        .io_is_busy = 1,
        .min_sample_time = 40000,
        .target_loads = "80",
        .target_loads_off = "80",
        .scaling_max_freq_ab = 2265600,
        .scaling_max_freq_ac = 2457600,
    },
};
