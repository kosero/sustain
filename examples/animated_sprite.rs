use std::env;
use sustain::prelude::*;

struct Game1 {
    animated_sprite: Option<Entity>,
}

impl Game1 {
    fn new() -> Self {
        Self {
            animated_sprite: None,
        }
    }
}

impl Game for Game1 {
    fn load_content(&mut self, _canvas: &mut WindowCanvas) {
        let cwd = env::current_dir().unwrap();
        let texture_path = cwd.join("res").join("lumball.png");

        let texture = load_texture(_canvas, texture_path.to_str().unwrap());

        let mut entity = Entity::new();
        entity.transform.position = Vector2::new(350.0, 250.0);
        entity.transform.scale = 8.;

        let mut spritesheet = AnimatedSprite::new(texture, 16, 16);
        spritesheet.add_animation("default", 0, 3, 1., true);
        spritesheet.play("default");

        entity.add_component(Box::new(spritesheet));
        self.animated_sprite = Some(entity);
    }

    fn update(&mut self, _delta: f32) {
        if let Some(entity) = &mut self.animated_sprite {
            entity.update(_delta);
        }
    }

    fn draw(&mut self, _canvas: &mut WindowCanvas, _delta: f32) {
        _canvas.set_draw_color(Color::RGB(40, 44, 49));
        _canvas.clear();

        if let Some(entity) = &mut self.animated_sprite {
            entity.draw(_canvas);
        }

        _canvas.present();
    }
}

fn main() {
    let mut core = Core::new(
        Vector2::new(800.0, 600.0),
        "Animated Sprite Example".to_string(),
        true,
        false,
    );

    let game = Game1::new();

    core.run(game);
}
