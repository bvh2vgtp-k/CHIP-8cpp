#include <cstdint>
#include <SDL2/SDL.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class sdl 
{
public:
	sdl(const char *title, int ww, int wh, int tw, int th);
	~sdl();

	void update(const void *uffer, int pitch);
	bool processInput(uint8_t *keys);

private:
	SDL_Window *window{nullptr};
	SDL_Renderer *renderer{nullptr};
	SDL_Texture *texture{nullptr};
};
