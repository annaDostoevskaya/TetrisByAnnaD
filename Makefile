SRC=$(CURDIR)
OUTPUT_DIR=$(SRC)/build
UTILS=$(CURDIR)/utils

all: linux

linux: sdl_game.cpp
	@mkdir -p $(OUTPUT_DIR) 
	@cd $(OUTPUT_DIR) 
	@$(UTILS)/ctime -begin ./ctime.csv
	gcc $(SRC)/sdl_game.cpp -o sdl_game `sdl2-config --cflags --libs`
	@$(UTILS)/ctime -end ./ctime.csv
	@cd $(SRC) 

win32: sdl_game.cpp
	.\build.bat
