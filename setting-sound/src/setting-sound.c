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

#include "setting-sound.h"

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void *setting_sound_ug_on_create(ui_gadget_h ug,
                                                  enum ug_mode mode, app_control_h service,
                                                  void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");

	SettingSoundUG *soundUG = priv;
	soundUG->ug = ug;
	soundUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	soundUG->win_get = (Evas_Object *) ug_get_window();

	soundUG->evas = evas_object_evas_get(soundUG->win_main_layout);

	setting_retvm_if(soundUG->win_main_layout == NULL, NULL,
	                 "cannot get main window ");
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_sound_main, NULL);

	/*  creating a view. */
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(soundUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(soundUG->itc_1text));

	setting_view_node_set_cur_view(&setting_view_sound_main);
	setting_view_create(&setting_view_sound_main, (void *)soundUG);

	return soundUG->ly_main;
}

static void setting_sound_ug_on_start(ui_gadget_h ug, app_control_h service,
                                                void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_sound_ug_on_pause(ui_gadget_h ug, app_control_h service,
                                                void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_sound_ug_on_resume(ui_gadget_h ug, app_control_h service,
                                                 void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");

	SettingSoundUG *soundUG = priv;
	setting_view_create(&setting_view_sound_main, (void *)soundUG);

	SETTING_TRACE_END;
}

static void setting_sound_ug_on_destroy(ui_gadget_h ug, app_control_h service,
                                                  void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingSoundUG *soundUG = priv;


	/*  called when this shared gadget is terminated. similar with app_exit */
    setting_view_destroy(&setting_view_sound_main, soundUG);

	SETTING_TRACE_END;
}

static void setting_sound_ug_on_message(ui_gadget_h ug, app_control_h msg,
                                                  app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_sound_ug_on_event(ui_gadget_h ug,
                                                enum ug_event event, app_control_h service,
                                                void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_sound_ug_on_key_event(ui_gadget_h ug,
                                                    enum ug_key_event event,
                                                    app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingSoundUG *soundUG = calloc(1, sizeof(SettingSoundUG));
	setting_retvm_if(!soundUG, -1, "Create SettingSoundUG obj failed");

	memset(soundUG, 0x00, sizeof(SettingSoundUG));

	ops->create = setting_sound_ug_on_create;
	ops->start = setting_sound_ug_on_start;
	ops->pause = setting_sound_ug_on_pause;
	ops->resume = setting_sound_ug_on_resume;
	ops->destroy = setting_sound_ug_on_destroy;
	ops->message = setting_sound_ug_on_message;
	ops->event = setting_sound_ug_on_event;
	ops->key_event = setting_sound_ug_on_key_event;
	ops->priv = soundUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingSoundUG *soundUG;
	setting_retm_if(!ops, "ops == NULL");

	soundUG = ops->priv;
	if (soundUG) {
		FREE(soundUG);
	}
}
