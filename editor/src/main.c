#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SDL3/SDL.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    lua_State *L = luaL_newstate();
    if (!L) return 1;
    luaL_openlibs(L);

    const char *base = SDL_GetBasePath();

    if (base) {
        lua_getglobal(L, "package");
        lua_getfield(L, -1, "path");

        lua_pushfstring(L, "%s;%s?.lua", lua_tostring(L, -1), base);
        lua_setfield(L, -3, "path");
        lua_pop(L, 2);

        char script[2048];
        snprintf(script, sizeof(script), "%seditor.lua", base);

        if (luaL_dofile(L, script)) {
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
        }
    }

    lua_close(L);
    SN_Window_Close();

    return 0;
}
