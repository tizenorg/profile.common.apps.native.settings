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
/**
 * @file setting-about.h
 *
 * @brief	   Setting > About
 * @details	   'About' view object by UI gadget
 */



/**
 * @defgroup setting-about Setting About
 * @{
 */

#ifndef __SETTING_ABOUT_H__
#define __SETTING_ABOUT_H__

#include <stdio.h>
#include <Elementary.h>
#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <notification.h>
#include <dbus/dbus.h>

#define MAX_DISPLAY_STR_LEN_ON_PHONE_INFO	256
#define SETTING_ABOUT_MOBILE_AP_TURNED_OFF "IDS_ST_POP_DISABLE_TETHERING_Q"
#define SETTING_ABOUT_MY_NUMBERS_LEN 3
#define SETTING_ABOUT_WIFI_MAC_STR_LEN 17
#define SETTING_ABOUT_PUK_LOCKED "PUK is locked"

typedef struct _SettingAboutUG SettingAboutUG;

/**
 * Setting About UG context
 * all UG function has void* as an agument. this is casted back to SettingAboutUG
 * and the functions access app context.
 */
struct _SettingAboutUG {
	ui_gadget_h ug;
	bool pause_flag;

	setting_view *view_to_load;

	Elm_Genlist_Item_Class itc_2text_2;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	Evas_Object *navi_bar;
	Evas_Object *ly_main;
	Evas_Object *genlsit;
	Elm_Object_Item *navi_item;
	Evas_Object *back_key;
	Evas_Object *btn_done;
	Evas_Object *btn_cancel;

	Ecore_Timer *update_timer;
	Ecore_Idler *update_idler;
	Setting_GenGroupItem_Data *item_dev_name;
	Setting_GenGroupItem_Data *item_dev_name_main;
	bool is_dev_name_focus;
	int cursor_pos;
	Setting_GenGroupItem_Data *item_version;
	Setting_GenGroupItem_Data *item_data_cpu;
	Setting_GenGroupItem_Data *item_data_battery;
	Setting_GenGroupItem_Data *item_data_bt;
	Setting_GenGroupItem_Data *item_data_wifi;
	Evas_Object *popup;
	Evas_Object *popup_space;
	bool empty_flag;
	bool popup_showed_flag; /** if popup has been showed, do not show again*/
	bool drag_flag;

	/*idler for popup */
	Ecore_Idler *idler_remove_popup;
	Ecore_Idler *idler_add_popup;
	Ecore_Idler *idler_remove_space_popup;

	Ecore_Event_Handler *event_handler;
	Ecore_Idler *name_update_idler;
};

extern setting_view setting_view_about_main;
extern void setting_about_main_get_wifi_mac_address_string(char *str, int size);
extern void setting_about_main_get_bluetooth_address_string(char *str, int size);

/**
 * @}
 */
#endif				/* __SETTING_ABOUT_H__ */
