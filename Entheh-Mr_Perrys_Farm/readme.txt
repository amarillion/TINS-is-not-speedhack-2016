Mr Perry's Farm
TINS Allegro Game Programming Competition 2016
tins.amarillion.org/2016
Ben 'Bruce "entheh" Perry' Wieczorek-Davis
ben@wieczorekdavis.com


Compiling
---------

I've written my entry in D this time. D is a wonderful language which offers
a lot of the safety you get from Java or C# - things like garbage collection,
bounds checking, everything initialised to a default value - while still
compiling to native. Also, the deeper I get into it, the more fantastically
good design decisions I find - global variables being thread-local by
default, type syntax like const(char)* being less confusing ...

However, that does mean it might be one of the less trivial entries to
compile.

The blurb above and the compilation instructions below are largely copied and
pasted from my TINS 2012 entry, and it's a while since I actually set D up.
So I hope the "bare minimum of instructions" I wrote last time will be
sufficient again!

I have included a Windows executable. If you downloaded from the TINS site
where there is a size limit, you will need to obtain the DLLs separately.
Otherwise at the time of writing you can get a zip including DLLs from:

http://bdavis.strangesoft.net/MrPerrysFarm.zip


Licence
-------

- You may distribute this game unmodified, or with minimal portability fixes,
  or with an added executable, or both - provided that you do it strictly for
  non-profit purposes.

- You may of course enjoy the game to your heart's content.

- All other rights are reserved.


Compiling the game
------------------

You will need Allegro 5.2, and you will need it built with libpng. For
Windows users, the easiest way to get it these days is via NuGet in
Visual Studio.

You can get D from:

http://dlang.org/

I wrote the game using D 2.063.2. The language does change, so other versions
may require tweaks to the code, but hopefully nothing major.

You will also need to get SiegeLord's D bindings from here:

https://github.com/SiegeLord/DAllegro5

Follow SiegeLord's instructions to get up and running. You'll probably need
to generate D-format 'lib' files that map on to Allegro's DLLs. You might
also need other D dependencies.

D has the concept of 'import' paths - these are directories where other .d
files can be found, sort of. To be precise, if my file says
"import allegro5.allegro;" in it, and you've specified an import path of
"C:\DAllegro5", then the D compiler will need to be able to find
"C:\DAllegro5\allegro5\allegro.d".

D also has library search paths - just specify the directories containing
Allegro 5's lib files.

You may need to mess around with DAllegro5's 'pragma(lib, "...")' lines -
search for pragma. It depends how your Allegro build is arranged. Or
alternatively you can just specify the required libraries on the command
line. Note that there is a library called dallegro5.lib which you need in
addition to the Allegro ones.

Other than that, note that D tends to like to compile everything together.
Specify all the .d files at once.

For Windows, you're best building it as a console application until you've
established that it runs without crashing, as exceptions are logged to the
console. Once it works, I recommend compiling with -L/SUBSYSTEM:WINDOWS:4.0
- this will make it a Windows application instead of a console one.


Rules
-----

Genre:
- Craftsmanship: You get to build a machine out of component parts.
Technical:
- Pathfinding: The animals do this. There's even a trick to get them to go roughly diagonally!
- Unicode: On the main menu and sometimes over the map during the 'Go!' phase.
Artistic:
- Include dialogue in poetic form: Present during the 'Go!' phase, and I wrote my own poetry for this!
- Include snow: Machine components can trigger various weather conditions, including snow.
Bonus:
- Act of Formaldehyde: Not implemented.
- Retro size rule: Not implemented.


Re-use of code and other materials
----------------------------------

This time I genuinely did everything from scratch for the competition,
with two notable exceptions: the font (which isn't mine), and this readme :P
(and the obvious stuff like Allegro).

The font included is slightly modified from the original:
I made the space narrower and made the digits all the same width.
The filename is adjusted accordingly so I don't misrepresent the font as the original.
The original font licence file, with original URL, is also included.

I drew all the other graphics myself on a Cintiq 24HD.
I recorded all the (rather incomplete set of) sound effects myself using a Zoom H2n.
