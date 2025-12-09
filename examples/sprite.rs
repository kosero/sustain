use std::env;
use sustain::prelude::*;

struct Game1 {
    sprite: Option<Entity>,
}

impl Game1 {
    fn new() -> Self {
        Self { sprite: None }
    }
}

impl Game for Game1 {
    fn load_content(&mut self, _canvas: &mut WindowCanvas) {
        let cwd = env::current_dir().unwrap();
        let texture_path = cwd.join("res").join("yoru.png");

        let texture = load_texture(_canvas, texture_path.to_str().unwrap());

        let mut entity = Entity::new();
        let image = Sprite::new(texture);
        entity.transform.scale = 8.;
        entity.transform.position = Vector2::new(400., 300.);

        entity.add_component(Box::new(image));
        self.sprite = Some(entity);
    }

    fn draw(&mut self, _canvas: &mut WindowCanvas, _delta: f32) {
        _canvas.set_draw_color(Color::RGB(40, 44, 49));
        _canvas.clear();

        if let Some(sprite) = &mut self.sprite {
            sprite.draw(_canvas);
        }

        _canvas.present();
    }
}

fn main() {
    let mut core = Core::new(
        Vector2::new(800.0, 600.0),
        "Sprite Example".to_string(),
        true,
        false,
    );

    let game = Game1::new();

    core.run(game);
}
