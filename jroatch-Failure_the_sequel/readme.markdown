Failed game for TINS 2016  
Copyright (C) 2016  Johnathan Roatch

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

-----------------------------------------------------------------------
Keyboard Controls:
- Left/Right arrow keys:
      Move Left/Right
- Spacebar
      Jump
- f / F11
      Fullscreen
- Ctrl+q, Alt+F4, Esc
      Quit
- Tilde
      Debug info

-----------------------------------------------------------------------
Credits:
ProggyTinySZBP.png
    http://upperbounds.net/

DroidSans.ttf
    https://www.google.com/fonts/specimen/Droid+Sans

cabin_fever.xm by Drozerix
    http://modarchive.org/index.php?request=view_profile&query=84702

-----------------------------------------------------------------------
# TINS 2016 Rule implementation #

TL;DR: I failed.

## Genre requirements ##
Craftsmanship:
    TODO.
    The plan was to find stuff in the platformer maze to progress further.
    
## Technical requirements ##
Include poetic dialog:
    TODO
    The plan was to use Droid Sans to display Russian poetry between levels.

Include snow:
    As a presistant particle effect.
    
## Artistic requirements ##
path finding:
    TODO
    The plan was to have a tilemaped maze for you to navigate.

non-english unicode characters:
    TODO
    The plan was to use Droid Sans to Russian poetry between levels.

## Act of Formaldehyde bonus rule ##
Incase a rule fulfillment wasn't good, I used the following three past runes
to replace that rule:
- Speedhack 2007 - Respect Screen Ratio.
    Fullscreen mode will scale and crop the screen for screen ratios
    between 16:9 (wide) and 2:3 (tall).
- TINS 2012 - parallax scrolling
    The background and snowfall has Z depth which
    effects their horizontal scroll speed.
- Speedhack 2005 - Make it a Classic: Four button joystick
    The controls only uses four directions arrow keys and a jump button.

## Retro size rule ##
    Under 400KiB before and after compression.

-----------------------------------------------------------------------
# Tools and Libraries Used #
- Allegro version: 5.2.0
- IDE: gedit 3.20.2 / Xfce 4.12
- Compiler/IDE: GCC 6.1.1 and GNU Make 4.1
- Platform: Arch GNU/Linux
- Language: C
- Allegro 5 add-ons used:
    - image (with JPG, and PNG support)
    - audio
    - acodec (with ogg, and XM playback)
    - font
    - ttf
    - native_dialog (for errors only)
