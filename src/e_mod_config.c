#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   int   enable_switch;
   struct
     {
	Evas_Object *o_desk_show_all;
	Evas_Object *o_desk_show_active;
     } gui;
};

Eina_List *show_label_list = NULL;

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _cb_disable_check_list(void *data, Evas_Object *obj);

void
_config_gad_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];

   v = E_NEW(E_Config_Dialog_View, 1);

   /* Dialog Methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = NULL;
   v->advanced.create_widgets = NULL;

   /* Create The Dialog */
   snprintf(buf, sizeof(buf), "%s/e-module-gad.edj", e_module_dir_get(gad_config->module));
   cfd = e_config_dialog_new(NULL,
			     "Gad Module Settings",
			     "E", "_e_mod_gad_config_dialog",
			     buf, 0, v, ci);
   gad_config->config_dialog = eina_list_append(gad_config->config_dialog, cfd);
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   cfdata->enable_switch = ci->enable_switch;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
//    show_label_list = eina_list_free(show_label_list);

   gad_config->config_dialog = eina_list_remove(gad_config->config_dialog, cfd);
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd EINA_UNUSED, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   E_Radio_Group *rg;
   Evas_Object *o, *of, *ob;
   Evas_Object *show_check = NULL;
   int zone_count = 0;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, "General Settings", 0);

   ob = e_widget_check_add(evas, "Enable", &(cfdata->enable_switch));
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   // handler for enable/disable widget array
//    e_widget_on_change_hook_set(show_check, _cb_disable_check_list, show_label_list);

 

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;

   ci = cfd->data;
   ci->enable_switch = cfdata->enable_switch;

//    _wp_slideshow_update(ci);
   e_config_save_queue();
   return 1;
}


/****** callbacks **********/

// static void
// _cb_zone_policy_change(void *data, Evas_Object *obj EINA_UNUSED)
// {
//    E_Config_Dialog_Data *cfdata;
// 
//    cfdata = data;
// 
//    if (cfdata->zone_policy == 0)
//      {
// 	e_widget_disabled_set(cfdata->gui.o_desk_show_all, 1);
// 	e_widget_disabled_set(cfdata->gui.o_desk_show_active, 1);
//      }
//    else
//      {
// 	e_widget_disabled_set(cfdata->gui.o_desk_show_all, 0);
// 	e_widget_disabled_set(cfdata->gui.o_desk_show_active, 0);
//      }
// }

/*!
 * @param data A Evas_Object to chain together with the checkbox
 * @param obj A Evas_Object checkbox created with e_widget_check_add()
 */

/*!
 * @param data A Eina_List of Evas_Object to chain widgets together with the checkbox
 * @param obj A Evas_Object checkbox created with e_widget_check_add()
 */
// static void
// _cb_disable_check_list(void *data, Evas_Object *obj)
// {
//    Eina_List *list = (Eina_List*) data;
//    Eina_List *l;
//    Evas_Object *o;
// 
//    EINA_LIST_FOREACH(list, l, o)
//       e_widget_disabled_set(o, !e_widget_check_checked_get(obj));
// }
