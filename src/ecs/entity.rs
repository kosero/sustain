use crate::{ecs::components::Component, math::Vector2};
use sdl2::render::WindowCanvas;

pub struct Transform {
    pub position: Vector2,
    pub rotation: f32,
    pub scale: f32,
}

impl Transform {
    pub fn new() -> Self {
        Self {
            position: Vector2::new(0.0, 0.0),
            rotation: 0.0,
            scale: 1.0,
        }
    }
}

pub struct Entity {
    pub transform: Transform,
    pub components: Vec<Box<dyn Component>>,
}

impl Entity {
    pub fn new() -> Self {
        Self {
            transform: Transform::new(),
            components: Vec::new(),
        }
    }

    pub fn add_component(&mut self, component: Box<dyn Component>) {
        self.components.push(component);
    }

    pub fn update(&mut self, delta: f32) {
        for comp in self.components.iter_mut() {
            comp.update(delta);
        }
    }

    pub fn draw(&mut self, canvas: &mut WindowCanvas) {
        for comp in self.components.iter_mut() {
            comp.draw(canvas, &self.transform);
        }
    }
}
