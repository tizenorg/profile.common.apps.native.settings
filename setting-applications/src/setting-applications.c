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

#include <setting-applications.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void setting_applications_ug_cb_resize(void *data, Evas *e,
											  Evas_Object *obj, void *event_info)
{
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
}

static void *setting_applications_ug_on_create(ui_gadget_h ug,
											   enum ug_mode mode, app_control_h service,
											   void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");
	SETTING_TRACE_BEGIN;

	SettingApplicationsUG *applicationsUG = priv;
	applicationsUG->ug = ug;
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	applicationsUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	applicationsUG->win_get = (Evas_Object *) ug_get_window();

	applicationsUG->evas = evas_object_evas_get(applicationsUG->win_main_layout);

	setting_retvm_if(applicationsUG->win_main_layout == NULL, NULL,
					 "cannot get main window ");

	/* register view node table */
	setting_view_node_table_intialize();

	/*	creating a view. */
	setting_view_node_set_cur_view(&setting_view_applications_main);
	setting_view_create(&setting_view_applications_main, (void *)applicationsUG);

	evas_object_event_callback_add(applicationsUG->win_main_layout,
								   EVAS_CALLBACK_RESIZE,
								   setting_applications_ug_cb_resize, applicationsUG);
	return applicationsUG->ly_main;
}

static void setting_applications_ug_on_start(ui_gadget_h ug, app_control_h service,
											 void *priv)
{
}

static void setting_applications_ug_on_pause(ui_gadget_h ug, app_control_h service,
											 void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
}

static void setting_applications_ug_on_resume(ui_gadget_h ug, app_control_h service,
											  void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");

}

static void setting_applications_ug_on_destroy(ui_gadget_h ug, app_control_h service,
											   void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingApplicationsUG *applicationsUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(applicationsUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_applications_ug_cb_resize);
	applicationsUG->ug = ug;

	/*	called when this shared gadget is terminated. similar with app_exit */
	setting_view_destroy(&setting_view_applications_main, applicationsUG);

	if (NULL != ug_get_layout(applicationsUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(applicationsUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(applicationsUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_applications_ug_on_message(ui_gadget_h ug, app_control_h msg,
											   app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_applications_ug_on_event(ui_gadget_h ug,
											 enum ug_event event, app_control_h service,
											 void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = priv;
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

static void setting_applications_ug_on_key_event(ui_gadget_h ug,
												 enum ug_key_event event,
												 app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = (SettingApplicationsUG *) priv;

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
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *applicationsUG = calloc(1, sizeof(SettingApplicationsUG));
	setting_retvm_if(!applicationsUG, -1, "Create SettingApplicationsUG obj failed");

	memset(applicationsUG, 0x00, sizeof(SettingApplicationsUG));

	ops->create = setting_applications_ug_on_create;
	ops->start = setting_applications_ug_on_start;
	ops->pause = setting_applications_ug_on_pause;
	ops->resume = setting_applications_ug_on_resume;
	ops->destroy = setting_applications_ug_on_destroy;
	ops->message = setting_applications_ug_on_message;
	ops->event = setting_applications_ug_on_event;
	ops->key_event = setting_applications_ug_on_key_event;
	ops->priv = applicationsUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingApplicationsUG *applicationsUG;
	setting_retm_if(!ops, "ops == NULL");

	applicationsUG = ops->priv;
	if (applicationsUG) {
		FREE(applicationsUG);
	}
}
