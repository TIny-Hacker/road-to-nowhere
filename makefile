# ----------------------------
# Makefile Options
# ----------------------------

NAME = ROAD
ICON = icon.png
DESCRIPTION = "TIny_Hacker's CC26 entry"
COMPRESSED = YES
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

tilesets:
	convbin -j csv -k c -i src/tilemap/background1.csv -o src/tilemap/background1.c -n background1

final:
	make
	convbin -r -e zx0 -k 8xp-compressed -u -n $(NAME) -i bin/$(NAME).bin -o bin/$(NAME).8xp
