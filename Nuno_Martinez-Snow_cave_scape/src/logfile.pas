UNIT LogFile;
(*<Creates a "log file" useful to debug.  It can be activated or deactivated in
  runtime.

   This code is barely inspired in the TRACE from Allagro by Shawn Hargreaves.
  *)
(*
  Copyright (c) 2006-2016 Guillermo Martínez J.

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
    sysutils;

  CONST
  (* The log file name.  You can change it as you wish. *)
    LOG_FILE_NAME = 'tins2016.log';
  (* Logs debug only. *)
    etDbg = [etWarning, etError, etDebug];
  (* To log all events. *)
    etAll = [etCustom, etInfo, etWarning, etError, etDebug];



(* Sets the event to log. @seealso(Events) @seealso(Trace) *)
  PROCEDURE SetEvents (aEvents: TEventTypes);

(* Returns events that will be logged. @seealso(SetEvents) @seealso(Trace) *)
  FUNCTION Events: TEventTypes;

(* Sends a trace message.
   @param(Msg The text message.)
   @param(aEvent The event type.)
   @seealso(SetEvents) @seealso(Events) @seealso(TraceLn) *)
  PROCEDURE Trace (CONST Msg: STRING; aEvent: TEventType);

(* Sends a trace message and adds a break line.
   @param(aEvent The event type.)
   @seealso(SetEvents) @seealso(Events) @seealso(Trace) *)
  PROCEDURE TraceLn (CONST Msg: STRING; aEvent: TEventType); INLINE;

IMPLEMENTATION

  VAR
    fEvents: TEventTypes = [];
    TheFile: TEXT;
    Opened: BOOLEAN;



(* Set's events to log. *)
  PROCEDURE SetEvents (aEvents: TEventTypes);
  BEGIN
    IF fEvents <> [] THEN
    BEGIN
      TraceLn ('End of log', etDebug);
      TraceLn ('^^^^^^^^^^', etDebug)
    END;
    fEvents := aEvents;
    IF fEvents <> [] THEN
    BEGIN
      TraceLn ('vvvvvvvvvvvvvvvv', etDebug);
      TraceLn ('Init log file...', etDebug)
    END
  END;



(* Returns current events logged. *)
  FUNCTION Events: TEventTypes;
  BEGIN
    EXIT (fEvents)
  END;



(* Sends a trace message. *)
  PROCEDURE Trace (CONST Msg: STRING; aEvent: TEventType);
  BEGIN
    IF aEvent IN fEvents THEN
    BEGIN
      IF NOT Opened THEN
      BEGIN
	Rewrite (TheFile);
	Opened := TRUE
      END;
      CASE aEvent OF
      etWarning:
	Write (TheFile, '/!\ ');
      etError:
	Write (TheFile, '/E\ ');
      etDebug:
	Write (TheFile, '[d] ');
      ELSE
	Write (TheFile, '    ');
      END;
      Write (TheFile, Msg)
    END
  END;

  PROCEDURE TraceLn (CONST Msg: STRING; aEvent: TEventType);
  BEGIN
    Trace (Msg+#10, aEvent)
  END;

INITIALIZATION
  Assign (TheFile, './'+LOG_FILE_NAME);
  Opened := FALSE
FINALIZATION
  IF Opened THEN Close (TheFile)
END.
