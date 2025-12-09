pub extern crate sdl2;

pub use crate::{
    audio::sound::SoundManager,
    core::core::{Core, Game},
    ecs::{components::Component, entity::Entity},
    graphics::{animated_sprite::AnimatedSprite, sprite::Sprite, texture::load_texture},
    math::Vector2,
    physics::{
        body::{physics_step, BodyType, PhysicsBody},
        collision::intersect,
        shape::CollisionShape,
    },
};
pub use sdl2::{event::Event, keyboard::Keycode, pixels::Color, render::WindowCanvas};
