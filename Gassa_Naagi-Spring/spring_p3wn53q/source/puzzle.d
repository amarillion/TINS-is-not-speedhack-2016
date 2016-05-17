module puzzle;
import piece;

import std.algorithm;
import std.array;
import std.string;

/// Rhyme Puzzle
class Puzzle
{
	Piece [] [] piece;

	this (string [] verse)
	{
		foreach (line; verse)
		{
			piece ~= line.split.map !(x => new Piece (x)).array;
		}
	}
}
