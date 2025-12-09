use crate::ecs::entity::Transform;
use sdl2::render::WindowCanvas;
use std::any::Any;

pub trait Component: Any {
    fn update(&mut self, _delta: f32) {}
    fn draw(&mut self, _canvas: &mut WindowCanvas, _parent: &Transform) {}
    fn as_any(&mut self) -> &mut dyn Any;
}
