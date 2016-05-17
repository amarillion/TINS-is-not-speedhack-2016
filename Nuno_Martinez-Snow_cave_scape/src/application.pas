UNIT Application;
(*<Defines and manages application stuff. *)
(* Copyright (c) 2016 Guillermo Martínez J.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*)

INTERFACE

  USES
    Engine,
    Allegro5, al5font;

  CONST
  (* Window title. *)
    TITLE = 'TINS 2016';
  (* Screen configuration. *)
    WIDTH = 800; HEIGHT = 600; BPP = 32;
  (* Game speed in frames per second. *)
    FPS = 50;

  TYPE
  (* The game class. *)
    TGame = CLASS (TObject)
    PRIVATE
      fDisplay: ALLEGRO_DISPLAYptr;
      fTimer: ALLEGRO_TIMERptr;
      fEventQueue: ALLEGRO_EVENT_QUEUEptr;
      fFont: ALLEGRO_FONTptr;
      fTerminated: BOOLEAN;
      fMouseBmp: ALLEGRO_BITMAPptr;

      fGameMap: TGameMap;

      FUNCTION CreateDisplay: BOOLEAN;
      PROCEDURE NewGame;
    PUBLIC
    (* Destructor. *)
      DESTRUCTOR Destroy; OVERRIDE;
    (* Initializes the application. *)
      PROCEDURE Initialize;
    (* Game execution. *)
      PROCEDURE Run;
    (* Terminates the application. *)
      PROCEDURE Terminate;
    (* Show error message. *)
      PROCEDURE ShowErrorMessage (CONST Message: STRING);

    (* Is game terminated? *)
      PROPERTY Terminated: BOOLEAN READ fTerminated;
    (* Text font. *)
      PROPERTY Font: ALLEGRO_FONTptr READ fFont;
    END;

  VAR
  (* Global reference to game. *)
    TheGame: TGame;
  (* Some colors. *)
    Black, White: ALLEGRO_COLOR;
  (* Zoom scale. *)
    Zoom: DOUBLE;


IMPLEMENTATION

  USES
    Config, Items, LogFile,
    al5image, al5nativedlg, al5ttf,
    sysutils;

(*****************************************************************************
 * TGame
 *)

  FUNCTION TGame.CreateDisplay: BOOLEAN;
  VAR
    DisplayFlags: INTEGER;
  BEGIN
    Config.Load;

    LogFile.TraceLn ('Creating Display...', etDebug);
    LogFile.TraceLn ('  Setting display options...', etInfo);
  {
    IF Config.FullScreen THEN
      DisplayFlags := ALLEGRO_FULLSCREEN
    ELSE
  }
      DisplayFlags := ALLEGRO_WINDOWED;
    al_set_new_display_flags (DisplayFlags);
    al_set_new_display_option (ALLEGRO_DEPTH_SIZE, BPP, ALLEGRO_SUGGEST);
    al_set_new_display_option (ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
    LogFile.TraceLn ('  Creating the display...', etInfo);
    fDisplay := al_create_display (WIDTH, HEIGHT);
    IF fDisplay = NIL THEN
    BEGIN
      LogFile.TraceLn ('Could not create the display!', etError);
      EXIT (FALSE)
    END;
    al_set_window_title (fDisplay, TITLE);
    al_register_event_source (fEventQueue, al_get_display_event_source (fDisplay));
    LogFile.TraceLn ('Display created [OK]', etDebug);
    RESULT := TRUE
  END;



  PROCEDURE TGame.NewGame;
  VAR
    Room, Item: INTEGER;
  BEGIN
    fGameMap.Clear;

    Room := fGameMap.AddRoom (TRoom.Create ('chemlab.png'));
    IdIceWallLab := fGameMap.AddItem (TItem.CreateObjectInRoom (
      'icewall_lab.png', 'ice', 'Hard water',
      Room, 288, 92
    ));
    fGameMap.Items[IdIceWallLab].Fixed := TRUE;
    Item := fGameMap.AddItem (TItem.CreateObjectInRoom (
      'poster_lab.png', 'poster', 'Warning',
      Room, 242, 86
    ));
    fGameMap.Items[Item].Fixed := TRUE;
    fGameMap.Items[Item].SetLongText (LabWarningText);
    Item := fGameMap.AddItem (TChemical.CreateFlask ('base'));
    Item := fGameMap.AddItem (TChemical.CreateFlask ('acid'));
    IdExplosiveFlask := fGameMap.AddItem (TExplosiveFlask.CreateFlask);

    SecondRoom := fGameMap.AddRoom (TRoom.Create ('room.png'));
    fGameMap.Rooms[SecondRoom].AddExit (
      TRoomExit.Create (
        'Exit',
	8, 94, 17, 86,
	Room, 310, 153
      )
    );
    Item := fGameMap.AddItem (TItem.CreateObjectInRoom (
      'end_book.png', 'book', 'book',
      SecondRoom, 150, 141
    ));
    fGameMap.Items[Item].SetLongText (EndBookText);

    fGameMap.Player.ResetInitial
  END;



(* Destructor. *)
  DESTRUCTOR TGame.Destroy;
  BEGIN
    LogFile.TraceLn ('Releasing stuff...', etDebug);
    FreeAndNil (fGameMap);
    IF fEventQueue <> NIL THEN
    BEGIN
      IF fDisplay <> NIL THEN
	al_unregister_event_source (fEventQueue, al_get_display_event_source (fDisplay));
      al_unregister_event_source (fEventQueue, al_get_timer_event_source (fTimer));
      al_destroy_event_queue (fEventQueue);
    END;
    IF fFont <> NIL THEN al_destroy_font (fFont);
    IF fMouseBmp <> NIL THEN al_destroy_bitmap (fMouseBmp);
    IF fDisplay <> NIL THEN
    BEGIN
      LogFile.TraceLn ('  Closing display...', etDebug);
      al_destroy_display (fDisplay)
    END;
    INHERITED Destroy
  END;



(* Initialize. *)
  PROCEDURE TGame.Initialize;
  BEGIN
  { Set up Allegro and add-ons. }
    LogFile.TraceLn ('Initialising system...', etDebug);
    IF al_init THEN
    BEGIN
      LogFile.TraceLn ('Initialising Allegro5...', etInfo);
    { At the moment, as bitmap management isn't very sophisticated. }
      al_set_new_bitmap_flags (ALLEGRO_MEMORY_BITMAP);

      al_install_keyboard;
      al_install_mouse;
      al_init_image_addon;
      al_init_ttf_addon;
      fTimer := al_create_timer (ALLEGRO_BPS_TO_SECS (FPS));
    { The event queue. }
      fEventQueue := al_create_event_queue;
      al_register_event_source (fEventQueue, al_get_keyboard_event_source);
      al_register_event_source (fEventQueue, al_get_mouse_event_source);
      al_register_event_source (fEventQueue, al_get_timer_event_source (fTimer));
      IF CreateDisplay THEN
      BEGIN
	Black := al_map_rgb_f (0, 0, 0);
	White := al_map_rgb_f (1, 1, 1);
	Zoom := WIDTH / 320; TraceLn (Format ('Zoom: %f', [Zoom]), etInfo);
      { Fonts. }
	TraceLn ('Loading text font...', etInfo);
        fFont := al_load_ttf_font ('data/FSEX300.ttf', TRUNC (16 * Zoom), ALLEGRO_TTF_MONOCHROME);
	IF fFont <> NIL THEN
	BEGIN
	{ Mouse cursor. }
	  fMouseBmp := LoadBitmap ('mouse.png');
          al_hide_mouse_cursor (fDisplay);
        { Game. }
	  fGameMap := TGameMap.Create;
	  fTerminated := FALSE;
	  EXIT
	END
	ELSE
	  SELF.ShowErrorMessage ('  Can''t load "FSEX300.png".')
      END
    END
    ELSE
      SELF.ShowErrorMessage ('  Can''t initialize Allegro5!');
    fTerminated := TRUE
  END;



(* Run game. *)
  PROCEDURE TGame.Run;
  VAR
    Event: ALLEGRO_EVENT;
    mX, mY: INTEGER;

    PROCEDURE HandleMouseClick (aX, aY: INTEGER); INLINE;
    BEGIN
      aX := TRUNC (aX / Zoom);
      aY := TRUNC (aY / Zoom);
    { If click in playfield. }
      IF aY <= L_BOTTOM THEN
      BEGIN
	IF fGameMap.Player.Action = acUse THEN
	BEGIN
	  IF fGameMap.Player.SelectedItem < 0 THEN
	    fGameMap.Player.SelectedItem := fGameMap.FindItem (aX, aY)
	  ELSE
	    fGameMap.Player.GoToPlace (aX, aY + 4)
	END
	ELSE BEGIN
	  fGameMap.Player.GoToPlace (aX, aY + 4);
	  fGameMap.Player.SelectedItem := fGameMap.FindItem (aX, aY);
	  IF fGameMap.Player.SelectedItem < 0 THEN
	    fGameMap.Player.SelectedExit := fGameMap.FindExit (aX, aY);
	END;
      END
    { If click in UI. }
      ELSE BEGIN
        IF aX <= 320 DIV 2 THEN
        BEGIN
          IF aX <= 320 DIV 4 THEN
          BEGIN
            IF aY <= L_BOTTOM + 24 + 16 THEN
              fGameMap.Player.Action := acWalkTo
            ELSE
              fGameMap.Player.Action := acPickUp
          END
          ELSE BEGIN
            IF aY <= L_BOTTOM + 24 + 16 THEN
              fGameMap.Player.Action := acLookAt
            ELSE
              fGameMap.Player.Action := acUse
          END
        END
        ELSE BEGIN
	  IF fGameMap.Player.Action = acUse THEN
	  BEGIN
	    IF fGameMap.Player.SelectedItem < 0 THEN
	      fGameMap.Player.SelectedItem := fGameMap.FindItem (aX, aY)
	    ELSE
	      fGameMap.Player.DoSomething
	  END
	  ELSE BEGIN
	    fGameMap.Player.SelectedItem := fGameMap.FindItem (aX, aY);
	    fGameMap.Player.DoSomething
	  END
        END
      END
    END;

    PROCEDURE HandleMouseMovement (CONST aX, aY: INTEGER); INLINE;
    BEGIN
      mX := TRUNC (aX / Zoom);
      mY := TRUNC (aY / Zoom);
      fGameMap.Player.Item := fGameMap.FindItem (mX, mY);
      IF fGameMap.Player.Item < 0 THEN
	fGameMap.Player.Exit := fGameMap.FindExit (mX, mY);
      mX := TRUNC (mX * Zoom);
      mY := TRUNC (mY * Zoom)
    END;

  BEGIN
    IF NOT fTerminated THEN
    TRY
      SELF.NewGame;
      al_start_timer (fTimer);
      REPEAT
	al_wait_for_event (fEventQueue, Event);
	CASE Event._type OF
	ALLEGRO_EVENT_DISPLAY_CLOSE:
	  SELF.Terminate;
	ALLEGRO_EVENT_KEY_DOWN:
	  IF Event.keyboard.keycode = ALLEGRO_KEY_ESCAPE THEN SELF.Terminate;
	ALLEGRO_EVENT_MOUSE_AXES:
	  HandleMouseMovement (Event.mouse.x, Event.mouse.y);
	ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
          HandleMouseClick (
            TRUNC (Event.mouse.x), TRUNC (Event.mouse.y)
	  );
	ALLEGRO_EVENT_TIMER:
	  BEGIN
	    fGameMap.Update;
	    IF al_is_event_queue_empty (fEventQueue) THEN
	    BEGIN
	      al_set_target_bitmap (al_get_backbuffer (fDisplay));
              al_clear_to_color (Black);
	      fGameMap.Draw;
	      al_draw_scaled_bitmap (
		fMouseBmp, 0, 0, 16, 16, mX - 16, my - 16, 16 * Zoom, 16 * Zoom, 0
	      );
	      al_flip_display
	    END
	  END;
	END
      UNTIL fTerminated
    EXCEPT
      ON Error: Exception DO
      BEGIN
        SELF.ShowErrorMessage (Error.Message);
	SELF.Terminate
      END
    END
  END;



(* Terminate. *)
  PROCEDURE TGame.Terminate;
  BEGIN
    LogFile.TraceLn ('Game termination requested...', etDebug);
    fTerminated := TRUE
  END;



(* Show error message. *)
  PROCEDURE TGame.ShowErrorMessage (CONST Message: STRING);
  BEGIN
    TraceLn (Message, etError);
    al_show_native_message_box (
      SELF.fDisplay, 'Error', 'An error raised:', Message,
      '', ALLEGRO_MESSAGEBOX_ERROR
    )
  END;

INITIALIZATION
  ;
FINALIZATION
  FreeAndNil (TheGame)
END.
