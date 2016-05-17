module Colours;
import allegro5.allegro;

auto black() { return al_map_rgb(0,0,0); }
auto white() { return al_map_rgb(255,255,255); }

//I've tuned this green against the grass sprites.
auto green() { return al_map_rgb(91,169,76); }
