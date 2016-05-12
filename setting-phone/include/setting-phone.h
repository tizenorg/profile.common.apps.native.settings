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
#ifndef __SETTING_PHONE_H__
#define __SETTING_PHONE_H__

#include <stdio.h>
#include <Elementary.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <aul.h>

#ifdef Status
#undef Status
#endif
#include <tapi_common.h>
#include <ITapiSim.h>
#include <TapiUtility.h>

#include <unicode/ucal.h>
#include <unicode/uloc.h>
#include <unicode/udat.h>
#include <unicode/ustring.h>
#include <unicode/udatpg.h>
#include <unicode/utmscale.h>
#include <unicode/ucol.h>

/* Choose default programe*/
#define REGION_ITEM_MAX 300
#define REGION_FORMAT_LOWER "IDS_ST_BODY_REGION"

typedef struct _SettingPhoneUG SettingPhoneUG;

/**
 * Setting Phone UG context
 * all UG function has void* as an agument. this is casted back to SettingPhoneUG
 * and the functions access app context.
 */
struct _SettingPhoneUG {
	ui_gadget_h ug;
	TapiHandle *handle;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	ui_gadget_h ug_loading;

	Evas_Object *gl_lang_region;
	Evas_Object *gl_lang;
	Evas_Object *gl_region;

	Evas_Object *ly_region;

	Evas_Object *navi_bar;
	Evas_Object *popup;
	Evas_Object *popup_clear_license;
	void (*popup_cb)(void *data, Evas_Object *obj, void *event_info);

	int intial_shaking_value;

	Evas_Object *ly_language;
	Evas_Object *ly_ticker;
	Evas_Object *ly_license;
	Evas_Object *nocontents;
	Evas_Object *ly_sub_region;

	/* [UI] notification details */
	Setting_GenGroupItem_Data *data_details_notification;
	Setting_GenGroupItem_Data *data_details_sound;
	Setting_GenGroupItem_Data *data_details_display_content;
	Setting_GenGroupItem_Data *data_details_badge;

	char *sel_string;

	/* [UI] notification */
	Setting_GenGroupItem_Data *data_n_msg;
	Setting_GenGroupItem_Data *data_n_email;
	Setting_GenGroupItem_Data *data_n_im;
	Setting_GenGroupItem_Data *data_n_fb;
	Setting_GenGroupItem_Data *data_n_twt;

	/* [UI] license */
	Evas_Object *chk_lang;
	int prev_lang;		/** prev is used for special lang. */
	Setting_GenGroupItem_Data *data_tracking;
	Setting_GenGroupItem_Data *data_expiry;
	Setting_GenGroupItem_Data *divx_vod;
	Setting_GenGroupItem_Data *data_roaming_nw;

	/* divx.... */
	Setting_GenGroupItem_Data *data_reg;
	Setting_GenGroupItem_Data *data_dereg;
	Evas_Object *pop_dereg;
	int mode;		/* 0: reg, 1: dereg */

	Evas_Object *chk_tracking;
	Evas_Object *chk_expiry;

	/* Roaming network */
	Setting_GenGroupItem_Data *auto_download;
	Setting_GenGroupItem_Data *manual;
	Setting_GenGroupItem_Data *always_reject;

	/*bundle *bundle_data; */
	app_control_h bundle_data;
	setting_view *view_to_load;
	TelSimImsiInfo_t imsi;
	const char *sim_lang;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_1text_tb;
	Elm_Genlist_Item_Class itc_1text_1icon_2;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_1icon_2text;
	Elm_Genlist_Item_Class itc_1icon_with_no_line;

	/* [view] Language & Region */
	Setting_GenGroupItem_Data *data_display_language;
	Setting_GenGroupItem_Data *data_region_fmt;
	Setting_GenGroupItem_Data *data_region_fmt_example;
	Setting_GenGroupItem_Data *data_keyboard;
	Setting_GenGroupItem_Data *data_title_keyboard;

	Evas_Object *chk_region;
	int prev_region;
	char *region_keyStr[REGION_ITEM_MAX];
	char *region_desc[REGION_ITEM_MAX];
	int region_index[REGION_ITEM_MAX];
	int region_num;
	int gl_region_index;
	int gl_region_cur_index;
	int selected_region_idx;
	char *region_vconf_str;
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_bg_1icon;
	UDateTimePatternGenerator *pattern_generator;
	char *prev_locale;
	Elm_Object_Item *selected_item;
	Eina_List *app_list;
	Evas_Object *pop_progress;
	Evas_Object *search_bar;
	char search_text[MAX_SEARCH_STR_LEN + 1];
	Ecore_Idler *search_idler;
	Ecore_Animator *animator;
	Ecore_Timer *set_timer;
	Ecore_Idler *set_idler;

	Elm_Object_Item *lang_region_navi_it;

	Ecore_Timer *lang_flush_timer;
	Eina_Bool lang_change;

	Ecore_Timer *searchlist_update_timer;
	Ecore_Timer *popup_timer;

	Ecore_Timer *event_freeze_timer;

	Evas_Object *pop_progress_lang;

	char *selected_locale;
	char *region_search_id;
};

/* extern setting_view setting_view_phone_main; */
extern setting_view setting_view_phone_language_region;
#ifdef ENABLE_TICKER_NOTI
extern setting_view setting_view_phone_ticker_notification;
extern setting_view setting_view_phone_ticker_notification_details;
#endif
extern setting_view setting_view_phone_license_main;
extern setting_view setting_view_phone_display_language;
extern setting_view setting_view_phone_region_format;

/* void setting_phone_lang_changed_cb(void *data); */
int app_terminate_dead_cb(int pid, void *data);
extern char *setting_phone_lang_get_region_str(const char *region);
extern char *setting_phone_lang_get_by_pattern(const char *locale,
											   const char *skeleton,
											   void *data);
extern char *setting_phone_lang_get_example_desc(const char *region,
												 void *data);
/**
* Change language string to UTF8 format, the invoker need to free the return string
*
* @param unichars The source string will be translated.
*
* @return UTF8 format string.
*/
extern char *setting_phone_lang_str_to_utf8(const UChar *unichars);


/* event system */
int set_language_helper(const char *lang);
int set_regionformat_helper(const char *region);

#endif				/* __SETTING_PHONE_H__ */
