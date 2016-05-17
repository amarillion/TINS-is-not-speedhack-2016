#include <sstream>

#include <inc/game.hpp>

using std::stringstream;

int main(int argc, char *argv[]) {

  Game Game;

  int seed = time(NULL);

  if (argc > 1) {

    // Convert the seed to an int.
    stringstream convert(argv[1]);

    convert >> seed;
  }

  // Set the seed for the game.
  Game.setSeed(seed);

  // Get the ball rolling.
  Game.initialize();

  return 0;
}
