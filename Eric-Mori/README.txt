Mori

--

ABOUT:
This game was created by Eric Johnson (not the guitarist) in 2016 between May 13 and May 16 for TINS 2016 (http://tins.amarillion.org/2016/). In the game, you play as an eyepatch-toting blue blob while exploring a seemingly randomly-generated forest full of trees, flowers, mushrooms and more. Interact with the environment, trade with the native forest folk, cover the world with snow, and enjoy the product of sleep deprivation!

--

CONTROLS:
Z = move item selector left
X = use selected item
C = move item selector right
Arrow keys = move around

--

CREDITS:
Art and programming: Eric Johnson
Sound effects from: http://www.superflashbros.net/as3sfxr/ and http://sfbgames.com/chiptone/
Font: http://www.geocities.jp/littlimi/misaki.htm

--

HELP:
Cut down trees to get acorns and sticks.
Trade with forest folk to get snowballs and bones.
Cover the world in snowballs to win!

--

BUGS:
- Sometimes message dialog doesn't appear.
- Player occasionally spawns in the same spot as an NPC.

--

TIPS:
- Sometimes NPCs will want a snowball. Trading snowballs for snowballs is a gamble, but sometimes you come out on top.

--

FINAL NOTES:
The source code is a mess, but that's to be expected from a product of a rushed game competition. I have little experience writing Makefiles, so mine might not work for you. I generally just use pkg-config on my Linux machine: g++ -Wall -std=c++11 -I. -o Mori src/*.cpp $(pkg-config --libs allegro-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 allegro_ttf-5 allegro_font-5 allegro_primitives-5)

Set the game seed by launching it from a terminal (example: ./mori 123456789). By default, the current epoch time is used as the seed.

This was my first time participating in this kind of event, so don't expect much.

--

Here's the gist of the competition's rules:

**** There will be 1 genre rule

genre rule #89
Craftsmanship: The game is centered around crafting items out of components.

**** There will be 2 artistical rules

artistical rule #47
Include dialog in poetic form (rhymed couplets, limerick, haiku) as much as you can

artistical rule #48
Include snow

**** There will be 2 technical rules

technical rule #48
path finding

technical rule #55
use unicode to display non-english characters (e.g. russian, japanese). If you don't know any of these languages, that doesn't matter. Just use a phrase.

**** There will be 1 bonus rule

bonus rule #3
Act of Formaldehyde: you can opt out of a rule if instead you decide to follow three other rules from a previous Speedhack/TINS. The rules page containing those rules should still be up for all to see.
