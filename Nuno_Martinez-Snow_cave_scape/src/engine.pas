UNIT Engine;
(*<Defines the engine structs and procedures. *)
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
    Allegro5;

  CONST
  (* Available actions. *)
    StrActions: ARRAY [0..3] OF STRING = (
      'Walk to ', 'Look at ', 'Pick up ', 'Use '
    );
  (* Positions and limits. *)
    P_W = 32;
    P_H = 43;
    P_X = 320 DIV 2;
    P_Y = 160;
    L_TOP = 127;
    L_BOTTOM = 240 - 64;
    L_LEFT = 8 + (P_W DIV 2);
    L_RIGHT = 320 - 8 - (P_W DIV 2);
    P_LEFT = 0;
    P_RIGHT = ALLEGRO_FLIP_HORIZONTAL;
    P_SPEED = 2;


  TYPE
  (* @exclude forward. *)
    TGameMap = CLASS;
  (* @exclude *)
    TRoom = CLASS;
  (* @exclude *)
    TPlayer = CLASS;
  (* @exclude *)
    TItem = CLASS;

  (* Identifies actions. *)
    TAction = (acWalkTo, acLookAt, acPickUp, acUse);

  (* Stores and manages an animation. *)
    TAnimation = CLASS (TObject)
    PRIVATE
      fBitmaps: ARRAY OF ALLEGRO_BITMAPptr;
      fCurrentFrame, fCnt, fFrameSpeed: INTEGER;

      FUNCTION GetBmp (CONST Ndx: INTEGER): ALLEGRO_BITMAPptr; INLINE;
    PUBLIC
    (* Destructor. *)
      DESTRUCTOR Destroy; OVERRIDE;
    (* Loads from bitmap. *)
      PROCEDURE LoadFromFile (CONST FileName: STRING; CONST aFrameWidth: INTEGER);
    (* Animation from bitmap. *)
      PROCEDURE FromBitmap (aBmp: ALLEGRO_BITMAPptr; CONST aFrameWidth: INTEGER);
    (* Resets animation. *)
      PROCEDURE Reset;
    (* Next frame. *)
      PROCEDURE NextFrame; INLINE;
    (* Current bitmap. *)
      FUNCTION CurrentBmp: ALLEGRO_BITMAPptr; INLINE;

    (* Access to frames. *)
      PROPERTY Frame[Ndx: INTEGER]: ALLEGRO_BITMAPptr READ GetBmp;
    (* Current frame. *)
      PROPERTY Current: INTEGER READ fCurrentFrame;
    (* Frame speed.  The bigger, the slower. *)
      PROPERTY Speed: INTEGER READ fFrameSpeed WRITE fFrameSpeed;
    END;



  (* Base class for all items. *)
    TBaseItem = CLASS (TObject)
    PRIVATE
      fX, fY: INTEGER;
      fGameMap: TGameMap;
    PROTECTED
    (* Owner of item. *)
      PROPERTY Owner: TGameMap READ fGameMap;
    PUBLIC
    (* Item position. *)
      PROPERTY X: INTEGER READ fX WRITE fX;
    (* Item position. *)
      PROPERTY Y: INTEGER READ fY WRITE fY;
    END;



  (* Base for selectable items. *)
    TSelectableItem = CLASS (TBaseItem)
    PRIVATE
      fName: STRING;
      fW, fH: INTEGER;
    PUBLIC
    (* Checks if point is inside object. *)
      FUNCTION Inside (CONST aX, aY: INTEGER): BOOLEAN;

    (* Item name. *)
      PROPERTY Name: STRING READ fName WRITE fName;
    (* Item size. *)
      PROPERTY W: INTEGER READ fW WRITE fW;
    (* Item size. *)
      PROPERTY H: INTEGER READ fH WRITE fH;
    END;



  (* Defines a game. *)
    TGameMap = CLASS (TObject)
    PRIVATE
      fRooms: ARRAY OF TRoom;
      fItems: ARRAY OF TItem;
      fPlayer: TPlayer;
      fUIBmp: ALLEGRO_BITMAPptr;

      FUNCTION GetRoom (CONST Ndx: INTEGER): TRoom; INLINE;
      FUNCTION GetItem (CONST Ndx: INTEGER): TItem; INLINE;
    PUBLIC
    (* Creates game. *)
      CONSTRUCTOR Create;
    (* Destructor. *)
      DESTRUCTOR Destroy; OVERRIDE;
    (* Adds a room. *)
      FUNCTION AddRoom (aRoom: TRoom): INTEGER;
    (* Adds an item. *)
      FUNCTION AddItem (aItem: TItem): INTEGER;
    (* Clears the game data. *)
      PROCEDURE Clear;
    (* Returns the item index in coordinates. *)
      FUNCTION FindItem (CONST aX, aY: INTEGER): INTEGER;
    (* Returns the exit index in coordinates. *)
      FUNCTION FindExit (CONST aX, aY: INTEGER): INTEGER;
    (* Updates. *)
      PROCEDURE Update;
    (* Renders the stuff. *)
      PROCEDURE Draw;

    (* Access to rooms. *)
      PROPERTY Rooms[Ndx: INTEGER]: TRoom READ GetRoom;
    (* Access to items. *)
      PROPERTY Items[Ndx: INTEGER]: TItem READ GetItem;
    (* The player. *)
      PROPERTY Player: TPlayer READ fPlayer;
    END;



  (* Defines an exit. *)
    TRoomExit = CLASS (TSelectableItem)
    PRIVATE
      fToRoom, feX, feY: INTEGER;
    PUBLIC
    (* Creates an exit. *)
      CONSTRUCTOR Create (
	CONST aName: STRING;
	CONST aX, aY, aW, aH, aToRoom, aToX, aToY: INTEGER
      );
    { Room, and detection rect. }
      PROPERTY ToRoom: INTEGER READ fToRoom WRITE fToRoom;
    { Where it appears in new room. }
      PROPERTY ToX: INTEGER READ feX WRITE feX;
      PROPERTY ToY: INTEGER READ feY WRITE feY;
    END;



  (* Defines a room. *)
    TRoom = CLASS (TObject)
    PRIVATE
      fBackground: ALLEGRO_BITMAPptr;
      fExits: ARRAY OF TRoomExit;
    PUBLIC
    (* Creates room. *)
      CONSTRUCTOR Create (CONST BmpFile: STRING);
    (* Destructor. *)
      DESTRUCTOR Destroy; OVERRIDE;
    (* Adds a new exit. *)
      PROCEDURE AddExit (aExit: TRoomExit);
    END;



  (* An item. *)
    TItem = CLASS (TSelectableItem)
    PRIVATE
      fRoom, fNdx: INTEGER;
      fAnimation: TAnimation;
      fDescription: STRING;
      fFixed: BOOLEAN;
      fLongtext: ARRAY OF STRING;
    PROTECTED
      PROCEDURE LoadAnimation (CONST BmpFile: STRING);
    (* Object index in item list. *)
      PROPERTY Ndx: INTEGER READ fNdx;
    PUBLIC
    (* Creates item. *)
      CONSTRUCTOR CreateObject (
	CONST BmpFile, aName, aDescription: STRING
      );
    (* Creates item. *)
      CONSTRUCTOR CreateObjectInRoom (
	CONST BmpFile, aName, aDescription: STRING;
	CONST aRoom, aX, aY: INTEGER
      );
    (* Destructor. *)
      DESTRUCTOR Destroy; OVERRIDE;
    (* Sets long text. *)
      PROCEDURE SetLongText (aText: ARRAY OF STRING);
    (* Use object. *)
      PROCEDURE UseWithObject (CONST aObject: INTEGER); VIRTUAL;
    (* Updates. *)
      PROCEDURE Update; VIRTUAL;
    (* Renders. *)
      PROCEDURE Draw;

    (* Item description. *)
      PROPERTY Description: STRING READ fDescription WRITE fDescription;
    (* Room index. (-1) inventory, (-2) don't exists. *)
      PROPERTY Room: INTEGER READ fRoom WRITE fRoom;
    (* Object is fixed and can't be picked up. *)
      PROPERTY Fixed: BOOLEAN READ fFixed WRITE fFixed;
    END;



  (* Player states. *)
    TPlayerState = (psWaiting, psWalking, psTalking);

  (* The player. *)
    TPlayer = CLASS (TBaseItem)
    PRIVATE
      fStand: ALLEGRO_BITMAPptr;
      fWalk: TAnimation;
      fRoom, fDirection,
      fDestX, fDestY: INTEGER;
      fState: TPlayerState;
      fAction: TAction;
      fCnt, fLine,
      fItem, fSelectedItem,
      fExit, fSelectedExit: INTEGER;
      fInventory: ARRAY OF INTEGER;
      fTalkingText: ARRAY OF STRING;

      PROCEDURE SortInventoryXY;
    PUBLIC
    (* Constructor. *)
      CONSTRUCTOR Create;
    (* Destructor. *)
      DESTRUCTOR Destroy; OVERRIDE;
    (* Sets to initial room center. *)
      PROCEDURE ResetInitial;
    (* Tells player to go to the given position. *)
      PROCEDURE GoToPlace (CONST dX, dY: INTEGER);
    (* Tells player that should do something.  What it does depends on
       @link(Action). *)
      PROCEDURE DoSomething;
    (* The player will talk with this. *)
      PROCEDURE Say (CONST aText: STRING);
      PROCEDURE SayText (aText: ARRAY OF STRING);
    (* Adds item to inventory. *)
      PROCEDURE AddToInventory (CONST aItem: INTEGER);
    (* Remove item from inventory. *)
      PROCEDURE RemoveFromInventory (CONST aItem: INTEGER);
    (* Updates. *)
      PROCEDURE Update;
    (* Returns a string that tells whats doing. *)
      FUNCTION ActionDescription: STRING;
    (* Renders. *)
      PROCEDURE Draw;

    (* Current room. *)
      PROPERTY Room: INTEGER READ fRoom;
    (* Current action. *)
      PROPERTY Action: TAction READ fAction WRITE fAction;
    (* Pointed item. *)
      PROPERTY Item: INTEGER READ fItem WRITE fItem;
    (* Selected item. *)
      PROPERTY SelectedItem: INTEGER READ fSelectedItem WRITE fSelectedItem;
    (* Pointed exit. *)
      PROPERTY Exit: INTEGER READ fExit WRITE fExit;
    (* Selected exit. *)
      PROPERTY SelectedExit: INTEGER READ fSelectedExit WRITE fSelectedExit;
    END;



(* Loads a bitmap from data directory, raising an exception on error. *)
  FUNCTION LoadBitmap (CONST BmpFile: STRING): ALLEGRO_BITMAPptr;

IMPLEMENTATION

  USES
    Application, LogFile,
    al5font,
    sysutils;

(* Load bitmap. *)
  FUNCTION LoadBitmap (CONST BmpFile: STRING): ALLEGRO_BITMAPptr;
  BEGIN
    TraceLn (Format ('Loading "%s"...', [BmpFile]), etDebug);
    RESULT := al_load_bitmap ('data/'+BmpFile);
    IF RESULT = NIL THEN
      RAISE Exception.CreateFmt ('Can''t load "%s".', [BmpFile])
  END;



(*****************************************************************************
 * TAnimation
 *)

  FUNCTION TAnimation.GetBmp (CONST Ndx: INTEGER): ALLEGRO_BITMAPptr;
  BEGIN
    RESULT := fBitmaps[Ndx]
  END;



(* Destructor. *)
  DESTRUCTOR TAnimation.Destroy;
  VAR
    Ndx: INTEGER;
  BEGIN
    IF Length (fBitmaps) > 0 THEN
      FOR Ndx := LOW (fBitmaps) TO HIGH (fBitmaps) DO
	IF fBitmaps[Ndx] <> NIL THEN al_destroy_bitmap (fBitmaps[Ndx]);
    INHERITED Destroy
  END;



(* Loads from bitmap. *)
  PROCEDURE TAnimation.LoadFromFile
    (CONST FileName: STRING; CONST aFrameWidth: INTEGER);
  VAR
    aBmp: ALLEGRO_BITMAPptr;
  BEGIN
    IF Length (fBitmaps) > 0 THEN
      RAISE Exception.Create ('Could not load animation twice!');
    TraceLn (Format ('Loading animation "%s"...', [FileName]), etDebug);
    aBmp := LoadBitmap (FileName);
    TRY
      SELF.FromBitmap (aBmp, aFrameWidth)
    FINALLY
      IF aBmp <> NIL THEN al_destroy_bitmap (aBmp)
    END
  END;



(* Creates from bitmap. *)
  PROCEDURE TAnimation.FromBitmap
    (aBmp: ALLEGRO_BITMAPptr; CONST aFrameWidth: INTEGER);
  VAR
    Num, aFrameHeight: INTEGER;
    Transparency: ALLEGRO_COLOR;
  BEGIN
    fFrameSpeed := 10;
    IF Length (fBitmaps) > 0 THEN
      RAISE Exception.Create ('Could not load animation twice!');
    Num := al_get_bitmap_width (aBmp) DIV aFrameWidth;
    IF Num < 1 THEN
      RAISE Exception.Create ('Can''t build animation: 0 frames found!');
    aFrameHeight := al_get_bitmap_height (aBmp);
    SetLength (fBitmaps, Num);
    TraceLn (Format ('  Loading %d frames...', [Num]), etInfo);
    Transparency := al_map_rgba_f (0, 0, 0, 0);
    al_set_blender (ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ALPHA);
    FOR Num := LOW (fBitmaps) TO HIGH (fBitmaps) DO
    BEGIN
      fBitmaps[Num] := al_create_bitmap (aFrameWidth, aFrameHeight);
      IF fBitmaps[Num] = NIL THEN
	RAISE Exception.Create ('Error building animation.');
      al_set_target_bitmap (fBitmaps[Num]);
      al_clear_to_color (Transparency);
      al_draw_bitmap_region (
        aBmp, Num * aFrameWidth, 0, aFrameWidth, aFrameHeight,
        0, 0, 0
      )
    END
  END;



(* Resets animation. *)
  PROCEDURE TAnimation.Reset;
  BEGIN
    fCurrentFrame := 0;
    fCnt := 0
  END;



(* Next frame. *)
  PROCEDURE TAnimation.NextFrame;
  BEGIN
    INC (fCnt);
    IF fCnt > fFrameSpeed THEN
    BEGIN
      fCnt := 0;
      INC (fCurrentFrame);
      IF fCurrentFrame > HIGH (fBitmaps) THEN fCurrentFrame := 0
    END
  END;



(* Current frame. *)
  FUNCTION TAnimation.CurrentBmp: ALLEGRO_BITMAPptr;
  BEGIN
    RESULT := fBitmaps[fCurrentFrame]
  END;



(*****************************************************************************
 * TSelectableItem
 *)

(* Checks if point is inside object. *)
  FUNCTION TSelectableItem.Inside (CONST aX, aY: INTEGER): BOOLEAN;
  BEGIN
    RESULT := (fX <= aX) AND (aX <= (fX + fW))
          AND (fY <= aY) AND (aY <= (fY + fH))
  END;



(*****************************************************************************
 * TGameMap
 *)

  FUNCTION TGameMap.GetRoom (CONST Ndx: INTEGER): TRoom;
  BEGIN
    RESULT := fRooms[Ndx]
  END;



  FUNCTION TGameMap.GetItem (CONST Ndx: INTEGER): TItem;
  BEGIN
    RESULT := fItems[Ndx]
  END;



(* Constructor. *)
  CONSTRUCTOR TGameMap.Create;
  VAR
    X, Y, N: INTEGER;
  BEGIN
    INHERITED Create;
    fPlayer := TPlayer.Create;
    fPlayer.fGameMap := SELF;
  { User interface. }
    TraceLn ('Creating user interface...', etDebug);
    fUIBmp := al_create_bitmap (TRUNC (320 * Zoom), TRUNC (64 * Zoom));
    N := 0;
    al_set_target_bitmap (fUIBmp);
    al_clear_to_color (Black);
    FOR Y := 0 TO 1 DO FOR X := 0 TO 1 DO
    BEGIN
      al_draw_text (
        TheGame.Font, White, ((X * 80) + 40) * Zoom, ((Y * 24) + 16) * Zoom,
        ALLEGRO_ALIGN_CENTRE, StrActions[N]
      );
      INC (N)
    END
  END;



(* Destructor. *)
  DESTRUCTOR TGameMap.Destroy;
  BEGIN
    IF fUIBmp <> NIL THEN al_destroy_bitmap (fUIBmp);
    SELF.Clear;
    fPlayer.Free;
    INHERITED Destroy
  END;



(* Adds a room. *)
  FUNCTION TGameMap.AddRoom (aRoom: TRoom): INTEGER;
  BEGIN
    RESULT := Length (fRooms);
    SetLength (fRooms, RESULT + 1);
    fRooms[RESULT] := aRoom
  END;



(* Adds an Item. *)
  FUNCTION TGameMap.AddItem (aItem: TItem): INTEGER;
  BEGIN
    RESULT := Length (fItems);
    SetLength (fItems, RESULT + 1);
    fItems[RESULT] := aItem;
    aItem.fGameMap := SELF;
    aItem.fNdx := RESULT
  END;



(* Clears game data. *)
  PROCEDURE TGameMap.Clear;
  VAR
    Ndx: INTEGER;
  BEGIN
    FOR Ndx := LOW (fRooms) TO HIGH (fRooms) DO
      IF fRooms[Ndx] <> NIL THEN fRooms[Ndx].Free;
    SetLength (fRooms, 0);
    FOR Ndx := LOW (fItems) TO HIGH (fItems) DO
      IF fItems[Ndx] <> NIL THEN fItems[Ndx].Free;
    SetLength (fItems, 0)
  END;



(* Find item in coordinates. *)
  FUNCTION TGameMap.FindItem (CONST aX, aY: INTEGER): INTEGER;
  VAR
    Ndx: INTEGER;
  BEGIN
    FOR Ndx := LOW (fItems) TO HIGH (fItems) DO
      IF ((fItems[Ndx].fRoom = fPlayer.fRoom) OR (fItems[Ndx].fRoom = -1))
      AND fItems[Ndx].Inside (aX, aY)
      THEN
	EXIT (Ndx);
    RESULT := -1
  END;



(* Find exit in coordinates. *)
  FUNCTION TGameMap.FindExit (CONST aX, aY: INTEGER): INTEGER;
  VAR
    Ndx: INTEGER;
    aRoom: TRoom;
  BEGIN
    aRoom := fRooms[fPlayer.Room];
    FOR Ndx := LOW (aRoom.fExits) TO HIGH (aRoom.fExits) DO
      IF aRoom.fExits[Ndx].Inside (aX, aY) THEN
	EXIT (Ndx);
    RESULT := -1
  END;



(* Updates. *)
  PROCEDURE TGameMap.Update;
  VAR
    Ndx: INTEGER;
  BEGIN
    FOR Ndx := LOW (fItems) TO HIGH (fItems) DO
      IF (fItems[Ndx].fRoom = fPlayer.fRoom)
      OR (fItems[Ndx].fRoom = -1)
      THEN
        fItems[Ndx].Update;
    fPlayer.Update
  END;



(* Draw. *)
  PROCEDURE TGameMap.Draw;
  VAR
    Ndx: INTEGER;
  BEGIN
    al_set_blender (ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_draw_scaled_rotated_bitmap (
      fRooms[fPlayer.fRoom].fBackground, 0, 0, 0, 0, Zoom, Zoom, 0, 0
    );
  { Don't zoom UI because it uses the BIG FONT! See game initialization. }
    al_draw_scaled_rotated_bitmap (
      fUIBmp, 0, 0, 0, (240 - 64) * Zoom, 1, 1, 0, 0
    );
    al_draw_text (
      TheGame.Font, White, (320 DIV 2) * Zoom, (240 - 64) * Zoom,
      ALLEGRO_ALIGN_CENTRE, fPlayer.ActionDescription
    );
  { Set alpha for sprites. }
    al_set_blender (ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
  { And draw sprites. }
    FOR Ndx := LOW (fItems) TO HIGH (fItems) DO
      IF (fItems[Ndx].fRoom = fPlayer.fRoom)
      OR (fItems[Ndx].fRoom = -1)
      THEN
        fItems[Ndx].Draw;
    fPlayer.Draw;
  END;



(*****************************************************************************
 * TRoomExit
 *)

(* Creates an exit. *)
  CONSTRUCTOR TRoomExit.Create
    (CONST aName: STRING; CONST aX, aY, aW, aH, aToRoom, aToX, aToY: INTEGER);
  BEGIN
    INHERITED Create;
    SELF.Name := aName;
    SELF.X := aX;
    SELF.Y := aY;
    SELF.W := aW;
    SELF.H := aH;
    SELF.fToRoom := aToRoom;
    SELF.feX := aToX;
    SELF.feY := aToY
  END;



(*****************************************************************************
 * TRoom
 *)

(* Constructor. *)
  CONSTRUCTOR TRoom.Create (CONST BmpFile: STRING);
  BEGIN
    INHERITED Create;
    fBackground := LoadBitmap (BmpFile)
  END;



(* Destructor. *)
  DESTRUCTOR TRoom.Destroy;
  VAR
    Ndx: INTEGER;
  BEGIN
    FOR Ndx := LOW (fExits) TO HIGH (fExits) DO FreeAndNil (fExits[Ndx]);
    IF fBackground <> NIL THEN al_destroy_bitmap (fBackground);
    INHERITED Destroy
  END;



(* Adds an exit. *)
  PROCEDURE TRoom.AddExit (aExit: TRoomExit);
  VAR
    Ndx: INTEGER;
  BEGIN
    Ndx := Length (fExits);
    SetLength (fExits, Ndx + 1);
    fExits[Ndx] := aExit
  END;



(*****************************************************************************
 * TItem
 *)

  PROCEDURE TItem.LoadAnimation (CONST BmpFile: STRING);
  VAR
    aBmp: ALLEGRO_BITMAPptr;
  BEGIN
    FreeAndNil (fAnimation);
    aBmp := LoadBitmap (BmpFile);
    fAnimation := TAnimation.Create;
    TRY
      fW := al_get_bitmap_width (aBmp);
      fH := al_get_bitmap_height (aBmp);
      IF fW > FH THEN 
        fAnimation.FromBitmap (aBmp, fH)
      ELSE
        fAnimation.FromBitmap (aBmp, fW)
    FINALLY
      IF aBmp <> NIL THEN al_destroy_bitmap (aBmp)
    END
  END;



(* Constructor. *)
  CONSTRUCTOR TItem.CreateObject
    (CONST BmpFile, aName, aDescription: STRING);
  BEGIN
    INHERITED Create;
    SELF.LoadAnimation (BmpFile);
    fName := aName;
    fDescription := aDescription;
    SELF.Room := -2;
    fFixed := FALSE
  END;

  CONSTRUCTOR TItem.CreateObjectInRoom
    (CONST BmpFile, aName, aDescription: STRING; CONST aRoom, aX, aY: INTEGER);
  BEGIN
    INHERITED Create;
    SELF.LoadAnimation (BmpFile);
    fName := aName;
    fDescription := aDescription;
    SELF.Room := aRoom;
    SELF.X := aX;
    SELF.Y := aY;
    fFixed := FALSE
  END;



(* Destructor. *)
  DESTRUCTOR TItem.Destroy;
  BEGIN
    FreeAndNil (fAnimation);
    INHERITED Destroy
  END;



(* Sets long text. *)
  PROCEDURE TItem.SetLongText (aText: ARRAY OF STRING);
  VAR
    aNdx: INTEGER;
  BEGIN
    SetLength (fLongtext, Length (aText));
    FOR aNdx := LOW (aText) TO HIGH (aText) DO fLongtext[aNdx] := aText[aNdx]
  END;



(* Use object. *)
  PROCEDURE TItem.UseWithObject (CONST aObject: INTEGER);
  BEGIN
    Owner.Player.Say ('Can''t do that')
  END;



(* Updates. *)
  PROCEDURE TItem.Update;
  BEGIN
    fAnimation.NextFrame
  END;



(* Draw. *)
  PROCEDURE TItem.Draw;
  BEGIN
    al_draw_scaled_bitmap (
      fAnimation.CurrentBmp,
      0, 0, fW, fH,
      fX * Zoom, fY * Zoom, fW * Zoom, fH * Zoom,
      0
    )
  END;



(*****************************************************************************
 * TPlayer
 *)

  PROCEDURE TPlayer.SortInventoryXY;
  VAR
    Ndx, iX, iY: INTEGER;
  BEGIN
    iX := 320 DIV 2; iY := L_BOTTOM + 16;
    FOR Ndx := LOW (fInventory) TO HIGH (fInventory) DO
      IF SELF.Owner.Items[fInventory[Ndx]].fRoom = -1 THEN
      BEGIN
        SELF.Owner.Items[fInventory[Ndx]].X := iX;
        SELF.Owner.Items[fInventory[Ndx]].Y := iY;
        INC (iX, 32);
        IF (iX + 32) > 320 THEN
        BEGIN
          INC (iY, 32);
          iX := 320 DIV 2
        END
      END
  END;



(* Constructor. *)
  CONSTRUCTOR TPlayer.Create;
  BEGIN
    INHERITED Create;
    TraceLn ('Loading player...', etDebug);
    fStand := LoadBitmap ('player_stand.png');
    fWalk := TAnimation.Create;
    fWalk.LoadFromFile ('player_walk.png', P_W);
    fWalk.Speed := 5
  END;



(* Destructor. *)
  DESTRUCTOR TPlayer.Destroy;
  BEGIN
    IF fStand <> NIL THEN al_destroy_bitmap (fStand);
    FreeAndNil (fWalk);
    INHERITED Destroy
  END;



(* Sets to initial room center. *)
  PROCEDURE TPlayer.ResetInitial;
  BEGIN
    fRoom := 0;
    fX := P_X; fY := P_Y;
    fDirection := P_RIGHT;
    fWalk.Reset;
    fAction := acWalkTo;
    fItem := -1; fSelectedItem := -1;
    fExit := -1; fSelectedExit := -1;
    fState := psWaiting
  END;



(* Tells player to go to the given position. *)
  PROCEDURE TPlayer.GoToPlace (CONST dX, dY: INTEGER);
  BEGIN
    IF (dX <> fX) OR (dY <> fY) THEN
    BEGIN
      TraceLn (Format ('Player walking to %d, %d', [dX, dY]), etInfo);
      fDestX := dX; fDestY := dY;
    { Some limits on movement. }
      IF dY > L_BOTTOM THEN fDestY := L_BOTTOM;
      IF dY < L_TOP THEN fDestY := L_TOP;
      IF dX < fX THEN fDestX := dX + (P_W DIV 3);
      IF dX > fX THEN fDestX := dX - (P_W DIV 3);
      IF fDestX < L_LEFT THEN
	fDestX := L_LEFT
      ELSE IF fDestX > L_RIGHT THEN
	fDestX := L_RIGHT;
      fWalk.Reset;
      fState := psWalking;
      IF fX < fDestX THEN fDirection := P_RIGHT ELSE fDirection := P_LEFT
    END
  END;



(* Force do something. *)
  PROCEDURE TPlayer.DoSomething;
  BEGIN
  { Simulates that Player is near destination, because action is triggered
    after a walk. }
    fDestX := fX; fDestY := fY;
    DEC (fX); DEC (fY); { Such small walk will not be seen. }
    fState := psWalking
  END;



(* The player will talk with this. *)
  PROCEDURE TPlayer.Say (CONST aText: STRING);
  BEGIN
TraceLn ('Saying "'+aText+'"', etDebug);
    fState := psTalking;
    SetLength (fTalkingText, 1);
    fTalkingText[0] := aText;
    fCnt := FPS * 5; fLine := 0
  END;

  PROCEDURE TPlayer.SayText (aText: ARRAY OF STRING);
  VAR
    Ndx: INTEGER;
  BEGIN
TraceLn ('Saying "'+IntToStr (Length (aText))+'" text lines.', etDebug);
    fState := psTalking;
    SetLength (fTalkingText, Length (aText));
    FOR Ndx := LOW (aText) TO HIGH (aText) DO fTalkingText[Ndx] := aText[Ndx];
    fCnt := FPS * 2; fLine := 0;
TraceLn ('First line: "'+fTalkingText[0]+'".', etInfo);
  END;



(* Add to inventory. *)
  PROCEDURE TPlayer.AddToInventory (CONST aItem: INTEGER);
  VAR
    Ndx: INTEGER;
  BEGIN
  { Only if item isn't in inventory. }
    IF SELF.Owner.Items[aItem].Room <> -1 THEN
    BEGIN
      Ndx := Length (fInventory);
      SetLength (fInventory, Ndx + 1);
      fInventory[Ndx] := aItem;
      SELF.Owner.Items[aItem].Room := -1;
      SELF.SortInventoryXY
    END
    ELSE
      SELF.Say ('I have one yet...')
  END;



(* Remove from inventory. *)
  PROCEDURE TPlayer.RemoveFromInventory (CONST aItem: INTEGER);
  VAR
    Ndx, Ndx2: INTEGER;
  BEGIN
  { Only if item isn't in inventory. }
    IF SELF.Owner.Items[aItem].Room = -1 THEN
    BEGIN
      FOR Ndx := LOW (fInventory) TO HIGH (fInventory) DO
	IF fInventory[Ndx] = aItem THEN
	BEGIN
	  IF Ndx < HIGH (fInventory) THEN
	    FOR Ndx2 := Ndx + 1 TO HIGH (fInventory) DO
	      fInventory[Ndx - 1] := fInventory[Ndx];
	  SetLength (fInventory, HIGH (fInventory));
	  Owner.fItems[aItem].fRoom := -2
	END
    END
  END;



(* Updates. *)
  PROCEDURE TPlayer.Update;
  BEGIN
    CASE fState OF
    psWalking:
      BEGIN
      { Animation. }
	fWalk.NextFrame;
      { Motion. }
	IF fX < fDestX THEN INC (fX, P_SPEED);
	IF fX > fDestX THEN DEC (fX, P_SPEED);
	IF fY < fDestY THEN INC (fY, P_SPEED);
	IF fY > fDestY THEN DEC (fY, P_SPEED);
	IF (ABS (fX - fDestX) < p_SPEED)
	AND (ABS (fY - fDestY) < P_SPEED)
	THEN BEGIN
	  fX := fDestX; fY := fDestY;
	  fState := psWaiting;
          IF fSelectedItem >= 0 THEN
            CASE fAction OF
	    acWalkTo:
	      ; { Nothing to do. }
            acLookAt:
	      BEGIN
		IF Length (SELF.Owner.Items[fSelectedItem].fLongtext) > 0 THEN
		  SELF.SayText (SELF.Owner.Items[fSelectedItem].fLongtext)
		ELSE
		  SELF.Say (SELF.Owner.Items[fSelectedItem].Description)
	      END;
            acPickUp:
	      BEGIN
		IF SELF.Owner.Items[fSelectedItem].Fixed THEN
		  SELF.Say ('I can''t pick that')
		ELSE
		  SELF.AddToInventory (fSelectedItem);
	      END;
	    acUse:
	      SELF.Owner.Items[fSelectedItem].UseWithObject (fItem);
	    ELSE
              SELF.Say ('I can''t do that');
            END
          ELSE IF fSelectedExit >= 0 THEN
            CASE fAction OF
	    acWalkTo:
	      BEGIN
		SELF.X :=
		  SELF.Owner.fRooms[SELF.fRoom].fExits[fSelectedExit].ToX;
		SELF.Y :=
		  SELF.Owner.fRooms[SELF.fRoom].fExits[fSelectedExit].ToY;
		SELF.fRoom :=
		  SELF.Owner.fRooms[SELF.fRoom].fExits[fSelectedExit].ToRoom
	      END;
            acLookAt:
              SELF.Say (
	        SELF.Owner.fRooms[SELF.fRoom].fExits[fSelectedExit].Name
	      );
	    ELSE
              SELF.Say ('I can''t do that');
            END;
          fAction := acWalkTo;
          fItem := -1; fSelectedItem := -1;
	  fExit := -1; fSelectedExit := -1
	END
      END;
    psTalking:
      BEGIN
        DEC (fCnt);
        IF fCnt < 1 THEN
	BEGIN
	  INC (fLine);
	  IF fLine < Length (fTalkingText) THEN
	  BEGIN
	    fCnt := FPS * 2;
TraceLn (Format ('Line %d: "%s".', [fLine, fTalkingText[fLine]]), etInfo);
	  END
	  ELSE
	    fState := psWaiting
	END
      END;
    ELSE
      fState := psWaiting;
    END;
  END;



(* Get action description. *)
  FUNCTION TPlayer.ActionDescription: STRING;
  BEGIN
    RESULT := StrActions[ORD (fAction)];
  { Don't select the same object twice. }
    IF fItem = fSelectedItem THEN fItem := -1;
  { Now, show what's selected: }
    IF fSelectedItem >= 0 THEN
    BEGIN
      RESULT :=
	RESULT + SELF.Owner.Items[fSelectedItem].Name;
      IF (fAction = acUse) AND (fItem >= 0) THEN
	RESULT :=
	  RESULT + ' with ' + SELF.Owner.Items[fItem].Name
    END
    ELSE IF fSelectedExit >= 0 THEN
      RESULT :=
	RESULT + SELF.Owner.fRooms[SELF.fRoom].fExits[fSelectedExit].Name
    ELSE IF fItem >= 0 THEN
      RESULT := RESULT + SELF.Owner.Items[fItem].Name
    ELSE IF fExit >= 0 THEN
      RESULT :=
	RESULT + SELF.Owner.fRooms[SELF.fRoom].fExits[fExit].Name
  END;



(* Draw. *)
  PROCEDURE TPlayer.Draw;
  VAR
    TheFrame: ALLEGRO_BITMAPptr;
  BEGIN
    CASE fState OF
    psWalking:
      TheFrame := fWalk.CurrentBmp;
    psTalking:
      BEGIN
        TheFrame := fStand;
        al_draw_text (
          TheGame.Font, Black,
          (fX + 1) * Zoom, (fY - P_H - 16 + 1) * Zoom,
          ALLEGRO_ALIGN_CENTRE, fTalkingText[fLine]
        );
        al_draw_text (
          TheGame.Font, White,
          fX  * Zoom, (fY - P_H - 16) * Zoom,
          ALLEGRO_ALIGN_CENTRE, fTalkingText[fLine]
        );
      END;
    ELSE
      TheFrame := fStand;
    END;
    al_draw_scaled_bitmap (
      TheFrame,
      0, 0, P_W, P_H,
      (fX - (P_W DIV 2)) * Zoom, (fY - P_H) * Zoom, 32 * Zoom, 43 * Zoom,
      fDirection
    );
  END;

END.
