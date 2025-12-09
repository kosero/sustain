use crate::{
    ecs::{components::Component, entity::Transform},
    math::Vector2,
};
use sdl2::{
    rect::Rect,
    render::{Texture, WindowCanvas},
};
use std::any::Any;

pub struct Sprite {
    pub texture: Texture,
    pub scale: f32,
    pub rotation: f32,
    pub origin: Vector2,
    pub offset: Vector2,
    pub position: Vector2,
    pub flip_h: bool,
    pub flip_v: bool,
    pub alpha: u8,
}

impl Sprite {
    pub fn new(texture: Texture) -> Self {
        let query = texture.query();
        let origin = Vector2::new(query.width as f32 / 2.0, query.height as f32 / 2.0);

        Self {
            texture,
            scale: 1.0,
            rotation: 0.0,
            origin,
            offset: Vector2::new(0.0, 0.0),
            position: Vector2::new(0.0, 0.0),
            flip_v: false,
            flip_h: false,
            alpha: 255,
        }
    }

    pub fn draw(&mut self, canvas: &mut WindowCanvas) {
        let query = self.texture.query();
        let scaled_width = query.width as f32 * self.scale;
        let scaled_height = query.height as f32 * self.scale;

        let dst_x = (self.position.x - scaled_width / 2.0 + self.offset.x) as i32;
        let dst_y = (self.position.y - scaled_height / 2.0 + self.offset.y) as i32;

        let dst = Rect::new(dst_x, dst_y, scaled_width as u32, scaled_height as u32);

        let rotation_center_x = (self.origin.x * self.scale) as i32;
        let rotation_center_y = (self.origin.y * self.scale) as i32;

        let tex = &mut self.texture;
        tex.set_alpha_mod(self.alpha);

        canvas
            .copy_ex(
                tex,
                None,
                Some(dst),
                self.rotation as f64,
                Some(sdl2::rect::Point::new(rotation_center_x, rotation_center_y)),
                self.flip_h,
                self.flip_v,
            )
            .expect("[ERR(SPRITE)]: Failed to draw sprite");
    }
}

impl Component for Sprite {
    fn update(&mut self, _delta: f32) {}

    fn draw(&mut self, canvas: &mut WindowCanvas, parent: &Transform) {
        self.position = parent.position + self.offset;
        self.rotation = parent.rotation;
        self.scale = parent.scale;

        self.draw(canvas);
    }

    fn as_any(&mut self) -> &mut dyn Any {
        self
    }
}
