use crate::{math::Vector2, physics::shape::CollisionShape};

pub fn intersect(
    a_pos: Vector2,
    a_shape: CollisionShape,
    b_pos: Vector2,
    b_shape: CollisionShape,
) -> bool {
    match (a_shape, b_shape) {
        (CollisionShape::Circle { radius: ra }, CollisionShape::Circle { radius: rb }) => {
            circle_vs_circle(a_pos, ra, b_pos, rb)
        }
        (CollisionShape::AABB { half_extents: ha }, CollisionShape::AABB { half_extents: hb }) => {
            aabb_vs_aabb(a_pos, ha, b_pos, hb)
        }
        _ => false,
    }
}

fn circle_vs_circle(a: Vector2, ra: f32, b: Vector2, rb: f32) -> bool {
    let dx = a.x - b.x;
    let dy = a.y - b.y;
    let r = ra + rb;
    dx * dx + dy * dy <= r * r
}

fn aabb_vs_aabb(a: Vector2, ha: Vector2, b: Vector2, hb: Vector2) -> bool {
    (a.x - b.x).abs() <= ha.x + hb.x && (a.y - b.y).abs() <= ha.y + hb.y
}
