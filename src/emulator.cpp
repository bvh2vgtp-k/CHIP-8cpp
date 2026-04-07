#include "emulator.hpp"
#include <cstdint>
#include <print>
#include <random>
#include <fstream>
#include <iostream>


const uint8_t chip8_fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,
	0x20, 0x60, 0x20, 0x20, 0x70,
	0xF0, 0x10, 0xF0, 0x80, 0xF0,
	0xF0, 0x10, 0xF0, 0x10, 0xF0,
	0x90, 0x90, 0xF0, 0x10, 0x10,
	0xF0, 0x80, 0xF0, 0x10, 0xF0,
	0xF0, 0x80, 0xF0, 0x90, 0xF0,
	0xF0, 0x10, 0x20, 0x40, 0x40,
	0xF0, 0x90, 0xF0, 0x90, 0xF0,
	0xF0, 0x90, 0xF0, 0x10, 0xF0,
	0xF0, 0x90, 0xF0, 0x90, 0x90,
	0xE0, 0x90, 0xE0, 0x90, 0xE0,
	0xF0, 0x80, 0x80, 0x80, 0xF0,
	0xE0, 0x90, 0x90, 0x90, 0xE0,
	0xF0, 0x80, 0xF0, 0x80, 0xF0,
	0xF0, 0x80, 0xF0, 0x80, 0x80
};




void emulator::emulate(){

	/*load font into memory*/
	for(int i = 0; i < 80; ++i){
		ram[i] = chip8_fontset[i];
	}

	/*fetch*/
	uint16_t opcode = (ram[pc] << 8) | ram[pc + 1];
	pc += 2;

	uint16_t x = (opcode & 0x0F00) >> 8;
	uint16_t y = (opcode & 0x00F0) >> 4;
	uint8_t  kk = opcode & 0x00FF; 
	uint16_t nnn = opcode & 0x0FFF;


	/*prepare for RND instruction*/
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 256);

	switch(opcode & 0xF000){
		case 0x0000:
			switch(kk){
				case 0xE0: /*CLS*/
					for(auto i = 0; i < 2048; ++i){
						screen[i] = 0;
					}
					drawflag = true;
					break;
				case 0xEE: /*RET*/
					pc = stack[--sp];
					break;
				default:
					/*ignored*/
					break;
			}
			break;
		case 0x1000: /*JP addr*/
			pc = nnn;
			break;
		case 0x2000: /*CALL addr*/
			stack[sp++] = pc;
			pc = nnn;
			break;
		case 0x3000: /*SE Vx, byte*/
			if(V[x] == kk){
				pc += 2;
			}
			break;
		case 0x4000: /*SNE Vx, byte*/
			if(V[x] != kk){
				pc += 2;
			}
			break;
		case 0x5000: /*SE Vx, Vy*/
			if(V[x] == V[y]){
				pc += 2;
			}
			break;
		case 0x6000: /*LD Vx, byte*/
			V[x] = kk;
			break;
		case 0x7000: /*ADD Vx, byte*/
			V[x] += kk;
			break;
		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0: /*LD Vx, Vy*/
					V[x] = V[y];
					break;
				case 0x1:/*OR Vx, Vy*/
					V[x] |= V[y];
					break;
				case 0x2: /*AND Vx, Vy*/
					V[x] &= V[y];
					break;
				case 0x3: /*XOR Vx, Vy*/
					V[x] ^= V[y];
					break;
				case 0x4: /*ADD Vx, Vy*/
					V[x] += V[y]; /*WARNING: может сломаться*/
					V[0xF] = (V[x] + V[y] > 255) ? 1 : 0;
					break;
				case 0x5: /*SUB Vx, Vy*/
					V[0xF] = (V[x] > V[y]) ? 0x1 : 0x0;
					V[x] -= V[y];
					break;
				case 0x6: /*SHR Vx, {, Vy}*/
					if((V[x] & 0x1) == V[x]){ 
						V[0xF] = 0x1;
					}else {
						V[0xF] = 0;
					}
					V[x] >>= 1;
					pc += 2;
					break;
					/*indus solution: V[0xF] = V[x] & 0x1*/
				case 0x7: /*SUBN Vx, Vy*/
					V[0xF] = (V[y] > V[x]) ? 0x1 : 0x0;
					V[x] -= V[y]; 
					break;
				case 0xE: /*SHL Vx, {, Vy}*/
					if((V[x] & 0x1) == V[x]){ 
						V[0xF] = 0x1;
					}else {
						V[0xF] = 0;
					}
					V[x] <<= 1;
					pc += 2;
					break;
			}
			break;
		case 0x9000: /* SNE Vx, Vy*/
			if(V[x] != V[y]){
				pc += 2;
			}
			break;
		case 0xA000: /* LD I, addr */
			I = nnn;
			break;
		case 0xB000: /*JP V0, addr*/
			pc = nnn + V[0];
			break;
		case 0xC000: /*RND Vx, byte*/
			V[x] = dist(rng) & kk;
			break;
		case 0xD000:
			{

				uint16_t height = opcode & 0x000F;
				uint16_t x_coord = V[x] % 64;
				uint16_t y_coord = V[y] % 32;
				V[0xF] = 0; // Collision flag

				for (int row = 0; row < height; ++row)
				{
					uint8_t sprite_byte = ram[I + row];
					for (int col = 0; col < 8; ++col)
					{
						if ((sprite_byte & (0x80 >> col)) != 0)
						{
							uint32_t *pixel = &screen[(y_coord + row) * 64 + (x_coord + col)];
							if (*pixel == 0xFFFFFFFF)
							{
								V[0xF] = 1; // Collision detected
							}
							*pixel ^= 0xFFFFFFFF; // XOR the pixel
						}
					}
				}
				drawflag = true;
				break;
			}
#if 0
			{ /*WARNING: idk */
				uint16_t height = opcode & 0x000F;
				uint16_t pixel = 0;
				uint16_t Vx = V[x];
				uint16_t Vy = V[y];
				V[0xF] = 0;
				for(int yline = 0; yline < height; yline++){
					pixel = ram[I + yline];
					for(int xline = 0; xline < 8; xline++){
						if((pixel & (0x80 >> xline))  != 0){
							if(screen[(Vx + xline +((Vy + yline) * 64))] == 1){
								V[0xF] = 1;
							}
							screen[Vx + xline + ((Vy + yline) *64)] ^=1;
						}
					}
				}
				drawflag = true;
				//pc += 2;
				break;
			}
#endif
		case 0xE000:
			switch (kk) {
				case 0x9E:
					if(keys[V[x]]){
						pc += 2;
					}
					break;
				case 0xA1:
					if(!keys[V[x]]){
						pc += 2;
					}
					break;
			}
			break;
		case 0xF000:
			switch (kk) {
				case 0x07:
					V[x] = dt;
					break;
				case 0x0A:
					{
						bool pressed = false;
						for(int i = 0; i < 16; ++i){
							if(keys[i]){
								V[x] = i;
								pressed = true;
								break;
							}
						}
						if(!pressed){
							pc+= 2;
						}
						break;
					}
				case 0x15:
					dt = V[x];
					break;
				case 0x18:
					st = V[x];
					break;
				case 0x1E:
					I += V[x];
					break;
				case 0x29:
					I = V[x] * 5;
					break;
				case 0x33:
					ram[I] = V[x] / 100;
					ram[I + 1] = (V[x] / 10) % 10;
					ram[I + 2] = V[x] % 10;
					break;
				case 0x55:
					for(auto i = 0; i <= x; ++i){
						ram[I + i] = V[i];
					}
					break;
				case 0x65:
					for(int i = 0; i < x; ++i){
						V[i] = ram[I + i];
					}
					break;
				default:
					std::println(std::cerr, "Error: Unknown opcode");
					break;
			}
	}
	if(dt > 0){
		--dt;
	}

	if(st > 0){
		if(st == 1){
			std::println("sound");
		}
		--st;
	}
}

void emulator::loadROM(const char* filename){
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		throw std::runtime_error("Error: Failed to open ROM " + std::string(filename));
	}

	std::streamsize rom_size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (rom_size > (4096 - 0x200)) {
		throw std::runtime_error("Error: ROM too large for memory");
	}

	std::vector<char> buffer(rom_size);
	if (!file.read(buffer.data(), rom_size)) {
		throw std::runtime_error("Error: Failed to read ROM"); // ))
	}

	std::copy(buffer.begin(), buffer.end(), ram + 0x200);

}

