#Im to lazy to learn how to make normal makwfiles so im just going to use this 
#maybe add Cmake later 

dbg:
	$(CXX) src/*.cpp -Wall -Wextra -Werror -O0 -ggdb -std=c++23 -lSDL2 

install:
	$(CXX) src/*.cpp -Wall -Wextra -lSDL2 -o chip8 -std=c++23 -Os
