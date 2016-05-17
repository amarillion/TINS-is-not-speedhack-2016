#
# 
# You can override this from the command line,
# e.g. invoke "make BUILD=DEBUG" or "make BUILD=RELEASE"
#
BUILD=DEBUG
#BUILD=RELEASE
#BUILD=STATIC
#BUILD=STATIC_DEBUG

CXX = g++
LD = g++
CFLAGS = -std=c++11 -Iinclude -W -Wall -Wno-unused -DUSE_MOUSE
LFLAGS = 

NAME = usagi

ifeq ($(BUILD),RELEASE)
	CFLAGS += -O3
	LFLAGS += -s
	BUILDDIR = build/release
endif
ifeq ($(BUILD),DEBUG)
	CFLAGS += -g -DDEBUG
	BUILDDIR = build/debug
endif
ifeq ($(BUILD),STATIC)
	CFLAGS += -O3 -DALLEGRO_STATICLINK
	LFLAGS += -s
	BUILDDIR = build/static
endif
ifeq ($(BUILD),STATIC_DEBUG)
	CFLAGS += -g -DDEBUG -DALLEGRO_STATICLINK
	BUILDDIR = build/static_debug
endif

ifndef WINDOWS
ifdef MINGDIR
	WINDOWS = 1
endif
endif

OBJDIR=$(BUILDDIR)/obj

ifdef WINDOWS
	CFLAGS += -D__GTHREAD_HIDE_WIN32API
	LFLAGS += -Wl,--subsystem,windows
	ifeq ($(BUILD),RELEASE)
		LIBS += -lallegro_monolith
	endif
	ifeq ($(BUILD),DEBUG)
		LIBS += -lallegro_monolith-debug
	endif
	BIN = $(BUILDDIR)/$(NAME).exe
	ICONOBJ = $(OBJDIR)/icon.o	
else
	ifeq ($(BUILD),RELEASE)
		LIBS += `pkg-config --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_main-5 allegro_dialog-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 allegro_ttf-5`
	endif
	ifeq ($(BUILD),DEBUG)
		LIBS += `pkg-config --libs allegro-debug-5 allegro_primitives-debug-5 allegro_font-debug-5 allegro_main-debug-5 allegro_dialog-debug-5 allegro_image-debug-5 allegro_audio-debug-5 allegro_acodec-debug-5 allegro_ttf-debug-5`
	endif
	BIN = $(BUILDDIR)/$(NAME)
endif

SRC = $(wildcard src/*.cpp)
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o, $(basename $(notdir $(SRC)))))
DEP = $(addprefix $(OBJDIR)/,$(addsuffix .d, $(basename $(notdir $(SRC)))))

.PHONY: game
game: $(OBJ) $(ICONOBJ) $(OBJ_SH)
	$(LD) -o $(BIN) $^ $(LIBS) $(LFLAGS)

.PHONY: data
data: origdata/tiles.bmp origdata/tiles.tll
	dat -u data/tiles1.dat

.PHONY: all
all: game

$(OBJ): $(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -MMD -o $@ -c $<

$(ICONOBJ) : icon.rc icon.ico
	windres -I rc -O coff -i icon.rc -o $(ICONOBJ)

# include the .d files
-include $(DEP)

RMFILES = $(OBJ) $(DEP)

.PHONY: clean
clean:
ifdef WINDOWS
	del /q $(subst /,\,$(RMFILES))
else
	rm -f $(RMFILES)
endif

.PHONY:distclean
distclean: clean
ifdef WINDOWS
	del /q $(subst /,\,$(BIN))
else
	rm -f $(BIN)
endif

