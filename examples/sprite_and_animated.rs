use sustain::prelude::*;

struct Game1 {
    sprite: Option<Entity>,
    anim: Option<Entity>,
}

impl Game1 {
    fn new() -> Self {
        Self {
            sprite: None,
            anim: None,
        }
    }
}

impl Game for Game1 {
    fn load_content(&mut self, _rl: &mut RaylibHandle, _thread: &mut RaylibThread) {
        let yoru_texture = _rl.load_texture(_thread, "res/yoru.png").unwrap();
        let lumball_texture = _rl.load_texture(_thread, "res/lumball.png").unwrap();

        let mut yoru = Entity::new();
        let mut lumball = Entity::new();

        let mut anim = AnimatedSprite::new(lumball_texture, 16, 16);
        anim.add_animation("default", 0, 3, 1., true);
        anim.play("default");

        lumball.add_component(Box::new(anim));
        yoru.add_component(Box::new(Sprite::new(yoru_texture)));

        lumball.transform.position = Vector2::new(300.0, 300.0);
        lumball.transform.scale = 8.;

        yoru.transform.position = Vector2::new(550.0, 300.0);
        yoru.transform.scale = 8.;

        self.sprite = Some(yoru);
        self.anim = Some(lumball);
    }

    fn update(&mut self, _delta: f32) {
        if let Some(anim) = &mut self.anim {
            anim.update(_delta);
        }
    }

    fn draw(&mut self, _d: &mut RaylibDrawHandle) {
        _d.clear_background(Color::GRAY);

        if let Some(sprite) = &mut self.sprite {
            sprite.draw(_d)
        }

        if let Some(anim) = &mut self.anim {
            anim.draw(_d)
        }
    }
}

fn main() {
    let mut window = Window::new(
        Vector2::new(800.0, 600.0),
        "Sprite and animated example",
        true,
        false,
    );

    let game = Game1::new();

    window.run(game);
}
