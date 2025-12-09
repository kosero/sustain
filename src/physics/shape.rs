use crate::math::Vector2;

#[derive(Clone, Copy, Debug)]
pub enum CollisionShape {
    Circle { radius: f32 },
    AABB { half_extents: Vector2 },
}

impl CollisionShape {
    pub fn circle(radius: f32) -> Self {
        CollisionShape::Circle { radius }
    }

    pub fn aabb(width: f32, height: f32) -> Self {
        CollisionShape::AABB {
            half_extents: Vector2::new(width * 0.5, height * 0.5),
        }
    }
}
