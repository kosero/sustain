local sustain = require("libsustain")
local event = sustain.new_event()
local running = true

sustain.SN_Init_Window(800, 600, "Sustain Engine", true)
local renderer = sustain.SN_Get_Renderer()

while running do
    while sustain.SDL_PollEvent(event) do
        local type = require("ffi").cast("uint32_t*", event)[0]
        if type == sustain.SDL_EVENT_QUIT then
            running = false
        end
    end

    sustain.SN_Clear_Background()

    if renderer ~= nil then
        sustain.SDL_RenderPresent(renderer)
    end
end


