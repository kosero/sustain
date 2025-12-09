use std::path::Path;
use sustain::prelude::*;

struct Game1 {
    sound_manager: SoundManager,
}

impl Game1 {
    fn new(sound_manager: SoundManager) -> Self {
        Self { sound_manager }
    }
}

impl Game for Game1 {
    fn input(&mut self, event: Event) {
        match event {
            Event::KeyDown {
                keycode: Some(Keycode::Space),
                ..
            } => {
                self.sound_manager.play(Path::new("res/bg.wav"), 0.001);
            }
            _ => {}
        }
    }

    fn update(&mut self, _delta: f32) {
        println!("{}", _delta);
    }
}

fn main() {
    let mut core = Core::new(
        Vector2::new(800.0, 600.0),
        "Sound Play Example".to_string(),
        true,
        false,
    );

    let sound = SoundManager::new(core.audio_subsystem.clone());
    let game = Game1::new(sound);

    core.run(game);
}
