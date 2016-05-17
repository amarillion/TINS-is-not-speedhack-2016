CXX=g++ -std=c++11 -Wall -I .
CPPFLAGS=-c
PKG=`pkg-config --libs allegro-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 allegro_ttf-5 allegro_font-5 allegro_primitives-5`

mori: map.o camera.o game.o main.o player.o engine.o
	$(CXX) -o mori map.o camera.o game.o main.o player.o engine.o $(PKG)

map.o: ./src/map.cpp
	$(CXX) $(CPPFLAGS) ./src/map.cpp

camera.o: ./src/camera.cpp
	$(CXX) $(CPPFLAGS) ./src/camera.cpp

game.o: ./src/game.cpp
	$(CXX) $(CPPFLAGS) ./src/game.cpp

main.o: ./src/main.cpp
	$(CXX) $(CPPFLAGS) ./src/main.cpp

player.o: ./src/player.cpp
	$(CXX) $(CPPFLAGS) ./src/player.cpp

engine.o: ./src/engine.cpp
	$(CXX) $(CPPFLAGS) ./src/engine.cpp

clean:
	rm -fr ./*.o
