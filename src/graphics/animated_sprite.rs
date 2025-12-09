use std::any::Any;
use std::collections::HashMap;
use std::time::Duration;

use crate::ecs::components::Component;
use crate::ecs::entity::Transform;
use raylib::{
    color::Color,
    drawing::{RaylibDraw, RaylibDrawHandle},
    math::{Rectangle, Vector2},
    texture::Texture2D,
};

#[derive(Clone)]
pub struct Animation {
    pub start_frame: usize,
    pub end_frame: usize,
    pub fps: f32,
    pub looping: bool,
    pub frame_duration: Duration,
}

impl Animation {
    pub fn new(start_frame: usize, end_frame: usize, fps: f32, looping: bool) -> Animation {
        Self {
            start_frame,
            end_frame,
            fps,
            looping,
            frame_duration: Duration::from_secs_f32(1.0 / fps),
        }
    }
}

pub struct AnimatedSprite {
    pub texture: Texture2D,
    pub scale: f32,
    pub rotation: f32,
    pub origin: Vector2,
    pub offset: Vector2,
    pub position: Vector2,
    pub flip_v: bool,
    pub flip_h: bool,
    pub alpha: u8,

    pub frame_width: u32,
    pub frame_height: u32,
    pub frames_per_row: u32,

    animations: HashMap<String, Animation>,
    current_animation: Option<String>,
    current_frame: usize,
    accumulated: Duration,
}

impl AnimatedSprite {
    pub fn new(texture: Texture2D, frame_width: u32, frame_height: u32) -> Self {
        let sheet_width = texture.width as u32;
        let frames_per_row = sheet_width / frame_width;
        let origin = Vector2::new(frame_width as f32 / 2.0, frame_height as f32 / 2.0);

        Self {
            texture,
            scale: 1.0,
            rotation: 0.0,
            origin,
            offset: Vector2::zero(),
            position: Vector2::zero(),
            flip_h: false,
            flip_v: false,
            alpha: 255,
            frame_width,
            frame_height,
            frames_per_row,
            animations: HashMap::new(),
            current_animation: None,
            current_frame: 0,
            accumulated: Duration::ZERO,
        }
    }

    pub fn add_animation(&mut self, name: &str, start: usize, end: usize, fps: f32, looping: bool) {
        self.animations
            .insert(name.to_string(), Animation::new(start, end, fps, looping));
    }

    pub fn play(&mut self, name: &str) {
        if self.current_animation.as_deref() != Some(name) {
            if let Some(anim) = self.animations.get(name) {
                self.current_animation = Some(name.to_string());
                self.current_frame = anim.start_frame;
                self.accumulated = Duration::ZERO;
            }
        }
    }

    pub fn update(&mut self, delta: f32) {
        if let Some(name) = &self.current_animation {
            if let Some(anim) = self.animations.get(name) {
                self.accumulated += Duration::from_secs_f32(delta);

                while self.accumulated >= anim.frame_duration {
                    self.accumulated -= anim.frame_duration;
                    self.current_frame += 1;

                    if self.current_frame > anim.end_frame {
                        if anim.looping {
                            self.current_frame = anim.start_frame;
                        } else {
                            self.current_frame = anim.end_frame;
                        }
                    }
                }
            }
        }
    }

    pub fn render(&self, d: &mut RaylibDrawHandle) {
        let src_x = (self.current_frame as u32 % self.frames_per_row) * self.frame_width;
        let src_y = (self.current_frame as u32 / self.frames_per_row) * self.frame_height;

        let source = Rectangle::new(
            src_x as f32,
            src_y as f32,
            if self.flip_h {
                -(self.frame_width as f32)
            } else {
                self.frame_width as f32
            },
            if self.flip_v {
                -(self.frame_height as f32)
            } else {
                self.frame_height as f32
            },
        );

        let scaled_width = self.frame_width as f32 * self.scale;
        let scaled_height = self.frame_height as f32 * self.scale;

        let dest = Rectangle::new(
            self.position.x - self.origin.x + self.offset.x,
            self.position.y - self.origin.y + self.offset.y,
            scaled_width,
            scaled_height,
        );

        let origin = Vector2::new(self.origin.x * self.scale, self.origin.y * self.scale);

        let tint = Color::new(255, 255, 255, self.alpha);

        d.draw_texture_pro(&self.texture, source, dest, origin, self.rotation, tint);
    }
}

impl Component for AnimatedSprite {
    fn update(&mut self, _delta: f32) {
        self.update(_delta);
    }

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
