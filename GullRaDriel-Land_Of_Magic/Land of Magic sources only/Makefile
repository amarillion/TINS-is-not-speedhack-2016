VPATH+= ./LIB/src/
INCLUDE=-I./LIB/include

CFLAGS=-std=c99 -g -W -Wall -L/usr/local/lib $(INCLUDE) -DNOALLEGRO -DLINUX -DNOSYSJRNL -D_GNU_SOURCE -DALLEGRO_UNSTABLE
LDLIBS=`pkg-config --cflags --libs allegro-5 allegro_acodec-5 allegro_audio-5 allegro_color-5 allegro_dialog-5 allegro_font-5 allegro_image-5 allegro_main-5 allegro_memfile-5 allegro_physfs-5 allegro_primitives-5 allegro_ttf-5` -lm

CC=gcc
SRC=LandsOfMagics.c LOMConfig.c LOMGfx.c LOMGame.c n_common.c n_str.c n_log.c n_list.c n_particles.c
OBJ=$(SRC:%.c=%.o)

.c.o:
	@echo "Compiling $<"
	$(COMPILE.c) $<
	-@chmod g+w $@

all: LandsOfMagics

clean:
	rm -rf *.o LandsOfMagics


LandsOfMagics: $(OBJ)
	echo "Linking $@"
	$(LINK.c) $(OBJ) -o $@ $(LDLIBS)
