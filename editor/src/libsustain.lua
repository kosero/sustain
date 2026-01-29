local ffi = require("ffi")

ffi.cdef[[
    typedef struct SDL_Renderer SDL_Renderer;

    typedef struct SDL_Event {
        uint8_t data[128];
    } SDL_Event;

    int SN_Init_Window(const int width, const int height, const char *title, bool vsync);
    void SN_Window_Close(void);
    void SN_Clear_Background(void);
    SDL_Renderer* SN_Get_Renderer(void);
    bool SDL_PollEvent(SDL_Event *event);
    void SDL_RenderPresent(SDL_Renderer *renderer);
]]

local path = debug.getinfo(1).source:match("@?(.*[\\/])") or ""
local lib = ffi.load(path .. "libsustain.so")

local M = {
    SDL_EVENT_QUIT = 0x100,
    new_event = function() return ffi.new("SDL_Event") end
}

setmetatable(M, { __index = lib })

return M
