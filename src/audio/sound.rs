use sdl2::{
    audio::{AudioCVT, AudioCallback, AudioDevice, AudioSpecDesired, AudioSpecWAV},
    AudioSubsystem,
};
use std::path::Path;

struct SoundData {
    data: Vec<u8>,
    volume: f32,
    pos: usize,
}

impl AudioCallback for SoundData {
    type Channel = u8;

    fn callback(&mut self, out: &mut [u8]) {
        for dst in out.iter_mut() {
            let pre_scale = *self.data.get(self.pos).unwrap_or(&128);
            let scaled_signed_float = (pre_scale as f32 - 128.0) * self.volume;
            let scaled = (scaled_signed_float + 128.0) as u8;
            *dst = scaled;
            self.pos += 1;
        }
    }
}

pub struct SoundManager {
    audio: AudioSubsystem,
    devices: Vec<AudioDevice<SoundData>>, // Cihazları sakla
}

impl SoundManager {
    pub fn new(audio: AudioSubsystem) -> Self {
        Self {
            audio,
            devices: Vec::new(),
        }
    }

    pub fn play(&mut self, path: &Path, volume: f32) {
        let desired = AudioSpecDesired {
            freq: Some(44100),
            channels: Some(1),
            samples: None,
        };

        let device = self
            .audio
            .open_playback(None, &desired, |spec| {
                let wav = AudioSpecWAV::load_wav(path).expect("wav yüklenemedi");

                let cvt = AudioCVT::new(
                    wav.format,
                    wav.channels,
                    wav.freq,
                    spec.format,
                    spec.channels,
                    spec.freq,
                )
                .unwrap();

                let data = cvt.convert(wav.buffer().to_vec());

                SoundData {
                    data,
                    volume,
                    pos: 0,
                }
            })
            .expect("audio device açılamadı");

        device.resume();

        self.devices.push(device);
    }
}
