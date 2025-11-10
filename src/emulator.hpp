#include <cstdint>


constexpr std::size_t  SCREEN_WIDTH  = 64;
constexpr std::size_t  SCREEN_HEIGHT = 32;


constexpr uint16_t 	START_ADDR   = 0x200;
constexpr std::size_t   RAM_SIZE     = 4096;
constexpr std::size_t   NUM_REGS     = 16;
constexpr std::size_t   STACK_SIZE   = 16;
constexpr std::size_t   NUM_KEYS     = 16;
constexpr std::size_t   FONT_SIZE    = 80;

class emulator {
	private:
		uint16_t pc			{START_ADDR}; /*program starts at adress 0x200 */
		uint16_t I			{0};	/* I-reg */
		uint16_t sp			{0}; 	/* stack pointer */

		uint8_t ram[RAM_SIZE]		{0};
		uint8_t V[NUM_REGS]		{0};	/*V-regs*/
		uint16_t stack[STACK_SIZE]	{0};

		uint8_t dt			{0};
		uint8_t st			{0};


	public:
		emulator() = default;
		uint32_t screen[SCREEN_HEIGHT * SCREEN_WIDTH] {0};
		uint8_t keys[NUM_KEYS] {0};
		bool 	drawflag;
		~emulator() = default;
		void emulate();
		void loadROM(const char *filename);
};
