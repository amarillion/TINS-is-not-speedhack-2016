module piece;
import spring;

import std.algorithm;
import std.array;
import std.string;

import allegro5.allegro;
import allegro5.allegro_font;

/// Rhyme Puzzle Piece
class Piece
{
	string contents;
	int w;

	this (string contents_)
	{
		contents = contents_;
		w = al_get_ustr_width (textFont, contents.toAllegroUstr ());
	}
}
