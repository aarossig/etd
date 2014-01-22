#
# Makefile the map tools of etd
#

# Configuration ################################################################

SRC = $(wildcard *.cs)
BIN = maptools.exe
MAP = map.txt
MAP_DEST = ../Game/Map.h

# Targets ######################################################################

all: $(BIN)
	
$(BIN): $(SRC)
	mcs $(SRC) -out:$(BIN)

clean:
	rm -f $(BIN)

run: all
	mono $(BIN) $(MAP) > $(MAP_DEST)
