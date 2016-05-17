UNIT Config;
(*<This unit loads the config file and parses the command-line options.  It
  also gets the correct path for data files. *)
(* Copyright (c) 2012-2016 Guillermo Martínez J.

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

{$MODE OBJFPC} { Because uses some objects from RTL }

INTERFACE

  CONST
    VERSION_STR = 'TINS 2016';
    COPYRIGHT_STR = '(c) Guillermo Martínez J., 2016';

  VAR
  (* If set, use full-screen mode. *)
    FullScreen: BOOLEAN;
  (* Sound and music volume. *)
    VolumeFX, VolumeMusic: INTEGER;

(* TODO: CHEATER *)
  HARRY, ARNOLD: BOOLEAN;

(* Loads the configuration file and parses the command-line options, setting
   all configuration values.  Should be called after init Allegro.
   @returns(@true if everything is Ok, @false if it shouldn't execute the game
     @(i.e. command-line help@).)
   @Error(Raises an exception explaining the problem.) *)
  FUNCTION Load: BOOLEAN;

(* Saves the current configuration. *)
  PROCEDURE Save;

IMPLEMENTATION

  USES
    LogFile,
    Classes, sysutils;

(* Loads the configuration file and parses the command-line options, setting
   all configuration values.  Should be called after init Allegro.
   @Error(Raises an exception explaining the problem.) *)
  FUNCTION Load: BOOLEAN;

    PROCEDURE GetConfigFromFile; INLINE;
    VAR
      CfgFile: TStringList;

      FUNCTION GetValue (Name, Default: STRING): STRING; INLINE;
      BEGIN
	GetValue := CfgFile.Values[Name];
	IF GetValue = '' THEN
	  GetValue := Default;
      END;

      FUNCTION GetInteger (Name: STRING; Default: INTEGER): INTEGER; INLINE;
      BEGIN
	GetInteger := StrToInt (GetValue (Name, IntToStr (Default)));
      END;

    BEGIN
      CfgFile := TStringList.Create;
      TRY
        IF FileExists (GetAppConfigFile (FALSE, TRUE)) THEN
	  CfgFile.LoadFromFile (GetAppConfigFile (FALSE, TRUE));
      { Loads the configuration file and sets the values. }
      { LogFile.SetLevel (GetInteger ('debug_level', LOG_LEVEL_ERROR)); }
      { Default should be "ERROR" but now we put "ALL" for debugging.
	LogFile.SetLevel (GetInteger ('debug_level', LOG_LEVEL_DEBUG));
	}
        LogFile.SetEvents (etAll);
LogFile.TraceLn ('WARNING: debug_level configuration is forced for ALL.  Change this before release.', etWarning);
	{
	FullScreen := GetValue ('fullscreen', 'true') = 'true';
       Default should be 'true' but now is 'false' because it's easer for debugging.
       }
	FullScreen := GetValue ('fullscreen', 'false') = 'true';
LogFile.TraceLn ('WARNING: FullScreen configuration modified for debugging.  Change this before release.', etWarning);
      FINALLY
	CfgFile.Free;
      END;
{ CHEATER }
      HARRY := FALSE;
      ARNOLD := FALSE;
    END;

    PROCEDURE GetConfigFromCommandLine; INLINE;
    VAR
      Cnt: INTEGER;
    BEGIN
      LogFile.TraceLn ('  Checking command line options...', etDebug);
      WriteLn;
      WriteLn ('TINS 2016 ', VERSION_STR);
      WriteLn (COPYRIGHT_STR);
      WriteLn;
      Cnt := 1;
      REPEAT
	IF (ParamStr (Cnt) = '-h') OR (ParamStr (Cnt) = '--help') THEN
	BEGIN
	  LogFile.TraceLn ('    Help requested.', etInfo);
	  WriteLn ('Usage:');
	  WriteLn ('  ', ApplicationName, ' [options]');
	  WriteLn;
	  WriteLn ('[options]');
	  WriteLn ('  -h, --help      : This help.');
	  WriteLn ('  -f, --fullscreen: Run in fullscreen mode.');
	  WriteLn ('  -w, --windowed  : Run in windowed mode.');
	  WriteLn ('  -l<l>, --log<l> : Set the log level, where <l> can be');
	  WriteLn ('    0 (none), 1 (errors) or 2 (debugging).');
	  WriteLn ('    Log file is created at the working directory.');
	  WriteLn ('  -lall : Set the log level to show everyting');
	  WriteLn;
	  Load := FALSE;
	END
	ELSE IF (ParamStr (Cnt) = '-f')
	OR (ParamStr (Cnt) = '--fullscreen') THEN
	  FullScreen := TRUE
	ELSE IF (ParamStr (Cnt) = '-w')
	OR (ParamStr (Cnt) = '--windowed') THEN
	  FullScreen := FALSE
	ELSE IF (ParamStr (Cnt) = '-l0') OR (ParamStr (Cnt) = '-log0') THEN
	  LogFile.SetEvents ([])
	ELSE IF (ParamStr (Cnt) = '-l1') OR (ParamStr (Cnt) = '-log1') THEN
	  LogFile.SetEvents ([etError])
	ELSE IF (ParamStr (Cnt) = '-l2') OR (ParamStr (Cnt) = '-log2') THEN
	  LogFile.SetEvents (etDbg)
	ELSE IF (ParamStr (Cnt) = '-lall') THEN
	  LogFile.SetEvents (etAll)
	ELSE IF ParamStr (Cnt) = 'harry' THEN
	  HARRY := TRUE
	ELSE IF ParamStr (Cnt) = 'arnold' THEN
	  ARNOLD := TRUE
	ELSE BEGIN
	  LogFile.TraceLn ('    Unknown command line option: "'+ParamStr (Cnt)+'"', etError);
	  WriteLn ('Unknown command line option.');
	  WriteLn ('Use "', ApplicationName,' --help" for command help.');
	  WriteLn;
	  Load := FALSE;
	END;
	INC (Cnt);
      UNTIL Cnt > ParamCount
    END;

  BEGIN
    Load := TRUE;
    GetConfigFromFile;
    IF ParamCount > 0 THEN
      GetConfigFromCommandLine
  END;



(* Saves the current configuration. *)
  PROCEDURE Save;
  VAR
    ConfigFile: TStringList;
  BEGIN
    ConfigFile := TStringList.Create;
    TRY
      IF FullScreen THEN
	ConfigFile.Add ('fullscreen=true')
      ELSE
	ConfigFile.Add ('fullscreen=false');
      ConfigFile.Add ('');
    {
      ConfigFile.Add ('# Delete next variable to deactivate the log file.');
      ConfigFile.Add ('#  0 - Nothing.');
      ConfigFile.Add ('#  1 - Messages and errors.  Nice to debug characters.');
      ConfigFile.Add ('#  2 - Internal debugging.');
      ConfigFile.Add ('debug_level='+IntToStr (LogFile.Level + 1));
    }
      ConfigFile.SaveToFile (GetAppConfigFile (FALSE));
      LogFile.TraceLn ('Configuration file saved at '+GetAppConfigFile(FALSE), etInfo);
    FINALLY
      ConfigFile.Free;
    END;
  END;



(* Returns the path where the data is. *)
  FUNCTION DataPath: STRING;
  BEGIN
  { TODO: Should look for the path, but at the moment it uses this: }
    DataPath := IncludeTrailingPathDelimiter ('./data/');
  END;

END.
