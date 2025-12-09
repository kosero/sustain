use sdl2::{
    image::LoadTexture,
    render::{Texture, WindowCanvas},
};

pub fn load_texture(canvas: &WindowCanvas, path: &str) -> Texture {
    canvas
        .texture_creator()
        .load_texture(path)
        .unwrap_or_else(|_| panic!("[INFO(LOAD TEXTURE)]: Failed to load texture: {}", path))
}
