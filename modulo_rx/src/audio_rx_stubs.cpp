extern "C" int audio_rx_get_underruns() { return 0; }
extern "C" int audio_rx_get_frames_played() { return 0; }
extern "C" void audio_rx_push_samples(const short*, unsigned char, unsigned char) {}