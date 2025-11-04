#include <SDL3/SDL.h>

#define SDL3_LOADERS_IMPLEMENTATION
#include "../sdl3_loaders.h"
#include "../assman.h"

int 
main(void)
{
    
    /* Initialize SDL3 */
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    
    /* Initialize SDL3_image */
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        SDL_Log("IMG_Init failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    /* Create window and renderer */
    SDL_Window *window = SDL_CreateWindow(
        "Asset Manager SDL3 Example",
        800, 600,
        0
    );
    
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    /* Create asset manager */
    AssMan *assman = AssMan_new();
    
    /* Register file types (textures need renderer as load_data) */
    AssMan_registerFiletype(assman, ".png", sdl3_textureLoader, sdl3_textureReleaser);
    AssMan_registerFiletype(assman, ".jpg", sdl3_textureLoader, sdl3_textureReleaser);
    AssMan_registerFiletype(assman, ".bmp", sdl3_surfaceLoader, sdl3_surfaceReleaser);
    AssMan_registerFiletype(assman, ".wav", sdl3_audioLoader, sdl3_audioReleaser);
    
    /* Load assets */
    SDL_Texture **player_tex = AssMan_load(assman, "player.png", renderer, NULL);
    SDL_Surface **icon = AssMan_load(assman, "icon.bmp", NULL, NULL);
    
    /* Main loop */
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        
        /* Clear screen */
        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
        SDL_RenderClear(renderer);
        
        /* Draw texture if loaded */
        if (player_tex && *player_tex) {
            SDL_FRect dest = {100, 100, 64, 64};
            SDL_RenderTexture(renderer, *player_tex, NULL, &dest);
        }
        
        SDL_RenderPresent(renderer);
    }
    
    /* Release assets */
    AssMan_release(assman, "player.png");
    AssMan_release(assman, "icon.bmp");
    
    /* Cleanup */
    AssMan_free(assman);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
