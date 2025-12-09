use crate::math::Vector2;
use sdl2::{
    event::Event, image::InitFlag, keyboard::Keycode, render::WindowCanvas, AudioSubsystem,
    EventPump,
};
use std::time::Instant;

pub struct Core {
    pub canvas: WindowCanvas,
    pub event_pump: EventPump,
    pub audio_subsystem: AudioSubsystem,
}

pub trait Game {
    fn init(&mut self) {}
    fn load_content(&mut self, _canvas: &mut WindowCanvas) {}
    fn input(&mut self, _event: Event) {}
    fn update(&mut self, _delta: f32) {}
    fn draw(&mut self, _canvas: &mut WindowCanvas, _delta: f32) {}
    fn exiting(&mut self) {}
}

impl Core {
    pub fn new(
        window_size: Vector2,
        window_title: String,
        mouse_visible: bool,
        fullscreen: bool,
    ) -> Self {
        let sdl_context = sdl2::init().unwrap();
        let _image_context = sdl2::image::init(InitFlag::PNG | InitFlag::JPG).unwrap();

        let video_subsystem = sdl_context.video().unwrap();
        let audio_subsystem = sdl_context.audio().unwrap();

        let mut builder =
            video_subsystem.window(&window_title, window_size.x as u32, window_size.y as u32);
        let window_builder = builder.position_centered();

        if fullscreen {
            window_builder.fullscreen_desktop();
        }

        let mouse = sdl_context.mouse();
        mouse.show_cursor(mouse_visible);

        let window = window_builder.build().unwrap();
        let mut canvas = window.into_canvas().accelerated().build().unwrap();

        canvas.clear();
        canvas.present();

        let event_pump = sdl_context.event_pump().unwrap();

        Self {
            canvas,
            event_pump,
            audio_subsystem,
        }
    }

    pub fn run<G: Game>(&mut self, mut game: G) {
        game.init();
        game.load_content(&mut self.canvas);

        let mut last_frame = Instant::now();

        'running: loop {
            let now = Instant::now();
            let delta = (now - last_frame).as_secs_f32();
            last_frame = now;

            while let Some(event) = self.event_pump.poll_event() {
                match event {
                    Event::Quit { .. }
                    | Event::KeyDown {
                        keycode: Some(Keycode::Escape),
                        ..
                    } => break 'running,
                    _ => {
                        game.input(event);
                    }
                }
            }

            game.update(delta);
            game.draw(&mut self.canvas, delta);
        }

        game.exiting();
    }
}
