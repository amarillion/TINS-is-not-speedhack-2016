module Levels;
import std.file;
import std.algorithm;
//import std.range;
//import std.string;

enum levelsPath = "Levels/";
enum levelExt = ".MrPFLevel";

void GetLevelNames(out string[] allLevelNames) {
	allLevelNames.length = 0;
	if (levelsPath.exists) {
		foreach (f; dirEntries(levelsPath, SpanMode.shallow)) {
			if (f.isFile && f.name.endsWith(levelExt)) {
				allLevelNames ~= f.name[levelsPath.length..$-levelExt.length];
			}
		}
		allLevelNames.sort();
	}
}
