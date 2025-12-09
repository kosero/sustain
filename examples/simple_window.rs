use sustain::prelude::*;

struct Game1 {}

impl Game1 {
    fn new() -> Self {
        Self {}
    }
}

impl Game for Game1 {
    fn init(&mut self) {
        println!("Hello, world!");
    }
}

fn main() {
    let mut window = Window::new(Vector2::new(800.0, 600.0), "simple window", true, false);

    let game1 = Game1::new();

    window.run(game1);
}
