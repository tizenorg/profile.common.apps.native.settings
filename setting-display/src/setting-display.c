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

#include <setting-display.h>
#include <Eina.h>
#include <setting-cfg.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void setting_display_ug_cb_resize(void *data, Evas *e,
										 Evas_Object *obj, void *event_info)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	setting_view_update(&setting_view_display_main, ad);
}

static void *setting_display_ug_on_create(ui_gadget_h ug,
										  enum ug_mode mode, app_control_h service,
										  void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");
	SETTING_TRACE_BEGIN;

	SettingDisplayUG *displayUG = priv;
	displayUG->ug = ug;
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	displayUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	displayUG->win_get = (Evas_Object *) ug_get_window();

	/*evas_object_show(displayUG->win_main_layout); */
	displayUG->evas = evas_object_evas_get(displayUG->win_main_layout);

	setting_retvm_if(displayUG->win_main_layout == NULL, NULL,
					 "cannot get main window ");

	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(displayUG->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(displayUG->itc_1text));

	/*	creating a view. */
	setting_view_node_set_cur_view(&setting_view_display_main);
	setting_view_create(&setting_view_display_main, (void *)displayUG);
	evas_object_event_callback_add(displayUG->win_main_layout,
								   EVAS_CALLBACK_RESIZE,
								   setting_display_ug_cb_resize, displayUG);

	return displayUG->ly_main;
}

static void setting_display_ug_on_start(ui_gadget_h ug, app_control_h service,
										void *priv)
{
}

static void setting_display_ug_on_pause(ui_gadget_h ug, app_control_h service,
										void *priv)
{
}

static void setting_display_ug_on_resume(ui_gadget_h ug, app_control_h service,
										 void *priv)
{
}

static void setting_display_ug_on_destroy(ui_gadget_h ug, app_control_h service,
										  void *priv)
{
	SettingDisplayUG *displayUG = priv;

	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");

	/* fix flash issue for gallery */
	evas_object_event_callback_del(displayUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_display_ug_cb_resize);
	displayUG->ug = ug;

	setting_view_destroy(&setting_view_display_main, displayUG);
	if (NULL != ug_get_layout(displayUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(displayUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(displayUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_display_ug_on_message(ui_gadget_h ug, app_control_h msg,
										  app_control_h service, void *priv)
{
}

static void setting_display_ug_on_event(ui_gadget_h ug,
										enum ug_event event, app_control_h service,
										void *priv)
{
	SettingDisplayUG *ad = priv;

	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == ad, "ad is NULL");

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		setting_navi_items_update(ad->navi_bar);
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_display_ug_on_key_event(ui_gadget_h ug,
											enum ug_key_event event,
											app_control_h service, void *priv)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) priv;

	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == ad, "ad is NULL");

	switch (event) {
	case UG_KEY_EVENT_END: {
			if (elm_naviframe_top_item_get(ad->navi_bar) ==
				elm_naviframe_bottom_item_get(ad->navi_bar)) {
				ug_destroy_me(ug);
			} else {
				setting_view_cb_at_endKey(ad);
			}
		}
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SettingDisplayUG *displayUG = calloc(1, sizeof(SettingDisplayUG));

	SETTING_TRACE_BEGIN;
	setting_retvm_if(!displayUG, -1, "Create SettingDisplayUG obj failed");

	memset(displayUG, 0x00, sizeof(SettingDisplayUG));

	ops->create = setting_display_ug_on_create;
	ops->start = setting_display_ug_on_start;
	ops->pause = setting_display_ug_on_pause;
	ops->resume = setting_display_ug_on_resume;
	ops->destroy = setting_display_ug_on_destroy;
	ops->message = setting_display_ug_on_message;
	ops->event = setting_display_ug_on_event;
	ops->key_event = setting_display_ug_on_key_event;
	ops->priv = displayUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	struct SettingDisplayUG *displayUG;

	SETTING_TRACE_BEGIN;
	setting_retm_if(!ops, "ops == NULL");

	displayUG = ops->priv;
	if (displayUG)
		FREE(displayUG);
}
