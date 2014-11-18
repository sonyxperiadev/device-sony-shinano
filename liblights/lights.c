/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2013 Sony Mobile Communications AB
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

#define LOG_TAG "lights.msm8974"

#include <cutils/log.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

enum {
	LED_SHARED_NOTIFICATION,
	LED_SHARED_BATTERY,
	LED_SHARED_BLUETOOTH,
	_LED_SHARED_CNT,
};

static struct light_state_t g_led_shared[_LED_SHARED_CNT];

enum led_ident {
	LED_RED,
	LED_GREEN,
	LED_BLUE,
	LED_BACKLIGHT,
};

static struct led_desc {
	int max_brightness;
        const char *max_brightness_s;
	const char *brightness;
	const char *pwm;
	const char *step;
} led_descs[] = {
	[LED_BACKLIGHT] = {
		.max_brightness = 0,
		.max_brightness_s = "/sys/class/leds/wled:backlight/max_brightness",
		.brightness = "/sys/class/leds/wled:backlight/brightness",
	},
	[LED_RED] = {
		.max_brightness = 0,
		.max_brightness_s = "/sys/class/leds/led:rgb_red/max_brightness",
		.brightness = "/sys/class/leds/led:rgb_red/brightness",
		.pwm = "/sys/class/leds/led:rgb_red/lut_pwm",
		.step = "/sys/class/leds/led:rgb_red/step_duration",
	},
	[LED_GREEN] = {
		.max_brightness = 0,
		.max_brightness_s = "/sys/class/leds/led:rgb_green/max_brightness",
		.brightness = "/sys/class/leds/led:rgb_green/brightness",
		.pwm = "/sys/class/leds/led:rgb_green/lut_pwm",
		.step = "/sys/class/leds/led:rgb_green/step_duration",
	},
	[LED_BLUE] = {
		.max_brightness = 0,
		.max_brightness_s = "/sys/class/leds/led:rgb_blue/max_brightness",
		.brightness = "/sys/class/leds/led:rgb_blue/brightness",
		.pwm = "/sys/class/leds/led:rgb_blue/lut_pwm",
		.step = "/sys/class/leds/led:rgb_blue/step_duration",
	},
};

static struct led_ctrl {
	const char *sync_state;
	const char *blink_start;
} led_ctrl = {
	.sync_state = "/sys/class/leds/rgb/sync_state",
	.blink_start = "/sys/class/leds/rgb/start_blink",
};

struct light {
	char name[256];
	struct light_device_t dev;
	int shared_which;
	struct light *next;
};

static struct light *g_light_head;

#define container_of(ptr, type, member) \
  ((type *)( (char *)(ptr) - offsetof(type,member)))
#define ll_light(dev) container_of(dev, struct light, dev)

static struct light *ll_find(const char *name)
{
	struct light *light;

	pthread_mutex_lock(&g_lock);
	light = g_light_head;
	while (light != NULL) {
		if (!strcmp(light->name, name)) {
			pthread_mutex_unlock(&g_lock);
			return light;
		}
		light = light->next;
	}
	pthread_mutex_unlock(&g_lock);
	return NULL;
}

static void ll_add(struct light *light)
{
	pthread_mutex_lock(&g_lock);
	light->next = g_light_head;
	g_light_head = light;
	pthread_mutex_unlock(&g_lock);
}

static void ll_remove(struct light *light)
{
	struct light *prev;
	struct light *iter;

	pthread_mutex_lock(&g_lock);
	iter = g_light_head;
	prev = NULL;
	while (iter != light) {
		prev = iter;
		iter = iter->next;
	}

	if (iter != NULL) {
		if (prev == NULL)
			g_light_head = iter->next;
		else
			prev->next = iter->next;
	}

	pthread_mutex_unlock(&g_lock);
}

static int write_string(const char *path, const char *value)
{
	static int already_warned = 0;
	int rc;
	int fd;

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		if (already_warned == 0) {
			ALOGE("write_string failed to open %s\n", path);
			already_warned = 1;
		}
		return -1;
	}

	rc = write(fd, value, strlen(value));
	close(fd);
	if (rc < 0)
		return -1;

	return 0;
}

static int write_int(const char *path, int value)
{
	char buffer[20];

	snprintf(buffer, sizeof(buffer), "%d\n", value);
	return write_string(path, buffer);
}

static int read_int(const char *path)
{
	static int already_warned = 0;
	char buffer[12];
	int fd, rc;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		if (already_warned == 0) {
			ALOGE("read_int failed to open %s\n", path);
			already_warned = 1;
		}
		return -1;
	}

	rc = read(fd, buffer, sizeof(buffer) - 1);
	close(fd);
	if (rc <= 0)
		return -1;

	buffer[rc] = 0;

	return strtol(buffer, 0, 0);
}

static void write_led_scaled(enum led_ident id, int brightness,
		const char *pwm, unsigned int duration)
{
	float max_brightness = read_int(led_descs[id].max_brightness_s);
	float brightness_c = ((float)brightness)/255.; 
	float scaled = max_brightness * brightness_c;

	if (pwm && led_descs[id].pwm)
		write_string(led_descs[id].pwm, pwm);

	if(duration!=0) {
		if (led_descs[id].step)
			write_int(led_descs[id].step, duration);
	}

	write_int(led_descs[id].brightness, ((int)scaled));
}

static int is_lit(struct light_state_t const* state)
{
	return state->color & 0x00ffffff;
}

static int rgb_to_brightness(struct light_state_t const* state)
{
	int color = state->color & 0x00ffffff;
	return ((77*((color>>16)&0x00ff))
			+ (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int set_light_backlight(struct light_device_t *dev, struct light_state_t const *state)
{
	pthread_mutex_lock(&g_lock);
	write_led_scaled(LED_BACKLIGHT, rgb_to_brightness(state), NULL, 0);
	pthread_mutex_unlock(&g_lock);

	return 0;
}

static const char *pwm_patterns[] = {
	"0,0,0,0,0,0,0,0,100,0,0,0,0,0,0,0",
	"0,0,0,0,0,0,0,100,100,0,0,0,0,0,0,0",
	"0,0,0,0,0,0,0,100,100,100,0,0,0,0,0,0",
	"0,0,0,0,0,0,100,100,100,100,0,0,0,0,0,0",
	"0,0,0,0,0,0,100,100,100,100,100,0,0,0,0,0",
	"0,0,0,0,0,100,100,100,100,100,100,0,0,0,0,0",
	"0,0,0,0,0,100,100,100,100,100,100,100,0,0,0,0",
	"0,0,0,0,100,100,100,100,100,100,100,100,0,0,0,0",
	"0,0,0,0,100,100,100,100,100,100,100,100,100,0,0,0",
	"0,0,0,100,100,100,100,100,100,100,100,100,100,0,0,0",
	"0,0,0,100,100,100,100,100,100,100,100,100,100,100,0,0",
	"0,0,100,100,100,100,100,100,100,100,100,100,100,100,0,0",
	"0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,0",
	"0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,0",
	"0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100",
	"100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100",
};

static const char *calc_pattern(int on, int off, int *duration)
{
	int index;

	index = on * 15 / (on + off);
	*duration = on / (index + 1);
	return pwm_patterns[index];
}

static int set_light_shared(struct light_device_t *dev, struct light_state_t const* state)
{
	struct light *light = ll_light(dev);
	const char *pwm;
	int duration;
	int i;

	pthread_mutex_lock(&g_lock);
	g_led_shared[light->shared_which] = *state;

	for (i = 0; i < _LED_SHARED_CNT; ++i) {
		state = &g_led_shared[i];
		if (is_lit(state))
			break;
	}

	if (state->flashMode != LIGHT_FLASH_NONE && state->flashOffMS != 0) {
		ALOGD("led [%d,%d] = %08x\n",
				state->flashOnMS, state->flashOffMS,
				state->color);
		write_int(led_ctrl.sync_state, 1);
		pwm = calc_pattern(state->flashOnMS,
				state->flashOffMS, &duration);
	} else {
		ALOGD("led [solid] = %08x\n", state->color);
		write_int(led_ctrl.sync_state, 0);
		duration = 0;
		pwm = NULL;
	}

	write_led_scaled(LED_RED, (state->color >> 16) & 0xFF, pwm, duration);
	write_led_scaled(LED_GREEN, (state->color >> 8) & 0xFF, pwm, duration);
	write_led_scaled(LED_BLUE, (state->color) & 0xFF, pwm, duration);

	if (state->flashMode != LIGHT_FLASH_NONE) {
		write_int(led_ctrl.blink_start, 1);
	}

	pthread_mutex_unlock(&g_lock);
	return 0;
}

static int close_lights(struct light_device_t *dev)
{
	struct light *light = ll_light(dev);
	ALOGI("closing light \"%s\"\n", light->name);
	ll_remove(light);
	free(light);
	return 0;
}

static int open_lights(const struct hw_module_t* module,
		char const* name, struct hw_device_t** device)
{
	struct light_state_t light_off = { 0 };
	struct light *light;
	int shared_which;
	int (*set_light)(struct light_device_t* dev,
					 struct light_state_t const *state);

	light = ll_find(name);
	if (light != NULL) {
		ALOGW("light \"%s\" already opened, returning old handle\n", name);
		*device = (struct hw_device_t *)&light->dev;
		return 0;
	}

	if (strcmp(name, LIGHT_ID_BACKLIGHT) == 0) {
		set_light = set_light_backlight;
		shared_which = -1;
	} else if (strcmp(name, LIGHT_ID_BATTERY) == 0) {
		set_light = set_light_shared;
		shared_which = LED_SHARED_BATTERY;
	} else if (strcmp(name, LIGHT_ID_BLUETOOTH) == 0) {
		set_light = set_light_shared;
		shared_which = LED_SHARED_BLUETOOTH;
	} else if (strcmp(name, LIGHT_ID_NOTIFICATIONS) == 0) {
		set_light = set_light_shared;
		shared_which = LED_SHARED_NOTIFICATION;
	} else {
		ALOGW("don't have light \"%s\"\n", name);
		return -EINVAL;
	}

	ALOGI("registering light \"%s\"\n", name);

	light = calloc(1, sizeof(struct light));
	if (light == NULL)
		return -ENOMEM;

	light->dev.common.tag		= HARDWARE_DEVICE_TAG;
	light->dev.common.version	= 0;
	light->dev.common.module	= (struct hw_module_t*)module;
	light->dev.common.close	= (int (*)(struct hw_device_t*))close_lights;
	light->dev.set_light		= set_light;
	light->shared_which		= shared_which;
	strncpy(light->name, name, sizeof(light->name));
	light->name[sizeof(light->name) - 1] = 0;

	ll_add(light);

	if (set_light != set_light_backlight)
		(* set_light)(&light->dev, &light_off);

	*device = (struct hw_device_t *)&light->dev;

	return 0;
}

static struct hw_module_methods_t lights_module_methods = {
	.open = open_lights,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = LIGHTS_HARDWARE_MODULE_ID,
	.name = "Rhine lights module",
	.author = "Bjorn Andersson <bjorn.andersson@sonymobile.com>",
	.methods = &lights_module_methods,
};
