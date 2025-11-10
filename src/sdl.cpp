#include "sdl.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>
#include <print>


const SDL_Scancode keymap[16] = {
    SDL_SCANCODE_X, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_A, // 7
    SDL_SCANCODE_S, // 8
    SDL_SCANCODE_D, // 9
    SDL_SCANCODE_Z, // A
    SDL_SCANCODE_C, // B
    SDL_SCANCODE_4, // C
    SDL_SCANCODE_R, // D
    SDL_SCANCODE_F, // E
    SDL_SCANCODE_V  // F
};

sdl::sdl(const char *title, int ww, int wh, int tw, int th){
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ww, wh, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, tw, th);
	std::println("SDL Constructed");
}

sdl::~sdl(){
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::println("SDL died");
}

void sdl::update(const void *buffer, int pitch){
	SDL_UpdateTexture(texture, nullptr, buffer, pitch); 
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool sdl::processInput(uint8_t *keys){
	bool quit = false;
	SDL_Event e;

	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN: {
				if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
					quit = true;
				}
				for(auto i = 0; i < 16; ++i){
					if(e.key.keysym.scancode == keymap[i]){
						keys[i] = 1;
					}
				}
				break;
			}
			case SDL_KEYUP: {
				for(auto i = 0; i < 16; ++i){
					if(e.key.keysym.scancode == keymap[i]){
						keys[i] = 0;
					}
				}
			}
		}
	}

	return !quit;
}
