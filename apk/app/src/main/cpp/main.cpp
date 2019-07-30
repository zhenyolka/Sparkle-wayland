#include <cstdio>
#include <cstring>

extern "C"
{
#include <lauxlib.h>
#include <lualib.h>
#include <lua.h>
}

static void create_arg_table(lua_State *L, int argc, char **argv)
{
    int narg = argc - 1;  /* number of positive indices */
    lua_createtable(L, narg, 1);

    for (int i = 0; i < argc; i++)
    {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i);
    }

    lua_setglobal(L, "arg");
}

int command_lua(int argc, char *argv[])
{
    int status;

    if (argc < 1)
        return 1;

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    status = luaL_loadfile(L, argv[0]);
    if (status)
    {
        fprintf(stderr, "Failed to load file: %s\n", lua_tostring(L, -1));
        return 1;
    }

    create_arg_table(L, argc, argv);

    status = lua_pcall(L, 0, 0, 0);
    if (status)
    {
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
        return 1;
    }

    lua_close(L);

    return 0;
}

int main(int argc, char *argv[])
{
    fprintf(stdout, "Sparkle\n");

    if (argc < 2)
        return 1;

    const char *command = argv[1];

    if (strcmp(command, "lua") == 0)
        return command_lua(argc - 2, &argv[2]);
    else
        return 1;

    return 0;
}
