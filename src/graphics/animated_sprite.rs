use crate::{
    ecs::{components::Component, entity::Transform},
    math::Vector2,
};
use sdl2::{
    rect::{Point, Rect},
    render::{Texture, WindowCanvas},
};
use std::any::Any;
use std::collections::HashMap;
use std::time::Duration;

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
    pub texture: Texture,
    pub scale: f32,
    pub rotation: f32,
    pub origin: Vector2,
    pub offset: Vector2,
    pub position: Vector2,
    pub flip_v: bool,
    pub flip_h: bool,

    pub frame_width: u32,
    pub frame_height: u32,
    pub frames_per_row: u32,

    animations: HashMap<String, Animation>,
    current_animation: Option<String>,
    current_frame: usize,
    accumulated: Duration,
}

impl AnimatedSprite {
    pub fn new(texture: Texture, frame_width: u32, frame_height: u32) -> Self {
        let sheet_width = texture.query().width;
        let frames_per_row = sheet_width / frame_width;
        let origin = Vector2::new(frame_width as f32 / 2.0, frame_height as f32 / 2.0);

        Self {
            texture,
            scale: 1.0,
            rotation: 0.0,
            origin,
            offset: Vector2::ZERO,
            position: Vector2::ZERO,
            flip_h: false,
            flip_v: false,
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
    pub fn draw(&self, canvas: &mut WindowCanvas) {
        let src_x = (self.current_frame as u32 % self.frames_per_row) * self.frame_width;
        let src_y = (self.current_frame as u32 / self.frames_per_row) * self.frame_height;

        let src_rect = Rect::new(
            src_x as i32,
            src_y as i32,
            self.frame_width,
            self.frame_height,
        );
        let dst_width = (self.frame_width as f32 * self.scale) as u32;
        let dst_height = (self.frame_height as f32 * self.scale) as u32;

        let dst_x = (self.position.x - self.origin.x + self.offset.x) as i32;
        let dst_y = (self.position.y - self.origin.y + self.offset.y) as i32;
        let dst_rect = Rect::new(dst_x, dst_y, dst_width, dst_height);

        let rotation_center = Point::new(
            (self.origin.x * self.scale) as i32,
            (self.origin.y * self.scale) as i32,
        );

        canvas
            .copy_ex(
                &self.texture,
                Some(src_rect),
                Some(dst_rect),
                self.rotation as f64,
                Some(rotation_center),
                self.flip_h,
                self.flip_v,
            )
            .expect("[ERR(ANIMATED SPRITE)]: Failed to draw animated sprite");
    }
}

impl Component for AnimatedSprite {
    fn update(&mut self, delta: f32) {
        AnimatedSprite::update(self, delta);
    }

    fn draw(&mut self, canvas: &mut WindowCanvas, parent: &Transform) {
        self.position = parent.position + self.offset;
        self.rotation = parent.rotation;
        self.scale = parent.scale;

        AnimatedSprite::draw(self, canvas);
    }

    fn as_any(&mut self) -> &mut dyn Any {
        self
    }
}
