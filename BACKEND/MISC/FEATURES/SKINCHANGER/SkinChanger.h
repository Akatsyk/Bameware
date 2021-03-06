#pragma once

namespace SDK
{
	class CBaseWeapon;
}

namespace FEATURES
{
	namespace MISC
	{
		class SkinChanger
		{
		public:
			void Do();

		public:
			static constexpr unsigned int NUM_WEAPONS = 35;
			char weapons_list[NUM_WEAPONS][256] =
			{
				"AK-47",
				"AUG",
				"AWP",
				"CZ75 Auto",
				"Desert Eagle",
				"Dual Berettas",
				"FAMAS",
				"Five-SeveN",
				"G3SG1",
				"Galil AR",
				"Glock-18",
				"M249",
				"M4A1-S",
				"M4A4",
				"MAC-10",
				"MAG-7",
				"MP7",
				"MP9",
				"Negev",
				"Nova",
				"P2000",
				"P250",
				"P90",
				"PP-Bizon",
				"R8 Revolver",
				"Sawed-Off",
				"SCAR-20",
				"SSG 08",
				"SG 553",
				"Tec-9",
				"UMP-45",
				"USP-S",
				"XM1014",
				"KNIFE CT",
				"KNIFE T"
			};
			static constexpr unsigned int NUM_QUALITIES = 11;
			char qualities_list[NUM_QUALITIES][256] =
			{
				"Default",
				"Genuine",
				"Vintage",
				"Unusual",
				"Community",
				"Developer",
				"Self-Made",
				"Customized",
				"Strange",
				"Completed",
				"Tournament"
			};
			static constexpr unsigned int NUM_SKINS = 669;
			char skins_list[NUM_SKINS][256] =
			{
				"Groundwater",
				"Candy Apple",
				"Forest DDPAT",
				"Arctic Camo",
				"Desert Storm",
				"Bengal Tiger",
				"Copperhead",
				"Skulls",
				"Crimson Web",
				"Blue Streak",
				"Red Laminate",
				"Gunsmoke",
				"Jungle Tiger",
				"Urban DDPAT",
				"Virus",
				"Granite Marbleized",
				"Contrast Spray",
				"Forest Leaves",
				"Lichen Dashed",
				"Bone Mask",
				"Anodized Navy",
				"Snake Camo",
				"Silver",
				"Hot Rod",
				"Metallic DDPAT",
				"Ossified",
				"Blaze",
				"Fade",
				"Bulldozer",
				"Night",
				"Copper",
				"Blue Steel",
				"Stained",
				"Case Hardened",
				"Contractor",
				"Colony",
				"Dragon Tattoo",
				"Lightning Strike",
				"Slaughter",
				"Dark Water",
				"Hypnotic",
				"Bloomstick",
				"Cold Blooded",
				"Carbon Fiber",
				"Scorpion",
				"Safari Mesh",
				"Wings",
				"Polar Camo",
				"Blizzard Marbleized",
				"Winter Forest",
				"Boreal Forest",
				"Forest Night",
				"Orange DDPAT",
				"Pink DDPAT",
				"Mudder",
				"Cyanospatter",
				"Caramel",
				"Grassland",
				"Blue Spruce",
				"Ultraviolet",
				"Sand Dune",
				"Storm",
				"Tornado",
				"Whiteout",
				"Grassland Leaves",
				"Polar Mesh",
				"Condemned",
				"Glacier Mesh",
				"Sand Mesh",
				"Sage Spray",
				"Jungle Spray",
				"Sand Spray",
				"Urban Perforated",
				"Waves Perforated",
				"Orange Peel",
				"Urban Masked",
				"Jungle Dashed",
				"Sand Dashed",
				"Urban Dashed",
				"Jungle",
				"Demolition",
				"Afterimage",
				"Bullet Rain",
				"Death by Kitty",
				"Palm",
				"Walnut",
				"Brass",
				"Splash",
				"Modern Hunter",
				"Splash Jam",
				"Blaze Orange",
				"Radiation Hazard",
				"Nuclear Threat",
				"Fallout Warning",
				"Predator",
				"Irradiated Alert",
				"Black Laminate",
				"BOOM",
				"Scorched",
				"Faded Zebra",
				"Memento",
				"Doomkitty",
				"Nuclear Threat",
				"Fire Serpent",
				"Corticera",
				"Emerald Dragon",
				"Overgrowth",
				"Corticera",
				"Golden Koi",
				"Wave Spray",
				"Zirka",
				"Graven",
				"Bright Water",
				"Black Limba",
				"Tempest",
				"Shattered",
				"Bone Pile",
				"Spitfire",
				"Demeter",
				"Emerald",
				"Anodized Navy",
				"Hazard",
				"Dry Season",
				"Mayan Dreams",
				"Palm",
				"Jungle DDPAT",
				"Rust Coat",
				"Mosaico",
				"Jungle",
				"Tornado",
				"Facets",
				"Sand Dune",
				"Groundwater",
				"Anodized Gunmetal",
				"Ocean Foam",
				"Graphite",
				"Ocean Foam",
				"Graphite",
				"X-Ray",
				"Blue Titanium",
				"Blood Tiger",
				"Hexane",
				"Hive",
				"Hemoglobin",
				"Serum",
				"Blood in the Water",
				"Nightshade",
				"Water Sigil",
				"Ghost Camo",
				"Blue Laminate",
				"Electric Hive",
				"Blind Spot",
				"Azure Zebra",
				"Steel Disruption",
				"Cobalt Disruption",
				"Crimson Web",
				"Tropical Storm",
				"Ash Wood",
				"VariCamo",
				"Night Ops",
				"Urban Rubble",
				"VariCamo Blue",
				"CaliCamo",
				"Hunting Blind",
				"Army Mesh",
				"Gator Mesh",
				"Teardown",
				"Army Recon",
				"Amber Fade",
				"Damascus Steel",
				"Red Quartz",
				"Cobalt Quartz",
				"Full Stop",
				"Pit Viper",
				"Silver Quartz",
				"Acid Fade",
				"Nitro",
				"Asiimov",
				"The Kraken",
				"Guardian",
				"Mehndi",
				"Redline",
				"Pulse",
				"Marina",
				"Rose Iron",
				"Rising Skull",
				"Sandstorm",
				"Kami",
				"Magma",
				"Cobalt Halftone",
				"Tread Plate",
				"The Fuschia Is Now",
				"Victoria",
				"Undertow",
				"Titanium Bit",
				"Heirloom",
				"Copper Galaxy",
				"Red FragCam",
				"Panther",
				"Stainless",
				"Blue Fissure",
				"Asiimov",
				"Chameleon",
				"Corporal",
				"Redline",
				"Trigon",
				"Heat",
				"Terrain",
				"Antique",
				"Pulse",
				"Sergeant",
				"Sandstorm",
				"Guardian",
				"Heaven Guard",
				"Death Rattle",
				"Green Apple",
				"Franklin",
				"Meteorite",
				"Tuxedo",
				"Army Sheen",
				"Caged Steel",
				"Emerald Pinstripe",
				"Atomic Alloy",
				"Vulcan",
				"Isaac",
				"Slashed",
				"Torque",
				"Antique",
				"Retribution",
				"Kami",
				"Howl",
				"Curse",
				"Desert Warfare",
				"Cyrex",
				"Orion",
				"Heaven Guard",
				"Poison Dart",
				"Jaguar",
				"Bratatat",
				"Road Rash",
				"Detour",
				"Red Python",
				"Master Piece",
				"Nitro",
				"Rust Coat",
				"Chalice",
				"Knight",
				"Chainmail",
				"Hand Cannon",
				"Dark Age",
				"Briar",
				"Royal Blue",
				"Indigo",
				"Twist",
				"Module",
				"Desert-Strike",
				"Tatter",
				"Pulse",
				"Caiman",
				"Jet Set",
				"First Class",
				"Leather",
				"Commuter",
				"Dragon Lore",
				"First Class",
				"Coach Class",
				"Pilot",
				"Red Leather",
				"Osiris",
				"Tigris",
				"Conspiracy",
				"Fowl Play",
				"Water Elemental",
				"Urban Hazard",
				"Desert-Strike",
				"Koi",
				"Ivory",
				"Supernova",
				"Asiimov",
				"Cyrex",
				"Abyss",
				"Labyrinth",
				"Traveler",
				"Business Class",
				"Olive Plaid",
				"Green Plaid",
				"Reactor",
				"Setting Sun",
				"Nuclear Waste",
				"Bone Machine",
				"Styx",
				"Nuclear Garden",
				"Contamination",
				"Toxic",
				"Radiation Hazard",
				"Chemical Green",
				"Hot Shot",
				"Fallout Warning",
				"Cerberus",
				"Wasteland Rebel",
				"Grinder",
				"Murky",
				"Basilisk",
				"Griffin",
				"Firestarter",
				"Dart",
				"Urban Hazard",
				"Cartel",
				"Fire Elemental",
				"Highwayman",
				"Cardiac",
				"Delusion",
				"Tranquility",
				"Cartel",
				"Man-o'-war",
				"Urban Shock",
				"Naga",
				"Chatterbox",
				"Catacombs",
				"龍王 (Dragon King)",
				"System Lock",
				"Malachite",
				"Deadly Poison",
				"Muertos",
				"Serenity",
				"Grotto",
				"Quicksilver",
				"Tiger Tooth",
				"Damascus Steel",
				"Damascus Steel",
				"Marble Fade",
				"Rust Coat",
				"Doppler",
				"Doppler",
				"Doppler",
				"Doppler",
				"Doppler",
				"Doppler",
				"Doppler",
				"Elite Build",
				"Armor Core",
				"Worm God",
				"Bronze Deco",
				"Valence",
				"Monkey Business",
				"Eco",
				"Djinn",
				"Hyper Beast",
				"Heat",
				"Man-o'-war",
				"Neon Rider",
				"Origami",
				"Pole Position",
				"Grand Prix",
				"Twilight Galaxy",
				"Chronos",
				"Hades",
				"Icarus Fell",
				"Minotaur's Labyrinth",
				"Asterion",
				"Pathfinder",
				"Daedalus",
				"Hot Rod",
				"Medusa",
				"Duelist",
				"Pandora's Box",
				"Poseidon",
				"Moon in Libra",
				"Sun in Leo",
				"Shipping Forecast",
				"Emerald",
				"Para Green",
				"Akihabara Accept",
				"Hydroponic",
				"Bamboo Print",
				"Bamboo Shadow",
				"Bamboo Forest",
				"Aqua Terrace",
				"Counter Terrace",
				"Terrace",
				"Neon Kimono",
				"Orange Kimono",
				"Crimson Kimono",
				"Mint Kimono",
				"Midnight Storm",
				"Sunset Storm 壱",
				"Sunset Storm 弐",
				"Daybreak",
				"Impact Drill",
				"Seabird",
				"Aquamarine Revenge",
				"Hyper Beast",
				"Yellow Jacket",
				"Neural Net",
				"Rocket Pop",
				"Bunsen Burner",
				"Evil Daimyo",
				"Nemesis",
				"Ruby Poison Dart",
				"Loudmouth",
				"Ranger",
				"Handgun",
				"Elite Build",
				"Cyrex",
				"Riot",
				"Torque",
				"Frontside Misty",
				"Dualing Dragons",
				"Survivor Z",
				"Flux",
				"Stone Cold",
				"Wraiths",
				"Nebula Crusader",
				"Golden Coil",
				"Rangeen",
				"Cobalt Core",
				"Special Delivery",
				"Wingshot",
				"Green Marine",
				"Big Iron",
				"Kill Confirmed",
				"Scumbria",
				"Point Disarray",
				"Ricochet",
				"Fuel Rod",
				"Corinthian",
				"Retrobution",
				"The Executioner",
				"Royal Paladin",
				"Power Loader",
				"Imperial",
				"Shapewood",
				"Yorick",
				"Outbreak",
				"Tiger Moth",
				"Avalanche",
				"Teclu Burner",
				"Fade",
				"Amber Fade",
				"Fuel Injector",
				"Elite Build",
				"Photic Zone",
				"Kumicho Dragon",
				"Cartel",
				"Valence",
				"Triumvirate",
				"Royal Legion",
				"The Battlestar",
				"Lapis Gator",
				"Praetorian",
				"Impire",
				"Hyper Beast",
				"Necropos",
				"Jambiya",
				"Lead Conduit",
				"Fleet Flock",
				"Judgement of Anubis",
				"Red Astor",
				"Ventilators",
				"Orange Crash",
				"Firefight",
				"Spectre",
				"Chantico's Fire",
				"Bioleak",
				"Oceanic",
				"Asiimov",
				"Fubar",
				"Atlas",
				"Ghost Crusader",
				"Re-Entry",
				"Primal Saber",
				"Black Tie",
				"Lore",
				"Lore",
				"Lore",
				"Lore",
				"Lore",
				"Black Laminate",
				"Black Laminate",
				"Black Laminate",
				"Black Laminate",
				"Black Laminate",
				"Gamma Doppler",
				"Gamma Doppler",
				"Gamma Doppler",
				"Gamma Doppler",
				"Gamma Doppler",
				"Autotronic",
				"Autotronic",
				"Autotronic",
				"Autotronic",
				"Autotronic",
				"Bright Water",
				"Bright Water",
				"Freehand",
				"Freehand",
				"Freehand",
				"Aristocrat",
				"Phobos",
				"Violent Daimyo",
				"Wasteland Rebel",
				"Mecha Industries",
				"Desolate Space",
				"Carnivore",
				"Exo",
				"Imperial Dragon",
				"Iron Clad",
				"Chopper",
				"Harvester",
				"Reboot",
				"Limelight",
				"Bloodsport",
				"Aerial",
				"Ice Cap",
				"Neon Revolution",
				"Syd Mead",
				"Imprint",
				"Directive",
				"Roll Cage",
				"Scumbria",
				"Ventilator",
				"Weasel",
				"Petroglyph",
				"Airlock",
				"Dazzle",
				"Grim",
				"Powercore",
				"Triarch",
				"Fuel Injector",
				"Briefing",
				"Slipstream",
				"Doppler",
				"Doppler",
				"Doppler",
				"Ultraviolet",
				"Ultraviolet",
				"Polymer",
				"Ironwork",
				"Dragonfire",
				"Royal Consorts",
				"Mecha Industries",
				"Cirrus",
				"Stinger",
				"Black Sand",
				"Sand Scale",
				"Flashback",
				"Buzz Kill",
				"Sonar",
				"Gila",
				"Turf",
				"Shallow Grave",
				"Cyrex",
				"Wasteland Princess",
				"Bloodsport",
				"Fever Dream",
				"Jungle Slipstream",
				"Blueprint",
				"Xiangliu",
				"Decimator",
				"Oxide Blaze",
				"Capillary",
				"Crimson Tsunami",
				"Emerald Poison Dart",
				"Akoben",
				"Ripple",
				"Last Dive",
				"Scaffold",
				"Neo-Noir",
				"Seasons",
				"Zander",
				"Orbit Mk01",
				"Blueprint",
				"Cobra Strike",
				"Macabre",
				"Hyper Beast",
				"Sugar Rush",
				"Oni Taiji",
				"Briefing",
				"Hellfire",
				"Aloha",
				"Hard Water",
				"Woodsman",
				"Red Rock",
				"Death Grip",
				"Death's Head",
				"Cut Out",
				"Metal Flowers",
				"Morris",
				"Triqua",
				"The Empress",
				"High Roller",
				"Hunter",
				"See Ya Later",
				"Goo",
				"Off World",
				"Leaded Glass",
				"Oceanic",
				"Llama Cannon",
				"Cracked Opal",
				"Jungle Slipstream",
				"Phantom",
				"Tacticat",
				"Exposure",
				"Ziggy",
				"Stymphalian",
				"Mortis",
				"Night Riot",
				"Flame Test",
				"Moonrise",
				"Neo-Noir",
				"Bloodsport",
				"Black Sand",
				"Lionfish",
				"Wild Six",
				"Urban Hazard",
				"Grip",
				"Aloha",
				"SWAG-7",
				"Arctic Wolf",
				"Cortex",
				"Oxide Blaze",
				"Charred",
				"Snakebite",
				"Bronzed",
				"Leather",
				"Spruce DDPAT",
				"Lunar Weave",
				"Convoy",
				"Crimson Weave",
				"Superconductor",
				"Arid",
				"Slaughter",
				"Eclipse",
				"Spearmint",
				"Boom!",
				"Cool Mint",
				"Forest DDPAT",
				"Crimson Kimono",
				"Emerald Web",
				"Foundation",
				"Badlands",
				"Pandora's Box",
				"Hedge Maze",
				"Guerrilla",
				"Diamondback",
				"King Snake",
				"Imperial Plaid",
				"Overtake",
				"Racing Green",
				"Amphibious",
				"Bronze Morph",
				"Omega",
				"Vice",
				"POW!",
				"Turtle",
				"Transport",
				"Polygon",
				"Cobalt Skulls",
				"Overprint",
				"Duct Tape",
				"Arboreal",
				"Emerald",
				"Mangrove",
				"Rattler",
				"Case Hardened",
				"Crimson Web",
				"Buckshot",
				"Fade",
				"Mogul"
			};

		private:
			int weapons_list_indexes[NUM_WEAPONS] =
			{
				7,
				8,
				9,
				63,
				1,
				2,
				10,
				2,
				11,
				13,
				4,
				14,
				60,
				16,
				17,
				27,
				33,
				34,
				28,
				35,
				32,
				36,
				19,
				26,
				64,
				29,
				38,
				40,
				39,
				30,
				24,
				61,
				25,
				42,
				59
			};
			int qualities_list_indexes[NUM_QUALITIES] =
			{
				0,
				1,
				2,
				3,
				5,
				6,
				7,
				8,
				9,
				10,
				12
			};
			int skins_list_indexes[NUM_SKINS] =
			{
				2,
				3,
				5,
				6,
				8,
				9,
				10,
				11,
				12,
				13,
				14,
				15,
				16,
				17,
				20,
				21,
				22,
				25,
				26,
				27,
				28,
				30,
				32,
				33,
				34,
				36,
				37,
				38,
				39,
				40,
				41,
				42,
				43,
				44,
				46,
				47,
				48,
				51,
				59,
				60,
				61,
				62,
				67,
				70,
				71,
				72,
				73,
				74,
				75,
				76,
				77,
				78,
				83,
				84,
				90,
				92,
				93,
				95,
				96,
				98,
				99,
				100,
				101,
				102,
				104,
				107,
				110,
				111,
				116,
				119,
				122,
				124,
				135,
				136,
				141,
				143,
				147,
				148,
				149,
				151,
				153,
				154,
				155,
				156,
				157,
				158,
				159,
				162,
				164,
				165,
				166,
				167,
				168,
				169,
				170,
				171,
				172,
				174,
				175,
				176,
				177,
				178,
				179,
				180,
				181,
				182,
				183,
				184,
				185,
				186,
				187,
				188,
				189,
				190,
				191,
				192,
				193,
				194,
				195,
				196,
				197,
				198,
				199,
				200,
				201,
				202,
				203,
				204,
				205,
				206,
				207,
				208,
				209,
				210,
				211,
				212,
				213,
				214,
				215,
				216,
				217,
				218,
				219,
				220,
				221,
				222,
				223,
				224,
				225,
				226,
				227,
				228,
				229,
				230,
				231,
				232,
				233,
				234,
				235,
				236,
				237,
				238,
				240,
				241,
				242,
				243,
				244,
				245,
				246,
				247,
				248,
				249,
				250,
				251,
				252,
				253,
				254,
				255,
				256,
				257,
				258,
				259,
				260,
				261,
				262,
				263,
				264,
				265,
				266,
				267,
				268,
				269,
				270,
				271,
				272,
				273,
				274,
				275,
				276,
				277,
				278,
				279,
				280,
				281,
				282,
				283,
				284,
				285,
				286,
				287,
				288,
				289,
				290,
				291,
				293,
				294,
				295,
				296,
				297,
				298,
				299,
				300,
				301,
				302,
				303,
				304,
				305,
				306,
				307,
				308,
				309,
				310,
				311,
				312,
				313,
				314,
				315,
				316,
				317,
				318,
				319,
				320,
				321,
				322,
				323,
				325,
				326,
				327,
				328,
				329,
				330,
				332,
				333,
				334,
				335,
				336,
				337,
				338,
				339,
				340,
				341,
				342,
				343,
				344,
				345,
				346,
				347,
				348,
				349,
				350,
				351,
				352,
				353,
				354,
				355,
				356,
				357,
				358,
				359,
				360,
				361,
				362,
				363,
				364,
				365,
				366,
				367,
				368,
				369,
				370,
				371,
				372,
				373,
				374,
				375,
				376,
				377,
				378,
				379,
				380,
				381,
				382,
				383,
				384,
				385,
				386,
				387,
				388,
				389,
				390,
				391,
				392,
				393,
				394,
				395,
				396,
				397,
				398,
				399,
				400,
				401,
				402,
				403,
				404,
				405,
				406,
				407,
				409,
				410,
				411,
				413,
				414,
				415,
				416,
				417,
				418,
				419,
				420,
				421,
				422,
				423,
				424,
				425,
				426,
				427,
				428,
				429,
				430,
				431,
				432,
				433,
				434,
				435,
				436,
				437,
				438,
				439,
				440,
				441,
				442,
				443,
				444,
				445,
				446,
				447,
				448,
				449,
				450,
				451,
				452,
				453,
				454,
				455,
				456,
				457,
				458,
				459,
				460,
				462,
				463,
				464,
				465,
				466,
				467,
				468,
				469,
				470,
				471,
				472,
				473,
				474,
				475,
				476,
				477,
				478,
				479,
				480,
				481,
				482,
				483,
				484,
				485,
				486,
				487,
				488,
				489,
				490,
				491,
				492,
				493,
				494,
				495,
				496,
				497,
				498,
				499,
				500,
				501,
				502,
				503,
				504,
				505,
				506,
				507,
				508,
				509,
				510,
				511,
				512,
				514,
				515,
				516,
				517,
				518,
				519,
				520,
				521,
				522,
				523,
				524,
				525,
				526,
				527,
				528,
				529,
				530,
				532,
				533,
				534,
				535,
				536,
				537,
				538,
				539,
				540,
				541,
				542,
				543,
				544,
				545,
				546,
				547,
				548,
				549,
				550,
				551,
				552,
				553,
				554,
				555,
				556,
				557,
				558,
				559,
				560,
				561,
				562,
				563,
				564,
				565,
				566,
				567,
				568,
				569,
				570,
				571,
				572,
				573,
				574,
				575,
				576,
				577,
				578,
				579,
				580,
				581,
				582,
				583,
				584,
				585,
				586,
				587,
				588,
				589,
				590,
				591,
				592,
				593,
				594,
				595,
				596,
				597,
				598,
				599,
				600,
				601,
				602,
				603,
				604,
				605,
				606,
				607,
				608,
				609,
				610,
				611,
				612,
				613,
				614,
				615,
				616,
				617,
				618,
				619,
				620,
				621,
				622,
				623,
				624,
				625,
				626,
				627,
				628,
				629,
				630,
				631,
				632,
				633,
				634,
				635,
				636,
				637,
				638,
				639,
				640,
				641,
				642,
				643,
				644,
				645,
				646,
				647,
				648,
				649,
				650,
				651,
				652,
				653,
				654,
				655,
				656,
				657,
				658,
				659,
				660,
				661,
				662,
				663,
				664,
				665,
				666,
				667,
				668,
				669,
				670,
				671,
				672,
				673,
				674,
				675,
				676,
				677,
				678,
				679,
				680,
				681,
				682,
				683,
				684,
				685,
				686,
				687,
				688,
				689,
				690,
				691,
				692,
				693,
				694,
				695,
				696,
				697,
				698,
				699,
				700,
				701,
				702,
				703,
				704,
				705,
				706,
				10006,
				10007,
				10008,
				10009,
				10010,
				10013,
				10015,
				10016,
				10018,
				10019,
				10021,
				10024,
				10026,
				10027,
				10028,
				10030,
				10033,
				10034,
				10035,
				10036,
				10037,
				10038,
				10039,
				10040,
				10041,
				10042,
				10043,
				10044,
				10045,
				10046,
				10047,
				10048,
				10049,
				10050,
				10051,
				10052,
				10053,
				10054,
				10055,
				10056,
				10057,
				10058,
				10059,
				10060,
				10061,
				10062,
				10063,
				10064
			};

		private:
			struct Skin_Info
			{
				Skin_Info()
				{
					fallback_paint_kit = -1;
					fallback_wear = -1;
					fallback_stat_trak = -1;
					fallback_seed = -1;
					fallback_quality = -1;
					custom_name = nullptr;
				}

				int fallback_paint_kit;
				int fallback_seed;
				int fallback_stat_trak;
				int fallback_quality;
				float fallback_wear;
				char* custom_name;
			};

			void ChangeSkin(SDK::CBaseWeapon* weapon, Skin_Info skin_info);
			void ChangeModel(SDK::CBaseWeapon* weapon, const char* model);
		};

		extern SkinChanger skin_changer;
	}
}
