use std::env;
use sustain::prelude::*;

struct GameObject {
    entity: Entity,
    body: PhysicsBody,
}

struct Game1 {
    objects: Vec<GameObject>,
}

impl Game1 {
    fn new() -> Self {
        Self {
            objects: Vec::new(),
        }
    }
}

impl Game for Game1 {
    fn init(&mut self) {}

    fn load_content(&mut self, canvas: &mut WindowCanvas) {
        let cwd = env::current_dir().unwrap();
        let texture_path = cwd.join("res").join("yoru.png");
        let texture_file = texture_path.to_str().unwrap();

        let mut spawn_dynamic = |pos: Vector2, vel: Vector2, w: f32, h: f32| {
            let texture = load_texture(canvas, texture_file);

            let mut e = Entity::new();
            let sprite = Sprite::new(texture);
            e.transform.position = pos;
            e.transform.scale = 8.0;
            e.add_component(Box::new(sprite));

            GameObject {
                entity: e,
                body: PhysicsBody::new_dynamic(w, h, vel),
            }
        };

        self.objects.push(spawn_dynamic(
            Vector2::new(200., 300.),
            Vector2::new(120., 80.),
            16.0,
            16.0,
        ));

        self.objects.push(spawn_dynamic(
            Vector2::new(600., 300.),
            Vector2::new(-140., -60.),
            16.0,
            16.0,
        ));
    }

    fn update(&mut self, delta: f32) {
        let width = 800.0;
        let height = 600.0;

        let mut physics_refs: Vec<(&mut Entity, &mut PhysicsBody)> = self
            .objects
            .iter_mut()
            .map(|o| (&mut o.entity, &mut o.body))
            .collect();

        physics_step(&mut physics_refs[..], delta);

        for obj in &mut self.objects {
            let pos = &mut obj.entity.transform.position;
            let scale = obj.entity.transform.scale;

            let half_w = obj.body.base_size.x * scale * 0.5;
            let half_h = obj.body.base_size.y * scale * 0.5;

            if pos.x - half_w < 0.0 {
                pos.x = half_w;
                obj.body.velocity.x = -obj.body.velocity.x;
            }
            if pos.x + half_w > width {
                pos.x = width - half_w;
                obj.body.velocity.x = -obj.body.velocity.x;
            }

            if pos.y - half_h < 0.0 {
                pos.y = half_h;
                obj.body.velocity.y = -obj.body.velocity.y;
            }
            if pos.y + half_h > height {
                pos.y = height - half_h;
                obj.body.velocity.y = -obj.body.velocity.y;
            }
        }
    }

    fn draw(&mut self, _canvas: &mut WindowCanvas, _delta: f32) {
        _canvas.set_draw_color(Color::RGB(40, 44, 49));
        _canvas.clear();

        for obj in &mut self.objects {
            obj.entity.draw(_canvas);
        }

        _canvas.present();
    }

    fn exiting(&mut self) {}
}

fn main() {
    let mut core = Core::new(
        Vector2::new(800.0, 600.0),
        "Physics Example".to_string(),
        true,
        false,
    );

    let game = Game1::new();
    core.run(game);
}
