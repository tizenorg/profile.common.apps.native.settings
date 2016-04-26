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

#include <feedback.h>
#include <mm_sound_private.h>
#include <app_manager.h>
#include <setting-profile-sound-main.h>
#include <setting-profile.h>
#include <setting-common-general-func.h>

#include <eventsystem.h>
#include <bundle_internal.h>
#include <notification_setting.h>
#include <notification_setting_internal.h>


#define SETTING_SOUND_VOL_MAX 15
#define SETTING_DEFAULT_NOTI_VOL_INT		11
#define SETTING_DEFAULT_MEDIA_VOL_INT		9
#define SETTING_DEFAULT_SYSTEM_VOL_INT		9

static int setting_sound_main_create(void *cb);
static int setting_sound_main_destroy(void *cb);
static int setting_sound_main_update(void *cb);
static int setting_sound_main_cleanup(void *cb);

setting_view setting_view_sound_main = {
	.create = setting_sound_main_create,
	.destroy = setting_sound_main_destroy,
	.update = setting_sound_main_update,
	.cleanup = setting_sound_main_cleanup,
};

/* should be called in function to return int */
#define ADD_GENLIST(genlist, parent) \
	{\
		genlist = elm_genlist_add(parent);\
		retvm_if(genlist == NULL, SETTING_RETURN_FAIL, "Cannot set genlist object as contento of layout");\
		elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);\
		elm_object_style_set(genlist, "dialogue");\
		elm_genlist_clear(genlist);\
		evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb, NULL);\
		elm_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);\
	}

#define ADD_SOUND_SLIDER(item_data, genlist, item_style, l_img, r_img, value, keystr, data, sld_cb, sld_max, start_cb, stop_cb) \
	{ \
		item_data = setting_create_Gendial_field_def(genlist, &(item_style), NULL,\
		                                             NULL, \
		                                             SWALLOW_Type_LAYOUT_SLIDER,\
		                                             l_img, r_img,\
		                                             value,\
		                                             keystr,\
		                                             NULL,\
		                                             sld_cb);\
		if (item_data) {\
			item_data->win_main = data->win_main_layout;\
			item_data->evas = data->evas;\
			item_data->isIndicatorVisible = FALSE;\
			item_data->slider_min = 0;\
			item_data->slider_max = sld_max;\
			item_data->userdata = data;\
			item_data->start_change_cb = start_cb;\
			item_data->stop_change_cb = stop_cb;\
			elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		} else {\
			SETTING_TRACE_ERROR("%s item is NULL", keystr);\
		} \
	}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
void __start_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	ret_if(ad == NULL);
	setting_sound_create_warning_popup_during_call(ad);
	if (list_item == ad->data_noti_volume) {
		setting_sound_close_other_mm_players_but_type(ad, SOUND_TYPE_NOTIFICATION);
	} else if (list_item == ad->data_media_volume) {
		setting_sound_close_other_mm_players_but_type(ad, SOUND_TYPE_MEDIA);
	} else if (list_item == ad->data_touch_volume) {
		setting_sound_close_all_mm_players(ad);
	}
}

static void __noti_slider_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	ret_if(data == NULL);

	double val = elm_slider_value_get(obj);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	sound_type_e sound_type = SOUND_TYPE_NOTIFICATION;

	if (list_item->chk_status != (int)(val + 0.5)) {
		list_item->chk_status = (int)(val + 0.5);
		setting_sound_update_slider_icon(list_item, SND_SLIDER_NOTI);
		sound_manager_set_volume(sound_type, list_item->chk_status);
		SETTING_TRACE_DEBUG("notification volume is %d, %f", list_item->chk_status, val);
	}

	setting_sound_create_warning_popup_during_call(ad);
	if (ad->calling_popup)
		return;

	/*
	 * S5 concept:
	 * On the volume popup,
	 *  1) if user controls the slider, don't play any sounds£¬just change volume.
	 *  2) if user controls the HW volume key for ringtone sound, the beep sound is supported

	 * On Setting > Sound
	 *  1) if user controls the slider, change volume & play sound
	 *  2) if user controls the HW volume key, do nothing(DO NOT change volume,DO NoT play sound)
	*/
	if (0 == safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		return;
	}

	if (ad->mp_noti == NULL) {
		setting_sound_close_other_mm_players_but_type(ad, sound_type);
		if (list_item->chk_status > 0) {
			char *pa_tone_path = vconf_get_str(VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR);
			if (pa_tone_path == NULL) {
				pa_tone_path = (char *)strdup(SETTING_DEFAULT_MSG_TONE);
				if (vconf_set_str(VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR, pa_tone_path) < 0) {
					FREE(pa_tone_path);
					return;
				}
			}
			setting_sound_play_sound_origin(&(ad->mp_noti), ad, NULL,
			                                pa_tone_path, (float)list_item->chk_status,
			                                sound_type);
			FREE(pa_tone_path);
		}
	} else {
		player_state_e state;
		player_get_state(*(ad->mp_noti), &state);
		SETTING_TRACE("noti player status : %d", state);
		if (state == PLAYER_STATE_PAUSED) {
			player_start(*(ad->mp_noti));
		}
	}
}


static void __media_slider_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	ret_if(data == NULL);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	sound_type_e sound_type = SOUND_TYPE_MEDIA;

	double val = elm_slider_value_get(obj);
	int value = (int)(val + 0.5);
	if (list_item->chk_status != value) {

		list_item->chk_status = value;
		setting_sound_update_slider_icon(list_item, SND_SLIDER_MEDIA);
		int ret = sound_manager_set_volume(sound_type, value);
		SETTING_TRACE_DEBUG("ret : 0x%.8x", ret);

		SETTING_TRACE_DEBUG("media volume is %d, %f", list_item->chk_status, val);
	}

	setting_sound_create_warning_popup_during_call(ad);
	if (ad->calling_popup)
		return;

	/*
	 * S5 concept:
	 * On the volume popup,
	 *  1) if user controls the slider, don't play any sounds£¬just change volume.
	 *  2) if user controls the HW volume key for ringtone sound, the beep sound is supported

	 * On Setting > Sound
	 *  1) if user controls the slider, change volume & play sound
	 *  2) if user controls the HW volume key, do nothing(DO NOT change volume,DO NoT play sound)
	*/
	if (0 == safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		return;
	}

	if (ad->mp_media == NULL) {
		setting_sound_close_other_mm_players_but_type(ad, sound_type);
		if (list_item->chk_status > 0) {
			char	*pa_tone_path = NULL;
			if (isEmulBin())
				pa_tone_path = (char *)strdup(SETTING_DEFAULT_CALL_TONE_SDK);
			else
				pa_tone_path = (char *)strdup(SETTING_DEFAULT_MEDIA_TONE);

			sound_type_e type;
			sound_manager_get_current_sound_type(&type);
			setting_sound_play_sound_origin(&(ad->mp_media), ad, NULL,
			                                pa_tone_path,
			                                (float)list_item->chk_status,
			                                sound_type);
			FREE(pa_tone_path);
		}
	} else {
		player_state_e state;
		player_get_state(*(ad->mp_media), &state);
		SETTING_TRACE("ringtone player status : %d", state);
		if (state == PLAYER_STATE_PAUSED) {
			player_start(*(ad->mp_media));
		}
	}
}

static void __sys_slider_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	ret_if(ad == NULL);
	setting_sound_close_all_mm_players(ad);

	/*Get the current real value of slider */
	double val = elm_slider_value_get(obj);
	int value = (int)(val + 0.5);
	sound_manager_set_volume(SOUND_TYPE_SYSTEM, value);
	SETTING_TRACE_DEBUG("system volume is %d, %f", value, val);

	setting_sound_create_warning_popup_during_call(ad);
	if (ad->calling_popup)
		return;

	if (list_item->chk_status != value) {

		/*
		 * S5 concept:
		 * On the volume popup,
		 *  1) if user controls the slider, don't play any sounds£¬just change volume.
		 *  2) if user controls the HW volume key for ringtone sound, the beep sound is supported

		 * On Setting > Sound
		 *  1) if user controls the slider, change volume & play sound
		 *  2) if user controls the HW volume key, do nothing(DO NOT change volume,DO NoT play sound)
		*/
		if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
			/*Play Tap Sound,then stop at once */
			if (feedback_initialize() == FEEDBACK_ERROR_NONE) {
				/*	sound_manager_session_set_state_others(SOUND_STATE_PAUSE); */
				feedback_play_type(FEEDBACK_TYPE_SOUND, FEEDBACK_PATTERN_TAP);
				feedback_deinitialize();
			}
		}
		list_item->chk_status = value;
		setting_sound_update_slider_icon(list_item, SND_SLIDER_SYSTEM);
	}
}


void __stop_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SETTING_TRACE("chk_status : %d, slider : %f", list_item->chk_status, elm_slider_value_get(obj));
	if (list_item->chk_status == 0) {
		elm_slider_value_set(obj, 0.0);
	} else if (list_item->chk_status == 15) {
		elm_slider_value_set(obj, 15.0);
	}
	SETTING_TRACE_END;
}

static void __get_lite_main_list(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingProfileUG *ad = (SettingProfileUG *)data;

	int vconf_value = 0;
	char *sub_desc = NULL;
	char *left_icon = NULL;
	int mm_value = 0;
	int sound_value = 0;
	int vconf_ret = 0;
	char *pa_ringtone = NULL;

	Evas_Object *genlist = ad->gl_lite_main;
	elm_genlist_block_count_set(genlist, 3);
	elm_genlist_homogeneous_set(genlist, 1);

	if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &sound_value) < 0)
		sound_value = TRUE;	/*  default value of sound : on */

	Elm_Genlist_Item_Class *itc_slider = NULL;


	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	itc_slider = &(ad->itc_layout_1icon);

	if (0 == safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
		itc_slider = &(ad->itc_layout);
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------

	/* 1. Notification alert */
	if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		pa_ringtone = vconf_get_str(VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR);
		if (setting_sound_check_file_exist(ad, pa_ringtone) == SETTING_RETURN_FAIL) {
			pa_ringtone = (char *)strdup(SETTING_DEFAULT_MSG_TONE);
			vconf_ret = vconf_set_str(VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR, SETTING_DEFAULT_MSG_TONE);
			if (vconf_ret < 0)
				SETTING_TRACE_DEBUG("failed to set vconf");
		}
		sub_desc = setting_media_basename(pa_ringtone);
		ad->data_msg_alert_tone =
		    setting_create_Gendial_field_def(genlist, &itc_2text_2,
		                                     setting_sound_main_mouse_up_Gendial_list_cb,
		                                     ad, SWALLOW_Type_INVALID, NULL,
		                                     NULL, 0,
		                                     "IDS_ST_BODY_NOTIFICATION"/*SETTING_SOUND_TEXT_MSG_ALERT*/,
		                                     sub_desc, NULL);
		if (ad->data_msg_alert_tone) {
			__BACK_POINTER_SET(ad->data_msg_alert_tone);
			ad->data_msg_alert_tone->userdata = ad;
		}

		FREE(pa_ringtone);
		G_FREE(sub_desc);
	}
	/* 2. noti volumn */
	if (sound_manager_get_volume(SOUND_TYPE_NOTIFICATION, &mm_value) < 0)
		mm_value = SETTING_DEFAULT_NOTI_VOL_INT;
	if (sound_value == FALSE)
		mm_value = 0;

	left_icon = setting_sound_get_slider_icon(SND_SLIDER_NOTI, mm_value);
	ADD_SOUND_SLIDER(ad->data_noti_volume, genlist,
	                 (*itc_slider),
	                 left_icon, NULL,
	                 mm_value, "IDS_ST_BODY_NOTIFICATION", ad,
	                 __noti_slider_change_cb,
	                 SETTING_SOUND_VOL_MAX,
	                 __start_change_cb, __stop_change_cb);
	__BACK_POINTER_SET(ad->data_noti_volume);

	/* 3. media volume */
	if (sound_manager_get_volume(SOUND_TYPE_MEDIA, &mm_value) < 0)
		mm_value = SETTING_DEFAULT_MEDIA_VOL_INT;

	left_icon = setting_sound_get_slider_icon(SND_SLIDER_MEDIA, mm_value);
	ADD_SOUND_SLIDER(ad->data_media_volume, genlist, ad->itc_layout,
	                 left_icon, NULL/*IMG_SLIDER_BTN_VOLUME02*/,
	                 mm_value, "IDS_ST_BODY_MEDIA", ad,
	                 __media_slider_change_cb,
	                 SETTING_SOUND_VOL_MAX,
	                 __start_change_cb, __stop_change_cb);

	__BACK_POINTER_SET(ad->data_media_volume);

	/* 4. system volume */
	if (sound_manager_get_volume(SOUND_TYPE_SYSTEM, &mm_value) < 0) {
		SETTING_TRACE_DEBUG("Fail to get volume");
		mm_value = SETTING_DEFAULT_SYSTEM_VOL_INT;
	}
	if (sound_value == FALSE)
		mm_value = 0;

	left_icon = setting_sound_get_slider_icon(SND_SLIDER_SYSTEM, mm_value);
	ADD_SOUND_SLIDER(ad->data_touch_volume, genlist, ad->itc_layout,
	                 left_icon, NULL/*IMG_SLIDER_BTN_SYSTEM_VOLUME*/,
	                 mm_value, "IDS_ST_BODY_SYSTEM", ad,
	                 __sys_slider_change_cb,
	                 SETTING_SOUND_VOL_MAX,
	                 __start_change_cb, __stop_change_cb);

	__BACK_POINTER_SET(ad->data_touch_volume);

	if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		/* Notifications */
		setting_create_Gendial_field_titleItem(genlist, &itc_group_item, _("IDS_ST_BODY_NOTIFICATIONS"), NULL);
		/* 5. Notifications - App notifications */
		setting_create_Gendial_field_def(genlist, &itc_2text_2,
										setting_sound_main_mouse_up_Gendial_list_cb,
										data, SWALLOW_Type_INVALID, NULL,
										NULL, 0,
										"IDS_ST_MBODY_APP_NOTIFICATIONS",
										"IDS_ST_BODY_ALLOW_OR_BLOCK_NOTIFICATIONS_FROM_INDIVIDUAL_APPLICATIONS", NULL);

		ad->gl_lite_main = genlist;

	}
	if (!sound_value) {
		setting_genlist_item_disabled_set(ad->data_noti_volume, 1);
		setting_genlist_item_disabled_set(ad->data_touch_volume, 1);
	}
}

static void __volume_popup_del_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/*SETTING_TRACE(" REMOVE THE POPUP OBJECT BY THIS FUNCTION "); */
	elm_exit();
}

void vibration_state_event_handler(const char *event_name, bundle *data, void *user_data)
{
	const char *vibration_state_set = NULL;
	SETTING_TRACE("vibration state set event (%s) received", event_name);

	vibration_state_set = bundle_get_val(data, EVT_KEY_VIBRATION_STATE);
	SETTING_TRACE("vibration_state_set(%s)", vibration_state_set);
}

int vibration_state_reg_id;

static Eina_Bool
setting_soudn_main_click_softkey_back_cb(void *data, Evas_Object *obj,
                                         void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, EINA_FALSE,
	                 "[Setting > Display] Data parameter is NULL");

	SettingProfileUG *ad = (SettingProfileUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;

}


static int setting_sound_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingProfileUG *ad = (SettingProfileUG *) cb;


	/* 1.Grab Hard Key */
	__setting_sound_ug_key_grab(ad);
	SETTING_TRACE("viewtype: %s", ad->viewtype);
	if (!safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		ad->ly_main = setting_create_popup_with_list(&ad->gl_lite_main, ad, ad->win_get,
		                                             "IDS_ST_BODY_VOLUME", __volume_popup_del_cb,
		                                             0 , false, false, 1, "IDS_ST_SK3_DONE");
		__get_lite_main_list(ad);
	} else {
		ad->ly_main = setting_create_win_layout(ad->win_get);
		ad->navi_bar = setting_create_navi_bar(ad->ly_main);

		ADD_GENLIST(ad->gl_lite_main, ad->navi_bar);

		__get_lite_main_list(ad);

		Evas_Object *lbtn = setting_create_button(ad->navi_bar, _(NULL),
		                                          NAVI_BACK_ARROW_BUTTON_STYLE,
		                                          setting_soudn_main_click_softkey_back_cb, ad);


		Elm_Object_Item *navi_it = elm_naviframe_item_push(ad->navi_bar, KeyStr_Sounds, lbtn, NULL, ad->gl_lite_main, NULL);
		elm_naviframe_item_pop_cb_set(navi_it, (Elm_Naviframe_Item_Pop_Cb)setting_sound_main_click_softkey_cancel_cb, ad);
		//elm_object_item_part_content_set(navi_it, "title_left_btn", lbtn);
		elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE, EINA_TRUE);
	}
	setting_view_sound_main.is_create = TRUE;

	/* eventsystem */
	if (ES_R_OK != eventsystem_register_event(SYS_EVENT_VIBRATION_STATE,
	                                          &vibration_state_reg_id,
	                                          (eventsystem_handler)vibration_state_event_handler, cb)) {
		SETTING_TRACE_ERROR("error");
	}
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_sound_main_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SETTING_TRACE_BEGIN;
	retv_if(!setting_view_sound_main.is_create, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) cb;

	/* 1.Ungrab Key */
	__setting_sound_ug_key_ungrab(ad);

	if (ad->play_ringtone_idler) {
		ecore_idler_del(ad->play_ringtone_idler);
		ad->play_ringtone_idler = NULL;
	}

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
	}
	setting_view_sound_main.is_create = FALSE;

	if (ES_R_OK != eventsystem_unregister_event(vibration_state_reg_id)) {
		SETTING_TRACE_ERROR("error");
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_sound_main_update(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_sound_main_cleanup(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_hide(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static Eina_Bool setting_sound_main_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	/* error check */
	retv_if(data == NULL, EINA_FALSE);

	SettingProfileUG *ad = (SettingProfileUG *) data;
	SETTING_TRACE_BEGIN;

	/* close mm plaer */
	setting_sound_close_all_mm_players(ad);

	/* Send destroy request */
	ug_destroy_me(ad->ug);

	return EINA_FALSE;
}

static void setting_sound_main_destroy_myfile_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingProfileUG *ad = (SettingProfileUG *) priv;

	int sound_value = 0, volume = 0;

	__setting_sound_ug_key_grab(ad);

	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_loading = NULL;
	}

	if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &sound_value) < 0)
		sound_value = TRUE;

	if (sound_value == FALSE)
		volume = 0;
	else
		(void)sound_manager_get_volume(SOUND_TYPE_RINGTONE, &volume);

	if (ad->data_call_volume && ad->data_call_volume->eo_check) {
		ad->data_call_volume->chk_status = volume;
		elm_slider_value_set(ad->data_call_volume->eo_check, volume);
	}

	if (sound_value == FALSE)
		volume = 0;
	else
		(void)sound_manager_get_volume(SOUND_TYPE_NOTIFICATION, &volume);

	if (ad->data_noti_volume && ad->data_noti_volume->eo_check) {
		ad->data_noti_volume->chk_status = volume;
		elm_slider_value_set(ad->data_noti_volume->eo_check, volume);
	}

	setting_sound_update_slider_icon(ad->data_call_volume, SND_SLIDER_CALL);
	setting_sound_update_slider_icon(ad->data_noti_volume, SND_SLIDER_NOTI);
}

static void
setting_sound_main_result_myfile_ug_cb(ui_gadget_h ug,
                                       app_control_h result, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);

	SettingProfileUG *ad = (SettingProfileUG *) priv;
	char *ringtone_file = NULL;
	char *ringtone_path = NULL;

	app_control_get_extra_data(result, "result", &ringtone_path);
	if (!ringtone_path)
		return;

	char *recommend_time = NULL;
	app_control_get_extra_data(result, "position", &recommend_time);

	SETTING_TRACE("ringtone_path:%s", ringtone_path);

	if (ringtone_path != NULL) {
		ringtone_file = setting_media_basename(ringtone_path);
		SETTING_TRACE("ringtone_file:%s", ringtone_file);

		if (0 ==
				safeStrCmp(ad->ringtone_type, "IDS_ST_BODY_NOTIFICATION")) {
			if (vconf_set_str(VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR, ringtone_path) == 0) {
				if (ad->data_msg_alert_tone) {
					ad->data_msg_alert_tone->sub_desc = (char *)g_strdup(ringtone_file);

					elm_object_item_data_set(ad->data_msg_alert_tone->item, ad->data_msg_alert_tone);
					elm_genlist_item_update(ad->data_msg_alert_tone->item);
				}
			}
		}
	}

	FREE(ringtone_path);
	FREE(recommend_time);
	G_FREE(ringtone_file);
	return;
}

static void setting_sound_main_create_myfile_ug(SettingProfileUG *ad, char *titleID)
{
	ret_if(ad == NULL);

	if (ad->ug_loading) {
		SETTING_TRACE("Myfiles ug is already loaded.");
		return;
	}

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	if (!cbs)
		return;

	cbs->layout_cb = setting_layout_ug_cb;
	cbs->result_cb = setting_sound_main_result_myfile_ug_cb;
	cbs->destroy_cb = setting_sound_main_destroy_myfile_ug_cb;
	cbs->priv = (void *)ad;

	app_control_h svc;
	if (app_control_create(&svc)) {
		FREE(cbs);
		return;
	}

	const char *pa_cur_ringtone = NULL;
	const char *dir_path = NULL;
	if (0 == safeStrCmp(ad->ringtone_type, "IDS_ST_MBODY_RINGTONE")) {
		pa_cur_ringtone = vconf_get_str(VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
		if (pa_cur_ringtone == NULL) {
			if (isEmulBin())
				pa_cur_ringtone = (char *)strdup(SETTING_DEFAULT_CALL_TONE_SDK);
			else
				pa_cur_ringtone = (char *)strdup(SETTING_DEFAULT_CALL_TONE);
		}
		dir_path = SETTING_PROFILE_SOUND_RINGTONES_PATH;
	} else if (0 == safeStrCmp(ad->ringtone_type, "IDS_ST_BODY_NOTIFICATION")) {
		pa_cur_ringtone = vconf_get_str(VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR);
		if (pa_cur_ringtone == NULL)
			pa_cur_ringtone = (char *)strdup(SETTING_DEFAULT_MSG_TONE);
		dir_path = SETTING_PROFILE_SOUND_ALERTS_PATH;
	}
	SETTING_TRACE("pa_cur_ringtone:\t%s", pa_cur_ringtone);
	SETTING_TRACE("dir_path:\t%s", dir_path);

	app_control_add_extra_data(svc, "title", titleID);
	app_control_add_extra_data(svc, "domain", SETTING_PACKAGE);
	app_control_add_extra_data(svc, "path", dir_path);
	app_control_add_extra_data(svc, "select_type", "SINGLE_FILE");
	app_control_add_extra_data(svc, "file_type", "SOUND");
	app_control_add_extra_data(svc, "drm_type", "DRM_ALL");
	app_control_add_extra_data(svc, "silent", "silent show"); /* Show Silent item in the list */
	if (safeStrCmp(pa_cur_ringtone, "silent") == 0)
		app_control_add_extra_data(svc, "marked_mode", "silent");	/* Set radio on Silent item */
	else
		app_control_add_extra_data(svc, "marked_mode", pa_cur_ringtone);

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	SETTING_TRACE("ad->ug->layout:%p", (void *)ug_get_layout(ad->ug));
	ad->ug_loading = setting_ug_create(ad->ug, "setting-ringtone-efl", UG_MODE_FULLVIEW, svc, cbs);
	if (NULL == ad->ug_loading) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_loading");
	}

	app_control_destroy(svc);
	FREE(cbs);

	/** @todo verify it's valid */
	if (pa_cur_ringtone) {
		free((void *)pa_cur_ringtone);
		pa_cur_ringtone = NULL;
	}

	/*__setting_sound_ug_key_ungrab(ad); */
	/*__setting_sound_ug_menu_key_grab(ad); */
	return;
}

static void
setting_sound_main_mouse_up_Gendial_list_cb(void *data,
                                            Evas_Object *obj,
                                            void *event_info)
{
	/* error check */
	ret_if(data == NULL);
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = elm_object_item_data_get(item);

	SettingProfileUG *ad = data;

	if (SETTING_RETURN_SUCCESS != setting_sound_close_all_mm_players(ad)) {
		SETTING_TRACE_ERROR("close player fail.");
	}

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	if (!safeStrCmp("IDS_ST_BODY_NOTIFICATION", list_item->keyStr)) {
		ad->ringtone_type = list_item->keyStr;
		setting_sound_main_create_myfile_ug(ad, list_item->keyStr);
		__setting_sound_ug_key_ungrab(ad);
	} else if (!safeStrCmp("IDS_ST_MBODY_APP_NOTIFICATIONS", list_item->keyStr)) {
		app_launcher("ug-setting-notification-app-notifications-efl");
	}
}
