//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: low.h,v $
// Revision 5.1.1.1  1999/10/16 04:32:20  batopr
// new branch
//
// Revision 5.1  1999/10/16 04:31:17  batopr
// new branch
//
// Revision 1.2  1999/09/15 22:22:37  peel
// Set vnum for GENERIC_SMOKE to smoke dummy object.
//
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef __LOW_H
#define __LOW_H

// --------------- ROOMS

const short int ROOM_AUTO_RENT         = -2; /* for auto-renting */
const short int ROOM_NOWHERE           = -1;
const short int ROOM_VOID              = 0;
const short int ROOM_IMPERIA           = 1;
const short int ROOM_HELL              = 70;
const short int ROOM_STORAGE           = 71;
const short int ROOM_POLY_STORAGE      = 72;
const short int ROOM_CORPSE_STORAGE    = 73;
const short int ROOM_NOCTURNAL_STORAGE = 74;
const short int ROOM_Q_STORAGE         = 76;
const short int ROOM_CS                = 100;
const short int ROOM_GREEN_DRAGON_INN  = 553;
const short int ROOM_KINDRED_INN       = 556;
const short int ROOM_DONATION          = 563;
const short int ROOM_PETS_GH           = 566;
const short int ROOM_PEW1              = 757;
const short int ROOM_PEW2              = 758;
const short int ROOM_PEW3              = 761;
const short int ROOM_PEW4              = 763;
const short int ROOM_TOP_OF_TREE       = 25487;
const short int ROOM_PETS_BM           = 1397;
const short int ROOM_DUMP              = 3700;
const short int ROOM_PETS_LOG          = 3738;
const short int ROOM_TICKET_DESTINATION= 6969;
const short int ROOM_PETS_AMB          = 7804;
const short int ROOM_BLACKJACK         = 8401;
const short int ROOM_HEARTS            = 8417;
const short int ROOM_WARD_1            = 13142;
const short int ROOM_WARD_2            = 9734;
const short int ROOM_TREE_BRIDGE       = 15277;
const short int ROOM_SLEEPTAG_CONTROL  = 23599;

// ------------------------- OBJS

const short int DEITY_TOKEN          = 2;
const short int GENERIC_L_BAG        = 6;
const short int OBJ_BANDAGE          = 9;
const short int OBJ_ROSEPETAL        = 10;
const short int GENERIC_TALEN        = 13;
const short int OBJ_BATS_JACUZZI     = 31;
const short int TREASURE_GOLD        = 50;
const short int TREASURE_SILVER      = 51;
const short int TREASURE_COPPER      = 52;
const short int TREASURE_STEEL       = 53;
const short int TREASURE_ELECTRUM    = 54;
const short int TREASURE_PLATINUM    = 55;
const short int TREASURE_TITANIUM    = 56;
const short int TREASURE_MITHRIL     = 57;
const short int TREASURE_ATHANOR     = 58;
const short int TREASURE_ADMANTIUM   = 59;
const short int TREASURE_BRASS       = 60;
const short int TREASURE_TIN         = 61;
const short int TREASURE_IRON        = 62;
const short int TREASURE_ALUMINUM    = 63;
const short int TREASURE_BRONZE      = 64;
const short int TREASURE_OBSIDIAN    = 65;
const short int OBJ_SALTPILE         = 99;
const short int GENERIC_FLARE        = 199;
const short int WEAPON_L_SWORD       = 300;
const short int WEAPON_S_SWORD       = 304;
const short int WEAPON_DAGGER        = 305;
const short int WEAPON_BOW           = 307;
const short int WEAPON_STAFF         = 309;
const short int WEAPON_CLUB          = 310;
const short int WEAPON_MACE          = 311;
const short int WEAPON_HAMMER        = 312;
const short int WEAPON_AXE           = 314;
const short int WEAPON_WHIP          = 316;
const short int WEAPON_WHIP_STUD     = 317;
const short int WEAPON_KNIFE         = 318;
const short int WEAPON_AVENGER1      = 319;
const short int WEAPON_AVENGER2      = 320;
const short int WEAPON_T_STAFF       = 324;
const short int WEAPON_T_DAGGER      = 325;
const short int WEAPON_T_SWORD       = 329;
const short int WEAPON_AVENGER3      = 326;
const short int OBJ_CARAVAN          = 519;
const short int OBJ_TRADE_GOOD       = 520;
const short int GENERIC_MONEYPOUCH   = 604;
// vnums for various set-trap items
const short int ST_FLINT      = 900;
const short int ST_SULPHUR    = 901;
const short int ST_BAG        = 902;
const short int ST_HYDROGEN   = 903;
const short int ST_POISON     = 904;
const short int ST_SHARDS     = 905;
const short int ST_NEEDLE     = 906;
const short int ST_NOZZLE     = 907;
const short int ST_FUNGUS     = 908;
const short int ST_SPRING     = 909;
const short int ST_BELLOWS    = 910;
const short int ST_GAS        = 911;
const short int ST_HOSE       = 912;
const short int ST_ACID_VIAL  = 913;
const short int ST_CON_POISON = 914;   // also used in poison weapon
const short int ST_SPIKE      = 915;
const short int ST_TRIPWIRE   = 916;
const short int ST_RAZOR_BLADE       = 917;
const short int ST_CONCRETE          = 918;
const short int ST_WEDGE             = 919;
const short int ST_FROST             = 920;
const short int ST_PENTAGRAM         = 921;
const short int ST_BLINK             = 922;
const short int ST_ATHANOR           = 923;
const short int ST_LANDMINE          = 924;
const short int ST_GRENADE           = 925;
const short int ST_CASE_MINE         = 926;
const short int ST_CASE_GRENADE      = 927;
const short int ST_CGAS              = 928;
const short int ST_TUBING            = 929;
const short int ST_PEBBLES           = 930;
const short int ST_BOLTS             = 931;
const short int ST_CANISTER          = 932;
const short int ST_CRYSTALINE        = 933;
const short int ST_RAZOR_DISK        = 934;
const short int GENERIC_DAGGER      = 1000;
const short int WOOD_COMPONENT      = 1700;
const short int ROCK_COMPONENT      = 1701;
const short int IRON_COMPONENT      = 1702;
const short int DIAMOND_COMPONENT   = 1703;
const short int GENERIC_NOTE        = 3090;
const short int GENERIC_PEN         = 3091;
const short int OBJ_VENISON         = 3114;
const short int WEAPON_ARROW        = 3409;
const short int OBJ_GENERIC_GRAVE   = 4200;
const short int OBJ_GOLD_FEATHER    = 4791;
const short int OBJ_PHOENIX_FEATHER = 4847;
const short int ITEM_DAYGATE        = 5890;
const short int ITEM_MOONGATE       = 5891;
const short int OBJ_MINELIFT_UP    =  7085;
const short int OBJ_MINELIFT_DOWN  =  7086;
const short int OBJ_TALISMAN        = 9700;
const short int ITEM_RAINBOW_BRIDGE1 = 10041;
const short int ITEM_RAINBOW_BRIDGE2 = 10042;
const short int OBJ_HERALD          = 10543;
const short int GENERIC_POOL        = 10559;
const short int GENERIC_SMOKE       = 10560;
const short int OBJ_INERT_STICK     = 10561;
const short int OBJ_MONK_QUEST_DOG_COLLAR = 12468;
const short int OBJ_FLAMING_PORTAL  = 16157;
const short int OBJ_RIVER_SERP_VENOM= 20409;
const short int OBJ_STONE_LEG_CLUB  = 20425;
const short int OBJ_ICE_SHARD_LARGE = 20437;
const short int OBJ_ICE_SHARD_SMALL = 20440;
const short int OBJ_ARCTIC_BEAR_HIDE= 20441;
const short int OBJ_SNOW_BEAST_HORN = 20442;
const short int OBJ_LARGE_ANIMAL_HIDE = 20443;
const short int OBJ_SEAL_SKIN_HIDE  = 20450;
const short int OBJ_EARMUFF         = 20456;
const short int OBJ_AQUA_DRAG_HEAD  = 22522;
const short int OBJ_SLEEPTAG_STAFF  = 23500;
const short int OBJ_QUEST_ORE       = 29020;
const short int OBJ_RED_ORE         = 29022;
const short int OBJ_GENERIC_SCROLL  = 29990;
const short int STATS_POTION        = 29991;
const short int YOUTH_POTION        = 29992;
const short int OBJ_GENERIC_POTION  = 29993;
const short int CRAPS_DICE          = 29999;

// ------------------------- MOBS

const short int MOB_NONE           = 0;
const short int FIRE_ELEMENTAL     = 16;
const short int WATER_ELEMENTAL    = 17;
const short int EARTH_ELEMENTAL    = 18;
const short int AIR_ELEMENTAL      = 19;
const short int FACTION_FAERY      = 20;
const short int WOOD_GOLEM         = 26;
const short int ROCK_GOLEM         = 27;
const short int IRON_GOLEM         = 28;
const short int DIAMOND_GOLEM      = 29;
const short int MOB_ANIMATION      = 41;
const short int MOB_MALE_CHURCH_GOER   =50;
const short int MOB_FEMALE_CHURCH_GOER =51;
const short int MOB_MALE_HOPPER    = 52;
const short int MOB_FEMALE_HOPPER  = 53;
const short int MOB_NEWSBOY        = 84;
const short int MOB_LAMPBOY        = 99;
const short int MOB_WATCHMAN        =100;
const short int MOB_CITYGUARD       =101;
const short int MOB_ELITE           =102;
const short int MOB_SWEEPER         =103;
const short int MOB_SMALL_CAT       =104;
const short int MOB_CITIZEN_M       =108;
const short int MOB_CITIZEN_F       =109;
const short int MOB_PEASANT         =110;
const short int MOB_OLD_SAGE        =113;
const short int MOB_DWARF_ADV       =125;
const short int MOB_GNOME_MISS      =126;
const short int MOB_OGRE_VIG        =127;
const short int MOB_HOBBIT_EMI      =128;
const short int MOB_ELF_TRADER      =138;
const short int MOB_DEITY_JUDGMENT  =146;
const short int MOB_PAWNGUY         =159;
const short int MOB_PETGUY_GH       =166;
const short int MOB_PIGEON          =172;
const short int MOB_SWEEPER2        =193;
const short int MOB_BOUNCER         =194;
const short int MOB_MAGE_GM_LEVEL15 =200;
const short int MOB_MONK_GM_LEVEL15 =207;
const short int MOB_MAGE_GM_LEVEL40 =216;
const short int MOB_MONK_GM_LEVEL40 =223;
const short int MOB_FIRST_RANGER_BASIC_TRAINER =261;
const short int MOB_BOUNCER_HEAD    =265;
const short int MOB_COMBAT_TRAINER  =286;
const short int MOB_FIRST_ANIMAL_TRAINER =308;
const short int MOB_TALAR           =324;
const short int MOB_KING_GH         =353;
const short int MOB_HUANG_LO        =385;
const short int MOB_OLD_WOMAN       =404;
const short int MOB_TROLL_GIANT     =405;
const short int MOB_FISTLAND        =407;
const short int MOB_TREE_SPIRIT     =408;
const short int MOB_BALCOR          =528;
const short int MOB_ROAD_THIEF      =650;
const short int MOB_ROAD_ROBBER     =651;
const short int MOB_ROAD_THIEF_LARGE=700;
const short int MOB_BEHOLDER        =904;
const short int MOB_DOPPLEGANGER    =914;
const short int MOB_BANSHEE         =928;
const short int MOB_EAGLE           =953;
const short int MOB_BULGE          =1024;
const short int MOB_PETGUY_BM      =1209;
const short int MOB_FONG_CHUN      =1304;
const short int MOB_BISHOP_BMOON   =1361;
const short int APOC_PESTILENCE    =1601;
const short int APOC_WAR           =1602;
const short int APOC_FAMINE        =1603;
const short int APOC_DEATH         =1604;
const short int APOC_PESTHORSE     =1605;
const short int APOC_WARHORSE      =1606;
const short int APOC_FAMINEHORSE   =1607;
const short int APOC_DEATHHORSE    =1608;
const short int APOC_WARRIOR       =1609;
const short int MOB_CARAVAN_MASTER =1610;
const short int MOB_DWARVEN_HIGH_PRIEST =2113;
const short int MOB_TAILLE         =2273;
const short int MOB_CREED	   =2495;
const short int MOB_SPARTAGUS	   =2874;
const short int MOB_BUCK_OLD       =3109;
const short int MOB_BUCK_YOUNG     =3114;
const short int MOB_GANGMEMBER_GIBBETT =3210;
const short int MOB_RANGER_LORD    =3415;
const short int MOB_SULTRESS	   =3459;
const short int MOB_PETGUY_LOG     =3707;
const short int MOB_ASSASSIN	   =3857;
const short int MOB_ROC            =4791;
const short int MOB_DRAGON_GOLD    =4796;
const short int MOB_RALIKI         =4822 ;
const short int MOB_PHOENIX        =4847;
const short int MOB_DRAGON_BRONZE  =4858;
const short int MOB_SNAKE          =5112;
const short int MOB_HOBBIT_ADVENTURER     =5347;
const short int MOB_OVERLORD	   =5443;
const short int MOB_MARCUS	   =5555;
const short int MOB_GRUUM          =5570;
const short int MOB_NITELLION      =5720;
const short int MOB_LEPER          =6602;
const short int MOB_BLACKWIDOW     =6605;
const short int MOB_PRIEST_HOLY    =6611;
const short int MOB_SCAR           =6761;
const short int MOB_DRAGON_WORKER  =6844;
const short int MOB_SONGBIRD           = 7500;
const short int MOB_GHOST_BISHOP   =7510;
const short int MOB_NESMUM	   =7531;
const short int MOB_BLINDMAN       =7800;
const short int MOB_NIGHTMARE      =7803;
const short int MOB_PEACOCK        =7806;
const short int MOB_GREYHOUND      =7816;
const short int MOB_CHAMELEON      =7818;
const short int MOB_LOCUSTS25      =7850;
const short int MOB_LOCUSTS30      =7851;
const short int MOB_LOCUSTS35      =7852;
const short int MOB_LOCUSTS40      =7853;
const short int MOB_LOCUSTS50      =7854;
const short int MOB_SNAKES25       =7855;
const short int MOB_SNAKES30       =7856;
const short int MOB_SNAKES35       =7857;
const short int MOB_SNAKES40       =7858;
const short int MOB_SNAKES50       =7859;
const short int MOB_SIMON_SPELLCRAFTER = 7876;
const short int MOB_BOUNCER2       =8418;
const short int MOB_MISER_BEN	   =8435;
const short int MOB_CHEETAH        =8511;
const short int MOB_ELEPHANT       =8525;
const short int MOB_MEDICINE_MAN   =8677;
const short int MOB_PETGUY_AMB     =8701;
const short int MOB_DWARVEN_AMBASSADOR =8784;
const short int MOB_LENGE_MERCHANT =8796;
const short int MOB_YOLA	   =9148;
const short int MOB_RATTLESNAKE    =9314;
const short int MOB_UNDEAD_CHIEF   =9319;
const short int MOB_RAULDOPLIC     =9322;
const short int MOB_ABNOR	   =9326;
const short int MOB_GRIZWALD	   =9331;
const short int MOB_MOAT_MONSTER   =9700;
const short int MOB_ASH            =9954;
const short int MOB_FREEZING_MIST  =10022;
const short int MOB_GERSARD        =10119;
const short int MOB_FALCON         =10913;
const short int MOB_HERMIT_JED     =10919;
const short int MOB_POACH_KOBOLD   =10922;
const short int MOB_POACH_ORC      =10923;
const short int MOB_GM_IRIS        =11001;
const short int MOB_DAKINOR        =11005;
const short int MOB_THALIA         =11006;
const short int MOB_MAGE_GM_LEVEL50=11039;
const short int MOB_CAT_SHADOW     =11105;
const short int MOB_CAPTAIN_RYOKEN =11113;
const short int MOB_ORC_MAGI       =11300;
const short int MOB_VAMP_BAT       =11340;
const short int MOB_BELIMUS        =12402;
const short int MOB_DRAGON_SILVER  =12404;
const short int MOB_SIREN          =12406;
const short int MOB_KRAKEN         =12407;
const short int MOB_WHALE_SPERM    =12411;
const short int MOB_TIGER_SHARK    =12413;
const short int MOB_WHALE_BLUE     =12415;
const short int MOB_EEL            =12416;
const short int MOB_SHARK_YOUNG    =12431;
const short int MOB_ASCALLION      =12433;
const short int MOB_MERMAID        =12440;
const short int MOB_MONK_GM_LEVEL50=12509;
const short int MOB_EEL2           =14125;
const short int MOB_JAQUIN	   =14159;
const short int MOB_BARARAKNA	   =14227;
const short int MOB_WORKER_WINERY  =14404;
const short int MOB_BERARDINIS     =15103;
const short int MOB_POACHER	   =15251;
const short int MOB_MERRITT        =15284;
const short int MOB_BONE_PYGMY     =15107;
const short int MOB_SILVERCLAW     =17106;
const short int MOB_ROCK_GIANT     =20408;
const short int MOB_RIVER_SERPENT  =20409;
const short int MOB_LOST_ICE_COM   =20420;
const short int MOB_LOST_ICE_SCOUT =20421;
const short int MOB_ARCTIC_BEAR1   =20422;
const short int MOB_ARCTIC_BEAR2   =20423;
const short int MOB_SNOW_BEAST     =20424;
const short int MOB_PENGUIN_ADULT  =20425;
const short int MOB_PENGUIN_YOUNG  =20426;
const short int MOB_MOUNTAIN_CAT   =20427;
const short int MOB_SEA_LION       =20430;
const short int MOB_AQUATIC_DRAGON =22517;
const short int MOB_SPIRIT_OF_WARRIOR =22518;
const short int MOB_LOGRUS_INITIATION = 22713;
const short int MOB_DRAGON_CLOUD   =23633;
const short int MOB_LORTO	   =28876;
const short int MOB_HERMIT_GHPARK  =25404;
const short int MOB_GNOBLE_FARMER  =22410;
const short int MOB_GNOME_FARMER   =22414;
const short int MOB_GNOME_CHILD    =22419;
const short int MOB_GNOME_FARMHAND =22421;
const short int MOB_JOHN_RUSTLER   =22427;
const short int MOB_CALDONIA       =24100;



#endif



