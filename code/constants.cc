//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//  Constant integer and string arrays
//
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"
#include "drug.h"

const dirTypeT rev_dir[MAX_DIR] =
{
  DIR_SOUTH,
  DIR_WEST,
  DIR_NORTH,
  DIR_EAST,
  DIR_DOWN,
  DIR_UP,
  DIR_SOUTHWEST,
  DIR_SOUTHEAST,
  DIR_NORTHWEST,
  DIR_NORTHEAST
};

// all references to this need to go through the mapping function
// since the slot order has changed.  -- bat 06-27-97
const byte ac_percent_pos[MAX_WEAR] =
{
  0,
  1,
  1,
  5,
  20,
  15,
  7,
  7,
  4,
  4,
  2,
  2,
  6,
  6,
  10,
  10,
  1,
  1,
  0,
  0
};

const int TrapDir[] =
{
  TRAP_EFF_NORTH,
  TRAP_EFF_EAST,
  TRAP_EFF_SOUTH,
  TRAP_EFF_WEST,
  TRAP_EFF_UP,
  TRAP_EFF_DOWN,
  TRAP_EFF_NE,
  TRAP_EFF_NW,
  TRAP_EFF_SE,
  TRAP_EFF_SW,
  0,
  0,
  0
};

TTerrainInfo *TerrainInfo[MAX_SECTOR_TYPES];
TTerrainInfo::TTerrainInfo(int m, int t, int hu, int th, int dr, int h, int hum, const char * const n) :
  movement(m),
  thickness(t),
  hunger(hu),
  thirst(th),
  drunk(dr),
  heat(h),
  humidity(hum),
  name(n)
{
}

TTerrainInfo::~TTerrainInfo()
{
}

// movement cost, thickness, hung, thirst, drunk, temp, humidity, name
void assignTerrainInfo()
{
  TerrainInfo[SECT_SUBARCTIC] = new TTerrainInfo(3,3,2,2,2, -30, -6, "Sub-Arctic Wastes");
  TerrainInfo[SECT_ARCTIC_WASTE] = new TTerrainInfo(3,3,2,2,2, -20, -6, "Arctic Wastes");
  TerrainInfo[SECT_ARCTIC_CITY] = new TTerrainInfo(1,6,2,2,2, 40,  0, "Arctic City");
  TerrainInfo[SECT_ARCTIC_ROAD] = new TTerrainInfo(1,3,2,2,2, 30,  0, "Arctic Road");
  TerrainInfo[SECT_TUNDRA] = new TTerrainInfo(2,0,2,2,2, -10,  0, "Tundra");	
  TerrainInfo[SECT_ARCTIC_MOUNTAINS] = new TTerrainInfo(6,6,4,2,2, -50, -4, "Arctic Mountains");
  TerrainInfo[SECT_ARCTIC_FOREST] = new TTerrainInfo(4,8,3,2,2, 20,  4, "Arctic Forest");
  TerrainInfo[SECT_ARCTIC_MARSH] = new TTerrainInfo(5,5,2,2,2, 30,  6, "Arctic Marsh");
  TerrainInfo[SECT_ARCTIC_RIVER_SURFACE] = new TTerrainInfo(3,1,2,1,2, 30,  8, "Arctic River Surface");
  TerrainInfo[SECT_ICEFLOW] = new TTerrainInfo(4,2,2,2,2, 0,  8, "Iceflow (ocean)");
  TerrainInfo[SECT_COLD_BEACH] = new TTerrainInfo(2,3,2,2,2, 20,  7, "Cold Beach");
  TerrainInfo[SECT_SOLID_ICE] = new TTerrainInfo(8,9,2,2,2, -40, 10, "Solid Ice");
  TerrainInfo[SECT_ARCTIC_BUILDING] = new TTerrainInfo(2,6,2,2,2, 45, -2, "Arctic Building");
  TerrainInfo[SECT_ARCTIC_CAVE] = new TTerrainInfo(3,7,2,2,2, 40, -2, "Arctic Cave");
  TerrainInfo[SECT_ARCTIC_ATMOSPHERE] = new TTerrainInfo(0,0,2,2,2, 10,  2, "Arctic Atmosphere");
  TerrainInfo[SECT_ARCTIC_CLIMBING] = new TTerrainInfo(9,2,3,2,2, 30,  0, "Arctic Vertical");
  TerrainInfo[SECT_ARCTIC_FOREST_ROAD] = new TTerrainInfo(2,5,2,2,2, 25,  2, "Arctic Forest Road");
  TerrainInfo[SECT_PLAINS] = new TTerrainInfo(2,3,2,2,2, 50, -2, "Plains");
  TerrainInfo[SECT_TEMPERATE_CITY] = new TTerrainInfo(1,6,2,2,2, 70,  0, "Temperate City");
  TerrainInfo[SECT_TEMPERATE_ROAD] = new TTerrainInfo(1,3,2,2,2, 60,  0, "Temperate Road");
  TerrainInfo[SECT_GRASSLANDS] = new TTerrainInfo(3,1,2,2,2, 70,  1, "Grasslands");
  TerrainInfo[SECT_TEMPERATE_HILLS] = new TTerrainInfo(3,5,3,2,2, 60, -1, "Temperate Hills");
  TerrainInfo[SECT_TEMPERATE_MOUNTAINS] = new TTerrainInfo(6,6,4,3,2, 50, -3, "Temperate Mountains");
  TerrainInfo[SECT_TEMPERATE_FOREST] = new TTerrainInfo(4,8,3,2,2, 70,  3, "Temperate Forest");
  TerrainInfo[SECT_TEMPERATE_SWAMP] = new TTerrainInfo(5,5,2,2,2, 60,  6, "Temperate Swamp");
  TerrainInfo[SECT_TEMPERATE_OCEAN] = new TTerrainInfo(4,2,2,2,2, 60,  8, "Temperate Ocean");
  TerrainInfo[SECT_TEMPERATE_RIVER_SURFACE] = new TTerrainInfo(3,1,2,1,2, 60,  5, "Temperate River Surface");
  TerrainInfo[SECT_TEMPERATE_UNDERWATER] = new TTerrainInfo(8,9,2,1,2, 50, 10, "Temperate Underwater");
  TerrainInfo[SECT_TEMPERATE_BEACH] = new TTerrainInfo(2,3,2,2,2, 70,  4, "Temperate Beach");
  TerrainInfo[SECT_TEMPERATE_BUILDING] = new TTerrainInfo(2,6,2,2,2, 75, -2, "Temperate Building");
  TerrainInfo[SECT_TEMPERATE_CAVE] = new TTerrainInfo(3,7,2,2,2, 70, -2, "Temperate Cave");
  TerrainInfo[SECT_TEMPERATE_ATMOSPHERE] = new TTerrainInfo(0,0,2,2,2, 60,  1, "Temperate Atmosphere");
  TerrainInfo[SECT_TEMPERATE_CLIMBING] = new TTerrainInfo(9,2,2,2,2, 60,  0, "Temperate Vertical");
  TerrainInfo[SECT_TEMPERATE_FOREST_ROAD] = new TTerrainInfo(2,5,2,2,2, 65,  2, "Temperate Forest Road");
  TerrainInfo[SECT_DESERT] = new TTerrainInfo(2,3,5,6,2, 120, -9, "Desert");
  TerrainInfo[SECT_SAVANNAH] = new TTerrainInfo(3,1,2,5,2, 90, -5, "Savannah");
  TerrainInfo[SECT_VELDT] = new TTerrainInfo(3,2,2,4,2, 90, -2, "Veldt");
  TerrainInfo[SECT_TROPICAL_CITY] = new TTerrainInfo(1,6,2,3,2, 100, -1, "Tropical City");
  TerrainInfo[SECT_TROPICAL_ROAD] = new TTerrainInfo(1,3,2,3,2, 90,  0, "Tropical Road");
  TerrainInfo[SECT_JUNGLE] = new TTerrainInfo(5,8,3,3,2, 110,  3, "Jungle");
  TerrainInfo[SECT_RAINFOREST] = new TTerrainInfo(4,8,3,2,2, 110,  6, "Rain Forest");
  TerrainInfo[SECT_TROPICAL_HILLS] = new TTerrainInfo(3,5,3,3,2, 90, -2, "Tropical Hills");
  TerrainInfo[SECT_TROPICAL_MOUNTAINS] = new TTerrainInfo(6,6,4,3,2, 80, -4, "Tropical Mountains");
  TerrainInfo[SECT_VOLCANO_LAVA] = new TTerrainInfo(6,6,3,3,2, 140, -8, "Volcano/Lava");
  TerrainInfo[SECT_TROPICAL_SWAMP] = new TTerrainInfo(5,5,3,3,2, 90,  6, "Tropical Swamp");
  TerrainInfo[SECT_TROPICAL_OCEAN] = new TTerrainInfo(4,2,2,3,2, 90,  7, "Tropical Ocean");
  TerrainInfo[SECT_TROPICAL_RIVER_SURFACE] = new TTerrainInfo(3,1,2,2,2, 80,  6, "Tropical River Surface");
  TerrainInfo[SECT_TROPICAL_UNDERWATER] = new TTerrainInfo(8,9,2,1,2, 70, 10, "Tropical Underwater");
  TerrainInfo[SECT_TROPICAL_BEACH] = new TTerrainInfo(2,3,2,3,2, 80,  4, "Tropical Beach");
  TerrainInfo[SECT_TROPICAL_BUILDING] = new TTerrainInfo(2,6,2,3,2, 90, -2, "Tropical Building");
  TerrainInfo[SECT_TROPICAL_CAVE] = new TTerrainInfo(3,7,2,3,2, 85, -2, "Tropical Cave");
  TerrainInfo[SECT_TROPICAL_ATMOSPHERE] = new TTerrainInfo(0,0,2,3,2, 85,  2, "Tropical Atmosphere");
  TerrainInfo[SECT_TROPICAL_CLIMBING] = new TTerrainInfo(9,2,2,3,2, 85,  2, "Tropical Vertical");
  TerrainInfo[SECT_RAINFOREST_ROAD] = new TTerrainInfo(2,5,2,2,2, 85,  2, "Rainforest Road");
  TerrainInfo[SECT_ASTRAL_ETHREAL] = new TTerrainInfo(0,4,2,2,2, 60,  0, "Astral/Ethereal");
  TerrainInfo[SECT_SOLID_ROCK] = new TTerrainInfo(13,9,5,3,2, 60,-10, "Solid Rock");
  TerrainInfo[SECT_FIRE] = new TTerrainInfo(3,6,2,6,2, 500,-10, "Fire");
  TerrainInfo[SECT_INSIDE_MOB] = new TTerrainInfo(6,8,3,3,3, 90, 0, "Inside Mob");
  TerrainInfo[SECT_FIRE_ATMOSPHERE] = new TTerrainInfo(0,0,2,6,2, 500, -10, "Fire Atmosphere");
  TerrainInfo[SECT_MAKE_FLY] = new TTerrainInfo(9,2,2,3,2, 85, 2, "Flying Sector");
}

const char * const exits[] =
{
  "North",
  "East ",
  "South",
  "West ",
  "Up   ",
  "Down ",
  "Northeast",
  "Northwest",
  "Southeast",
  "Southwest",
};

const char * const dirs[] =
{
  "north",
  "east",
  "south",
  "west",
  "up",
  "down",
  "northeast",
  "northwest",
  "southeast",
  "southwest",
  "\n"
};

const char * const scandirs[] =
{
  "north",
  "east",
  "south",
  "west",
  "up",
  "down",
  "ne",
  "nw",
  "se",
  "sw",
  "\n"
};

const char * const dirs_to_leading[] =
{
  "to the north",
  "to the east",
  "to the south",
  "to the west",
  "leading upward",
  "leading downward",
  "to the northeast",
  "to the northwest",
  "to the southeast",
  "to the southwest"
};

const char * const dirs_to_blank[] =
{
  "to the north",
  "to the east",
  "to the south",
  "to the west",
  "upwards",
  "downwards",
  "to the northeast",
  "to the northwest",
  "to the southeast",
  "to the southwest"
};

const char * const weekdays[7] =
{
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

const char * const month_name[12] =
{
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December",
};

const char * const fullness[] =
{
  "less than half ",
  "about half ",
  "more than half ",
  ""
};

itemInfo *ItemInfo[MAX_OBJ_TYPES];

itemInfo::itemInfo(const char * const n, const char * const cn, const char * const v0, int v0x, int v0n, const char * const v1, int v1x, int v1n, const char * const v2, int v2x, int v2n, const char * const v3, int v3x, int v3n) :
  name(n),
  common_name(cn),
  val0_info(v0),
  val0_max(v0x), val0_min(v0n),
  val1_info(v1),
  val1_max(v1x), val1_min(v1n),
  val2_info(v2),
  val2_max(v2x), val2_min(v2n),
  val3_info(v3),
  val3_max(v3x), val3_min(v3n)
{
}

itemInfo::~itemInfo()
{
}

void assign_item_info()
{
  ItemInfo[ITEM_UNDEFINED] = new itemInfo("Undefined","something unknown",
     "",0, 0, 
     "",0, 0, 
     "",0, 0,
     "",0, 0);
  ItemInfo[ITEM_LIGHT] = new itemInfo("Light","a lighting device",
     "Amount of light obj gives off when lit", 20, -2,
     "Max number of ticks for refueling light. -1 means no refueling.",10000,-1,
     "Ticks left before burns out.",10000,0,
     "1 = lit, 0 = unlit.  Should be set 0", 1, 0);
  ItemInfo[ITEM_SCROLL] = new itemInfo("Scroll","a scroll",
     "Level/learnedness of spells on scroll", 50, 1,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1);
  ItemInfo[ITEM_WAND] = new itemInfo("Wand","a wand",
     "Level/learnedness of spell in wand", 50, 1,
     "Max Charges wand has", 10, 0,
     "Charges Left", 10, 0,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1);
  ItemInfo[ITEM_STAFF] = new itemInfo("Staff","a staff",
     "Level/learnedness of spell on staff", 50, 1,
     "Max Charges staff has", 10, 0,
     "Changes Left", 10, 0,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1);
  ItemInfo[ITEM_WEAPON] = new itemInfo("Weapon","some sort of weapon",
     "Special: sharpness", 0, 0,
     "Damage Level * 4", 240, 0,
     "Damage Precision", 10, 0,
     "Weapon type.  See HELP WEAPON TYPES", TYPE_MAX_HIT - TYPE_MIN_HIT, 1);
  ItemInfo[ITEM_FUEL] = new itemInfo("Fuel","a flammable liquid",
     "Amount of fuel", 256, 0,
     "Max Amount of Fuel", 256, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_OPAL] = new itemInfo("Opal/Powerstone","an opal",
     "Opal size in carats. >10 unusual  >20 very rare", 50, 1,
     "Powerstone strength (<= carat weight) (0 = not a powerstone yet)", 50, 0,
     "Current mana (<= 10* powerstone strength)", 500, 0,
     "Current successive failures. 2= can't grow stronger.", 2, 0);
  ItemInfo[ITEM_TREASURE] = new itemInfo("Treasure","an item of value",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_ARMOR] = new itemInfo("Armor","a piece of armor",
     "Apply to armor - DO NOT SET!", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_POTION] = new itemInfo("Potion","a potion",
     "Level/learnedness of spells in potion", 0, 0,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1);
  ItemInfo[ITEM_WORN] = new itemInfo("Worn Object","a piece of clothing",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_OTHER] = new itemInfo("Other","something",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_TRASH] = new itemInfo("Trash","a piece of garbage",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_TRAP] = new itemInfo("Trap","a dangerous trap",
     "Level of trap", 50, 0,
     "What triggers trap.", (1<<MAX_TRAP_EFF) - 1, 0,
     "Trap-Type (damage-type)", MAX_TRAP_TYPES, 0,
     "Charges", 50, 0);
  ItemInfo[ITEM_CHEST] = new itemInfo("Chest","a large container",
     "Max weight inside", 300, 1,
     "Special - container flags", 0, 0,
     "Vnum of key that unlocks.  -1 == none", WORLD_SIZE - 1, -1,
     "Volume chest can hold", 30000, 1);
  ItemInfo[ITEM_NOTE] = new itemInfo("Note","a sheet of paper",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_DRINKCON] = new itemInfo("Liquid Container","a liquid container",
     "Max drink units   (1 drink unit = 1.0 fl.oz., 1 gallon = 128 units)", 2560, 0,
     "Number of units left", 2560, 0,
     "Liquid type - see help liquids", MAX_DRINK_TYPES - 1, 0,
     "1 = Poisoned, 2 = Unlimited drinks, 4 = Spillable", 7, 0);
  ItemInfo[ITEM_KEY] = new itemInfo("Key","some sort of key",
     "unused?", 4, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_FOOD] = new itemInfo("Food","something edible",
     "Number of hours filled", 24, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "1 = Poisoned, 2 = Spoiled, 3 = both", 3, 0);
  ItemInfo[ITEM_MONEY] = new itemInfo("Money","a pile of money",
     "Number of talens in money pile", 50000, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_PEN] = new itemInfo("Pen","a writing implement",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_BOAT] = new itemInfo("Boat","a flotation device",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_AUDIO] = new itemInfo("Audio","a communication device",
     "Frequency of noise", 500, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_BOARD] = new itemInfo("Board","a bulletin board",
     "Min. Level to View", 60, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_BOW] = new itemInfo("Bow","a bow",
     "Unused", 0, 0,
     "Internal usage (bow flags)", 1, 0,
     "Arrow Type  [SEE HELP ARROWS]", 7, 0,
     "Range of bow(in rooms)", 10, 0);
  ItemInfo[ITEM_ARROW] = new itemInfo("Arrow","an arrow",
     "Sharpness of arrow.", 100, 0,
     "Damage Level * 4", 240, 0,
     "Damage Precision", 10, 0,
     "Arrow Type  [SEE HELP ARROWS]", 7, 0);
  ItemInfo[ITEM_BAG] = new itemInfo("Bag","a portable container",
     "Weight bag can hold", 500, 1,
     "Special - container flags", 0, 0,
     "Vnum of key that unlocks.  -1 == none", WORLD_SIZE - 1, -1,
     "Volume bag can hold", 100000, 1);
  ItemInfo[ITEM_CORPSE] = new itemInfo("Corpse","a dead body",
     "Flags", 1<<MAX_CORPSE_FLAGS - 1, 0,
     "Former Race", MAX_RACIAL_TYPES -1, 1,
     "Former Level", 0, 0,
     "Former Vnum", 0, 0);
  ItemInfo[ITEM_SPELLBAG] = new itemInfo("Spellbag","an enchanted bag",
     "Weight spellbag can hold", 500, 1,
     "Special - container flags", 0, 0,
     "Vnum of key that unlocks.  -1 == none", WORLD_SIZE - 1, -1,
     "Volume spellbag can hold", 100000, 1);
  ItemInfo[ITEM_COMPONENT] = new itemInfo("Component","a spell component",
     "Number of uses left", 10, 0,
     "Maximum number of uses", 10, 0,
     "Spell # component is for", MAX_SKILL, 0,
     "Special - decay/useage", 15, 0);
  ItemInfo[ITEM_BOOK] = new itemInfo("Book","a book",
     "", 0, 0,
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_PORTAL] = new itemInfo("Portal","an exit",
     "Special: destination & charges", 0, 0,
     "Portal type  See HELP PORTAL INFO", 12, 0,
     "Special: trap type", 0, 0,
     "Special: portal states & key number", 0, 0);
  ItemInfo[ITEM_WINDOW] = new itemInfo("Window","a window",
     "The vnum of the room window \"looks\" into", WORLD_SIZE - 1, -(WORLD_SIZE - 1),
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_TREE] = new itemInfo("Tree","some sort of vegetation",
     "", 0, 0,
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_TOOL] = new itemInfo("Tool","a tool",
     "Tool type", MAX_TOOL_TYPE - 1, 0,
     "Strength of tool. number of uses left", 100, 0,
     "Max Strength", 100, 0,
     "", 0, 0);
  ItemInfo[ITEM_HOLY_SYM] = new itemInfo("Holy Symbol","a holy symbol",
     "Strength of the holy symbol", 1250000, 0,
     "Max Strength of the symbol", 1250000, 0,
     "Faction of the Symbol", MAX_FACTIONS, 0,
     "", 0, 0);
  ItemInfo[ITEM_QUIVER] = new itemInfo("Quiver","a quiver",
     "Weight quiver can hold", 500, 1,
     "Special - container flags", 0, 0,
     "Unused", 0, 0,
     "Volume quiver can hold", 100000, 1);
  ItemInfo[ITEM_BANDAGE] = new itemInfo("Bandage","a bandage",
     "", 0, 0,
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_STATUE] = new itemInfo("Statue","a piece of statuary",
     "", 0, 0,
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_BED] = new itemInfo("Bed/Chair","a piece of furniture",
     "Special : min. position, max users", 0, 0,
     "Maximum Designed Size (users height in inches)", 100, 2,
     "Seat height above ground (in inches)", 100, 1,
     "Extra regen", 40, -1);
  ItemInfo[ITEM_TABLE] = new itemInfo("Table","a table",
     "", 0, 0,
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_RAW_MATERIAL] = new itemInfo("Raw Material","a commodity",
     "", 0, 0,
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_GEMSTONE] = new itemInfo("Gemstone","a gemstone",
     "", 0, 0,
     "", 0, 0,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_MARTIAL_WEAPON] = new itemInfo("Martial Weapon","a weapon of the martial-arts",
     "Special: sharpness", 0, 0,
     "Number of damage dice", 30, 1,
     "Size of damage dice", 30, 1,
     "Weapon type.  See HELP WEAPON TYPES", TYPE_MAX_HIT - TYPE_MIN_HIT, 1);
  ItemInfo[ITEM_JEWELRY] = new itemInfo("Jewelry","a piece of jewelry",
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0,
     "Unused", 0, 0);
  ItemInfo[ITEM_VIAL] = new itemInfo("Vial","a vial",
     "Max liquid units", 3000, 0,
     "Number of units left", 3000, 0,
     "Liquid type - see help liquids", MAX_DRINK_TYPES - 1, 0,
     "1 = Poisoned, 2 = Unlimited drinks, 4 = Spillable", 7, 0);
  ItemInfo[ITEM_PCORPSE] = new itemInfo("Player Corpse","a player's dead body",
     "Flags", 1<<MAX_CORPSE_FLAGS - 1, 0,
     "Former Race", MAX_RACIAL_TYPES -1, 1,
     "Former Level", 0, 0,
     "Former Vnum", 0, 0);
  ItemInfo[ITEM_POOL] = new itemInfo("Pool", "a pool of liquid",
     "Unused", 0, 0,		      
     "Number of drink units", 2560, 0,
     "Liquid type - see help liquids", MAX_DRINK_TYPES - 1, 0,
     "Decay (non-zero = no)", 1, 0);
  ItemInfo[ITEM_KEYRING] = new itemInfo("Keyring", "a keyring",
     "Weight keyring can hold", 500, 1,
     "Special - container flags", 0, 0,
     "Unused", 0, 0,
     "Volume keyring can hold", 100000, 1);
  ItemInfo[ITEM_RAW_ORGANIC] = new itemInfo("Fabric/Organic", "a commodity",
     "Classification Type.  See HELP RAW ORGANIC", 10, 1,
     "Total Units (-1 == Non-Unit)", 500, -1,
     "Level", 127, 1,
     "Added Effect.  See HELP RAW ORGANIC", 10, -1);
  ItemInfo[ITEM_FLAME] = new itemInfo("Fire/Flame", "a flame",
     "Light Value", 100, -100,
     "Heat Value", 99, 1,
     "", 0, 0,
     "", 0, 0);
  ItemInfo[ITEM_APPLIED_SUB] = new itemInfo("Applied Substance", "an applied substance",
     "Apply Flags", 1, 0,
     "Apply Method.  See HELP APPLY METHODS", 10, 1,
     "Level", 50, 1,
     "Spell/Skillnum", MAX_SKILL - 1, 0);
  ItemInfo[ITEM_SMOKE] = new itemInfo("Smoke", "smoke",
      "Unused", 0, 0,
      "Unused", 0, 0,
      "Unused", 0, 0,
      "Unused", 0, 0);
  ItemInfo[ITEM_ARMOR_WAND] = new itemInfo("ArmorWand","power armor",
     "Level/learnedness of spell in armor-wand", 50, 1,
     "Max Charges armor-wand has", 10, 0,
     "Charges Left", 10, 0,
     "Which spell (-1 = NONE)", MAX_SKILL - 1, -1);
  ItemInfo[ITEM_DRUG_CONTAINER] = new itemInfo("Drug Container", "a drug container",
     "Type of Drug", MAX_DRUG-1, 0,
     "Max number of ticks for refuel light. -1 means no refuleing.",10000,-1,
     "Ticks left before burns out.",10000,0,
     "1 = lit, 0 = unit.  Should be set 0", 1, 0);   
  ItemInfo[ITEM_DRUG] = new itemInfo("Drug","a drug",
     "Amount of drug", 75, 0,
     "Max Amount of drug", 75, 0,
     "Type of Drug", MAX_DRUG-1, 0,
     "", 0, 0);
  
};

const char * const wear_bits[MAX_ITEM_WEARS] =
{
  "Take",
  "Finger",
  "Neck",
  "Body",
  "Head",
  "Legs",
  "Feet",
  "Hands",
  "Arms",
  "Unused",
  "Back",
  "Waist",
  "Wrist",
  "Unused",
  "Hold",
  "Throw"
};

const char * const bodyParts[MAX_WEAR + 1] =
{
  "",
  "head",
  "neck",
  "body",
  "back",
  "arm-right",
  "arm-left",
  "wrist-right",
  "wrist-left",
  "hand-right",
  "hand-left",
  "finger-right",
  "finger-left",
  "waist",
  "leg-right",
  "leg-left",
  "foot-right",
  "foot-left",
  "hold-right",
  "hold-left",
  "leg-back-right",
  "leg-back-left",
  "foot-back-right",
  "foot-back-left",
  "\n"
};

const char * const extra_bits[] =
{
  "Glow",
  "Hum",
  "Strung",
  "Shadowy",
  "Prototype",
  "Invisible",
  "Magic",
  "Nodrop",
  "Bless",
  "Spiked",
  "Hover",
  "Rusty",
  "Anti-CLERIC",
  "Anti-MAGE",
  "Anti-THIEF",
  "Anti-WARRIOR",
  "Anti-SHAMAN",
  "Anti-DEIKHAN",
  "Anti-RANGER",
  "Anti-MONK",
  "Paired",
  "No rent",
  "Float",
  "No purge",
  "Newbie",
  "*Do Not Use*", // now unused
  "*Do Not Use*", // now unused
  "*Do Not Use*", // now unused
  "Item Attached",
  "Burning",
  "Charred",
  "No Locate",
  "\n"
};

const char * const room_bits[MAX_ROOM_BITS] =
{
  "ALWAYS-LIT",
  "DEATH",
  "NO-MOB",
  "INDOORS",
  "PEACEFUL",
  "NO-STEAL",
  "NO-SUM",
  "NO-MAGIC",
  "NO-PORTAL",
  "PRIVATE",
  "SILENCE",
  "NO-ORDER",
  "NO-FLEE",
  "HAVE-TO-WALK",
  "ARENA",
  "NO-HEAL",
  "HOSPITAL",
  "SAVE ROOMS",
  "BEING EDITED",
};


const char * const exit_bits[MAX_DOOR_CONDITIONS] =
{
  "Closed",
  "Locked",
  "Secret",
  "Destroyed",
  "No-enter",
  "Trapped",
  "Caved-In",
  "Magically Warded",
  "Sloped up",
  "Sloped down"
};

const char * const chest_bits[] =
{
  "Closeable",
  "Pickproof",
  "Closed",
  "Locked",
  "Trapped",
  "Secret (No-See)",
  "Detect-Trap/Empty-Trap",
  "Detect-Trap/Ghost-Trap",
};

const char * const door_types[] =
{
  "None",
  "Door",
  "Trapdoor",
  "Gate",
  "Grate",
  "Portcullis",
  "Drawbridge",
  "Rubble",
  "Panel",
  "Screen",
  "Hatch",
};

const char * const affected_bits[] =
{"Blind",
 "Invisible",
 "Swimming",
 "Detect-invisible",
 "Detect-magic",
 "Sense-life",
 "Levitating",
 "Sanctuary",
 "Group",
 "Webbed",
 "Curse",
 "Flying",
 "Poison",
 "Stunned",
 "Paralysis",
 "Infravision",
 "Water-breath",
 "Sleep",
 "Scrying",
 "Sneak",
 "Hide",
 "",
 "Charm",
 "",
 "Unused",
 "True_sight",
 "Eating Corpse",
 "Dragon Ride",
 "Silent",
 "",
 "Aggressor",
 "\n"
};

APP_type apply_types[MAX_APPLY_TYPES] = 
{
  {TRUE,"None"},
  {TRUE, "Strength"},
  {TRUE, "Intelligence"},
  {TRUE, "Wisdom"},
  {TRUE, "Dexterity"},
  {TRUE, "Constitution"},    // 5 
  {TRUE, "Karma"},
  {TRUE, "Sex"},
  {TRUE, "Age"},
  {TRUE, "Height"},
  {TRUE, "Weight"},      // 10 
  {TRUE, "Armor/AC"},
  {TRUE, "Max Hit Points"},
  {TRUE, "Mana"},
  {TRUE, "Move"},
  {FALSE, "Hitroll"},
  {FALSE, "Damroll"},
  {FALSE, "Hit & Dam"},  // 20 
  {TRUE, "Immunity"},
  {TRUE, "Skill/Spell"},
  {TRUE, "Magic Affect"},
  {TRUE, "Light"},
  {TRUE, "Noise"},
  {TRUE, "Can Be Seen"},
  {TRUE, "Vision"},   // 30 
  {FALSE, "Protection"},
  {TRUE, "Brawn"},
  {TRUE, "Agility"},
  {TRUE, "Focus"},
  {TRUE, "Speed"},   // 35 
  {TRUE, "Perception"},
  {TRUE, "Charisma"},
  {TRUE, "Discipline"},
  {FALSE, "Spell Hitroll"},
  {FALSE, "Hit Points"},
};


const struct pc_class_names_data classNames[MAX_CLASSES] =
{
  {MAGE_LEVEL_IND, CLASS_MAGE, "mage", "Mage"},
  {CLERIC_LEVEL_IND, CLASS_CLERIC, "cleric", "Cleric"},
  {WARRIOR_LEVEL_IND, CLASS_WARRIOR, "warrior", "Warrior"},
  {THIEF_LEVEL_IND, CLASS_THIEF, "thief", "Thief"},
  {SHAMAN_LEVEL_IND, CLASS_SHAMAN, "shaman", "Shaman"},
  {DEIKHAN_LEVEL_IND, CLASS_DEIKHAN, "deikhan", "Deikhan"},
  {MONK_LEVEL_IND, CLASS_MONK, "monk", "Monk"},
  {RANGER_LEVEL_IND, CLASS_RANGER, "ranger","Ranger"}
};

const char * const action_bits[] =
{
  "Changed Strings",
  "Sentinel",
  "Scavenger",
  "Disguised",
  "Nice-thief",
  "Aggresive",
  "Stay-zone",
  "Wimpy",
  "Annoying",
  "Hateful",
  "Afraid",
  "Immortal",
  "Hunting",
  "Deadly Poison",
  "Polymorphed",
  "Guarding",
  "Skeleton",
  "Zombie",
  "Ghost",
  "Diurnal",
  "Nocturnal",
  "Protector",
  "Protectee",
  "\n"
};


const char * const player_bits[] =
{
  "Brief",
  "Compact",
  "Wimpy",
  "Newbie-Help",
  "No-Hassle",
  "Stealth",
  "Hunting",
  "Mailing",
  "Logged",
  "Bugging",
  "Vt100",
  "Color",
  "Immortal",
  "Ansi",
  "Seeksgroup",
  "Banished",
  "Regular-Vt100",
  "No-Messages",
  "Right-Handed",
  "AFK",
  "132 columns",
  "Solo_quest",
  "Group_quest",
  "No snoop",
  "",
  "God-NoShout",
  "",
  "",
  "Killable",
  "Anonymous",
  "\n"
};


const char * const attr_player_bits[] =
{
  "Brief",
  "Compact",
  "Wimpy",
  "Newbie-Helper",
  "No-hassle",
  "Stealth",
  "Hunting",
  "Tell-an-immort-you-saw-this (MAILING)",
  "",				// note ... no LOGGED 
  "Tell-an-immort-you-saw-this (BUGGING)",
  "VT-100",
  "Color",
  "Immortal",
  "Ansi",
  "Seeks Group",
  "*BANISHED*",
  "Regular-VT100",
  "No-Messages",
  "Right-Handed",
  "AFK",
  "132 columns",
  "Solo quest",
  "Group quest",
  "",
  "Tell-an-immort-you-saw-this (unused1)",
  "God No-Shout",
  "Tell-an-immort-you-saw-this (NODIMD)",
  "Tell-an-immort-you-saw-this (unused5)",
  "Killable",
  "Anonymous",
  "\n"
};

const char * const prompt_mesg[] =
{
  "near death",
  "horrid",
  "awful",
  "bad",
  "wounded",
  "hurt",
  "injured",
  "good",
  "excellent",
  "perfect",
  "heroic",
  "\n"
};


const char * const position_types[] =
{
  "Dead",
  "Mortally wounded",
  "Incapacitated",
  "Stunned",
  "Sleeping",
  "Resting",
  "Sitting",
  "Engaged",
  "Fighting",
  "Crawling",
  "Standing",
  "Mounted",
  "Flying",
  "\n"
};

const char * const connected_types[MAX_CON_STATUS] =
{
  "Playing",    // 0
  "Get name",
  "Confirm name",
  "Read Password",
  "Confirm password",    // 5
  "Get sex",
  "Read motd",
  "Get class",     // 10
  "Confirm changed pword",
  "Wizlock",
  "Get race",    // 15
  "General Delete",
  "Physical Stats",
  "Choose handedness",   // 20
  "Disconnection",
  "New account",
  "Account Password",
  "New login",
  "New account Password",   // 25
  "Account email",
  "Account terminal",
  "Account connection",
  "Account change password",
  "Account change password",   // 30
  "Account change password",
  "Account delete character",
  "Account delete account",
  "Editing",
  "Disclaimer",   // 35
  "Timezone",
  "Delete confirmation",
  "Disclaimer page 2",
  "Disclaimer page 3",    // 40
  "Wizlock New",
  "Mental Stats",
  "Utility Stats",
  "Done Creation",
  "Stats Start",   // 45
  "Done Enter",
  "Stats rules",
  "Stats rules 2",
  "Listserver choice",
  "Hometown : human",
  "Hometown : elf",
  "Hometown : dwarf",
  "Hometown : gnome",
  "Hometown : ogre",
  "Hometown : hobbit",
};


const char * const portal_self_enter_mess[MAX_PORTAL_TYPE] =
{
  "enter",
  "step into",
  "step onto",
  "climb down",
  "climb up",
  "slide down",
  "are sucked into",
  "are swallowed by",
  "are flushed down",
  "fall into",
  "step through",
  "fall down",
  "climb over",
  "climb into"
};


const char * const portal_other_enter_mess[MAX_PORTAL_TYPE] =
{
  "enters",
  "steps into",
  "steps onto",
  "climbs down",
  "climbs up",
  "slides down",
  "is sucked into",
  "is swallowed by",
  "is flushed down",
  "falls into",
  "steps through",
  "falls down",
  "climbs over",
  "climbs into"
};


const char * const portal_self_exit_mess[MAX_PORTAL_TYPE] =
{
  "exits",
  "steps out of",
  "steps off of",
  "climbs up from",
  "climbs down from",
  "climbs off of",
  "is thrown into the room from",
  "is spit out of",
  "is spewed out of",
  "climbs out of",
  "steps from",
  "climbs out of",
  "climbs over",
  "climbs out of"
};

const char * const editor_types_oedit[] =
{
  "save",           //  1
  "load",           //  2
  "modify",         //  3
  "list",           //  4
  "remove",         //  5
  "create",         //  6
  "name",           //  7
  "long_desc",      //  8
  "short_desc",     //  9
  "max_struct",     // 10
  "cur_struct",     // 11
  "weight",         // 12
  "volume",         // 13
  "cost",           // 14
  "decay_time",     // 15
  "extra",          // 16
  "can_be_seen",    // 17
  "max_exist",      // 18
  "average",        // 19
  "replace",        // 20
  "resave",         // 21
  "\n"
};

const char * const editor_types_medit[] =
{
  "save",           //  1
  "load",           //  2
  "modify",         //  3
  "list",           //  4
  "remove",         //  5
  "name",           //  6
  "short_desc",     //  7
  "long_desc",      //  8
  "description",    //  9
  "level",          // 10
  "attacks",        // 11
  "hitroll",        // 12
  "armor_class",    // 13
  "hp_bonus",       // 14
  "faction",        // 15
  "bare_hand",      // 16
  "money_constant", // 17
  "race",           // 18
  "sex",            // 19
  "max_exist",      // 20
  "default_pos",    // 21
  "height",         // 22
  "weight",         // 23
  "class",          // 24
  "vision",         // 25
  "can_be_seen",    // 26
  "room_sound",     // 27
  "oroom_sound",    // 28
  "replace",        // 29
  "resave",         // 30
  "average",        // 31
  "\n"
};

const char * const material_groups[] =
{
  "General",
  "Nature/Skin Types",
  "Mineral",
  "Metals",
  "\n"
};

const char * const attack_modes[] =
{
  "normal",
  "defensive",
  "offensive",
  "berserk",
  "\n"
};

// formula for fall/water susceptibility is:
// chance in 10 of being damaged + (10 * points to remove) 
// So... if the number is 99 it would mean, 90% (9 in 10)
// chance of being damaged, with 9 struct points removed
// when it is damaged.  Note, maximum damage rate is:
// 249, meaning 90% chance of 24 struct points damage.
// 0 means no chance of damage.  11 would mean, 10% chance
// of being damaged 1 struct point.


extern int repairMetal(TBeing *,TObj *);

// float weight == maximum weight an object can be of this
// type and stay floating.  0 means object will sink no matter
// what.  255 means object will float no matter what.  */
// hardness: 0 = air
// 5 = paper, 10 = rubber, 25 = wood, 45+ = metal, 100 = dimond 
// fall/water susc:
// chance of damage: dice(1,10) <= number%10
// amount of damage: number/10
// flammability: how many cubic inches burn per tick
// slash susc, blunt susc, fire susc, pierce susc,
// hardness, water susc, fall susc, float weight, noise, vol_mult, conduct
// flammability, acid_susc, price, availability, repair proc, mat_name */

const struct material_type_numbers material_nums[200] =
{
  {100, 100, 100, 100, 50, 11, 11, 5,10, 1, 0, 0, 0, 0, 0, NULL, "Undefined"},
  {100,  20, 100, 100, 5, 209, 0, 5,-1,8, 0, 1000, 75, 0, 0, NULL,"Paper"},
  {90, 0, 100, 100, 1, 11, 0, 30, -3,8, 0, 750, 65, 0, 0, NULL,"Cloth"},
  {50, 50, 95, 80, 7, 153, 105, 0,-2,1, 0, 250, 40, 0, 0, NULL,"Wax"},
  {0, 100, 0, 50, 40, 0, 249, 0, 3,1, 0, 0, 5, 0, 0, NULL,"Glass"},
  {40, 30, 75, 50, 25, 42, 53, 255, 3,1, 0, 500, 25, 0, 0, NULL,"Wood"},
  {90, 0, 50, 100, 7, 22, 0, 40, -2,6, 0, 900, 80, 0, 0, NULL,"Silk"},
  {75, 75, 75, 100, 5, 35, 55, 30, 1,2, 0, 0, 95, 0, 0, NULL,"Foodstuff"},
  {65, 25, 25, 100, 3, 0, 51, 0, -1,2, 0, 500, 50, 0, 0, NULL,"Plastic"},
  {75, 0, 0, 100, 10, 0, 0, 0, -2,1, 0, 0, 66, 0, 0, NULL,"Rubber"},
  {40, 20, 100, 100, 6, 209, 0, 10, 0,1, 0, 900, 80, 0, 0, NULL,"Cardboard"},
  {40, 10, 75, 50, 2, 105, 0, 100, -1,2, 0, 750, 85, 0, 0, NULL,"String"},
  {75, 50, 25, 50, 4, 59, 79, 2, 0,1, 0, 0, 25, 0, 0, NULL,"Plasma"},
  {83, 0, 100, 95, 2, 11, 0, 25, -3,6, 0, 600, 60, 0, 0, NULL,"Toughened cloth"},
  {80, 100, 10, 40, 3, 0, 99, 20, 3, 1, 0, 0, 90, 0, 0, NULL,"Coral"},
  {80, 3, 80, 10, 2, 101, 0, 100, -1, 3, 0, 750, 70, 0, 0, NULL,"Horsehair"},
  {40, 10, 75, 50, 2, 105, 0, 100, -1, 2, 0, 750, 85, 0, 0, NULL,"Hair"},
  {90, 5,  2, 50, 2, 103, 11,  99, -1, 4, 0, 0, 30, 0, 0, NULL,"Ash/powder"},
  {50, 65, 70, 25, 2,  92, 53, 100, -1, 1, 0, 0, 60, 0, 0, NULL, "Pumice"},
// slash susc, blunt susc, fire susc, pierce susc,
// hardness, water susc, fall susc, float weight, noise, vol_mult, conduct
// flammability, acid_susc, price, availability, repair proc, mat_name 
  {50, 25, 4, 40, 12, 30, 36, 3, -1, 3, 0, 0, 25, 0, 0, NULL,"Laminate"},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, NULL,""},
  {50, 50, 50, 50, 0, 11, 55, 5, 10, 1, 0, 0, 50, 0, 0, NULL,"Generic organic"},
  {75, 5, 5, 70, 20, 55, 55, 3, -2, 4, 0, 400, 20, 0, 0, NULL,"Leather"},
  {55, 5, 1, 50, 25, 35, 36, 3, -1, 3, 0, 200, 30, 0, 0, NULL,"Toughened leather"},
  {10, 50, 10, 30, 50, 11, 0, 0, 5, 1, 0, 0, 10, 0, 0, NULL,"Dragon scale"},
  {70, 0, 100, 75, 15, 33, 0, 50, -2, 4, 0, 800, 0,   0,   0, NULL,"Wool"},
  {45, 5, 100, 60, 15, 33, 0, 50, -2, 4, 0, 800, 30,   0,   0, NULL,"Fur"},
  {30, 5, 100, 45, 15, 0, 0, 60, -3, 1, 0, 900, 15,   0,   0, NULL,"Feathered"},
  {0, 0, 0, 0, 4, 0, 0, 255, 3, 1, 0, 0, 3,   0,   0, NULL,"Water/liquid"},
  {0, 0, 0, 0, 0, 249, 105, 0, 8, 1, 0, 0, 3,   0,   0, NULL,"Fire/flaming"},
  {0, 0, 0, 0, 0, 0, 205, 0, 10, 1, 0, 0, 55,   0,   0, NULL,"Earth/stone"},
  {25, 25, 100, 25, 0, 0, 205, 0, 10, 1, 0, 0, 35,   0,   0, NULL,"Generic elemental"},
  {25, 25, 25, 25, 22, 205, 109, 255, 2, 1, 0, 0, 6,   0,   0, NULL,"Ice"},
  {0, 0, 0, 0, 0, 249, 0, 0, 13, 1, 0, 0, 7,   0,   0, NULL,"Lightning"},
  {0, 0, 5, 0, 0, 0, 0, 0, 5, 1, 0, 0, 10,   0,   0, NULL,"Chaos"},
  {45, 25, 0, 50, 5, 101, 249, 0, 3, 1, 0, 0, 3,   0,   0, NULL,"Clay"},
  {0, 100, 50, 0, 30, 101, 249, 0, 10, 1, 0, 0, 2,   0,   0, NULL,"Porcelain"},
  {50, 10, 100, 10, 10, 22, 0, 30, 2, 1, 0, 1000, 77,   0,   0, NULL,"Straw"},
  {5, 50, 10, 0, 40, 0, 39, 0, 10, 1, 0, 0, 66,   0,   0, NULL,"Pearl"},
  {100, 10, 60, 100, 12, 11, 0, 10, 0, 1, 0, 150, 85,   0,   0, NULL,"Flesh"},
  {55, 5, 63, 60, 15, 11, 0, 10, -1, 1, 0, 800, 72,   0,   0, NULL,"Cat fur"},
  {45, 5, 60, 70, 15, 11, 0, 10, 1, 1, 0, 800, 66,   0,   0, NULL,"Dog fur"},
  {65, 5, 60, 80, 13, 11, 0, 10, -2, 1, 0, 800, 68,   0,   0, NULL,"Rabbit fur"},
  {45, 10, 10, 50, 1, 249, 0, 255, -5, 1, 0, 0, 5,   0,   0, NULL,"Ghostly"},
  {43, 7, 5, 45, 24, 36, 36, 0, -1, 5, 0, 300, 25,   0,   0, NULL,"Dwarven leather"},
  {83, 13,15, 75, 25, 56, 56, 9, -3, 6, 0, 400, 33,   0,   0, NULL,"Soft leather"},
  {53,57,45, 65, 30, 0, 12,30, 2, 2, 0, 0, 12,   0,   0, NULL,"Fishscale"},
  {90, 15,20,95,33,12,11, 7, 1, 2, 0, 200, 10,   0,   0, NULL,"Ogre hide"},
  {70, 0, 100, 75, 10, 33, 0, 50, -2, 4, 0, 750, 0,   0,   0, NULL,"Hemp"},
// slash susc, blunt susc, fire susc, pierce susc,
// hardness, water susc, fall susc, float weight, noise, vol_mult, conduct
// flammability, acid_susc, price, avail, repair proc, mat_name 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {50, 50, 50, 50, 50, 0, 59, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Generic mineral"},
  {5, 50, 8, 20, 50, 0, 39, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Jeweled"},
  {10, 60, 10, 24, 48, 0, 39, 0, 10, 1, 1, 0, 31,   0,   0, NULL,"Runed"},
  {25, 45, 0, 25, 60, 0, 39, 0, 13, 1, 0, 0, 31,   0,   0, NULL,"Crystal"},
  {1, 12, 0, 5, 100, 0, 0, 0, 14, 1, 0, 0, 21,   0,   0, NULL,"Diamond"},
  {10, 30, 0, 10, 50, 0, 39, 0, 10, 1, 0, 0, 24,   0,   0, NULL,"Ebony"},
  {3, 15, 0, 3, 75, 0, 0, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Emerald"},
  {21, 20, 0, 21, 50, 0, 39, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Ivory"},
  {21, 20, 0, 21, 50, 0, 39, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Obsidian"},
  {21, 20, 0, 21, 50, 0, 39, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Onyx"},
  {11, 18, 0, 11, 60, 0, 39, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Opal"},
  {2, 56, 0, 2, 80, 0, 39, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Ruby"},
  {5, 47, 0, 5, 70, 0, 39, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Sapphire"},
  {6, 20, 0, 6, 90, 0, 79, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Marble"},
  {8, 24, 0, 8, 95, 0, 99, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Stone"},
  {15, 40, 0, 15, 40, 0, 39, 0, 8, 1, 0, 0, 83,   0,   0, NULL,"Skull/bone"},
  {13, 35, 0, 13, 80, 0, 29, 0, 10, 1, 0, 0, 44,   0,   0, NULL,"Jade"},
  {5, 60, 0, 10, 65, 0, 39, 0, 10, 1, 0, 0, 20,   0,   0, NULL,"Amber"},
  {11, 45, 0, 15, 55, 0, 39, 0, 11, 1, 0, 0, 31,   0,   0, NULL,"Turquoise"},
  {16, 42, 0, 12, 55, 0, 39, 0, 11, 1, 0, 0, 31,   0,   0, NULL,"Amethyst"},
  {45, 15, 0, 25, 40, 0, 39, 0, 10, 1, 1, 0, 25,   0,   0, NULL,"Mica"},
  {15, 15, 0, 10, 85, 0, 10, 0, 14, 1, 0, 0, 25,   0,   0, NULL,"Dragonbone"},
  {16, 42, 0, 12, 55, 0, 39, 0, 11, 1, 0, 0, 31,   0,   0, NULL,"Malachite"},
  {6, 20, 0, 6, 90, 0, 71, 0, 10, 1, 0, 0, 31,   0,   0, NULL,"Granite"},
  {25, 45, 0, 15, 60, 0, 89, 0, 6, 1, 0, 0, 31,   0,   0, NULL,"Quartz"},
  {2, 56, 0, 2, 90, 0, 39, 0, 6, 1, 0, 0, 31,   0,   0, NULL,"Jet"},
  {1, 12, 0, 5, 100, 0, 0, 0, 14, 1, 0, 0, 21,   0,   0, NULL,"Corundum"},
// slash susc, blunt susc, fire susc, pierce susc,
// hardness, water susc, fall susc, float weight, noise, vol_mult, conduct
// flammability, acid_susc, price, avail, repair proc, mat_name 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
  {0, 50, 0, 20, 55, 101, 101, 0, 20, 1, 1, 0, 70,   0,   0, repairMetal,"Generic metal"},
  {0, 75, 0, 20, 45, 101, 101, 0, 20, 1, 1, 0, 70,   0,   0, repairMetal,"Copper"},
  {0, 70, 0, 15, 55, 101, 101, 0, 15, 1, 1, 0, 70,   0,   0, repairMetal,"Scale"},
  {0, 60, 0, 12, 55, 101, 101, 0, 16, 1, 1, 0, 70,   0,   0, repairMetal,"Banded"},
  {0, 75, 0, 20, 55, 101, 101, 0, 30, 1, 1, 0, 70,   0,   0, repairMetal,"Chain"},	
  {0, 60, 0, 0,  65, 101, 101, 0, 35, 1, 1, 0, 70,   0,   0, repairMetal,"Plate"},
  {0, 70, 0, 10, 50, 101, 101, 0, 25, 1, 1, 0, 70,   0,   0, repairMetal,"Bronze"},
  {0, 79, 0, 5,  50, 101, 101, 0, 25, 1, 1, 0, 70,   0,   0, repairMetal,"Brass"},
  {0, 50, 0, 0,  60, 101, 101, 0, 27, 1, 1, 0, 70,   0,   0, repairMetal,"Iron"},
  {0, 45, 0, 0,  63, 101, 101, 0, 33, 1, 1, 0, 70,   0,   0, repairMetal,"Steel"},
  {0, 40, 0, 0,  65, 101, 101, 0, 23, 1, 1, 0, 50,   0,   0, repairMetal,"Mithril"},
  {0, 40, 0, 0,  70, 101, 101, 0, 29, 1, 1, 0, 45,   0,   0, repairMetal,"Admantium"},
  {0, 50, 0, 0,  50, 101, 101, 0, 35, 1, 1, 0, 70,   0,   0, repairMetal,"Silver"},
  {0, 50, 0, 2,  49, 101, 101, 0, 35, 1, 1, 0, 70,   0,   0, repairMetal,"Gold"},
  {0, 50, 0, 0,  50, 101, 101, 0, 39, 1, 1, 0, 70,   0,   0, repairMetal,"Platinum"},
  {0, 25, 0, 0,  65, 101,  51, 0, 31, 1, 1, 0, 70,   0,   0, repairMetal,"Titanium"},
  {0, 80, 0, 30, 43, 101, 101, 0, 34, 1, 1, 0, 70,   0,   0, repairMetal,"Aluminum"},
  {0, 55, 0, 42, 55, 101, 101, 0, 30, 1, 1, 0, 70,   0,   0, repairMetal,"Ringmail"},
  {20, 70, 0, 42, 35, 101, 101, 0, 15, 1, 0, 0, 40,   0,   0, repairMetal,"Gnomemail"},
  {0, 50, 0, 0, 55, 101, 101, 0, 30, 1, 1, 0, 42,   0,   0, repairMetal,"Electrum"},
  {30, 20, 5, 20, 55, 101, 101, 0, 37, 1, 1, 0, 10,   0,   0, repairMetal,"Athanor"},
  {0, 85, 0, 0, 40, 101, 101, 0, 33, 1, 1, 0, 77,   0,   0, repairMetal, "Tin"},
  {0, 47, 0, 3, 47, 101, 101, 0, 35, 1, 1, 0, 70,   0,   0, repairMetal,"Tungstan"},
  {0, 50, 0, 20, 65, 101, 101, 0, 10, 1, 1, 0, 70,   0,   0, repairMetal,"Admintite"},
  {0, 25, 0,  0, 50,   0,  25, 0, 10, 1, 0, 0, 89,   0,   0, repairMetal, "Terbium"},
  {0, 40, 0, 0,  65, 101, 101, 0, 23, 1, 1, 0, 50,   0,   0, repairMetal,"Mithril"},
  {0, 45, 0, 0,  63, 101, 101, 0, 33, 1, 1, 0, 70,   0,   0, repairMetal,"Steel"},
  //these previous two types are duplicates, of previous metals. Smile and nod.
  {0, 0, 0, 0, 55, 0, 0, 0, 0, 1, 0, 0, 0,   0,   0, NULL,""},
// slash susc, blunt susc, fire susc, pierce susc,
// hardness, water susc, fall susc, float weight, noise, vol_mult, conduct
// flammability, acid_susc, price, avail, repair proc, mat_name 
};

// this is how blunt/sharp/pointy weapon is
const byte sharpness[] =
{
  20,         // TYPE_HIT
  50,         // TYPE_BLUDGEON
  70,         // TYPE_PIERCE
  50,         // TYPE_SLASH
  20,         // TYPE_WHIP
  40,         // TYPE_CLAW
  35,         // TYPE_BITE
  65,         // TYPE_STING
  60,         // TYPE_CRUSH
  60,         // TYPE_CLEAVE
  70,         // TYPE_STAB
  60,         // TYPE_SMASH
  40,         // TYPE_SMITE
  30,         // TYPE_PUMMEL
  25,         // TYPE_FLAIL
  15,         // TYPE_BEAT
  66,         // TYPE_THRASH
  70,         // TYPE_THUMP
  80,         // TYPE_WALLOP
  75,         // TYPE_BATTER
  55,         // TYPE_STRIKE
  45,         // TYPE_CLUB
  70,         // TYPE_SLICE
  50,         // TYPE_POUND
};     

const char * const body_flags[] =
{
  "bleeding",
  "infected",
  "paralyzed",
  "broken",
  "scarred",
  "bandaged",
  "missing",
  "useless",
  "leprosed",
  "transformed",
  "\n"
};

const char * const card_names[14] =
{
  "Nothing",
  "Ace",
  "Two",
  "Three",
  "Four",
  "Five",
  "Six",
  "Seven",
  "Eight",
  "Nine",
  "Ten",
  "Jack",
  "Queen",
  "King"
};

const int race_vol_constants[MAX_WEAR] = {
  0,     // none
  0,     // finger R
  0,     // finger L
  900,   // neck
  11000, // body
  2500,  // head
  6000,  // leg right
  6000,  // leg left
  1600,  // foot right
  1600,  // foot left
  800,   // hand right
  800,   // hand left
  2000,  // arm right
  2000,  // arm left
  2500,  // back
  4000,  // waiste
  400,   // wrist right
  400,   // wrist left
  0,     // hold right
  0,     // hold left
  6000,  // extra leg right
  6000,  // extra leg left
  1600,  // extra foot right
  1600,  // extra foot left
};

const char * const immunity_names[MAX_IMMUNES] =
{
  "Heat",     // 0
  "Cold",
  "Acid",
  "Poison",
  "Sleep",
  "Paralysis",     // 5
  "Charm",
  "Pierce",
  "Slash",
  "Blunt",
  "Non-Magic",   // 10
  "+1 Weapons",
  "+2 Weapons",
  "+3 Weapons",
  "Air",
  "Energy/Thaumaturgy",    // 15
  "Electricity",
  "Disease",
  "Suffocation",
  "Skin Problems",
  "Bone Problems",      // 20
  "Bleeding",
  "Water",
  "Drain",
  "Fear",
  "Earth",     // 25
  "Summon",
  "",
};

// make first letter '*' to block the string anywhere in the name
// otherwise, blocks the exact name (case insensative)
// that is "*fuck" blocks "somefuck", "fucksome", and "sfucky"
const char * const illegalnames[] = 
{
  // SWEAR WORDS
  "*fuk",
  "*fuck",
  "*shit",
  "*dick",
  "*cunt",
  "*pussy",
  "*twat",
  "*bitch",
  "*bastard",
  "*fart",
  "*feces",
  "*mofo",
  "*pimp",
  "*nigger",
  "*penis",
  "*talen",
  "*crap",
  "ass",
  "jackmeoff",
  "piss",
  "pee",
  "urine",
  "cum",
  "butt",
  "suck",
  "crotch",

  // IMM NAME SIMILARITY
  "brutus",
  "fatopr",
  "batoper",

  // BASIC OBJECTS
  "*shield",

  // COLORS
  "gold",
  "silver",
  "bronze",
  "iron",
  "metal",
  "black",
  "white",
  "red",
  "blue",
  "green",

  // SILLY COMMON WORDS
  "*some",  // someone, something, etc
  "himself",
  "itself",
  "herself",
  "the",
  "an",
  "and",
  "there",
  "you",
  "then",
  "women",
  "men",

  // BLOCKED AS CAUSES CODE PROBLEMS
  "blob",     // needed for infravision 
  "blobs",    // needed for infravision 
  "link",     // needed for purging links 
  "links",    // needed for purging links 
  "noone",   // needed for makeleader
  "comment", // needed for comments
  "all",      // "group all"
  "unknown",   // Log messages use this sometimes
  "share",   // group needs this

  // MAJOR CHARACTERS
  "logrus",
  "galek",
  "anilstathis",
  "theoman",
  "mezan", // deities
  "luna", // deities
  "icon", // deities
  "elyon", // deities
  "jevon", // deities
  "omnon", // deities
  "amana", // deities
  "menanon", // deities

  // DIRECTIONS that are abbrevs
  "nor",
  "nort",
  "north",
  "eas",
  "east",
  "sou",
  "sout",
  "south",
  "wes",
  "west",
  "up",
  "dow",
  "down",
  "northe",
  "northea",
  "northeas",
  "northeast",
  "northw",
  "northwe",
  "northwes",
  "northwest",
  "southe",
  "southea",
  "southeas",
  "southeast",
  "southw",
  "southwe",
  "southwes",
  "southwest",
  "exit",
  "exits",
  "\n"
};

// CHANGE NUMBER if options added
const char * const color_options[10] =
{
  "Color Basic           (color enabled)",
  "Color Objects         (color objects)",
  "Color Creatures       (color creatures)",
  "Color Rooms           (color rooms)",
  "Color Room Name       (color room_name)",
  "Color Spellcasting    (color spells)",
  "Color Communications  (color communications)",
  "Color Shouts          (color shouts)",
  "Color Logs (Imm Only) (color logs)",
  "Color Codes(Imm Only) (color codes)",
};

const char * const auto_name[MAX_AUTO] =
{
  "No Spam          (auto spam)",
  "Auto Eat         (auto eat)",
  "Auto Kill        (auto kill)",
  "Auto Loot        (auto loot-money)",
  "Auto Loot        (auto loot-all)",
  "No Harm PCs      (auto harm)",
  "No Shout         (auto shout)",
  "Auto AFK         (auto afk)",
  "Auto Split       (auto split)",
  "Auto Success     (auto success)",
  "Auto Pouch       (auto pouch)",
  "Auto Join        (auto join)",
  "Auto Dissect     (auto dissect)",
  "Auto Engage      (auto engage)",
  "Auto No-fight    (auto engage-all)",
  "Auto Hunt        (auto hunt)",
  "Auto No Spells   (auto nospells)",
  "Auto Half Spells (auto halfspells)",
  "Auto Limbs       (auto limbs)",
  "Auto Tips        (auto tips)",
};

// disc_num, class_nums, practice
// COSMO CLASS MARKER
const struct disc_names_data discNames[MAX_DISCS] =
{
  {DISC_MAGE, CLASS_MAGIC_USER, "mage abilities"},   // 0 
  {DISC_AIR, CLASS_MAGIC_USER, "air magic"},         // 1
  {DISC_ALCHEMY, CLASS_MAGIC_USER, "alchemy"},
  {DISC_EARTH, CLASS_MAGIC_USER, "earth magic"},
  {DISC_FIRE, CLASS_MAGIC_USER, "fire magic"},
  {DISC_SORCERY, CLASS_MAGIC_USER, "sorcery"},        // 5
  {DISC_SPIRIT, CLASS_MAGIC_USER, "spirit magic"},
  {DISC_WATER, CLASS_MAGIC_USER, "water magic"},
  {DISC_CLERIC, CLASS_CLERIC, "clerical abilities"},
  {DISC_WRATH, CLASS_CLERIC, "wrath of the deities"},
  {DISC_AFFLICTIONS, CLASS_CLERIC, "afflictions"},   // 10
  {DISC_AEGIS, CLASS_CLERIC, "aegis"},
  {DISC_CURES, CLASS_CLERIC, "cures"},
  {DISC_HAND_OF_GOD, CLASS_CLERIC, "hand of the deities"},
  {DISC_WARRIOR, CLASS_WARRIOR, "warrior abilities"},
  {DISC_HTH, CLASS_WARRIOR, "hand to hand abilities"},  //15
  {DISC_BRAWLING, CLASS_WARRIOR, "brawling skills"},
  {DISC_PHYSICAL, CLASS_WARRIOR, "physical skills"},
  {DISC_SMYTHE, CLASS_WARRIOR, "smythe"},
  {DISC_RANGER, CLASS_RANGER, "ranger abilities"},
  {DISC_RANGER_FIGHT, CLASS_RANGER, "fighting skills"},  //20
  {DISC_SHAMAN_ARMADILLO, CLASS_SHAMAN, "armadillo abilities"},
  {DISC_ANIMAL, CLASS_RANGER, "animals"},
  {DISC_PLANTS, CLASS_RANGER, "plants"},
  {DISC_SURVIVAL, CLASS_RANGER, "survival skills"}, 
  {DISC_DEIKHAN, CLASS_DEIKHAN, "deikhan abilities"},   // 25
  {DISC_DEIKHAN_FIGHT, CLASS_DEIKHAN, "fighting skills"},
  {DISC_MOUNTED, CLASS_DEIKHAN, "mounted skills"},
  {DISC_DEIKHAN_AEGIS, CLASS_DEIKHAN, "aegis"},
  {DISC_DEIKHAN_CURES, CLASS_DEIKHAN, "cures"},
  {DISC_DEIKHAN_WRATH, CLASS_DEIKHAN, "wrath of the deities"},  // 30
  {DISC_MONK, CLASS_MONK, "monk abilities"},
  {DISC_MEDITATION_MONK, CLASS_MONK, "meditation/internal abilities"},
  {DISC_LEVERAGE, CLASS_MONK, "balance and leverage skill"},
  {DISC_MINDBODY, CLASS_MONK, "mind and body control"},
  {DISC_FOCUSED_ATTACKS, CLASS_MONK, "focused attacks"},
  {DISC_BAREHAND, CLASS_MONK, "barehand"},
  {DISC_THIEF, CLASS_THIEF, "thief abilities"},          // 40
  {DISC_THIEF_FIGHT, CLASS_THIEF, "fighting skills"},
  {DISC_MURDER, CLASS_THIEF, "murder"},
  {DISC_LOOTING, CLASS_THIEF, "looting"},
  {DISC_POISONS, CLASS_THIEF, "poisons"},
  {DISC_STEALTH, CLASS_THIEF, "stealth"},               // 45
  {DISC_TRAPS, CLASS_THIEF, "traps"},
  {DISC_SHAMAN, CLASS_SHAMAN, "shaman abilities"},      
  {DISC_SHAMAN_FROG, CLASS_SHAMAN, "frog abilities"},
  {DISC_SHAMAN_ALCHEMY, CLASS_SHAMAN, "alchemy"},
  {DISC_SHAMAN_SKUNK, CLASS_SHAMAN, "skunk abilities"},       // 50
  {DISC_SHAMAN_SPIDER, CLASS_SHAMAN, "spider abilities"},
  {DISC_SHAMAN_CONTROL, CLASS_SHAMAN, "control"},            
  {DISC_TOTEM, CLASS_SHAMAN, "totemism"},
  {DISC_WIZARDRY, CLASS_MAGE | CLASS_SHAMAN, "wizardry"},
  {DISC_FAITH, CLASS_CLERIC | CLASS_DEIKHAN, "faith"},  // 55
  {DISC_SLASH, CLASS_WARRIOR | CLASS_RANGER | CLASS_THIEF | CLASS_DEIKHAN , "slash"},
  {DISC_BLUNT, CLASS_WARRIOR | CLASS_CLERIC | CLASS_DEIKHAN | CLASS_SHAMAN, "blunt"},                              
  {DISC_PIERCE, CLASS_WARRIOR | CLASS_THIEF | CLASS_RANGER | CLASS_MAGE | CLASS_SHAMAN, "pierce"},
  {DISC_RANGED, CLASS_RANGER, "ranged"},
  {DISC_COMBAT, 0, "combat"},                           // 60
  {DISC_ADVENTURING, 0, "adventuring"},                  
  {DISC_THEOLOGY, CLASS_CLERIC | CLASS_DEIKHAN, "theology"},   
  {DISC_LORE, CLASS_MAGE, "lore"},
  {DISC_NATURE, CLASS_RANGER, "nature"} // 64 is last

};

const char * const disc_names[MAX_DISCS] =
{
// COSMO CLASS MARKER

  "Mage Abilities",           // 0 
  "Air Magic",                // 1
  "Alchemy",
  "Earth Magic",
  "Fire Magic",
  "Sorcery",                   // 5
  "Spirit Magic",
  "Water Magic",
  "Clerical Abilities",
  "Wrath of the Deities",
  "Afflictions",               // 10
  "Aegis of the Deities",
  "Cures",
  "Hand Of the Deities",
  "Warrior Abilities",
  "Hand to Hand Abilities",     // 15
  "Brawling Skills",
  "Physical Skills",
  "Smythe Skills",
  "Ranger Abilities",
  "Fighting Skills",            // 20
  "Armadillo Abilities",
  "Animal Abilities",
  "Plant Abilities",
  "Survival Skills",
  "Deikhan Abilities",          // 25
  "Fighting Skills",
  "Mounted Skills",
  "Aegis of the Deities",
  "Cures",
  "Wrath of the Deities",        // 30
  "Monk Abilities",
  "Meditation/Internal Abilities",
  "Balance and Leverage Skill",
  "Mind and Body Control",
  "Focused Attacks",
  "Barehand Specialization",
  "Thief Abilities",              // 40
  "Fighting Skills",
  "Murder",
  "Looting Skills",
  "Poisons",
  "Stealth",                     // 45
  "Traps",
  "Shaman Abilities",
  "Frog Abilities",
  "Alchemy Abilities",
  "Skunk Abilities",           // 50
  "Spider Abilities",
  "Control Abilities",
  "Totemism",
  "Wizardry",
  "Faith",                        // 55
  "Slash Weapons",
  "Blunt Weapons",
  "Pierce Weapons",
  "Ranged Weapons",
  "Combat Skills",               // 60
  "Adventuring Skills",  
  "Theological Learning",
  "Magic Lore",
  "Nature",
};

const char* const home_terrains[MAX_HOME_TERS] =
{
  "unknown",
  "urban dweller",  // human
  "villager",
  "plains person",
  "recluse",
  "hills person",
  "mountaineer",
  "forester",
  "mariner",
  "urban dweller", //elf
  "tribal villager",
  "plains elf",
  "snow elf",
  "recluse",
  "wood elf",
  "sea elf",
  "urban dweller",  // dwarf
  "villager",
  "recluse",
  "hill dwarf",
  "mountain dwarf",
  "urban dweller",  // gnome
  "villager",
  "hill gnome",
  "swamp gnome",
  "villager",   // ogre
  "plains ogre",
  "hill ogre",
  "urban dweller",  // hobbit
  "shire hobbit",
  "grasslands hobbit",
  "hill hobbit",
  "woodland hobbit",
  "maritime hobbit",
};

const char * const corpse_flags[MAX_CORPSE_FLAGS] =
{
  "No-Resurrection",
  "No-Dissection",
  "No-Skinning",
};

const char * const deities[MAX_DEITIES] =
{
  "none",
  "Mezan, the father",
  "Luna, the lifegiver",
  "Sa'Sukey, the stable",
  "Sin'Sukey, the unstable",
  "Icon, the rich",
  "Elyon, the wise",
  "Jevon, the thinker",
  "Omnon, the omnipresent",
  "Menanon, the unbound",
  "Amana, the pure",
  "Malshyra, the glorious",
  "Shroud, the undying one",
  "Lesprit, the desirable",
  "Talana, the lawgiver",
  "Salurel, the proud",
};

const char * const trap_effects[MAX_TRAP_EFF] =
{
  "Movement",
  "Get/Put",
  "Affect-Room",
  "North",
  "East",
  "South",
  "West",
  "Up",
  "Down",
  "Northeast",
  "Northwest",
  "Southeast",
  "Southwest",
  "Throw",
  "Thrown_armed_1",
  "Thrown_armed_2",
  "Thrown_armed_3",
};

// some factors from stdsneezy.h
const int PROD_GAMEPORT = 7900;
const int BETA_GAMEPORT = 5678;
const int ALPHA_GAMEPORT = 6969;
const int BUILDER_GAMEPORT = 8900;
const int ITEM_DAMAGE_RATE = 1;
const int RENT_CREDIT_VAL = 75;
const bool RENT_SELL_TO_PAWN = false;
const bool RENT_RESTRICT_INNS_BY_LEVEL = false;
const int WEAPON_DAM_MIN_HARDNESS = 20;
const int WEAPON_DAM_MAX_HARDNESS = 150;
const int WEAPON_DAM_MAX_SHARP = 150;
const int MIN_EXIST_IMMORTAL = 9;
