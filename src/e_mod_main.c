#include "e_mod_main.h"

////////////////////////////////////////////////////////////////////////////
// module "global tracking" stuff for this module
static E_Module *_module = NULL; // a handle to this module given to us

////////////////////////////////////////////////////////////////////////////
// the only actual content we have - handling a left mouse click by count++
// per gadget instance data
typedef struct {
   E_Gadcon_Client *gcc;
   E_Gadcon_Popup  *popup;
   Evas_Object     *popup_label;
   int count; // dummy data - repce with whatever data you need
} Instance;

// store private data for all the instances of the gadget - may be more than 1
static Eina_List *_instances = NULL;

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *gad_config = NULL;

static void
_popup_free(Instance *inst)
{
   E_FREE_FUNC(inst->popup, e_object_del);
}

static void
_cb_popup_del(void *obj)
{
   _popup_free(e_object_data_get(obj));
}

static void
_cb_popup_comp_del(void *data, Evas_Object *obj)
{
   _popup_free(data);
}

static Eina_Bool
_cb_key_down(void *data, Ecore_Event_Key *ev)
{
   if (!strcmp(ev->key, "Escape")) _popup_free(data);
   return ECORE_CALLBACK_PASS_ON;
}



static void
_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event)
{
   Instance *inst = data;
   Evas_Event_Mouse_Down *ev = event;
   char buf[64];
   
   E_Menu *m;
   E_Menu_Item *mi;
   int cx, cy;

   if ((ev->button == 1)) // left mouse
     {
        if (!inst->popup)
          {  // we dont have popup content - create it and track deletion
             inst->popup = e_gadcon_popup_new(inst->gcc, 0);
             // put an elm label in it - the compositor canvas is our win
             inst->popup_label = elm_label_add(e_comp->elm);
             elm_object_text_set(inst->popup_label, "XXXXXXXXXX");
             e_gadcon_popup_content_set(inst->popup, inst->popup_label);
             e_gadcon_popup_show(inst->popup);
             // this means click outisde popup and it dismisses. without
             // this it does not.
             e_comp_object_util_autoclose(inst->popup->comp_object,
                                          _cb_popup_comp_del,
                                          _cb_key_down, inst);
             // track deletion of the gadcon popup
             e_object_data_set(E_OBJECT(inst->popup), inst);
             E_OBJECT_DEL_SET(inst->popup, _cb_popup_del);
          }
        // increase  instance count
        inst->count++;
        // update label to display it
        snprintf(buf, sizeof(buf), "Click: %i", inst->count);
        elm_object_text_set(inst->popup_label, buf);
        // ensure smart objects update
        evas_smart_objects_calculate(e);
        e_gadcon_popup_content_set(inst->popup, inst->popup_label);
     }
      else if (ev->button == 3)
     {

         m = e_menu_new();
         mi = e_menu_item_new(m);
         e_menu_item_label_set(mi, "Settings");
         e_util_menu_item_theme_icon_set(mi, "configure");
         e_menu_item_callback_set(mi, _config_gad_module, inst);

         m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);

         e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
                                          &cx, &cy, NULL, NULL);
         e_menu_activate_mouse(m,
                              e_zone_current_get(),
                              cx + ev->output.x, cy + ev->output.y, 1, 1,
                              E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
         evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

////////////////////////////////////////////////////////////////////////////
// gadget controller class - delcare
static E_Gadcon_Client *_gc_init     (E_Gadcon *gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown (E_Gadcon_Client *gcc);
static void             _gc_orient   (E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static const char      *_gc_label    (const E_Gadcon_Client_Class *client_class);
static Evas_Object     *_gc_icon     (const E_Gadcon_Client_Class *client_class, Evas *evas);
static const char      *_gc_id_new   (const E_Gadcon_Client_Class *client_class);

static const E_Gadcon_Client_Class _gc_class =
{
   GADCON_CLIENT_CLASS_VERSION, "gad",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL, NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{  // gadcon is requesting a new instance of your gadgets
   Instance *inst;
   E_Gadcon_Client *gcc;
   Evas_Object *o;

   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;

   inst->count = 99; // dummy data for demo

   // create a new gadget object - edje here. you could just create an evas
   // rectangle or any other kind of object here...
   o = edje_object_add(gc->evas);
   // try find a theme overridden group first so theme gets first say
   if (!e_theme_edje_object_set(o, "base/theme/modules/gad", "modules/gad/main"))
     {  // fallback - going to cheat for now and use the icon object as the
        // gadge but normsally you'd provide another edj file with a fallback
        // design
        char buf[4096];
        snprintf(buf, sizeof(buf), "%s/e-module-gad.edj", e_module_dir_get(_module));
        edje_object_file_set(o, buf, "icon");
     }
   // create the new client
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   _instances = eina_list_append(_instances, inst);

   // attach standard right mouse menu
   e_gadcon_client_util_menu_attach(gcc);
   // to demo/test - listen for mouse presses
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _cb_mouse_down, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{  // shutdown the gadget - free out instance data then
   Instance *inst = gcc->data;
//    _popup_free(inst);
   
   
   gad_config->instances = eina_list_remove(gad_config->instances, inst);
   _instances = eina_list_remove(_instances, inst);
   E_FREE(inst);

}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{  // take input orientation given for the gadget and set an aspect and size
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static const char *
_gc_label(const E_Gadcon_Client_Class *client_class)
{  // return a label to be used in config dialogs selecting gadgets
   return "Gad";
}

static Evas_Object *
_gc_icon(const E_Gadcon_Client_Class *client_class, Evas *evas)
{  // return an icon for config dialogs selecting gadgets
   Evas_Object *o;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-gad.edj", e_module_dir_get(_module));
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(const E_Gadcon_Client_Class *client_class)
{ // return a string "id" for this gadget to use in config like shelf to
   // identify this gadget - make it unique for this gadget
   static char buf[4096];

   snprintf(buf, sizeof(buf), "%s.%d", client_class->name,
            eina_list_count(gad_config->instances) + 1);
   return buf;
   
//    return _gc_class.name;
}


static Config_Item *
_gad_config_item_get(const char *id)
{
   Config_Item *ci;

   GADCON_CLIENT_CONFIG_GET(Config_Item, gad_config->items, _gc_class, id);

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->enable_switch = 1;
   gad_config->items = eina_list_append(gad_config->items, ci);
   return ci;
}


////////////////////////////////////////////////////////////////////////////
// core api for all modules if they have gadgets or not - e calls these
// api's or uses the structures to find out core module info. rememebr
// a module can extend e in any way. a module can provide 1 0 or more gadgets
// too...
E_API E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION, "Gad" // we must have a name for this module
};

E_API void *
e_modapi_init(E_Module *m)
{  // called when e loads this module
///
   conf_item_edd = E_CONFIG_DD_NEW("Gad_Config_Item", Config_Item);
   #undef T
   #undef D
   #define T Config_Item
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, enable_switch, INT);

   conf_edd = E_CONFIG_DD_NEW("Gad_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   gad_config = e_config_domain_load("module.gad", conf_edd);
   if (!gad_config)
     {
        Config_Item *ci;

        gad_config = E_NEW(Config, 1);

        ci = E_NEW(Config_Item, 1);
        ci->id = eina_stringshare_add("gad.1");
        ci->enable_switch = 0;
        gad_config->items = eina_list_append(gad_config->items, ci);
     }

   gad_config->module = m;

   ///
   _module = m; // store module handle that e passes in to use later
   e_gadcon_provider_register(&_gc_class); // register our gadcon class
   return m;
}

E_API int
e_modapi_shutdown(E_Module *m)
{  // called when e unloads this modue - clean up everything we created
   

   Config_Item *ci;
   e_gadcon_provider_unregister(&_gc_class);

   E_FREE_LIST(gad_config->handlers, ecore_event_handler_del);

   while (gad_config->config_dialog)
     /* there is no need to eves_list_remove_list. It is done implicitly in
      * dialog _free_data function
      */
     e_object_del(E_OBJECT(gad_config->config_dialog->data));

   EINA_LIST_FREE(gad_config->items, ci)
     {
        eina_stringshare_del(ci->id);
        free(ci);
     }

   E_FREE(gad_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

E_API int
e_modapi_save(E_Module *m)
{  // called when e wants this module to save its config - no config to save
   e_config_domain_save("module.gad", conf_edd, gad_config);
   return 1;
}
