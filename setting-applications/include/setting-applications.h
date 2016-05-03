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
#ifndef __SETTING_DISPLAY_H__
#define __SETTING_DISPLAY_H__

#include <Elementary.h>
#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

typedef struct _SettingApplicationsUG SettingApplicationsUG;

/**
 * Setting Applications UG context
 * all UG function has void* as an agument. this is casted back to SettingApplicationsUG
 * and the functions access app context.
 */
struct _SettingApplicationsUG {
	ui_gadget_h ug;

	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *ly_main;
	Evas_Object *navi_bar;	/*	for navigation effect */
	Ecore_Timer *event_freeze_timer;

	/* genlist */
	Evas_Object *genlist;
};

extern setting_view setting_view_applications_main;

#endif				/* __SETTING_DISPLAY_H__ */
