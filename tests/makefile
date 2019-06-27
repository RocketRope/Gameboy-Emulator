
all:
	rgbasm -o game.obj game.z80
	rgblink -m game.map -n game.sym -o game.gb game.obj
	rgbfix -p 0 -v game.gb