#include "emulator.hpp"
#include "sdl.hpp"
#include <chrono>
#include <cstdint>
#include <print>

constexpr int32_t MODIFIER = 10;
constexpr int32_t WINDOW_WIDTH =  64 * MODIFIER;
constexpr int32_t WINDOW_HEIGTH = 32 * MODIFIER;
constexpr uint32_t VIDEO_SCALE = 15;
constexpr uint32_t SPEED = 500;

constexpr uint32_t TARGET_FPS = 15;

using Clock = std::chrono::high_resolution_clock;

int main(int argc, char **argv){
	if(argc != 2){
		std::println("Usage: {} <filename>", argv[0]);
		return 1;
	}
	sdl render("test", WINDOW_WIDTH, WINDOW_HEIGTH, 64, 32);

	emulator emu;
	emu.loadROM(argv[1]);

	auto last = Clock::now();
	bool quit = false;
	uint32_t delay = (1 / TARGET_FPS) * 1000;

	while(!quit){
		if(!render.processInput(emu.keys)){
			quit = true;
		}

		auto curr = Clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(curr - last).count();

		if(dt > (1000.0f / SPEED)){
			last = curr;
			emu.emulate();

			if(emu.drawflag){
				render.update(emu.screen, 64 * sizeof(uint32_t));
				emu.drawflag = false;
			}
		}
		SDL_Delay(delay);
	}

	return 0;


}
