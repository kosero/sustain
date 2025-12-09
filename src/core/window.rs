use raylib::texture::Texture2D;
use raylib::{
    drawing::RaylibDrawHandle,
    {RaylibHandle, RaylibThread, math::Vector2},
};

pub struct Window {
    pub rl: RaylibHandle,
    thread: RaylibThread,
}

pub trait Game {
    fn init(&mut self) {}
    fn load_content(&mut self, _rl: &mut RaylibHandle, _thread: &mut RaylibThread) {}
    fn update(&mut self, _delta: f32) {}
    fn draw(&mut self, _d: &mut RaylibDrawHandle) {}
    fn exiting(&mut self) {}
}

impl Window {
    pub fn new(
        window_size: Vector2,
        window_title: &str,
        mouse_visible: bool,
        fullscreen: bool,
    ) -> Self {
        let (mut rl, thread) = raylib::init()
            .size(window_size.x as i32, window_size.y as i32)
            .title(window_title)
            .vsync()
            .resizable()
            .build();

        if fullscreen {
            rl.toggle_fullscreen();
        }

        if mouse_visible {
            rl.show_cursor()
        } else {
            rl.hide_cursor();
        }

        Self { rl, thread }
    }

    pub fn run<G: Game>(&mut self, mut game: G) {
        game.init();
        game.load_content(&mut self.rl, &mut self.thread);

        while !self.rl.window_should_close() {
            let delta = self.rl.get_frame_time();
            game.update(delta);
            let mut d = self.rl.begin_drawing(&self.thread);
            game.draw(&mut d);
        }

        game.exiting();
    }

    pub fn load_texture(&mut self, path: &str) -> Texture2D {
        self.rl
            .load_texture(&self.thread, path)
            .expect(&format!("Failed to load texture: {}", path))
    }
}
