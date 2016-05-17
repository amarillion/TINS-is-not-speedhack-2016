#pragma once
#ifndef __included_sprite_h
#define __included_sprite_h

#include <cstdint>
#include <cstdbool>

enum Sprite_Type
{
  SPRITETYPE_NONE,
  SPRITETYPE_DEBUGPOINT,
  SPRITETYPE_PLAYER,
  NUMBER_OF_SPRITETYPETYPES
};

struct Sprite
{
  enum Sprite_Type type;
  uint32_t state;
  double x;
  double y;
  double z;
  double dx;
  double dy;
  double dz;
  double t;
};

#endif
