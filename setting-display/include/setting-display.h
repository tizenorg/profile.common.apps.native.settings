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

#include <stdio.h>
#include <string.h>

#include <Elementary.h>
#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <device.h>
#include <dd-display.h>
#include <dd-deviced.h>
#include <dbus/dbus.h>

#define SETTING_DISPLAY_ICON_PATH 12

typedef struct _SettingDisplayUG SettingDisplayUG;

/**
 * Setting Display UG context
 * all UG function has void* as an agument. this is casted back to SettingDisplayUG
 * and the functions access app context.
 */
struct _SettingDisplayUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	Evas_Object *ly_main;	/**< link to src/setting-display-main.c */

	Evas_Object *navi_bar;	/*	for navigation effect */
	Elm_Object_Item *nf_it;
	Ecore_Timer *event_freeze_timer;

	Evas_Object *screen_timeout_popup;
	Evas_Object *screen_timeout_rdg;

	Setting_GenGroupItem_Data *data_br;
	Setting_GenGroupItem_Data *data_font;
	Setting_GenGroupItem_Data *data_back;
	Setting_GenGroupItem_Data *data_br_auto;
	Setting_GenGroupItem_Data *data_br_sli;
	Setting_GenGroupItem_Data *data_adjust;
	Setting_GenGroupItem_Data *data_overheating;
	Setting_GenGroupItem_Data *data_auto_rotate;

	int low_bat;

	setting_view *view_to_load;

	Evas_Object *popup;

	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_2text_3;
	Elm_Genlist_Item_Class itc_1icon;

	Setting_GenGroupItem_Data *data_backlight_always_on;
	Setting_GenGroupItem_Data *data_backlight_15sec;
	Setting_GenGroupItem_Data *data_backlight_30sec;
	Setting_GenGroupItem_Data *data_backlight_1min;
	Setting_GenGroupItem_Data *data_backlight_2min;
	Setting_GenGroupItem_Data *data_backlight_5min;
	Setting_GenGroupItem_Data *data_backlight_10min;
	/*Improve response speed */
	Ecore_Idler *set_idler;

	/* genlist */
	Evas_Object *genlist;

	DBusConnection *bus;

	/*for brightness*/
	Eina_Bool is_event_registered;
	int last_requested_level;
};

extern setting_view setting_view_display_main;
extern setting_view setting_view_display_brightness;

extern void destruct_brightness(void *data);
extern void construct_brightness(void *data, Evas_Object *genlist);


#endif				/* __SETTING_DISPLAY_H__ */
