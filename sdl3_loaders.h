#ifndef SDL3_LOADERS_H
#define SDL3_LOADERS_H

/* Texture loaders */
void *sdl3_textureLoader(  const char *path,  void *data);
void  sdl3_textureReleaser(void       *asset, void *data);

/* Surface loaders */
void *sdl3_surfaceLoader(  const char *path,  void *data);
void  sdl3_surfaceReleaser(void       *asset, void *data);

/* Audio loaders */
void *sdl3_audioLoader(  const char *path,  void *data);
void  sdl3_audioReleaser(void       *asset, void *data);

    #ifdef SDL3_LOADERS_IMPLEMENTATION
        #include <stdlib.h>
        #include <SDL3/SDL.h>
        #include <SDL3_image/SDL_image.h>


/* Texture loaders */
void *
sdl3_textureLoader(const char *path, void *data)
{
    SDL_Renderer *renderer = (SDL_Renderer *)data;
    if (!renderer) return NULL;
    
    SDL_Texture **texture = malloc(sizeof(SDL_Texture *));
    if (!texture) return NULL;
    
    *texture = IMG_LoadTexture(renderer, path);
    if (!*texture) {
        free(texture);
        return NULL;
    }
    
    return texture;
}

void 
sdl3_textureReleaser(void *asset, void *data)
{
    (void)data;
    if (!asset) return;
    
    SDL_Texture **texture = (SDL_Texture **)asset;
    if (*texture) {
        SDL_DestroyTexture(*texture);
    }
    free(texture);
}

/* Surface loaders */
void *
sdl3_surfaceLoader(const char *path, void *data)
{
    (void)data;
    SDL_Surface **surface = malloc(sizeof(SDL_Surface *));
    if (!surface) return NULL;
    
    *surface = IMG_Load(path);
    if (!*surface) {
        free(surface);
        return NULL;
    }
    
    return surface;
}

void 
sdl3_surfaceReleaser(void *asset, void *data)
{
    (void)data;
    if (!asset) return;
    
    SDL_Surface **surface = (SDL_Surface **)asset;
    if (*surface) {
        SDL_DestroySurface(*surface);
    }
    free(surface);
}

/* Audio loaders */
void *
sdl3_audioLoader(const char *path, void *data)
{
    (void)data;
    SDL_AudioSpec *spec = malloc(sizeof(SDL_AudioSpec));
    if (!spec) return NULL;
    
    Uint8 *wav_buffer;
    Uint32 wav_length;
    
    if (!SDL_LoadWAV(path, spec, &wav_buffer, &wav_length)) {
        free(spec);
        return NULL;
    }
    
    // Store both spec and buffer - need custom struct
    typedef struct {
        SDL_AudioSpec spec;
        Uint8 *buffer;
        Uint32 length;
    } AudioData;
    
    AudioData *audio = malloc(sizeof(AudioData));
    if (!audio) {
        SDL_free(wav_buffer);
        free(spec);
        return NULL;
    }
    
    audio->spec = *spec;
    audio->buffer = wav_buffer;
    audio->length = wav_length;
    
    free(spec);
    return audio;
}

void 
sdl3_audioReleaser(void *asset, void *data)
{
    (void)data;
    if (!asset) return;
    
    typedef struct {
        SDL_AudioSpec spec;
        Uint8 *buffer;
        Uint32 length;
    } AudioData;
    
    AudioData *audio = (AudioData *)asset;
    if (audio->buffer) {
        SDL_free(audio->buffer);
    }
    free(audio);
}


    #endif /* SDL3_LOADERS_IMPLEMENTATION */
#endif /* SDL3_LOADERS_H */
