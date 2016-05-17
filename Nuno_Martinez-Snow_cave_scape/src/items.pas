UNIT Items;
(*<Defines stuff for game items. *)
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
    Engine;

  CONST
    LabWarningText: ARRAY [0..3] OF STRING = (
      'Roses are red',
      'Sky is blue',
      'Mix acid with base',
      'And will do BOOM!'
    );

    EndBookText: ARRAY [0..19] OF STRING = (
      'It says:',
      'Dear player,',
      'I tryed hard,',
      'but I was too ambitious.',
      'So I''m not able to',
      'add more puzzles.',
      'I''m very sorry.',
      'May be I''ll continue',
      'and complete the game.',
      'May be a very different',
      'one, or following the',
      'story I imagined...',
      'See the README file',
      'for more information.',
      'I hope you like what I did.',
      'Ñuño Martínez',
      'P.S: My name needs UTF-8.',
      'Does it count?',
      'I''ll write "России"',
      'just to be sure.'
    );

  TYPE
  (* Chemical. *)
    TChemical = CLASS (TItem)
    PUBLIC
    (* Constructor. *)
      CONSTRUCTOR CreateFlask (CONST aName: STRING);
    (* Use. *)
      PROCEDURE UseWithObject (CONST aObject: INTEGER); OVERRIDE;
    END;



  (* Flask with explosive. *)
    TExplosiveFlask = CLASS (TItem)
    PRIVATE
      Cnt: INTEGER;
      WillExpode: BOOLEAN;
    PUBLIC
    (* Constructor. *)
      CONSTRUCTOR CreateFlask;
    (* Use. *)
      PROCEDURE UseWithObject (CONST aObject: INTEGER); OVERRIDE;
    (* Updates. *)
      PROCEDURE Update; OVERRIDE;
    END;


  VAR
  (* Index to explosive Flask. *)
    IdExplosiveFlask: INTEGER;
  (* First ice wall. *)
    IdIceWallLab: INTEGER;
  (* Second room. *)
    SecondRoom: INTEGER;

IMPLEMENTATION

  USES
    Application;

(*****************************************************************************
 * TChemical
 *)

(* Constructor. *)
  CONSTRUCTOR TChemical.CreateFlask (CONST aName: STRING);
  BEGIN
    IF aName = 'base' THEN
      INHERITED CreateObjectInRoom (
	'flask_base.png', 'flask', '"Sodium"', Room, 80, 86
      )
    ELSE IF aName = 'acid' THEN
      INHERITED CreateObjectInRoom (
	'flask_acid.png', 'flask', '"Sulfur trioxide"', Room, 116, 88
      )
    ELSE
      INHERITED Create;
  END;



(* Use. *)
  PROCEDURE TChemical.UseWithObject (CONST aObject: INTEGER);
  BEGIN
    IF (aObject >= 0) AND (Owner.Items[aObject].Name = 'flask') THEN
    BEGIN
      Owner.Player.RemoveFromInventory (SELF.Ndx);
      Owner.Player.RemoveFromInventory (aObject);
      SELF.Room := -2;
      Owner.Items[aObject].Room := -2;
      Owner.Player.AddToInventory (IdExplosiveFlask);
      Owner.Player.Say ('Playing chemicals...')
    END
    ELSE
      INHERITED UseWithObject (aObject)
  END;



(*****************************************************************************
 * TExplosiveFlask
 *)

(* Constructor. *)
  CONSTRUCTOR TExplosiveFlask.CreateFlask;
  BEGIN
    INHERITED CreateObject (
      'flask_explosive.png', 'hot flask', 'Is getting hotter...'
    );
    SELF.Fixed := TRUE;
    SELF.WillExpode := FALSE;
  END;



(* Use. *)
  PROCEDURE TExplosiveFlask.UseWithObject (CONST aObject: INTEGER);
  BEGIN
    IF NOT WillExpode AND (SELF.Room = -1) AND (aObject = IdIceWallLab) THEN
    BEGIN
      WillExpode := TRUE;
      Owner.Player.RemoveFromInventory (SELF.Ndx);
      SELF.Room := Owner.Player.Room;
      SELF.X := Owner.Player.X + 8;
      SELF.Y := Owner.Player.Y - 10;
      Cnt := FPS * 5
    END
    ELSE
      INHERITED UseWithObject (aObject)
  END;



(* Updates. *)
  PROCEDURE TExplosiveFlask.Update;
  BEGIN
    IF WillExpode THEN
    BEGIN
      Dec (Cnt);
      IF Cnt < 1 THEN
      BEGIN
	Owner.Rooms[SELF.Room].AddExit (
	  TRoomExit.Create (
	    'Exit',
	    300, 90, 19, 46,
	    SecondRoom, 18, 153
	  )
	);
        SELF.Room := -2;
        Owner.Items[IdIceWallLab].Room := -2;
      END
    END;
    INHERITED Update
  END;

END.
