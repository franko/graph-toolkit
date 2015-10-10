#ifndef WINDOW_LUA_H
#define WINDOW_LUA_H

#include <pthread.h>

#include "defs.h"

__BEGIN_DECLS

#include "lua.h"

extern int  window_slot_update             (lua_State *L);
extern int  window_slot_refresh            (lua_State *L);
extern int  window_save_slot_image         (lua_State *L);
extern int  window_restore_slot_image      (lua_State *L);
extern int  window_update                  (lua_State *L);
extern int  window_new                     (lua_State *L);
extern int  window_show                    (lua_State *L);
extern int  window_attach                  (lua_State *L);
extern int  window_close                   (lua_State *L);
extern int  window_close_wait              (lua_State *L);
extern int  window_wait                    (lua_State *L);

extern void natwin_register                (lua_State *L);

__END_DECLS

#endif
