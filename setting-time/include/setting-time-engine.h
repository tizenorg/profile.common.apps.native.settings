/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef __SETTING_TIME_ENGINE_H__
#define __SETTING_TIME_ENGINE_H__

#include <setting-time.h>
#include <dd-deviced.h>

#define SETTING_TIME_ENGINE_TIME_STR_LEN 15

int setting_save_time(time_t _time);

int setting_time_update_cb(void *data);

#endif				/* __SETTING_TIME_ENGINE_H__ */
