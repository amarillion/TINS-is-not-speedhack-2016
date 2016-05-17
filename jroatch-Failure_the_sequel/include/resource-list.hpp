#pragma once
#ifndef __included_resource_list_h
#define __included_resource_list_h

#include <cstdint>
#include <cstdbool>
#include <cstddef>

enum Resource_Type
{
  RESOURCETYPE_UNKOWN,
  RESOURCETYPE_IMAGE,
  RESOURCETYPE_SAMPLE,
  RESOURCETYPE_BITMAP_FONT,
  RESOURCETYPE_TTF_FONT,
  RESOURCETYPE_STREAM,
  RESOURCETYPE_TEXT,
  RESOURCETYPE_TILEMAP,
  NUMBER_OF_RESOURCETYPES
};

enum Resource_Id
{
  RESOURCE_POETRY,
  RESOURCE_MAINFONT,
  RESOURCE_DEBUGFONT,
  RESOURCE_BOOP_SFX,
  RESOURCE_BGM,
  NUMBER_OF_RESOURCES
};

struct Resource_Item
{
  enum Resource_Type type;
  const char *filename;
  void *item;
};

#endif
