#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <e.h>


typedef struct _Config      Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
   /* just config state */
   E_Module        *module;
   Eina_List       *instances;
   Eina_List       *handlers;
   Eina_List       *items;
   Eina_List	   *config_dialog;
};

struct _Config_Item
{
   const char *id;
   int enable_switch;
   E_Gadcon_Client *gcc;
};

E_API extern E_Module_Api e_modapi;

// E_API void *e_modapi_init     (E_Module *m);
// E_API int   e_modapi_shutdown (E_Module *m);
// E_API int   e_modapi_save     (E_Module *m);

void _config_gad_module(Config_Item *ci);
extern Config *gad_config;

/**
 * @addtogroup Optional_Gadgets
 * @{
 *
 * @defgroup Module_Gad Gad (Skeleton Module)
 *
 * Skeleton Module to learn
 *
 * @}
 */
#endif
