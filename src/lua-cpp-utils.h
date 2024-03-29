#ifndef LUA_CPP_UTILS_H
#define LUA_CPP_UTILS_H

#include <new>
#include <utility>

#include "defs.h"
#include "lua-defs.h"
__BEGIN_DECLS
#include "lua.h"
__END_DECLS

#include "gs-types.h"

namespace gslshell {

class ret_status {
    bool m_success;
    const char *m_error_msg;
    const char *m_context;

public:
    ret_status() : m_success(true) {};

    void success() {
        m_success = true;
    }
    void error(const char *msg, const char *ctx)
    {
        m_success = false;
        m_error_msg = msg;
        m_context = ctx;
    };

    const char *error_msg() const {
        return (m_success ? 0 : m_error_msg);
    };
    const char *context()   const {
        return (m_success ? 0 : m_context);
    };
};
}

inline void* operator new(size_t nbytes, lua_State *L, enum gs_type_e tp)
{
    void* p = lua_newuserdata(L, nbytes);
    gs_set_metatable (L, tp);
    return p;
}

#ifdef GRAPH_TK_USE_LUA54
inline void* operator new(size_t nbytes, lua_State *L, enum gs_type_e tp, int nuvalue)
{
    void* p = lua_newuserdatauv(L, nbytes, nuvalue);
    gs_set_metatable (L, tp);
    return p;
}
#endif

template <class T, class... Args>
T* push_new_object(lua_State *L, enum gs_type_e tp, Args&&... args) {
    return new(L, tp) T(std::forward<Args>(args)...);
}

#ifdef GRAPH_TK_USE_LUA54
template <class T, class... Args>
T* push_new_object_uv(lua_State *L, enum gs_type_e tp, int nuvalue, Args&&... args) {
    return new(L, tp, nuvalue) T(std::forward<Args>(args)...);
}
#else
template <class T, class... Args>
T* push_new_object_uv(lua_State *L, enum gs_type_e tp, int nuvalue, Args&&... args) {
    // We ignore the nuvalue argument when using Lua 5.1 because later lua_setfenv
    // will be used and no upvalues have to be requested when the object is created.
    return new(L, tp) T(std::forward<Args>(args)...);
}
#endif

template <class T>
int object_free (lua_State *L, int index, enum gs_type_e tp)
{
    T *obj = (T *) gs_check_userdata (L, index, tp);
    obj->~T();
    return 0;
}

template <class T>
T* object_check (lua_State *L, int index, enum gs_type_e tp)
{
    return (T *) gs_check_userdata (L, index, tp);
}

template <class T>
T* object_cast (lua_State *L, int index, enum gs_type_e tp)
{
    return (T *) gs_is_userdata (L, index, tp);
}

#endif
