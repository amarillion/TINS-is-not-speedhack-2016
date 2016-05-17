module Editor;
import LevelModule;
import allegro5.allegro;
import allegro5.allegro_primitives;
import allegro5.allegro_font;
import allegro5.allegro_native_dialog;
import Main;
import MainMenu;
import Colours;
import Scopes;
import Buttons;
import std.array;
import Game;
import Levels;
import std.file;
import std.algorithm;
import std.range;
import std.string;

class Editor
{
	enum w=Level.w, h=Level.h;

	Level level;

	int buttons;

	string myLevelName;
	string[] allLevelNames;
	bool saving, renaming;
	bool modified;
	string lastSavedLevelName;

	Tile currentTile = Tile(TileType.Wall);

	this()
	{
		GetLevelNames(allLevelNames);

		level.placeGrass();
		foreach (y; 0..h) level.map[y][0] = level.map[y][$-1] = Tile(TileType.Wall);
		foreach (x; 0..w) level.map[0][x] = level.map[$-1][x] = Tile(TileType.Wall);
	}

	void handleEvent(ALLEGRO_EVENT* event)
	{
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) buttons |= 1 << (event.mouse.button - 1);
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) buttons &= ~(1 << (event.mouse.button - 1));

		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			int x, y;
			Level.decodeMouse(event, &x, &y);
			if (x >= 0 && x < w && y >= 0 && y < h) {
				if (buttons & 1) {
					level.map[y][x] = currentTile;
					modified = true;
				} else if (buttons & 2) {
					level.map[y][x] = Tile(TileType.None);
					modified = true;
				}
			}
		}

		if (saving) {
			//Level name editing
			if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
				int c = event.keyboard.unichar;
				if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
					if (myLevelName.length > 0) myLevelName.length--;
				} else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
					confirmSave();
				} else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
					cancelSave();
				} else if (c < 0x10000 && `\/:*?"<>|`.indexOf(c) < 0) {
					int[2][] ranges;
					ranges.length = al_get_font_ranges(theFont, 0, null);
					al_get_font_ranges(theFont, ranges.length, cast(int*)ranges);
					if (ranges.any!(range => c >= range[0] && c <= range[1])) {
						myLevelName ~= cast(char)c;
						if (al_get_text_width(theFont, myLevelName.toStringz()) > uiRight-uiLeft)
							myLevelName.length--;
					}
				}
			}
		}
	}

	void update()
	{
	}

	void draw()
	{
		level.draw();
		al_draw_filled_rectangle(Level.gameWidth, 0, uiLeft, screenHeight, black);
		drawTileUi();
		drawLoadSaveUi();
	}

	enum margin = 16;
	enum uiLeft = Level.gameWidth + margin;
	enum uiRight = screenWidth;
	enum xTileSpacing = 160;
	enum yTileSpacing = Level.th + margin;
	enum loadSaveUiTop = margin+yTileSpacing*5;
	enum lineSpacing = MainMenu.lineSpacing;

	void drawTileUi()
	{
		enum top = 0, bottom = loadSaveUiTop;

		al_draw_filled_rectangle(uiLeft, top, uiRight, bottom, black);

		drawEditorOnlyTileUi(Tile(TileType.Wall), uiLeft, top+margin);

		drawPlaceableTileUi(Tile(AnimalType.Cow  .toTileType), uiLeft,            top+margin+yTileSpacing*1);
		drawPlaceableTileUi(Tile(AnimalType.Sheep.toTileType), uiLeft,            top+margin+yTileSpacing*2);
		drawPlaceableTileUi(Tile(AnimalType.Pig  .toTileType), uiLeft,            top+margin+yTileSpacing*3);
		drawPlaceableTileUi(Tile(AnimalType.Duck .toTileType), uiLeft,            top+margin+yTileSpacing*4);
		drawPlaceableTileUi(Tile(FoodType.Beef. toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*1);
		drawPlaceableTileUi(Tile(FoodType.Lamb. toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*2);
		drawPlaceableTileUi(Tile(FoodType.Bacon.toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*3);
		drawPlaceableTileUi(Tile(FoodType.Duck. toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*4);

		enum xMiddle = uiRight - xTileSpacing*2;

		drawComponentUi(ComponentType.Bomb             , xMiddle, top+margin+yTileSpacing*1);
		drawComponentUi(ComponentType.TurnChanger      , xMiddle, top+margin+yTileSpacing*2);
		drawComponentUi(ComponentType.Multiply         , xMiddle, top+margin+yTileSpacing*3);
		drawComponentUi(ComponentType.Rain, xMiddle+xTileSpacing, top+margin+yTileSpacing*1);
		drawComponentUi(ComponentType.Snow, xMiddle+xTileSpacing, top+margin+yTileSpacing*2);
		drawComponentUi(ComponentType.Sun , xMiddle+xTileSpacing, top+margin+yTileSpacing*3);

		clickableTextLeft(xMiddle, top+margin+yTileSpacing*5-lineSpacing-6, uiRight, "Test Level", &testLevel);
	}

	void testLevel() {
		theGame = new Game(level, myLevelName);
	}

	void endLevelTest() {
		delete theGame; theGame = null;
	}

	void drawEditorOnlyTileUi(Tile tile, int x, int y)
	{
		enum tw = Level.tw, th = Level.th;
		registerButton(x, y, x+tw, y+th, { currentTile = tile; });
		if (currentTile.type == tile.type) al_draw_rectangle(x-1, y-1, x+tw+1, y+th+1, white, 2);
		Level.drawTile(x, y, tile);
	}

	void drawPlaceableTileUi(Tile tile, int x, int y)
	{
		drawEditorOnlyTileUi(tile, x, y);

		enum tw = Level.tw, th = Level.th;
		int textWidth = al_get_text_width(theFont, "000");
		registerButton(x+tw+2, y, x+tw+2+textWidth, y+th, {
			if (buttonEvent.mouse.button == 1 && level.placeables[tile.type]<256) { level.placeables[tile.type]++; modified = true; }
			if (buttonEvent.mouse.button == 2 && level.placeables[tile.type]>0) { level.placeables[tile.type]--; modified = true; }
		}, true);
		al_draw_textf(theFont, green, x+tw+2+textWidth, y-8, ALLEGRO_ALIGN_RIGHT, "%d", level.placeables[tile.type]);
	}

	void drawComponentUi(ComponentType type, int x, int y)
	{
		Level.drawComponent(x, y, type, 0);

		enum tw = Level.tw, th = Level.th;
		int textWidth = al_get_text_width(theFont, "000");
		registerButton(x+tw+2, y, x+tw+2+textWidth, y+th, {
			if (buttonEvent.mouse.button == 1 && level.components[type]<256) { level.components[type]++; modified = true; }
			if (buttonEvent.mouse.button == 2 && level.components[type]>0) { level.components[type]--; modified = true; }
		}, true);
		al_draw_textf(theFont, green, x+tw+2+textWidth, y-8, ALLEGRO_ALIGN_RIGHT, "%d", level.components[type]);
	}

	void drawLoadSaveUi()
	{
		enum top = loadSaveUiTop;
		enum bottom = screenHeight - lineSpacing;

		al_draw_filled_rectangle(uiLeft, top, uiRight, screenHeight, black);

		{
			auto clip = Clip(uiLeft, top, uiRight, bottom);

			//Trisect the list into earlier levels, our level (if present) and later levels.
			auto ranges = allLevelNames.assumeSorted.trisect(myLevelName);
			int oursStart = ranges[0].length;
			int laterStart = oursStart + ranges[1].length;
			auto earlier = allLevelNames[0..oursStart];
			string[] ours; if (myLevelName.length > 0 || saving) ours ~= myLevelName;
			auto later = allLevelNames[laterStart..$];

			int oursTop = (myLevelName.length == 0 && !saving ? top : (top+bottom-ours.length*lineSpacing)/2);
			int earlierTop = oursTop - earlier.length*lineSpacing;
			int laterTop = oursTop + ours.length*lineSpacing;

			// http://stackoverflow.com/questions/32780138/lambda-capture-by-value-in-d-language
			auto getLambdaWithCapturedLevelName(string levelName) { return { onLevelNameClick(levelName); }; }

			foreach (i, levelName; earlier) {
				clickableTextLeft(uiLeft, earlierTop+i*lineSpacing, uiRight, levelName.toStringz(), getLambdaWithCapturedLevelName(levelName));
			}
			foreach (i, levelName; ours) {
				bool showCursor = (saving && time % 0.5f < 0.25f);
				al_draw_textf(theFont, white, uiLeft, oursTop+i*lineSpacing, ALLEGRO_ALIGN_LEFT, showCursor ? "%s|" : "%s", levelName.toStringz());
			}
			foreach (i, levelName; later) {
				clickableTextLeft(uiLeft, laterTop+i*lineSpacing, uiRight, levelName.toStringz(), getLambdaWithCapturedLevelName(levelName));
			}
		}

		enum xCentre1 = uiLeft+(uiRight-uiLeft)/3;
		enum xCentre2 = xCentre1+(uiRight-uiLeft)/3;
		if (saving) {
			clickableTextCentre(uiLeft, bottom, xCentre1, "Confirm", &confirmSave);
			clickableTextCentre(xCentre1, bottom, xCentre2, "Cancel", &cancelSave);
		} else if (lastSavedLevelName.length > 0) {
			clickableTextCentre(uiLeft, bottom, xCentre1, "Save As", { saving = true; });
			clickableTextCentre(xCentre1, bottom, xCentre2, "Rename", &startRename);
		} else {
			clickableTextCentre(uiLeft, bottom, xCentre2, "Save As", { saving = true; });
		}
		clickableTextCentre(xCentre2, bottom, uiRight, "Main Menu", &mainMenu);
	}

	bool checkSaved() {
		if (!modified) return true;
		//Buttons parameter doesn't work in Windows :(
		int answer = al_show_native_message_box(display, "Unsaved changes", "Unsaved changes", "Save changes?", null, ALLEGRO_MESSAGEBOX_QUESTION | ALLEGRO_MESSAGEBOX_YES_NO);
		if (answer == 1) { saving = true; endLevelTest(); return false; }
		answer = al_show_native_message_box(display, "Unsaved changes", "Unsaved changes", "Discard changes?", null, ALLEGRO_MESSAGEBOX_QUESTION | ALLEGRO_MESSAGEBOX_YES_NO);
		return answer == 1;
	}

	void onLevelNameClick(string levelName) {
		if (saving) {
			myLevelName = levelName;
		} else {
			if (!checkSaved()) return;
			try {
				level.load(levelsPath ~ levelName ~ levelExt);
				lastSavedLevelName = myLevelName = levelName;
				modified = false;
			} catch (Exception e) {
				al_show_native_message_box(display, "Error", "Error", e.msg.toStringz(), null, ALLEGRO_MESSAGEBOX_ERROR);
				lastSavedLevelName = myLevelName = "";
				modified = false;
			}
		}
	}

	void startRename() {
		saving = renaming = true;

		//Remove our old level name from the list.
		auto ranges = allLevelNames.assumeSorted.trisect(myLevelName);
		int oursStart = ranges[0].length;
		int laterStart = oursStart + ranges[1].length;
		allLevelNames.replaceInPlace(oursStart, laterStart, cast(string[])[]);
	}

	void confirmSave() {
		if (myLevelName.length == 0) {
			al_show_native_message_box(display, "Error", "Error", "Please type a name!", null, ALLEGRO_MESSAGEBOX_ERROR);
			return;
		}

		try {
			mkdirRecurse(levelsPath);
			level.save(levelsPath ~ myLevelName ~ levelExt);
			if (renaming) remove(levelsPath ~ lastSavedLevelName ~ levelExt);
			lastSavedLevelName = myLevelName;
			saving = renaming = false;
			modified = false;
			GetLevelNames(allLevelNames);
		} catch (Exception e) {
			al_show_native_message_box(display, "Error", "Error", e.msg.toStringz(), null, ALLEGRO_MESSAGEBOX_ERROR);
		}
	}

	void cancelSave() {
		myLevelName = lastSavedLevelName;
		saving = renaming = false;
		GetLevelNames(allLevelNames);
	}

	void mainMenu() {
		if (checkSaved()) { delete theEditor; theEditor = null; }
	}
}
