================================
Happy Usagi no Yuki Fortress
幸せなウサギの雪要塞
================================

TINS 2016 Competition entry by Max & Amarillion

--------------------------------
            How to play
--------------------------------

Happy Usagi no Yuki Fortress is a sandbox game,
without a win condition (The game continues forever). 

Your goal is to keep a bunch of bunnies happy,
well-fed, and to buid a nice play
environment for them out of cardboard boxes.

== Feeding == 

When you start, there are empty food containers
in the field. You need to refill then using the 
refill button. Click on the button, and then click on
the container that you want to 

== Building == 

To build, click on one of the buttons on the right.
Then click with the mouse in the field to place

If you don't have enough money for a certain item, the
button will be disabled (greyed out).

Use the "delete" button to remove an item that was 
misplaced or to make space. You don't get back your 
money - no refunds.

At any point you can right-click or escape to 
cancel the current action (building, deleting or
refilling)

You can stack certain items on top of each other:
boxes on top of boxes, and food containers on top
of boxes. To stack items high, move the mouse to the 
*floor position* where you want to stack above.

== Maximising Money == 

You get "free" money over time. So you'll always
get more money simply by waiting.

However, you can grow your stash more quickly by 
encouraging certain behaviours. There are two
moves that generate extra cash: hops to a higher / lower
level, and binkies.

To encourage hops, place boxes in the field.
If you stack food on top of boxes, then bunnies
will follow the smell and are even more likely to hop.
(but note that bunnies need to stand next to the food,
so there needs to be enough space on the higher platform to eat)
 
To encourage binkies, feed them well. A well-fed bunny
is happier and more likely to do a binky. 

Furthermore, both hops and binkies generate more cash
if they are performed at higher level, so make
sure you stack some boxes on top of each other and lure
the bunnies up with some food.

== Bunny behaviour ==

See if you can observe the following bunny behaviours:

* Sleeping
* Eating
* Walking
* Hopping
* Standing to sniff the air
* Running (and sliding on slippery snow)
* Binkies - a crazy flip-jump in the air
(Binkies are a real thing! See https://www.youtube.com/watch?v=_HJ6weqeiQw)

--------------------------------
            Credits
--------------------------------

Code & Music : Amarillion
Graphics : Max

Inspiration: Alis & Boterham

The original sound track "Boterham's happy tune" was imagined by Max and
 sequenced by Amarillion (using milkytracker)

Sound effects generated with as3sfxr 
(http://www.superflashbros.net/as3sfxr/)

A lot of the engine / framework code was re-used from
our TINS 2012 entry "laundryDay", updated to use Allegro 5.

--------------------------------
How does the game fit the rules?
--------------------------------

		**** There will be 1 genre rule
		
		genre rule #89
		Craftsmanship: The game is centered around crafting items out of components.


The goal of the game is to craft a fortress for the bunnies out of small and big boxes


	**** There will be 2 artistical rules
	
	artistical rule #47
	Include dialog in poetic form (rhymed couplets, limerick, haiku) as much as you can


We wrote a haiku that is displayed at the start of the game


	artistical rule #48
	Include snow

During the game, you will see occasional bouts of snowfall.
If it gets very snowy, your bunnies might slip and crash 
(don't worry, this doesn't hurt them much)

	**** There will be 2 technical rules
	
	technical rule #48
	path finding


Bunnies find their path towards the food through a food-smell-diffusion system.
Each cell in the xyz grid has a "smell" (During the game, press F5 to enter
debug mode and see the "smell" in each of the ground cells). Smell is adapted
each tick, with addition  of fresh smell around food sources, diffusing the smell
to neighbouring cells by averaging, and decaying the smell by substraction. 
During movement, bunnies tend to move towards places where the food smell is higher

For added realism, there is some randomness. The bunnies don't move continuously 
towards the food, but they are definitely attracted towards the smell of the food.

	technical rule #55
	use unicode to display non-english characters (e.g. russian, japanese). If you don't know any of these languages, that doesn't matter. Just use a phrase.

Unicode is displayed in the main menu, as well as in the "Buy Bunny" button. 
Also, the setting of the game (a patio outside a japanese fortress) is inspired by this rule.

	**** There will be 1 bonus rule
	
	bonus rule #3
	Act of Formaldehyde: you can opt out of a rule if instead you decide to follow two other rules from a previous Speedhack/TINS. The rules page containing those rules should still be up for all to see.


We do not invoke the bonus rule.
