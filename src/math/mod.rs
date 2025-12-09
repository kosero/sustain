pub type Vector2 = glam::Vec2;
pub type IVector2 = glam::IVec2;
pub type Matrix2 = glam::Mat2;
pub type Matrix3 = glam::Mat3;
pub type Affine2 = glam::Affine2;

pub struct Transform2D {
    pub position: Vector2,
    pub rotation: f32,
    pub scale: Vector2,
}

impl Default for Transform2D {
    fn default() -> Self {
        Self {
            position: Vector2::ZERO,
            rotation: 0.0,
            scale: Vector2::ONE,
        }
    }
}
