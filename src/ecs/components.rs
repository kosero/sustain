use crate::prelude::Transform;
use raylib::drawing::RaylibDrawHandle;
use std::any::Any;

pub trait Component: Any {
    fn update(&mut self, _delta: f32) {}
    fn draw(&mut self, _d: &mut RaylibDrawHandle, parent: &Transform) {}
    fn as_any(&mut self) -> &mut dyn Any;
}
