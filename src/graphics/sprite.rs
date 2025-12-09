use crate::ecs::{components::Component, entity::Transform};
use raylib::prelude::*;
use std::any::Any;

pub struct Sprite {
    pub texture: Texture2D,
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
    pub fn new(texture: Texture2D) -> Self {
        let origin = Vector2::new(texture.width as f32 / 2.0, texture.height as f32 / 2.0);
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

    pub fn render(&self, d: &mut RaylibDrawHandle) {
        let source = Rectangle::new(
            0.0,
            0.0,
            if self.flip_h {
                -self.texture.width as f32
            } else {
                self.texture.width as f32
            },
            if self.flip_v {
                -self.texture.height as f32
            } else {
                self.texture.height as f32
            },
        );

        let dest = Rectangle::new(
            self.position.x + self.offset.x,
            self.position.y + self.offset.y,
            self.texture.width as f32 * self.scale,
            self.texture.height as f32 * self.scale,
        );

        let origin = Vector2::new(self.origin.x * self.scale, self.origin.y * self.scale);
        let tint = Color::new(255, 255, 255, self.alpha);

        d.draw_texture_pro(&self.texture, source, dest, origin, self.rotation, tint);
    }
}

impl Component for Sprite {
    fn update(&mut self, _delta: f32) {}

    fn draw(&mut self, _d: &mut RaylibDrawHandle, parent: &Transform) {
        self.position = parent.position + self.offset;
        self.rotation = parent.rotation;
        self.scale = parent.scale;

        self.render(_d);
    }

    fn as_any(&mut self) -> &mut dyn Any {
        self
    }
}
