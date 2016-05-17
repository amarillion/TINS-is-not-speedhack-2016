module Text;
import LevelModule;

string[8][] poems = [
	[
		"Did you know, today's the day",
		"When farm equipment goes astray?",
		"But there's a simple way to stop",
		"This little mishap up to pop.",
		"It's oh so easy, you will find,",
		"And Mr Perry's wife won't mind.",
		"You must just see to it that she",
		"Is always under lock and key.",
	],
	[
		"Today on Mr Perry's Farm,",
		"Some animals would come to harm.",
		"The pigs, all five, will not come home;",
		"Explosions launched them all to Rome.",
		"So now we're in the deadly throes",
		"Of cows that bit off Baby's toes,",
		"And ducks that fell into a ditch,",
		"And sheep that disobeyed the dog.",
	],
	[
		"Greetings, little girls and boys!",
		"I'm here to introduce the joys",
		"Of Mr Perry's Farm, where you",
		"Can hear our many cows say \"Moo\".",
		"Of course we had to improvise",
		"Since, given that a cow's great size",
		"Enables it to smash our balls,",
		"We didn't dare record its calls.",
	],
	[
		"Well isn't this just handsome luck;",
		"Our animals have run amok.",
		"We'll have to round them up and send",
		"Their giblets flying round the bend.",
		"I'll grow up broken, scarred for life.",
		"You'll never find yourself a wife.",
		"If only we had listened when",
		"Mum told us not to play again.",
	],
];

const char*[AnimalType.max+1][] expletives = [
	//Cow
	[
		"もおおぉおぉぉ～",
		"おい、何やってんだよ！",
		"私は牛でした",
		"モえるぜ",
	],
	//Sheep
	[
		"べええぇえぇぇ～",
		"羊最高～",
		"缶ズは羊ハックじゃねえよ",
		"さようならぁ～",
	],
	//Pig
	[
		"ぶひ",
		"ああ豚れたよ～",
		"警察は大切な施設だ。",
		"馬鹿発！！",
	],
	//Duck
	[
		"がぁ",
		"偽医者だよおまえ",
		"すくめろ！",
		"こりゃ終わり鴨",
	],
];
