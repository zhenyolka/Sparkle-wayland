#include "sparkle_settings.h"
#include "were_exception.h"

extern "C"
{
#include <lauxlib.h>
#include <lualib.h>
#include <lua.h>
}


sparkle_settings::~sparkle_settings()
{
}

sparkle_settings::sparkle_settings(const std::string &file) :
    file_(file)
{
    load();
}

static void lua_stack(lua_State *L)
{
    int i;
    int top = lua_gettop(L);

    fprintf(stdout, "STACK: ");
    for (i = 1; i <= top; i++) /* repeat for each level */
    {
        int t = lua_type(L, i);
        switch (t)
        {
            case LUA_TSTRING:  /* strings */
                fprintf(stdout, "`%s'", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:  /* booleans */
                fprintf(stdout, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  /* numbers */
                fprintf(stdout, "%g", lua_tonumber(L, i));
                break;
            default:  /* other values */
                fprintf(stdout, "%s", lua_typename(L, t));
                break;
        }
        fprintf(stdout, "  ");  /* put a separator */
    }
    fprintf(stdout, "\n");  /* end the listing */
}

void sparkle_settings::load()
{
    int status;


    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    status = luaL_loadfile(L, file_.c_str());
    if (status)
    {
        fprintf(stdout, "Failed to load script: %s\n", lua_tostring(L, -1));
        goto finish;
    }

#if 1 /* Sandbox */
    lua_newtable(L);

    //lua_pushstring(L, "print");
    //lua_getglobal(L, "print");
    //lua_settable(L, -3);

    lua_setglobal(L, "sandbox");
    lua_getglobal(L, "sandbox");

    lua_setupvalue(L, -2, 1);
#endif

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        goto finish;
    }

    //lua_stack(L);

    lua_getglobal(L, "sandbox");
    if (!lua_istable(L, -1))
        goto finish;

    lua_pushstring(L, "sparkle");
    lua_gettable(L, -2);
    if (!lua_istable(L, -1))
        goto finish;

    lua_pushnil(L);  /* first key */

    while (lua_next(L, -2) != 0)
    {
        int key_type = lua_type(L, -2);
        int value_type = lua_type(L, -1);

        if (key_type == LUA_TSTRING)
        {
            const char *key = lua_tostring(L, -2);

            if (value_type == LUA_TSTRING)
            {
                const char *value = lua_tostring(L, -1);
                fprintf(stdout, "%s = %s\n", key, value);
                settings_.insert(std::make_pair(std::string(key), were_variant(1)));
            }
            else if (value_type == LUA_TBOOLEAN)
            {
                bool value = lua_toboolean(L, -1);
                fprintf(stdout, "%s = %d\n", key, value);
                settings_.insert(std::make_pair(std::string(key), were_variant(value)));
            }
            else if (value_type == LUA_TNUMBER)
            {
                double value = lua_tonumber(L, -1);
                fprintf(stdout, "%s = %f\n", key, value);
                settings_.insert(std::make_pair(std::string(key), were_variant(value)));
            }
            else
            {
                fprintf(stdout, "value type: %s\n", lua_typename(L, value_type));
            }
        }
        else
        {
            fprintf(stdout, "key type: %s\n", lua_typename(L, key_type));
        }

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
    lua_pop(L, 1);

    //lua_stack(L);

finish:
    lua_close(L);
}

std::string sparkle_settings::get_string(const std::string &key, const std::string &default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<std::string>();
}

bool sparkle_settings::get_bool(const std::string &key, bool default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<bool>();
}

int sparkle_settings::get_int(const std::string &key, int default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<double>();
}

double sparkle_settings::get_float(const std::string &key, double default_value)
{
    auto it = settings_.find(key);
    if (it == settings_.end())
        return default_value;

    return it->second.get<double>();
}
