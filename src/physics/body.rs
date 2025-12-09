use crate::{
    ecs::entity::Entity,
    math::Vector2,
    physics::{collision::intersect, shape::CollisionShape},
};

#[derive(Copy, Clone, PartialEq, Eq)]
pub enum BodyType {
    Static,
    Dynamic,
}
pub struct PhysicsBody {
    pub body_type: BodyType,
    pub velocity: Vector2,
    pub base_size: Vector2,
    pub shape: CollisionShape,
}

impl PhysicsBody {
    pub fn new_dynamic(w: f32, h: f32, velocity: Vector2) -> Self {
        Self {
            body_type: BodyType::Dynamic,
            velocity,
            base_size: Vector2::new(w, h),
            shape: CollisionShape::aabb(w, h),
        }
    }

    pub fn new_static(w: f32, h: f32) -> Self {
        Self {
            body_type: BodyType::Static,
            velocity: Vector2::ZERO,
            base_size: Vector2::new(w, h),
            shape: CollisionShape::aabb(w, h),
        }
    }

    pub fn update_shape(&mut self, scale: f32) {
        self.shape = CollisionShape::aabb(self.base_size.x * scale, self.base_size.y * scale);
    }
}
pub fn physics_step(objects: &mut [(&mut Entity, &mut PhysicsBody)], delta: f32) {
    // move dynamics
    for (entity, body) in objects.iter_mut() {
        if let BodyType::Dynamic = body.body_type {
            entity.transform.position += body.velocity * delta;
            body.update_shape(entity.transform.scale);
        }
    }

    let len = objects.len();
    for i in 0..len {
        let (left, right) = objects.split_at_mut(i + 1);
        let (ent_a, body_a) = &mut left[i];

        for (ent_b, body_b) in right {
            let pos_a = ent_a.transform.position;
            let pos_b = ent_b.transform.position;

            if intersect(pos_a, body_a.shape, pos_b, body_b.shape) {
                let a_type = body_a.body_type;
                let b_type = body_b.body_type;

                match (a_type, b_type) {
                    (BodyType::Dynamic, BodyType::Dynamic) => {
                        body_a.velocity = -body_a.velocity;
                        body_b.velocity = -body_b.velocity;
                    }
                    (BodyType::Dynamic, BodyType::Static) => {
                        body_a.velocity = -body_a.velocity;
                    }
                    (BodyType::Static, BodyType::Dynamic) => {
                        body_b.velocity = -body_b.velocity;
                    }
                    _ => {}
                }
            }
        }
    }
}
