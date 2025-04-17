#ifndef AUDIO_TASK_H
#define AUDIO_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FILE_NYAN,
    FILE_EXPLOSION,
    //FILE_BACKGROUND,
    //FILE_EFFECT,
    FILE_COUNT // Always keep this as the last item to count files
} FileID;


static const char *file_paths[FILE_COUNT] = {
    "/spiffs/nyan_16bit_mono_8khz.wav",         // FILE_NYAN
    "/spiffs/explosion_16bit_mono_8khz.wav",    // FILE_EXPLOSION
    //"/spiffs/background_music.wav",             // FILE_BACKGROUND
    //"/spiffs/effect_sound.wav"                  // FILE_EFFECT
};

void audio_loop_task(void * FILE);
void audio_task(void * FILE);
void audio_set_volume(void * FILE);
void audio_init(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_TASK_H */