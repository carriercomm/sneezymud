//////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//      "components.c" - All functions and routines related to components
//
//////////////////////////////////////////////////////////////////////////

#include "stdsneezy.h"
#include "components.h"
#include "shop.h"

vector<compPlace>component_placement(0);
vector<compInfo>CompInfo(0);
vector<COMPINDEX>CompIndex(0);

void assign_component_placement()
{
  if(gamePort==8900){
    vlogf(LOG_LOW, "Skipping assign_component_placement for builder mud");
    return;
  }

// rainbow bridge 1
  component_placement.push_back(compPlace(BRIDGE_ROOM, -1, MOB_NONE, 
     ITEM_RAINBOW_BRIDGE1,
     CACT_PLACE, 1, 100,
     HOUR_SUNRISE, HOUR_SUNSET, -1, -1, -1, -1, 1<<WEATHER_RAINY | 1<<WEATHER_LIGHTNING,
     "<p>$p slowly shimmers into existence!<1>", ""));
  component_placement.push_back(compPlace(BRIDGE_ROOM, -1, MOB_NONE, 
     ITEM_RAINBOW_BRIDGE1,
     CACT_REMOVE, 1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY | 1<<WEATHER_LIGHTNING),
     "<p>$p slowly fades out of existence!<1>", ""));
  component_placement.push_back(compPlace(BRIDGE_ROOM, -1, MOB_NONE, 
     ITEM_RAINBOW_BRIDGE1,
     CACT_REMOVE, 1, 100,
     HOUR_SUNSET, -1, -1, -1, -1, -1, -1,
     "<p>$p slowly fades out of existence!<1>", ""));
// rainbow bridge 2
  component_placement.push_back(compPlace(BRIDGE_ROOM2, -1, MOB_NONE, 
     ITEM_RAINBOW_BRIDGE2,
     CACT_PLACE, 1, 100,
     HOUR_SUNRISE, HOUR_SUNSET, -1, -1, -1, -1, 1<<WEATHER_RAINY | 1<<WEATHER_LIGHTNING,
     "<p>$p slowly shimmers into existence!<1>", ""));
  component_placement.push_back(compPlace(BRIDGE_ROOM2, -1, MOB_NONE, 
     ITEM_RAINBOW_BRIDGE2,
     CACT_REMOVE, 1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY | 1<<WEATHER_LIGHTNING),
     "<p>$p slowly fades out of existence!<1>", ""));
  component_placement.push_back(compPlace(BRIDGE_ROOM2, -1, MOB_NONE, 
     ITEM_RAINBOW_BRIDGE2,
     CACT_REMOVE, 1, 100,
     HOUR_SUNSET, -1, -1, -1, -1, -1, -1,
     "<p>$p slowly fades out of existence!<1>", ""));

  // cheval
  component_placement.push_back(compPlace(6101, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<k>$p stupidly dives into the ground and lands on it's back.<1>" ,""));
  component_placement.push_back(compPlace(6101, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<k>$p skitters away into the night.<1>", ""));
  component_placement.push_back(compPlace(6101, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<k>$p flips over and flies away.<1>", ""));
  component_placement.push_back(compPlace(6104, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<k>$p stupidly dives into the ground and lands on it's back.<1>" ,""));
  component_placement.push_back(compPlace(6104, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<k>$p skitters away into the night.<1>", ""));
  component_placement.push_back(compPlace(6104, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<k>$p flips over and flies away.<1>", ""));
  component_placement.push_back(compPlace(6108, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<k>$p stupidly dives into the ground and lands on it's back.<1>" ,""));
  component_placement.push_back(compPlace(6108, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<k>$p skitters away into the night.<1>", ""));
  component_placement.push_back(compPlace(6108, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<k>$p flips over and flies away.<1>", ""));
  component_placement.push_back(compPlace(6200, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<k>$p stupidly dives into the ground and lands on it's back.<1>" ,""));
  component_placement.push_back(compPlace(6200, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<k>$p skitters away into the night.<1>", ""));
  component_placement.push_back(compPlace(6200, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<k>$p flips over and flies away.<1>", ""));

  component_placement.push_back(compPlace(6205, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<k>$p stupidly dives into the ground and lands on it's back.<1>" ,""));
  component_placement.push_back(compPlace(6205, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<k>$p skitters away into the night.<1>", ""));
  component_placement.push_back(compPlace(6205, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<k>$p flips over and flies away.<1>", ""));

  component_placement.push_back(compPlace(6211, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<k>$p stupidly dives into the ground and lands on it's back.<1>" ,""));
  component_placement.push_back(compPlace(6211, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<k>$p skitters away into the night.<1>", ""));
  component_placement.push_back(compPlace(6211, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<k>$p flips over and flies away.<1>", ""));
  component_placement.push_back(compPlace(6220, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<k>$p stupidly dives into the ground and lands on it's back.<1>" ,""));
  component_placement.push_back(compPlace(6220, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<k>$p skitters away into the night.<1>", ""));
  component_placement.push_back(compPlace(6220, -1, MOB_NONE, 
     COMP_CHEVAL,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<k>$p flips over and flies away.<1>", ""));


// color spray
  component_placement.push_back(compPlace(11328, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<B>A small stone nearby begins to sparkle and glow from the kiss of a rainbow.<1>" ,""));
  component_placement.push_back(compPlace(11328, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<b>$p dulls and fades away as the night approaches.<1>", ""));
  component_placement.push_back(compPlace(11328, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<b>$p dulls and fades away.<1>", ""));

  component_placement.push_back(compPlace(1008, 1015, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<B>A small stone nearby begins to sparkle and glow from the kiss of a rainbow.<1>",
     "<y>A beautiful rainbow streaks across the sky.<1>"));
  component_placement.push_back( compPlace(1008, 1015, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<b>$p dulls and fades away as the night approaches.<1>", ""));
  component_placement.push_back( compPlace(1008, 1015, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<b>$p dulls and fades away.<1>", ""));
  component_placement.push_back( compPlace(11368, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<B>A small stone nearby begins to sparkle and glow from the kiss of a rainbow.<1>",
     "<y>A beautiful rainbow streaks across the sky.<1>"));
  component_placement.push_back( compPlace(11368, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<b>$p dulls and fades away as the night approaches.<1>", ""));
  component_placement.push_back( compPlace(11368, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<b>$p dulls and fades away.<1>", ""));
  component_placement.push_back( compPlace(1018, 1023, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<B>A small stone nearby begins to sparkle and glow from the kiss of a rainbow.<1>", ""));
  component_placement.push_back( compPlace(1018, 1023, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<b>$p dulls and fades away as the night approaches.<1>",
     ""));
  component_placement.push_back( compPlace(1018, 1023, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<b>$p dulls and fades away.<1>", ""));

  component_placement.push_back( compPlace(7510, 7512, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<B>A small stone nearby begins to sparkle and glow from the kiss of a rainbow.<1>",
     "<y>A beautiful rainbow streaks across the sky.<1>"));
  component_placement.push_back( compPlace(7510, 7512, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<b>$p dulls and fades away as the night approaches.<1>", ""));
  component_placement.push_back( compPlace(7510, 7512, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<b>$p dulls and fades away.<1>", ""));

  component_placement.push_back(compPlace(11324, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_PLACE, 20, 65,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "<B>A small stone nearby begins to sparkle and glow from the kiss of a rainbow.<1>",
     "<y>A beautiful rainbow streaks across the sky.<1>"));
  component_placement.push_back(compPlace(11324, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_END, -1, -1, -1, -1, -1, -1,
     "<b>$p dulls and fades away as the night approaches.<1>", ""));
  component_placement.push_back(compPlace(11324, -1, MOB_NONE, 
     COMP_COLOR_SPRAY,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY),
     "<b>$p dulls and fades away.<1>", ""));

  component_placement.push_back(compPlace(7514, -1, MOB_NONE,
     COMP_ACID_BLAST,
     CACT_PLACE | CACT_UNIQUE, -1, 60,
     -1, -1, -1, -1, -1, -1, -1,
     "Drops of clear liquid drip from the ceiling and collect in a forgotten decanter.", "", SOUND_WATER_DROP, 5));

// Some white silicon [sand blast] //
  // sand only CREATED during day, night doesn't destroy
  component_placement.push_back(compPlace(6798, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(6798, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(7524, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(7524, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(9110, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(9110, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(9114, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(9114, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(9118, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(9118, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(9157, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(9157, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(9163, 9164, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(9163, 9164, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(9170, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(9170, -1, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1,
     (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));
  component_placement.push_back(compPlace(12641, 12643, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, 1<<WEATHER_CLOUDLESS,
     "The extreme heat from the sun dries and cracks the $g.",
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(12641, 12643, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));

  component_placement.push_back(compPlace(12795, 12799, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_PLACE, 20, 50,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, 1<<WEATHER_CLOUDLESS,
     "The extreme heat from the sun dries and cracks the $g.", 
     "The blazing sun blisters the $g."));
  component_placement.push_back(compPlace(12795, 12799, MOB_NONE,
     COMP_SAND_BLAST,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, (1<<WEATHER_RAINY | WEATHER_SNOWY | WEATHER_LIGHTNING),
     "Moisture hangs heavily in the air as bad weather approaches.", ""));

// Some porous bedrock [stoneskin] //
  component_placement.push_back(compPlace(10990, 10992, MOB_NONE, 
     COMP_STONE_SKIN,
     CACT_PLACE | CACT_UNIQUE, 40, 15,
     -1, -1, -1, -1, -1, -1, -1,
     "A gust of wind exposes $p.", ""));

// A dryad's footprint [stealth] //
  component_placement.push_back(compPlace(7829, -1, MOB_NONE, 
     COMP_STEALTH,
     CACT_PLACE | CACT_UNIQUE, 40, 80,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_RAINY | 1<<WEATHER_SNOWY),
     "", ""));
  component_placement.push_back(compPlace(7829, -1, MOB_NONE, 
     COMP_STEALTH,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, (1<<WEATHER_RAINY | 1<<WEATHER_SNOWY),
     "", ""));

// A prism of condensation [concealment] //
  // created during daylight rain, destroyed by sun
  // nighttime has no effect on destruction 
  component_placement.push_back(compPlace(2712, 2717, MOB_NONE, 
     COMP_CLOUD_OF_CONCEAL,
     CACT_PLACE | CACT_UNIQUE, 40, 80,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "Raindrops slowly fall from the leaves.",
     "The falling rain gathers on the foliage around you."));

  component_placement.push_back(compPlace(2712, 2717, MOB_NONE, 
     COMP_CLOUD_OF_CONCEAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The bright heat from the sun warms the air around you.", ""));

  component_placement.push_back(compPlace(10101, 10159, MOB_NONE, 
     COMP_CLOUD_OF_CONCEAL,
     CACT_PLACE | CACT_UNIQUE, 40, 80,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "Raindrops slowly fall from the leaves.", 
     "The falling rain gathers on the foliage around you."));

  component_placement.push_back(compPlace(10101, 10159, MOB_NONE, 
     COMP_CLOUD_OF_CONCEAL,
     CACT_REMOVE, -1, 100,
     HOUR_DAY_BEGIN, HOUR_DAY_END, -1, -1, -1, -1, (1<<WEATHER_CLOUDLESS),
     "The bright heat from the sun warms the air around you.", ""));

// Some gnome flour [dispel invisible] //
  component_placement.push_back(compPlace(22480, -1, MOB_NONE, 
     COMP_DISPEL_INVIS,
     CACT_PLACE, 40, 90,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

// A bit of brain coral [telepathy] //
  component_placement.push_back(compPlace(4300, 4399, MOB_NONE, 
     COMP_TELEPATHY,
     CACT_PLACE | CACT_UNIQUE, 40, 10,
     12, 25, -1, -1, -1, -1, -1,
     "$p washes up onto shore.",
     "The shallow waters churn, kicking up sediment and debris."));
  component_placement.push_back(compPlace(4300, 4399, MOB_NONE, 
     COMP_TELEPATHY,
     CACT_REMOVE, -1, 100,
     25, -1, -1, -1, -1, -1, -1,
     "The waterline recedes as the tides change.", ""));

  component_placement.push_back(compPlace(14111, 14115, MOB_NONE, 
     COMP_TELEPATHY,
     CACT_PLACE | CACT_UNIQUE, 40, 10,
     40, 2, -1, -1, -1, -1, -1,
     "$p washes up onto shore.",
     "The shallow waters churn kicking up sediment and debris."));

  component_placement.push_back(compPlace(14111, 14115, MOB_NONE, 
     COMP_TELEPATHY,
     CACT_REMOVE, -1, 100,
     2, -1, -1, -1, -1, -1, -1,
     "The waterline recedes as the tides change.", ""));

// A jar of whale grease [fumble] //
  component_placement.push_back(compPlace(1293, -1, 1389, 
     COMP_FUMBLE,
     CACT_PLACE, 40, 80,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

  component_placement.push_back(compPlace(2794, -1, 2773, 
     COMP_FUMBLE,
     CACT_PLACE, 40, 80,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
// A piece of cloud stone [conjure air elemental] //
  component_placement.push_back(compPlace(11000, 11017, MOB_NONE, 
     COMP_CONJURE_AIR,
     CACT_PLACE | CACT_UNIQUE, 40, 60,
     -1, -1, -1, -1, -1, -1, (1<<WEATHER_CLOUDY | 1<<WEATHER_RAINY),
     "Heavy winds buffet the clouds, sending $p across the $g.",
     "Howling winds scream in your ears."));

  component_placement.push_back(compPlace(11000, 11017, MOB_NONE, 
     COMP_CONJURE_AIR,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, ~(1<<WEATHER_CLOUDY | 1<<WEATHER_RAINY),
     "The winds begin to die down somewhat.", ""));

// A bit of cloud foam [fly] //
  component_placement.push_back(compPlace(11000, 11017, MOB_NONE, 
     COMP_FLIGHT,
     CACT_PLACE | CACT_UNIQUE, 40, 40,
     -1, -1, -1, -1, -1, -1, (1<<WEATHER_RAINY),
     "The moist air creates $p on the $g.",
     "The $g becomes heavy with moisture."));

  component_placement.push_back(compPlace(11000, 11017, MOB_NONE, 
     COMP_FLIGHT,
     CACT_REMOVE, -1, 100,
     -1, -1, -1, -1, -1, -1, (1<<WEATHER_CLOUDY),
     "The clouds firm up as the rain dissipates.", ""));

// Some liquid brimstone [hellfire] //
  component_placement.push_back(compPlace(9994, -1, 9917, 
     COMP_HELLFIRE,
     CACT_PLACE | CACT_UNIQUE, 40, 90,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(7515, -1, MOB_NONE, 
     COMP_HELLFIRE,
     CACT_PLACE | CACT_UNIQUE, 40, 70,
     -1, -1, -1, -1, -1, -1, -1,
     "Some brimstone boils out of the earth and collects in a nearby pot.", ""));

#if 0
// Please talk to Batopr before enabling these
// mob load doesn't work well, so maybe should be dissect...

// Some eyes from a blind man [sense life] //
  component_placement.push_back(compPlace(1, 99, 7800, 
     COMP_BLIND_EYE,
     CACT_PLACE, 40, 70,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

// A golden carrot [true sight] //
  component_placement.push_back(compPlace(10117, 10128, 10108, 
     COMP_GOLDEN_CARROT,
     CACT_PLACE, 40, 15,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

// A bag of pixie dust [slumber] //
  component_placement.push_back(compPlace(10258, 10299, 7817, 
     COMP_PIXIE_DUST,
     CACT_PLACE, 40, 80,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

  component_placement.push_back(compPlace(10258, 10299, 7805, 
     COMP_PIXIE_DUST,
     CACT_PLACE, 40, 80,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

  component_placement.push_back(compPlace(2700, 2740, 7805, 
     COMP_PIXIE_DUST,
     CACT_PLACE, 40, 80,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

  component_placement.push_back(compPlace(2700, 2740, 7805, 
     COMP_PIXIE_DUST,
     CACT_PLACE, 40, 80,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

// A pixie torch [faerie fire] //
  component_placement.push_back(compPlace(10258, 10299, 7805, 
     COMP_PIXIE_TORCH,
     CACT_PLACE, 40, 85,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(10258, 10299, 7805, 
     COMP_PIXIE_TORCH,
     CACT_PLACE, 40, 85,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(10258, 10299, 7805, 
     COMP_PIXIE_TORCH,
     CACT_PLACE, 40, 85,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

// A dropper of pixie tears [faerie fog] //
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));
  component_placement.push_back(compPlace(199, 245, 7805, 
     COMP_PIXIE_TEAR,
     CACT_PLACE, 40, 50,
     -1, -1, -1, -1, -1, -1, -1,
     "", ""));

#endif
}

void do_components(int situ)
{
  unsigned int i;
  int j;
  TRoom *rp = NULL;
  TThing *m = NULL, *temp = NULL, *o = NULL;
  TThing *t, *t2;
  TObj *obj = NULL;
  int value;
  int l_room, t_room;
  int found;
  int start, stop;

  // situ == -1 indicates a tick pulse
  // situ == 0-6 indicates a weather change
  //    1 = getting cloudy, 2 = rain/snow start
  //    3 = clouds disappear, 4 = blizzard/lightning start
  //    5 = rain/snow stop, 6 = bliz/light stop
  if (situ != -1)
    return;

  for (i = 0; i < component_placement.size(); i++) {
    if ((value = real_object(component_placement[i].number)) < 0) {
      vlogf(LOG_MISC, "Bad component (%d, %d)", i, component_placement[i].number);
      continue;
    }

    bool placed = FALSE;

    // check hour info
    start = component_placement[i].hour1;
    stop = component_placement[i].hour2;
    if (start == HOUR_SUNRISE)
      start = sunTime(SUN_TIME_RISE);
    else if (start == HOUR_SUNSET)
      start = sunTime(SUN_TIME_SET);
    else if (start == HOUR_MOONRISE)
      start = moonTime(MOON_TIME_RISE);
    else if (start == HOUR_MOONSET)
      start = moonTime(MOON_TIME_SET);
    else if (start == HOUR_DAY_BEGIN)
      start = sunTime(SUN_TIME_DAY);
    else if (start == HOUR_DAY_END)
      start = sunTime(SUN_TIME_SINK);
    if (stop == HOUR_SUNRISE)
      stop = sunTime(SUN_TIME_RISE);
    else if (stop == HOUR_SUNSET)
      stop = sunTime(SUN_TIME_SET);
    else if (stop == HOUR_MOONRISE)
      stop = moonTime(MOON_TIME_RISE);
    else if (stop == HOUR_MOONSET)
      stop = moonTime(MOON_TIME_SET);
    else if (stop == HOUR_DAY_BEGIN)
      stop = sunTime(SUN_TIME_DAY);
    else if (stop == HOUR_DAY_END)
      stop = sunTime(SUN_TIME_SINK);

    if (start != -1) {
      if (stop == -1) {
        // only the one hour was specified
        if (start != time_info.hours)
          continue;
      } else if (time_info.hours < start)
        continue;  // too early
      else if (time_info.hours >= stop)
        continue;  // too late
    }

    // check day data
    if (component_placement[i].day1 != -1) {
      if (component_placement[i].day2 == -1) {
        // only the one day was specified
        if (component_placement[i].day1 != time_info.day)
          continue;
      } else if (time_info.day < component_placement[i].day1)
        continue;  // too early
      else if (time_info.day >= component_placement[i].day2)
        continue;  // too late
    }

    // check month data
    if (component_placement[i].month1 != -1) {
      if (component_placement[i].month2 == -1) {
        // only the one month was specified
        if (component_placement[i].month1 != time_info.month)
          continue;
      } else if (time_info.month < component_placement[i].month1)
        continue;  // too early
      else if (time_info.month >= component_placement[i].month2)
        continue;  // too late
    }

    if (component_placement[i].room2 == -1)
      l_room = component_placement[i].room1;
    else {
      // set ACTUAL l_room
      j = 0;
      do {
        l_room = ::number(component_placement[i].room1, component_placement[i].room2);
        j++;
     
      } while (!real_roomp(l_room) && j < 20);
      if (j >= 20)
        continue;
    }

    if ((rp = real_roomp(l_room))) {
      // check weather condition
      if (component_placement[i].weather != -1) {
        if (!IS_SET(component_placement[i].weather, (1<<rp->getWeather())))
          continue;
      }

      if (IS_SET(component_placement[i].place_act, CACT_PLACE)) {

        // first, send message to all rooms
        if (*component_placement[i].glo_msg) {
          for (t_room = component_placement[i].room1; t_room <= component_placement[i].room2; t_room++) {
            if ((rp = real_roomp(t_room)) && rp->stuff) {
              act(component_placement[i].glo_msg,
                    TRUE, rp->stuff, NULL, NULL, TO_CHAR);
              act(component_placement[i].glo_msg, 
                    TRUE, rp->stuff, NULL, NULL, TO_ROOM);
            }
          }
          rp = real_roomp(l_room);  // reset rp pointer
        }
        // check variance here, so global messages still shown regardless
        if (::number(1,100) > component_placement[i].variance)
          continue;

        if ((component_placement[i].max_number <= 0) ||
            (obj_index[value].number < component_placement[i].max_number)) {

          // uniqueness check
          if (IS_SET(component_placement[i].place_act, CACT_UNIQUE)) {
            found = FALSE;
            if (component_placement[i].mob) {
              // limit to 1 on any 1 mob in room
              for (m = rp->stuff; m && !found; m = m->nextThing) {
                TMonster *tmon = dynamic_cast<TMonster *>(m);
                if (!tmon)
                  continue;
                if (!tmon->isPc() && 
                    (tmon->mobVnum() == component_placement[i].mob)) {
                  for (t = tmon->stuff; t; t = t->nextThing) {
                    TObj *tobj = dynamic_cast<TObj *>(t);
                    if (!tobj)
                      continue;
                    if (tobj->objVnum() == component_placement[i].number) {
                      found = TRUE;
                      break;
                    }
                  }
                }
              }
              if (found) 
                continue;
            } else {
              // limit to 1 in the room
              for (t = rp->stuff; t; t = t->nextThing) {
                TObj *tobj = dynamic_cast<TObj *>(t);
                if (!tobj)
                  continue;
                if (tobj->objVnum() == component_placement[i].number) {
                  found = TRUE;
                  break;
                }
              }
              if (found) 
                continue;
            }
          }

          if (!(obj = read_object(component_placement[i].number, VIRTUAL)))
            continue;
          placed = FALSE;
          // if mob ALSO specified, place it on mob in room
          if (component_placement[i].mob) {
            for (m = rp->stuff; m; m = m->nextThing) {
              TMonster *tmon = dynamic_cast<TMonster *>(m);
              if (!tmon)
                continue;
        
              if (!tmon->isPc() && 
                  (tmon->mobVnum() == component_placement[i].mob)) {
                *tmon += *obj;
                placed = TRUE;
                break;
              }
            }
            if (!placed) {
              delete obj;
              obj = NULL;
            }
          } else {
            m = NULL;
            *rp += *obj;
            placed = TRUE;
          }
        }
        if (placed && *component_placement[i].message && rp->stuff) {
          act(component_placement[i].message, TRUE, rp->stuff, obj, m, TO_CHAR);
          act(component_placement[i].message, TRUE, rp->stuff, obj, m, TO_ROOM);

          if (component_placement[i].sound != SOUND_OFF)
            rp->playsound(component_placement[i].sound, SOUND_TYPE_NOISE, 100, 5, component_placement[i].sound_loop);
        }
      } else if (IS_SET(component_placement[i].place_act, CACT_REMOVE)) {
        // removing the object
        // check variance
        if (::number(1,100) > component_placement[i].variance)
          continue;

        found = 0;
        for (t_room = component_placement[i].room1;
             (t_room <= component_placement[i].room2 || t_room == component_placement[i].room1) && (found < component_placement[i].max_number || component_placement[i].max_number < 0);
             t_room++) {
          placed = FALSE;
          if ((rp = real_roomp(t_room))) {
            if (component_placement[i].mob) {
              for (m = rp->stuff; m; m = m->nextThing) {
                TMonster *tmon = dynamic_cast<TMonster *>(m);
                if (!tmon)
                  continue;
                if (!tmon->isPc() &&
                     tmon->mobVnum() == component_placement[i].mob) {
                  for (t = tmon->stuff; t; t = t2) {
                    t2 = t->nextThing;
                    TObj * tobj = dynamic_cast<TObj *>(t);
                    if (!tobj)
                      continue;
                    if (tobj->objVnum() == component_placement[i].number) {
                      if (!placed && *component_placement[i].message && rp->stuff) {
                        act(component_placement[i].message,
                                TRUE, rp->stuff, tobj, tmon, TO_CHAR);
                        act(component_placement[i].message,
                                TRUE, rp->stuff, tobj, tmon, TO_ROOM);
                      }
                      placed = TRUE;
                      found++;
                      delete tobj;
                      t2 = tmon->stuff;
                      continue;
                    }
                  }
                }
              }
            } else {
              for (o = rp->stuff; o; o = temp) {
                temp = o->nextThing;
                TObj *to = dynamic_cast<TObj *>(o);
                if (to) {
                  if (to->objVnum() == component_placement[i].number) {
                    if (!placed && *component_placement[i].message && rp->stuff) {
                      act(component_placement[i].message,
                             TRUE, rp->stuff, to, NULL, TO_CHAR);
                      act(component_placement[i].message,
                             TRUE, rp->stuff, to, NULL, TO_ROOM);
                    }
                    delete to;
                    placed = TRUE;
                    found++;
                    continue;
                  }
                }
              }
            }
          } // else no room
        } // end of for loop
      } else {
        vlogf(LOG_MISC, "No direction specified on component placement (%d)", i);
      }
    } else {
      vlogf(LOG_MISC, "No room specified on component placement (%d)", i);
    }
  }
}

void buildComponentArray()
{
  CompInfo.push_back(compInfo( SPELL_IMMOBILIZE,
    "You twirl $p around your finger as you point at $N.",
    "$n twirls $p around $s finger as $e points at $N.",
    "$n twirls $p around $s finger as $e points at you.",
    "You twirl $p around your finger as you point at yourself.",
    "$n twirls $p around $s finger as $e points at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SUFFOCATE,
    "You heave $p at $N.",
    "$n heaves $p at $N.",
    "$n heaves $p at you.",
    "You heave $p over your head.",
    "$n heaves $p over $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DUST_STORM,
    "",                       
    "",
    "",
    "You open $p allowing a bit of vapor to seep out.",
    "$n opens $p allowing a bit of vapor to seep out.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_TORNADO,
    "",                       
    "",
    "",
    "You open $p, unleashing a tiny vortex.",
    "$n opens $p, unleashing a tiny vortex.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CONJURE_AIR,
    "",
    "",
    "",
    "You toss $p upward and it forms into a small grey cloud.",
    "$n tosses $p upward and it forms into a cloud.",
    "",                       
    ""));
  CompInfo.push_back(compInfo(SPELL_FEATHERY_DESCENT,
    "You fling $p at $N.",
    "$n flings $p at $N.",
    "$n flings $p at you.",                       
    "You fling $p at yourself.",
    "$n flings $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FALCON_WINGS,
    "You run $p up and down $N's arms.",
    "$n runs $p up and down $N's arms.",
    "$n runs $p up and down your arms.",                       
    "You run $p up and down your arms.",
    "$n runs $p up and down $s arms.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ANTIGRAVITY,
    "",
    "",
    "",
    "You chew on $p.",
    "$n chews on $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_IDENTIFY,
    "You pop $p into your eye and examine $N.",
    "$n pops $p into $s eye and examines $N.",
    "$n pops $p into $s eye and examines you.",
    "You pop $p into your eye.",
    "$n pops $p into $s eye.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DIVINATION,
    "You hold $p high, and aim it at $N.",
    "$n holds $p high, and aims it at $N.",
    "$n holds $p high, and aims it at you.",
    "You hold $p high over your head.",
    "$n holds $p high over $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_EYES_OF_FERTUMAN,
    "",
    "",
    "",
    "You allow $p to spin.",
    "$n allows $p to spin.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_POWERSTONE,
    "",
    "",
    "",
    "",
    "",
    "You crush $p between your fingers and rub it over $N.",
    "$n crushes $p between $s fingers and rubs it over $N."));
  CompInfo.push_back(compInfo(SPELL_SHATTER,
    "",
    "",
    "",
    "You blow into $p causing a high pitched scream to be emitted.",
    "$n blows into $p causing a high pitched scream to be emitted.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ILLUMINATE,
    "",                       
    "",
    "",
    "",
    "",
    "You crumple $p and scatter the dust over $N.",
    "$n crumples $p and scatters the dust over $N."));
  CompInfo.push_back(compInfo(SPELL_DETECT_MAGIC,
    "You toss $p onto $N.",
    "$n tosses $p onto $N.",
    "$n tosses $p onto you.",                       
    "You toss $p on yourself.",
    "$n tosses $p onto $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DISPEL_MAGIC,
    "",
    "",
    "",
    "You squeeze $p tightly.",
    "$n squeezes $p.",
    "You squeeze $p tightly.",
    "$n squeezes $p."));
  CompInfo.push_back(compInfo(SPELL_COPY,
    "",                       
    "",
    "",
    "",
    "",
    "You stretch $p between your forefingers.",
    "$n stretches $p between $s forefingers."));
  CompInfo.push_back(compInfo(SPELL_VOODOO,
    "",                       
    "",
    "",
    "You taste $p and spit it out.",
    "$n tastes $p and then $e spits it out.",
    "You taste $p and spit it out.",
    "$n tastes $p and then $e spits it out."));
  CompInfo.push_back(compInfo(SPELL_DANCING_BONES,
    "",                       
    "",
    "",
    "You blow $p into the air.",
    "$n blows $p into the air.",
    "You blow $p into the air.",
    "$n blows $p into the air."));
  CompInfo.push_back(compInfo(SPELL_RESURRECTION,
    "",                       
    "",
    "",
    "You toss $p into the air and it vanishes.",
    "$n tosses $p into the air and it vanishes.",
    "You toss $p into the air and it vanishes.",
    "$n tosses $p into the air and it vanishes."));
  CompInfo.push_back(compInfo(SPELL_GALVANIZE,
    "",                       
    "",
    "",
    "",
    "",
    "You drop $p and command it to begin heating.",
    "$n drops $p and issues a command word."));
  CompInfo.push_back(compInfo(SPELL_GRANITE_FISTS,
    "You tightly squeeze $p as you stare at $N.",
    "$n tightly squeezes $p as $e stares at $N.",
    "$n tightly squeezes $p as $e stares at you.",                       
    "You tightly squeeze $p as your eyes roll back into your head.",
    "$n tightly squeezes $p as $s eyes roll back into $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_METEOR_SWARM,
    "You toss $p at $N.  The stone floats skywards before it can hit $M.",
    "$n tosses $p at $N.  The stone floats skywards before it can hit $M.",
    "$n tosses $p at you, but it soars into the sky before it can strike you.",
    "You toss $p at yourself.  The stone floats skywards before it can hit you.",
    "$n tosses $p at $mself.  The stone floats skywards before it can hit $m.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_PEBBLE_SPRAY,
    "",                       
    "",
    "",
    "You press $p to the $g.",
    "$n presses $p to the $g.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FLATULENCE,
    "",                       
    "",
    "",
    "You quickly eat $p.",
    "$n quickly eats $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SAND_BLAST,
    "",                       
    "",
    "",
    "You pour a portion of sand from $p.",
    "$n pours a bit of sand from $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_LAVA_STREAM,
    "",
    "",
    "",
    "You press $p between your hands.",
    "$n presses $p between $s hands.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DEATH_MIST,
    "",
    "",
    "",
    "You eat $p and begin to choke on them.",
    "$n eats $p and begins to choke on them.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_STONE_SKIN,
    "You rub $p upon $N.",
    "$n rubs $p on $N.",
    "$n rubs $p on you.",                       
    "You rub $p on yourself.",
    "$n rubs $p on $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_TRAIL_SEEK,
    "You touch $p to $N's nose.",
    "$n touches $p to $N's nose.",
    "$n touches $p to your nose.",                       
    "You touch $p to your nose.",
    "$n touches $p to $s nose.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_LEVITATE,
    "You uncork $p and hold it beneath $N's nose.",
    "$n uncorks $p and holds it beneath $N's nose.",
    "$n uncorks $p and holds it beneath your nose.",                       
    "You uncork $p and sniff it.",
    "$n uncorks $p and sniffs it.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CONJURE_EARTH,
    "",                       
    "",
    "",
    "You crush $p in your hand.",
    "$n crushes $p in $s hand.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FAERIE_FIRE,
    "You hold $p between two fingers and look at $N.",
    "$n holds $p between two fingers and looks at $N.",
    "$n holds $p between two fingers while looking at you.",
    "You hold $p between two fingers and close your eyes.",
    "$n holds $p between two fingers and closes $s eyes.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_STUPIDITY,
    "You repeat $p and look at $N.",
    "$n repeats $p and looks at $N.",
    "$n repeats $p while looking at you.",
    "You repeat $p and close your eyes.",
    "$n repeats $p and closes $s eyes.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FLAMING_SWORD,
    "You grip $p, slashing at the air before $N.",
    "$n grips $p, slashing at the air before $N.",
    "$n grips $p, slashing it at the air before you.",                       
    "You grip $p, slashing at the air around you.",
    "$n grips $p, slashing at the air around $m.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_INFERNO,
    "You blow $p in $N's direction.",
    "$n blows $p in $N's direction.",
    "$n blows $p in your direction.",                       
    "You blow $p in a cloud about yourself.",
    "$n blows $p in a cloud about $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FIREBALL,
    "",                       
    "",
    "",
    "You scatter $p into the air.",
    "$n scatters $p into the air.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_HELLFIRE,
    "",                       
    "",
    "",
    "You pour $p into your palm.",
    "$n pours $p into your palm.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CONJURE_FIRE,
    "",                       
    "",
    "",
    "You touch $p to your forehead.",
    "$n touches $p to $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FLAMING_FLESH,
    "You apply $p to $N.",
    "$n applies $p to $N.",
    "$n applies $p to you.",                       
    "You apply $p to yourself.",
    "$n applies $p to $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FLARE,
    "",                       
    "",
    "",
    "You take a globe from $p launching it upwards.",
    "$n launches a small globe of sodium vapor upwards.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_STUNNING_ARROW,
    "You wrap $p around your wrist and reach for $N.",
    "$n wraps $p around a wrist and reaches for $N.",
    "$n wraps $p around a wrist and reaches for you.",                       
    "You wrap $p around your wrist and grab yourself.",
    "$n wraps $p around a wrist and grabs $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_BLAST_OF_FURY,
    "You lick $p and spit it at $N.",
    "$n licks $p and spits it at $N.",
    "$n licks $p and spits it at you.",                       
    "You lick $p and start to drool.",
    "$n licks $p and drools on $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ENERGY_DRAIN,
    "You wave $p at $N.",
    "$n waves $p at $N.",
    "$n waves $p at you.",                       
    "You wave $p at yourself.",
    "$n waves $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_LICH_TOUCH,
    "You fling $p at $N.",
    "$n flings $p at $N.",
    "$n flings $p at you.",                       
    "You fling $p at yourself.",
    "$n flings $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SQUISH,
    "You squeeze $p tightly.",
    "$n squeezes $p tightly.",
    "$n squeezes $p tightly.",                       
    "You squeeze $p tightly.",
    "$n squeezes $p tightly.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CARDIAC_STRESS,
    "You chew up $p and then kiss $N deeply on the mouth.",
    "$n chews up $p and then kisses $N deeply on the mouth.",
    "$n chews up $p and then kisses you deeply on the mouth.",                       
    "You chew up $p and accidentally swallow the poisonous juice.",
    "$n chews up $p and accidentally swallows it.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SOUL_TWIST,
    "You wrap $p around your wrist and reach for $N.",
    "$n wraps $p around a wrist and reaches for $N.",
    "$n wraps $P around a wrist and reaches for you.",                       
    "You wrap $p around your wrist and grab yourself.",
    "$n wraps $p around a wrist and grabs $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_VAMPIRIC_TOUCH,
    "You consume $p and spit it on $N.",
    "$n consumes $p and spits it on $N.",
    "$n consumes $p and spits it on you.",                       
    "You consume $p and accidentally swallow it.",
    "$n consumes $p and swallows it down.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_BLOOD_BOIL,
    "You spit $p at $N.",
    "$n spits $p at $N.",
    "$n spits $p at you.",                       
    "You swallow $p and start to drool.",
    "$n swallows $p and drools on $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_LIFE_LEECH,
    "You throw $p onto $N's chest.",
    "$n throws $p onto $N's chest.",
    "$n throws $p onto your chest!",                       
    "You fumble $p and it latches on to you.",
    "$n fumbles $p and it latches tightly to $s chest.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ATOMIZE,
    "You blow $p at $N.",
    "$n blow $p at $N.",
    "$n blows $p at you.",                       
    "You blow $p at yourself.",
    "$n blow $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DEATHWAVE,
    "You blow $p at $N.",
    "$n blow $p at $N.",
    "$n blows $p at you.",                       
    "You blow $p at yourself.",
    "$n blow $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_COLOR_SPRAY,
    "",                       
    "",
    "",
    "You squeeze $p and wave it over your head.",
    "$n squeezes $p and waves it over $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ACID_BLAST,
    "",                       
    "",
    "",
    "You uncork $p.",
    "$n uncorks $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ANIMATE,
    "",                       
    "",                       
    "",
    "You toss $p into the pile of armor.",
    "$n tosses $p into the pile of armor.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CONTROL_UNDEAD,
    "",                       
    "",                       
    "",
    "You toss $p on the ground.",
    "$n tosses $p on the ground.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SHIELD_OF_MISTS,
    "You masticate $p and spit it at $N.",
    "$n masticates $p and spits it at $N.",
    "$n masticates $p and spits it at you.",                       
    "You chew $p and swallow it down.",
    "$n chews $p and swallows it down.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SORCERERS_GLOBE,
    "You twirl $p in front of $N.",
    "$n twirls $p in front of $N.",
    "$n twirls $p in front of you.",                       
    "You twirl $p in front of yourself.",
    "$n twirls $p in front of $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_BIND,
    "You drape $p over a finger and point at $N.",
    "$n drapes $p over a finger and points at $N.",
    "$n drapes $p over a finger and points at you.",                       
    "You drape $p over a finger and point at yourself.",
    "$n drapes $p over a finger and points at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_TELEPORT,
    "You crumble $p and toss it at $N.",
    "$n crumbles $p and tosses it at $N.",
    "$n crumbles $p and tosses it at you.",                       
    "You crumble $p and toss it at yourself.",
    "$n crumbles $p and tosses it at $mself.",
    "",
    ""));
#if 0
  CompInfo.push_back(compInfo(SPELL_FIND_FAMILIAR,
    "",                       
    "",
    "",
    "You clutch $p and stare off into space.",
    "$n clutches $p and stares off into space.",
    "",
    ""));
#endif
  CompInfo.push_back(compInfo(SPELL_SENSE_LIFE,
    "You touch $p to $N's head.",
    "$n touches $p to $N's head.",
    "$n touches $p to your head.",                       
    "You touch $p to your head.",
    "$n touches $p to $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SENSE_LIFE_SHAMAN,
    "You touch $p to $N's head.",
    "$n touches $p to $N's head.",
    "$n touches $p to your head.",                       
    "You touch $p to your head.",
    "$n touches $p to $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FARLOOK,
    "",                       
    "",
    "",
    "You look into $p.",
    "$n looks into $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SILENCE,
    "You unleash $p at $N.",
    "$n unleashes $p at $N.",
    "$n unleashes $p at you.",                       
    "You unleash $p upon yourself.",
    "$n unleashes $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_STEALTH,
    "You place $p on $N's sole.",
    "$n places $p on $N's sole.",
    "$n places $p on your sole.",                       
    "You place $p on your sole.",
    "$n places $p on $s sole.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CALM,
    "You hold up $p at $N.",
    "$n holds up $p at $N.",
    "$n holds up $p at you.",                       
    "You hold up $p.",
    "$n holds up $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ENSORCER,
    "You tie $p in a knot while thinking of $N.",
    "$n ties $p in a knot.",
    "$n ties $p in a knot.",                       
    "You tie $p in a knot while thinking of yourself.",
    "$n ties $p in a knot.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_INTIMIDATE,
    "You flick $p at $N.",
    "$n flicks $p at $N.",
    "$n flicks $p at you.",                       
    "You rub $p on your face? YUCK!",
    "$n rubs $p on $s face! What a dweeb!",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FEAR,
    "You throw $p at $N.",
    "$n throws $p at $N.",
    "$n throws $p at you.",                       
    "You throw $p on the ground.",
    "$n throws $p on the ground.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_INVISIBILITY,
    "You douse $N with $p.",
    "$n douses $N with $p.",
    "$n douses you with $p.",                       
    "You douse yourself with $p.",
    "$n douses $mself with $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CLOUD_OF_CONCEALMENT,
    "",                       
    "",
    "",
    "You hold $p before your group, bending the light around it.",
    "$n holds $p before $s group, bending the light around it.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DETECT_INVISIBLE,
    "You toss $p at $N.",
    "$n tosses $p at $N.",
    "$n tosses $p at you.",                       
    "You toss $p on yourself.",
    "$n tosses $p on $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DETECT_SHADOW,
    "You sprinkle $p on $N.",
    "$n sprinkles $p on $N.",
    "$n sprinkles $p on you.",                       
    "You sprinkle $p on yourself.",
    "$n sprinkles $p on $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DISPEL_INVISIBLE,
    "You scatter $p near $N.",
    "$n scatters $p near $N.",
    "$n scatters $p near you.",                       
    "You scatter $p on the $g.",
    "$n scatters $p on the $g at $s feet.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_TELEPATHY,
    "",                       
    "",
    "",
    "You touch $p to your forehead.",
    "$n touches $p to $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ROMBLER,
    "",                       
    "",
    "",
    "You stretch out $p.",
    "$n stretches out $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_RAZE,
    "You toss $p at $N.",
    "$n tosses $p at $N.",
    "$n tosses $p at you.",                       
    "You toss $p to the ground.",
    "$n tosses $p to the ground.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_TRUE_SIGHT,
    "You nibble on $p and spit it upon $N.",
    "$n nibbles on $p and spits it at $N.",
    "$n nibbles on $p and spits it at you.",                       
    "You nibble on $p and swallow it down.",
    "$n nibbles on $p and swallows it down.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_POLYMORPH,
    "",                       
    "",
    "",
    "You wave $p over your head.",
    "$n waves $p over $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ACCELERATE,
    "You shake $p at $N.",
    "$n shakes $p at $N.",
    "$n shakes $p at you.",                       
    "You shake $p at yourself.",
    "$n shakes $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_HASTE,
    "You shake $p at $N.",
    "$n shakes $p at $N.",
    "$n shakes $p at you.",                       
    "You shake $p at yourself.",
    "$n shakes $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SLUMBER,
    "You blow $p upon $N.",
    "$n blows $p upon $N.",
    "$n blows $p upon you.",                       
    "You inhale $p.",
    "$n inhales $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FUMBLE,
    "You lather with $p and point at $N.",
    "$n lathers with $p and points at $N.",
    "$n lathers with $p and points at you.",                       
    "You lather with $p and hug yourself.",
    "$n lathers with $p and hugs $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_FAERIE_FOG,
    "",                       
    "",
    "",
    "You squeeze $p.",
    "$n squeezes $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ICY_GRIP,
    "You clutch $p, shaking it at $N.",
    "$n clutches $p, shaking it at $N.",
    "$n clutches $p, shaking it at you.",                       
    "You clutch $p, shaking it at no one in particular.",
    "$n clutches $p, shaking it at no one in particular.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_WATERY_GRAVE,
    "You toss $p at $N.",
    "$n tosses $p at $N.",
    "$n tosses $p at you.",                       
    "You toss $p at yourself.",
    "$n tosses $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ARCTIC_BLAST,
    "",                       
    "",
    "",
    "You breathe through $p.",
    "$n breathes through $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ICE_STORM,
    "",                       
    "",
    "",
    "You shatter $p.",
    "$n shatters $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_TSUNAMI,
    "",                       
    "",
    "",
    "You uncork $p.",
    "$n uncorks $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CONJURE_WATER,
    "",                       
    "",
    "",
    "You shatter $p.",
    "$n shatters $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_GILLS_OF_FLESH,
    "You apply $p to $N's throat.",
    "$n applies $p to $N's throat.",
    "$n applies $p to your throat.",                       
    "You apply $p to your throat.",
    "$n applies $p to $s throat.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_BREATH_OF_SARAHAGE,
    "",                       
    "",
    "",
    "You squeeze $p in your hands.",
    "$n squeezes $p in $s hands.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_INFRAVISION,
    "You grind $p against $N's eyelids.",
    "$n grinds $p against $N's eyelids.",
    "$n grinds $p against your eyelids.",                       
    "You grind $p against your eyelids.",
    "$n grinds $p against $s eyelids.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ENHANCE_WEAPON,
    "",                       
    "",
    "",
    "You toss $p into the air.",
    "$n tosses $p into the air.",
    "You toss $p into the air and wave $N through it.",
    "$n tosses $p into the air and waves $N through it."));
  CompInfo.push_back(compInfo(SPELL_FLY,
    "You smear $p upon $N.",
    "$n smears $p on $N.",
    "$n smears $p upon you.",                       
    "You smear $p upon yourself.",
    "$n smears $p on $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SKILL_BARKSKIN,
    "You smear $p upon $N.",
    "$n smears $p on $N.",
    "$n smears $p upon you.",                       
    "You smear $p upon yourself.",
    "$n smears $p on $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_STORMY_SKIES,
    "",                       
    "",
    "",
    "You crush $p releasing a blue-green mist.",
    "$n crushes $p which produces a blue-green mist.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_STICKS_TO_SNAKES,
    "",                       
    "",
    "",
    "You wave $p over your head.",
    "$n waves $p over $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_LIVING_VINES,
    "You squeeze $p at $N.",
    "$n squeezes $p in $N's direction.",
    "$n squeezes $p in your direction.",                       
    "You squeeze $p at yourself.",
    "$n squeezes $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_HYPNOSIS,
    "You shake $p in $N's face.",
    "$n shakes $p.",
    "$n shakes $p.",                       
    "You shake $p in your own face.",
    "$n shakes $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CHEVAL,
    "",                       
    "",
    "",
    "You swallow $p.",
    "$n swallows $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CELERITE,
    "",                       
    "",
    "",
    "You eat $p.",
    "$n eats $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CLARITY,
    "",
    "",
    "",                       
    "You put a few drops of $p in your eyes.",
    "$n put a few drops of $p in $s eyes.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SHADOW_WALK,
    "",
    "",
    "",                       
    "You drop $p to the ground and crush it under your feet.",
    "$n drops $p to the ground and crushes it under $s feet.",
    "",
    ""));
#if 1
  CompInfo.push_back(compInfo(SPELL_EARTHMAW,
			      "You grind $p into the ground.",
			      "$n grinds $p into the ground.",
			      "$n grinds $p into the ground.",
			      "",
			      "",
			      "",
			      ""));
  CompInfo.push_back(compInfo(SPELL_CREEPING_DOOM,
                              "You uncork $p blow it in $N's direction.",
                              "$n uncorks $p and blows it in $N's direction.",
                              "$n uncorks $p and blows in in your direction.",
                              "You uncork $p and sprinkle some on yourself.",
                              "$n uncorks $p and sprinkles some on $mself.",
                              "",
                              ""));
  CompInfo.push_back(compInfo(SPELL_FERAL_WRATH,
                              "",
                              "",
                              "",
                              "You thrust $p into your palm.",
                              "$n thrusts $p $s palm.",
                              "",
                              ""));
  CompInfo.push_back(compInfo(SPELL_SKY_SPIRIT,
                              "You toss $p at $N.",
                              "$n tosses $p at $N.",
                              "$n tosses $p at you.",
                              "You toss $p into the air.",
                              "$n tosses $p into the air..",
                              "",
                              ""));
#endif
  CompInfo.push_back(compInfo(SKILL_BEAST_SOOTHER,
    "You coat $N with $p.",
    "$n coats $N with $p.",
    "$n coats you with $p.",                       
    "You coat yourself with $p.",
    "$n coats $mself with $p.",
    "",
    ""));
  CompInfo.push_back(compInfo(SKILL_TRANSFIX,
    "You smear $N with $p.",
    "$n smears $N with $p.",
    "$n smears you with $p.",                       
    "You smear yourself with $p.",
    "$n smears $mself with $p",
    "",
    ""));
  CompInfo.push_back(compInfo(SKILL_BEAST_SUMMON,
    "",                       
    "",
    "",
    "You rattle $p over your head.",
    "$n rattles $p over $s head.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_SHAPESHIFT,
    "",                       
    "",
    "",
    "You sip $p and quickly spit it out.",
    "$n sips $p and quickly spits it out.",
    "",
    ""));
  CompInfo.push_back(compInfo(SKILL_BEAST_CHARM,
    "You crumple $p and toss it at $N.",
    "$n crumples $p and tosses it in $N's direction.",
    "$n crumples $p and tosses it in your direction.",                       
    "You crumple $p and toss it on yourself.",
    "$n crumples $p and tosses it on $mself",
    "",
    ""));
  CompInfo.push_back(compInfo(SKILL_BEFRIEND_BEAST,
    "You point $p at $N.",
    "$n points $p in $N's direction.",
    "$n points $p in your direction.",                       
    "You point $p at yourself.",
    "$n points $p at $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_DJALLA,
    "You smear $p on $N's forehead.",
    "$n smears $p on $N's forehead.",
    "$n smears $p on your forehead.",                       
    "You smear $p on your forehead.",
    "$n smears $p on $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_LEGBA,
    "You smear $p on $N's forehead.",
    "$n smears $p on $N's forehead.",
    "$n smears $p on your forehead.",                       
    "You smear $p on your forehead.",
    "$n smears $p on $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_PROTECTION_FROM_FIRE,
    "You touch $p to $N's forehead.",
    "$n touches $p to $N's forehead.",
    "$n touches $p to your forehead.",                       
    "You touch $p to your forehead.",
    "$n touches $p to $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_PROTECTION_FROM_EARTH,
    "You touch $p to $N's forehead.",
    "$n touches $p to $N's forehead.",
    "$n touches $p to your forehead.",                       
    "You touch $p to your forehead.",
    "$n touches $p to $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_PROTECTION_FROM_AIR,
    "You touch $p to $N's forehead.",
    "$n touches $p to $N's forehead.",
    "$n touches $p to your forehead.",                       
    "You touch $p to your forehead.",
    "$n touches $p to $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_PROTECTION_FROM_WATER,
    "You touch $p to $N's forehead.",
    "$n touches $p to $N's forehead.",
    "$n touches $p to your forehead.",                       
    "You touch $p to your forehead.",
    "$n touches $p to $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_PROTECTION_FROM_ELEMENTS,
    "You touch $p to $N's forehead.",
    "$n touches $p to $N's forehead.",
    "$n touches $p to your forehead.",                       
    "You touch $p to your forehead.",
    "$n touches $p to $s forehead.",
    "",
    ""));
  CompInfo.push_back(compInfo(SKILL_TRANSFORM_LIMB,
    "",
    "",
    "",
    "You put a drop from $p on the tip of your tongue.",
    "$n puts a drop from $p on the tip of $s tongue.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_GARMULS_TAIL,
    "You smear $p on $N.",
    "$n smears $p on $N.",
    "$n smears $p on you.",
    "You smear $p on your skin.",
    "$n smears $p on $s skin.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ROOT_CONTROL,
    "You take a drop from $p and throw it on the $g.",
    "$n takes a drop from $p and throws it on the $g.",
    "$n takes a drop from $p and throws it on the $g.",
    "You take a drop from $p and throw it on the $g.",
    "$n takes a drop from $p and throws it on the $g.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_TREE_WALK,
    "",
    "",
    "",
    "You ignite $p and inhale the fumes.",
    "$n ignites $p and inhales the fumes.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_PLASMA_MIRROR,
    "",
    "",
    "",
    "You twirl $p about yourself.",
    "$n twirls $p about $mself.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_AQUALUNG,
    "You apply $p to $N's face.",
    "$n applies $p to $N's face.",
    "$n applies $p to your face.",                       
    "You apply $p to your face.",
    "$n applies $p to $s face.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_AQUATIC_BLAST,
    "",
    "",
    "",
    "You drink $p and, before swallowing it, spit it out forcefully.",
    "$n drinks $p and, before swallowing it, spits it out forcefully.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_THORNFLESH,
    "",
    "",
    "",
    "You nibble on $p and swallow it down.",
    "$n nibbles on $p and swallows it down.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ENTHRALL_SPECTRE,
    "",                       
    "",
    "",
    "You sprinkle $p on the ground.",
    "$n sprinkles $p on the ground.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ENTHRALL_GHAST,
    "",                       
    "",
    "",
    "You sprinkle $p on the ground.",
    "$n sprinkles $p on the ground.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ENTHRALL_GHOUL,
    "",                       
    "",
    "",
    "You sprinkle $p on the ground.",
    "$n sprinkles $p on the ground.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_ENTHRALL_DEMON,
    "",                       
    "",
    "",
    "You sprinkle $p on the ground.",
    "$n sprinkles $p on the ground.",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CREATE_WOOD_GOLEM,
    "",                       
    "",
    "",
    "You throw $p into the air and it disappears!",
    "$n throws $p into the air and it disappears!",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CREATE_ROCK_GOLEM,
    "",                       
    "",
    "",
    "You throw $p into the air and it disappears!",
    "$n throws $p into the air and it disappears!",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CREATE_IRON_GOLEM,
    "",                       
    "",
    "",
    "You throw $p into the air and it disappears!",
    "$n throws $p into the air and it disappears!",
    "",
    ""));
  CompInfo.push_back(compInfo(SPELL_CREATE_DIAMOND_GOLEM,
    "",                       
    "",
    "",
    "You throw $p into the air and it disappears!",
    "$n throws $p into the air and it disappears!",
    "",
    ""));


  COMPINDEX ci;
  unsigned int j;
  int vnum, usage;
  spellNumT spell;
  MYSQL_RES *res;
  MYSQL_ROW row;

  int retdb = dbquery(&res, "sneezy", "buildComponentArray", "select vnum, val2, val3 from obj where type=%d", mapFileToItemType(ITEM_COMPONENT));
  if (retdb) {
    vlogf(LOG_BUG, "Terminal database error (buildComponentArray)!  ret=%d", retdb);
    exit(0);
  }
  while((row=mysql_fetch_row(res))){
    vnum=atoi(row[0]);
    spell=mapFileToSpellnum(atoi(row[1]));
    usage=atoi(row[2]);

    if(spell != TYPE_UNDEFINED &&
       (((usage & COMP_SPELL) != 0))){
      for(j=0;j<CompInfo.size();j++){
	if(CompInfo[j].spell_num == spell){
	  if(CompInfo[j].comp_num == -1 ||
	     vnum < CompInfo[j].comp_num){
	    CompInfo[j].comp_num = vnum;
	    break;
	  }
	}
      }
    }

    ci.comp_vnum = vnum;
    ci.spell_num = spell;
    ci.usage = usage;
    CompIndex.push_back(ci);
  }
  mysql_free_result(res);
}

TComponent::TComponent() :
  TObj(),
  charges(0),
  max_charges(0),
  comp_spell(TYPE_UNDEFINED),
  comp_type(0)
{
}

TComponent::TComponent(const TComponent &a) :
  TObj(a),
  charges(a.charges),
  max_charges(a.max_charges),
  comp_spell(a.comp_spell), 
  comp_type(a.comp_type)
{
}

TComponent & TComponent::operator=(const TComponent &a)
{
  if (this == &a) return *this;
  TObj::operator=(a);
  charges = a.charges;
  max_charges = a.max_charges;
  comp_spell = a.comp_spell;
  comp_type = a.comp_type;
  return *this;
}

TComponent::~TComponent()
{
  // because of weight/volume manipulation for comps and spellbags
  // make sure comp is out of any spellbag it might be in
  if (parent)
    --(*this);
}

int TComponent::getComponentCharges() const
{
  return charges;
}

void TComponent::setComponentCharges(int n)
{
  charges = n;
}

void TComponent::addToComponentCharges(int n)
{
  charges += n;
}

int TComponent::getComponentMaxCharges() const
{
  return max_charges;
}

void TComponent::setComponentMaxCharges(int n)
{
  max_charges = n;
}

void TComponent::addToComponentMaxCharges(int n)
{
  max_charges += n;
}

spellNumT TComponent::getComponentSpell() const
{
  return comp_spell;
}

void TComponent::setComponentSpell(spellNumT n)
{
  comp_spell = n;
}

unsigned int TComponent::getComponentType() const
{
  return comp_type;
}

void TComponent::setComponentType(unsigned int num)
{
  comp_type = num;
}

void TComponent::addComponentType(unsigned int num)
{
  comp_type |= num;
}

void TComponent::remComponentType(unsigned int num)
{
  comp_type &= ~num;
}

bool TComponent::isComponentType(unsigned int num) const
{
  return ((comp_type & num) != 0);
}

void TComponent::changeObjValue4(TBeing *ch)
{
  ch->specials.edit = CHANGE_COMPONENT_VALUE4;
  change_component_value4(ch, this, "", ENTER_CHECK);
  return;
}

bool TComponent::sellMeCheck(const TBeing *ch, TMonster *keeper) const
{
  int total = 0;
  TThing *t;
  TComponent *tComp;
  char buf[256];

  if (gamePort != PROD_GAMEPORT) {
    for (t = keeper->stuff; t; t = t->nextThing) {
      if ((t->number == number) &&
          (t->getName() && getName() && !strcmp(t->getName(), getName())) &&
          (tComp = dynamic_cast<TComponent *>(t))) {
        total += tComp->getComponentCharges();

        if (total >= 300) {
          sprintf(buf, "%s I already have plenty of that.", ch->name);
          keeper->doTell(buf);
          return TRUE;
        }
      }
    }
  } else {
    for (t = keeper->stuff; t; t = t->nextThing) {
      if ((t->number == number) &&
          (t->getName() && getName() &&
           !strcmp(t->getName(), getName()))) {
        total += 1;
        if (total > 50) {
          sprintf(buf, "%s I already have plenty of those.", ch->name);
          keeper->doTell(buf);
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

// returns DELETE_THIS, VICT (ch), ITEM(sub)
int TComponent::componentSell(TBeing *ch, TMonster *keeper, int shop_nr, TThing *sub)
{
  int rc;

  if (equippedBy)
    *ch += *ch->unequip(eq_pos);

  if (sub) {
    rc = get(ch, this, sub, GETOBJOBJ, true);
    if (IS_SET_DELETE(rc, DELETE_ITEM))
      return DELETE_THIS;

    if (IS_SET_DELETE(rc, DELETE_VICT))
      return DELETE_ITEM;

    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_VICT;

    if (!parent || dynamic_cast<TBeing *>(parent)) {
      // failed on volume or sumthing
      generic_sell(ch, keeper, this, shop_nr);
    }
  } else
    generic_sell(ch, keeper, this, shop_nr);

  return FALSE;
}

int TComponent::componentValue(TBeing *ch, TMonster *keeper, int shop_nr,
TThing *sub)
{
  int rc;

  if (equippedBy)
    *ch += *ch->unequip(eq_pos);

  if (sub) {
    rc = get(ch, this, sub, GETOBJOBJ, true);
    if (IS_SET_DELETE(rc, DELETE_ITEM))
      return DELETE_THIS;

    if (IS_SET_DELETE(rc, DELETE_VICT))
      return DELETE_ITEM;

    if (IS_SET_DELETE(rc, DELETE_THIS))
      return DELETE_VICT;

    if (!parent || dynamic_cast<TBeing *>(parent)) 
      valueMe(ch, keeper, shop_nr);
  } else
    valueMe(ch, keeper, shop_nr);

  return FALSE;
}

int TComponent::getShopPrice(int *discount) const
{
  return shop_price(discount);
}

void TComponent::recalcShopData(int bought, int cost)
{
  genericCalc(bought, cost);
}

void TComponent::changeComponentValue4(TBeing *ch, const char *arg, editorEnterTypeT type)
{
  int loc_update;
  int bittog = 0;
  char buf[256];

  if (type != ENTER_CHECK) {
    if (!*arg || (*arg == '\n')) {
      ch->specials.edit = CHANGE_OBJ_VALUES;
      change_obj_values(ch, this, "", ENTER_CHECK);
      return;
    }
  }
  loc_update = atoi(arg);

  switch (loc_update) {
    case 1:
    case 2:
    case 3:
    case 4:
      bittog = 1<<(loc_update-1);
      if (isComponentType(bittog))
        remComponentType(bittog);
      else
        addComponentType(bittog);
    default:
      break;
  }

  ch->sendTo(VT_HOMECLR);

  ch->sendTo(VT_CURSPOS, 3, 5);
  sprintf(buf, "1: [%c]  Decay-Enabled",
             (isComponentType(COMP_DECAY) ? 'X' : ' '));
  ch->sendTo(buf);

  ch->sendTo(VT_CURSPOS, 4, 5);
  sprintf(buf, "2: [%c]  Component for Spell-casting",
             (isComponentType( COMP_SPELL) ? 'X' : ' '));
  ch->sendTo(buf);

  ch->sendTo(VT_CURSPOS, 5, 5);
  sprintf(buf, "3: [%c]  Component for Potion-brewing",
             (isComponentType( COMP_POTION) ? 'X' : ' '));
  ch->sendTo(buf);

  ch->sendTo(VT_CURSPOS, 6, 5);
  sprintf(buf, "4: [%c]  Component for Scribing",
             (isComponentType( COMP_SCRIBE) ? 'X' : ' '));
  ch->sendTo(buf);

  ch->sendTo(VT_CURSPOS, 22, 1);
  ch->sendTo("Hit return when finished.");
  ch->sendTo(VT_CURSPOS, 23, 1);
  ch->sendTo("Select the property to toggle.\n\r--> ");
}

void TComponent::boottimeInit()
{
  unsigned int j;

  // init component info
  if (getComponentSpell() != TYPE_UNDEFINED &&
      (isComponentType(COMP_SPELL))) {
    for (j = 0; j < CompInfo.size(); j++) {
      if (CompInfo[j].spell_num == getComponentSpell()) {
        if (CompInfo[j].comp_num == -1 ||
            objVnum() < CompInfo[j].comp_num) {
          CompInfo[j].comp_num = objVnum();
          break;
        }
      }
    }
  }
  COMPINDEX ci;
  ci.comp_vnum = objVnum();
  ci.spell_num = getComponentSpell();
  ci.usage = getComponentType();
  CompIndex.push_back(ci);
}

int TComponent::rentCost() const
{
  int num = TObj::rentCost();

  num *= getComponentCharges();
  num /= max(1, getComponentMaxCharges());

  num = (int) (num / priceMultiplier());

  return num;
}

void TComponent::decayMe()
{
  if (obj_flags.decay_time <= 0)
    return;

  // not a decaying component
  if (!isComponentType(COMP_DECAY))
    return;

  // don't decay if a shopkeeper has the comp
  if (dynamic_cast<TMonster *>(parent) &&
      parent->spec == SPEC_SHOPKEEPER)
    return;

  // decay if it doesn't have a parent (lying on the ground)
  // or if the parent is not spellbag
  // or random chance
  if(!parent ||
     !dynamic_cast<TSpellBag *>(parent) || 
     (!::number(0,5))){
    obj_flags.decay_time--;
  }
}

void TComponent::assignFourValues(int x1, int x2, int x3, int x4)
{
  setComponentCharges(x1);
  setComponentMaxCharges(x2);
  setComponentSpell(mapFileToSpellnum(x3));
  setComponentType(x4);
}

void TComponent::getFourValues(int *x1, int *x2, int *x3, int *x4) const
{
  *x1 = getComponentCharges();
  *x2 = getComponentMaxCharges();
  *x3 = mapSpellnumToFile(getComponentSpell());
  *x4 = getComponentType();
}

string TComponent::statObjInfo() const
{
  char buf[256];

  if (getComponentSpell() <= TYPE_UNDEFINED) {
    sprintf(buf, "UNKNOWN/BOGUS spell\n\r");
  } else {
    sprintf(buf, "Spell for : %d (%s)\n\r",
      getComponentSpell(),
      (discArray[getComponentSpell()] &&
       discArray[getComponentSpell()]->name) ? 
       discArray[getComponentSpell()]->name :
       "UNKNOWN/BOGUS");
  }

  if (isComponentType(COMP_DECAY))
    strcat(buf, "Component will decay.\n\r");
  if (isComponentType(COMP_SPELL))
    strcat(buf, "Component is for spell casting.\n\r");
  if (isComponentType(COMP_POTION))
    strcat(buf, "Component is for brewing.\n\r");
  if (isComponentType(COMP_SCRIBE))
    strcat(buf, "Component is for scribing.\n\r");

  sprintf(buf + strlen(buf), "Current Uses : %d, Max Uses : %d",
      getComponentCharges(),
      getComponentMaxCharges());

  string a(buf);
  return a;
}

bool TComponent::objectRepair(TBeing *ch, TMonster *repair, silentTypeT silent)
{
  if (!silent) {
    char buf[256];
  
    sprintf(buf, "%s You might wanna take that to the magic shop!", fname(ch->name).c_str());

    repair->doTell(buf);
  }
  return TRUE;
}

void TComponent::lowCheck()
{
  if (!isname("component", name)) {
    vlogf(LOG_LOW, "Component without COMPONENT in name (%s : %d)", getName(), objVnum());
  }
  int sp = suggestedPrice();
  if (obj_flags.cost != sp) {
    vlogf(LOG_LOW, "component (%s:%d) with bad price %d should be %d.",
          getName(), objVnum(), obj_flags.cost, sp);
    obj_flags.cost = sp;
  }

  TObj::lowCheck();
}

int TComponent::objectSell(TBeing *ch, TMonster *keeper)
{
  if (gamePort != PROD_GAMEPORT)
    return FALSE;

#if 0
  char buf[256];

  if ((getComponentCharges() != getComponentMaxCharges())) {
    sprintf(buf, "%s I'm sorry, I don't buy back partially used components.", ch->getName());
    keeper->doTell(buf);
    return TRUE;
  }
#endif
  return FALSE;
}

void TComponent::findComp(TComponent **best, spellNumT spell)
{
  if (getComponentSpell() == spell &&
      isComponentType(COMP_SPELL)) {
    // it's the proper component
    if (!*best) {
      *best = this;
      return;
    }
    // avoid 0-cost components if at all possible
    // otherwise, use one with least charges left
    int rc_me = rentCost();
    int rc_best = (*best)->rentCost();
    if (rc_best <= 0 && rc_me > 0) {
      *best = this;
      return;
    } else if (rc_best > 0 && rc_me <= 0)
      return; 

    // check charges ONLY if: both comps are 0 cost, or both are non-0 cost
    if (getComponentCharges() < (*best)->getComponentCharges()) {
      *best = this;
    }
  }
}

int TComponent::putMeInto(TBeing *, TOpenContainer *)
{
  // components can only be put into spellbags
  return FALSE;
}

void TComponent::describeObjectSpecifics(const TBeing *ch) const
{
  if (getComponentCharges() != getComponentMaxCharges())
    ch->sendTo(COLOR_OBJECTS,"%s has been partially used.\n\r",
        good_cap(getName()).c_str());

  ch->sendTo(COLOR_OBJECTS,"%s has about %d uses left.\n\r",
        good_cap(getName()).c_str(), getComponentCharges());
}

void TComponent::update(int use)
{
  if (obj_flags.decay_time > 0) {
    if (!parent || !dynamic_cast<TSpellBag *>(parent))
      obj_flags.decay_time -= use;
  }

  if (stuff)
    stuff->update(use);

  if (nextThing) {
    if (nextThing != this)
      nextThing->update(use);
  }
}

void TComponent::findSomeComponent(TComponent **comp_gen, TComponent **spell_comp, TComponent **comp_brew, spellNumT which, int type)
{
  int dink;
  if (type == 1)
    dink = COMP_POTION;
  else if (type == 2)
    dink = COMP_SCRIBE;
  else {
    vlogf(LOG_MISC, "Unknown type in findSomeComps");
    return;
  }

  if (!*comp_gen && (getComponentSpell() == TYPE_UNDEFINED) &&
       isComponentType(dink))
          *comp_gen = this;
  else if (!*spell_comp && (getComponentSpell() == which) &&
       isComponentType(COMP_SPELL))
          *spell_comp = this;
  else if (!*comp_brew && (getComponentSpell() == which) &&
              isComponentType(dink))
          *comp_brew = this;
}

void TComponent::evaluateMe(TBeing *ch) const
{
  int learn;

  learn = ch->getSkillValue(SKILL_EVALUATE);
  if (learn <= 0) {
    ch->sendTo("You are not sufficiently knowledgeable about evaluation.\n\r");
    return;
  }

  // adjust for knowledge about magic stuff
  if (ch->hasClass(CLASS_RANGER)) {
    learn *= ch->getClassLevel(CLASS_RANGER);
    learn /= 200;
  } else {
    learn *= ch->getSkillValue(SPELL_IDENTIFY);
    learn /= 100;
  }
  if (learn <= 0) {
    ch->sendTo("You lack the knowledge to identify that item of magic.\n\r");
    return;
  }
  ch->sendTo(COLOR_OBJECTS, "You evaluate the magical powers of %s...\n\r\n\r",
             getName());

  ch->describeObject(this);

  if (learn > 10)
    ch->describeComponentSpell(this, learn);

  if (learn > 20)
    ch->describeComponentUseage(this, learn);

  if (learn > 30)
    ch->describeComponentDecay(this, learn);
}

bool TComponent::fitInShop(const char *, const TBeing *ch) const
{
  if (ch->hasClass(CLASS_MAGE | CLASS_RANGER)) {
    // skip brew and scribe comps
    if (!IS_SET(getComponentType(), COMP_SPELL))
      return false;

    spellNumT skill = getComponentSpell();
    if (skill <= TYPE_UNDEFINED)
      // a "generic" comp, this should probably "fit"
      return TRUE;

    // if bogus spell, skip
    discNumT which = getDisciplineNumber(skill, FALSE);
    if (which == DISC_NONE)
      return false;

    return (ch->getSkillValue(skill));  
  }
  return FALSE;
}

bool TComponent::splitMe(TBeing *ch, const char *tString)
{
  if (gamePort == PROD_GAMEPORT)
    return false;

  int         tCount = 0,
              tValue = 0;
  double      tCost  = 0.0;
  TComponent *tComponent;
  string      tStString(""),
              tStBuffer("");

  for (; isspace(*tString); tString++);

  if (*tString)
    two_arg(tString, tStString, tStBuffer);

  if (!*tString || ((tCount = atoi(tStBuffer.c_str())) <= 0)) {
    ch->sendTo("Syntax: split <component> <charges>\n\r");
    return true;
  }

  if (tCount >= getComponentCharges()) {
    ch->sendTo("Charges must be between 1 and %d.\n\r",
               (getComponentCharges() -1));
    return true;
  }

  if (!obj_flags.cost || objVnum() < 0) {
    ch->sendTo("This component is special, it can not be split up.\n\r");
    return true;
  }

  if ((tValue = real_object(objVnum())) < 0 || tValue > (signed) obj_index.size() ||
      !(tComponent = dynamic_cast<TComponent *>(read_object(tValue, REAL)))) {
    ch->sendTo("For some reason that component resists being split up.\n\r");
    return true;
  }

  tCost = ((double) tCount / (double) getComponentCharges());

  tComponent->obj_flags.cost = (int) ((double) obj_flags.cost * tCost);
  obj_flags.cost -= tComponent->obj_flags.cost;

  tComponent->setComponentCharges(tCount);
  tComponent->setComponentMaxCharges(tCount);
  addToComponentCharges(-tCount);
  addToComponentMaxCharges(-tCount);

  *ch += *tComponent;

  return true;
}

int TComponent::putSomethingIntoContainer(TBeing *ch, TOpenContainer *cont)
{
  int rc = TObj::putSomethingIntoContainer(ch, cont);
  if (rc != TRUE)
    return rc;

  // put succeeded
  mud_assert(parent == cont, "Bizarre situation in putSomethig int (%d)", rc);

  // Enable for !prod for re-introduction.
  if (gamePort != PROD_GAMEPORT) {
    TThing *t;
    TComponent *tComp;

    for (t = cont->stuff; t; t = t->nextThing) {
      // Basically find another component of the same type that is:
      // Same VNum.
      // Has a cost greater than 0 (ignore comps from leveling)
      if (t == this || !(tComp = dynamic_cast<TComponent *>(t)) ||
          (tComp->objVnum() != objVnum()) || tComp->obj_flags.cost <= 0 ||
          obj_flags.cost <= 0)
        continue;

      // they just threw the same component into the bag, merge them...
      act("$p glows brightly and merges with $N.",
          FALSE, ch, this, tComp, TO_CHAR);
      addToComponentCharges(tComp->getComponentCharges());
      addToComponentMaxCharges(tComp->getComponentMaxCharges());
      obj_flags.cost += tComp->obj_flags.cost;
      --(*tComp);
      delete tComp;
    }
  }

  return rc;
}

string TComponent::getNameForShow(bool useColor, bool useName, const TBeing *ch) const
{
  string buf;
  buf = useName ? name : (useColor ? getName() : getNameNOC(ch));

  spellNumT spell_num = getComponentSpell();
  if (spell_num > TYPE_UNDEFINED && discArray[spell_num]) {
    buf += " [";
    buf += discArray[spell_num]->name;
    buf += "]";
  }

  return buf;
}

compInfo::compInfo(spellNumT sn, const char *tc, const char *to, const char *tv, const char *ts, const char *tr, const char *tso, const char *tro) :
  comp_num(-1),
  spell_num(sn),
  to_caster(tc),
  to_other(to),
  to_vict(tv),
  to_self(ts),
  to_room(tr),
  to_self_object(tso),
  to_room_object(tro)
{
}

int TComponent::suggestedPrice() const
{
  spellNumT curspell = getComponentSpell();
  int value = 0;
  if (curspell > TYPE_UNDEFINED) {
    // since it's from a component, treat the level as lowest possible
    // a correction to lev is made inside SpellCost, so we can safely
    // pass it L1 and let SpellCost fix it for us
    // for level, we'll assign a value of 100% arbitrarily
    value = getSpellCost(curspell, 1, 100);
    value *= getComponentCharges();
    //    value *= getComponentMaxCharges();
  }

  value = (int) (value * priceMultiplier());

  return value;
}

void TComponent::objMenu(const TBeing *ch) const
{
  ch->sendTo(VT_CURSPOS, 3, 1);
  ch->sendTo("%sSuggested price:%s %d%s",
             ch->purple(), ch->norm(), suggestedPrice(),
             (suggestedPrice() != obj_flags.cost ? " *" : ""));
}

bool isDissectComponent(int vnum)
{
  switch (vnum) {
    case COMP_SAND_BLAST:   // periodic loads
    case COMP_COLOR_SPRAY:   // periodic loads
    case COMP_ACID_BLAST:        // periodic loads
    case COMP_TELEPATHY:        // periodic loads
    case COMP_FUMBLE:        // periodic loads
    case COMP_STONE_SKIN:        // periodic loads
    case COMP_STEALTH:        // periodic loads
      //case COMP_CLOUD_OF_CONCEAL:        // periodic loads
    case COMP_CONJURE_AIR:        // periodic loads
    case COMP_FLIGHT:        // periodic loads
    case COMP_HELLFIRE:        // periodic loads

    // hard coded dissects
    case COMP_FLAMING_SWORD:           // dissect loads
    case COMP_INVISIBILITY_BREW:        // dissect loads
    case COMP_SORCERERS_GLOBE_BREW:        // dissect loads
    case COMP_TRUE_SIGHT_BREW:          // dissect loads

    // items in lib/objdata/dissect
    case COMP_FEATHERY_DESCENT:        // dissect loads
    // case COMP_FUMBLE:  // also a periodic load
    case COMP_TRAIL_SEEK_BREW:          // dissect loads
    case COMP_SENSE_LIFE:        // dissect loads
    case COMP_SHATTER:          // dissect loads
    case COMP_FALCON_WINGS:          // dissect loads
    case COMP_ENERGY_DRAIN:        // dissect loads
    case COMP_FARLOOK:        // dissect loads
    case COMP_ENSORCER:        // dissect loads
    case COMP_FEAR:        // dissect loads
    case COMP_ACCELERATE:        // dissect loads
    case COMP_GILLS_OF_FLESH:        // dissect loads
    case COMP_INFRAVISION:        // dissect loads
    case COMP_BREATH_SARAHAGE:        // dissect loads
    case COMP_DISPEL_MAGIC_BREW:        // dissect loads
    case COMP_SENSE_LIFE_BREW:          // dissect loads
    case COMP_INFRAVISION2_BREW:          // dissect loads
    case COMP_GILLS_OF_FLESH_BREW:        // dissect loads
    case COMP_HASTE:        // dissect loads
    case COMP_BIND:        // dissect loads
    case COMP_STUNNING_ARROW:               // dissect loads
    case COMP_ANTIGRAVITY:        // dissect loads
    case COMP_INFRAVISION_BREW:        // dissect loads

    // skinning items
    case COMP_POLYMORPH:
    case COMP_COPY:
    // numerous hides not accounted for here...

    // these are just generic items (non-comps), from dissect
    case 4791:
    case OBJ_AQUA_DRAG_HEAD:
    case 20425:
    case 20409:
    case 20437:
    case 20440:
    case 20441:
    case 20442:
    case 20443:
    case 20450:

      return true;
    default:
      return false;
  }
}

// we want some components to have an inflated price so they can be sold
// for mega-bucks, and are thus sought ought
// but we don't want to make them expensive to rent at the same time
double TComponent::priceMultiplier() const
{
  int ov = objVnum();

  if (ov == COMP_ENHANCE_WEAPON)  // enhance weapon
    return 100.0;
  if (ov == COMP_GALVANIZE)
    return 30.0;

  if (isDissectComponent(ov))
    return 3.0;

  return 1.0;
}

void TComponent::purchaseMe(TBeing *ch, TMonster *keeper, int cost, int shop_nr)
{
  ch->addToMoney(-cost, GOLD_SHOP_COMPONENTS);
  if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY)) {
    keeper->addToMoney(cost, GOLD_SHOP_COMPONENTS);
  }

  shoplog(shop_nr, ch, keeper, getName(), cost, "buying");
}

void TComponent::sellMeMoney(TBeing *ch, TMonster *keeper, int cost, int shop_nr)
{
  ch->addToMoney(cost, GOLD_SHOP_COMPONENTS);
  if (!IS_SET(shop_index[shop_nr].flags, SHOP_FLAG_INFINITE_MONEY))
    keeper->addToMoney(-cost, GOLD_SHOP_COMPONENTS);

  shoplog(shop_nr, ch, keeper, getName(), cost, "selling");
}

TThing & TComponent::operator -- ()
{
  // if we are in a spellbag, then spellbag does NOT have the weight/volume
  // of our component
  // counterintuitive: we are UNDOing what will happen in TThing::
  // we want things to be 25% of true mass/vol
  if (parent && dynamic_cast<TSpellBag *>(parent)) {
    parent->addToCarriedWeight(getTotalWeight(TRUE) * 0.75);
    parent->addToCarriedVolume((int) (getReducedVolume(NULL) * 0.75));
  }

  TObj::operator -- ();

  return *this;
}

const string TComponent::shopList(const TBeing *ch, const char *tArg,
                                  int iMin, int iMax, int num,
                                  int shop_nr, int k, unsigned long int FitT) const
{
  if (gamePort == PROD_GAMEPORT)
    return TObj::shopList(ch, tArg, iMin, iMax, num, shop_nr, k, FitT);

  char        tString[256],
              tStObj[256],
              tStSpell[256];
  int         tCost,
              tDiscount,
              tCharges = 0,
              tLearn;
  TThing     *tThing;
  TComponent *tComp;
  TBeing     *tKeeper = NULL;

  sprintf(tStObj, getNameNOC(ch).c_str());

  if (strlen(tStObj) > 29) {
    tStObj[26] = '\0';
    strcat(tStObj, "...");
  }

  for (tThing = real_roomp(shop_index[shop_nr].in_room)->stuff;
       tThing; tThing = tThing->nextThing)
    if ((tKeeper = dynamic_cast<TBeing *>(tThing)) &&
        shop_index[shop_nr].keeper == tKeeper->mobVnum() &&
        !tKeeper->isPc())
      break;

  if (!tKeeper) {
    vlogf(LOG_MISC, "TComponent::shopList called by non-existant keeper.");
    return "";
  }

  for (tThing = tKeeper->stuff; tThing; tThing = tThing->nextThing)
    if ((tComp = dynamic_cast<TComponent *>(tThing)) &&
        tComp->objVnum() == objVnum())
      tCharges += tComp->getComponentCharges();

  tCost = (shopPrice(1, shop_nr,
                     min(18, ch->plotStat(STAT_CURRENT, STAT_CHA, 3, 18, 13)),
                     &tDiscount) / getComponentCharges());

  tLearn = ch->getSkillValue(SKILL_EVALUATE);

  if (ch->hasClass(CLASS_RANGER)) {
    tLearn *= ch->getClassLevel(CLASS_RANGER);
    tLearn /= 200;
  } else {
    tLearn *= ch->getSkillValue(SPELL_IDENTIFY);
    tLearn /= 100;
  }

  if (tLearn > 10) {
    strcpy(tStSpell, discArray[getComponentSpell()]->name);

    if (strlen(tStSpell) > 29) {
      tStSpell[26] = '\0';
      strcat(tStSpell, "...");
    }
  } else
    strcpy(tStSpell, "*You have no idea*");

  sprintf(tString, "[%2d] %-31s %-6d%s [%-3d] %s\n\r",
          (k + 1), cap(tStObj), tCost,
          (tDiscount < 100 ? "(*)" : "   "),
          tCharges, tStSpell);

  if (((FitT & (1 << 0)) == 0 || fitInShop("", ch)) &&
      (!*tArg || isname(tArg, name)) &&
      (iMin == 999999 || (tCost >= iMin && tCost <= iMax)))
    return tString;

  return "";
}

void TComponent::buyMe(TBeing *ch, TMonster *tKeeper, int tNum, int tShop)
{
  if (gamePort == PROD_GAMEPORT) {
    TObj::buyMe(ch, tKeeper, tNum, tShop);
    return;
  }

  int     tChr,
          tCost,
          tDiscount = 100,
          tValue = 0;
  double  tCount;
  char    tString[256];
  TObj   *tObj;

  if ((ch->getCarriedVolume() + getTotalVolume()) > ch->carryVolumeLimit()) {
    ch->sendTo("%s: You can not carry that much volume.\n\r", fname(name).c_str());
    return;
  }

  if (compareWeights(getTotalWeight(TRUE),
                     (ch->carryWeightLimit() - ch->getCarriedWeight())) == -1) {
    ch->sendTo("%s: You can not carry that much weight.\n\r", fname(name).c_str());
    return;
  }

  tChr = ch->plotStat(STAT_CURRENT, STAT_CHA, 3, 18, 13);

  if (ch->doesKnowSkill(SKILL_SWINDLE)) {
    // make 5 separate rolls so chr goes up amount based on learning
    for (int tRunner = 0; tRunner < 5; tRunner++)
      if (bSuccess(ch, ch->getSkillValue(SKILL_SWINDLE), SKILL_SWINDLE))
        tChr += 1;
  }

  tChr   = min(18, tChr);
  tCount = ((double) tNum / (double) getComponentCharges());
  tCost  = (int) ((double) shopPrice(1, tShop, tChr, &tDiscount) * tCount);

  if ((ch->getMoney() < tCost) && !ch->hasWizPower(POWER_GOD)) {
    sprintf(tString, shop_index[tShop].missing_cash2, ch->name);
    tKeeper->doTell(tString);

    switch (shop_index[tShop].temper1) {
      case 0:
        tKeeper->doAction(ch->name, CMD_SMILE);
        break;
      case 1:
        act("$n grins happily.", 0, tKeeper, 0, 0, TO_ROOM);
        break;
      default:
        break;
    }
  }

  if (shop_producing(this, tShop)) {
    if (!(tObj = read_object(number, REAL))) {
      vlogf(LOG_MISC, "Shop producing unlimited of an item not in db!  [%d]", number);
      return;
    }

    tObj->purchaseMe(ch, tKeeper, tCost, tShop);
    *ch += *tObj;
    ch->logItem(tObj, CMD_BUY);
    tValue++;
    tObj->recalcShopData(TRUE, tCost);
  } else {
    TThing     *tThing;
    TComponent *tComp;

    for (tThing = tKeeper->stuff; tThing; tThing = tThing->nextThing) {
      // Basically find another component of the same type that is:
      // Same VNum.
      // Has a cost greater than 0 (ignore comps from leveling)
      if (tThing == this || !(tComp = dynamic_cast<TComponent *>(tThing)) ||
          (tComp->objVnum() != objVnum()) || tComp->obj_flags.cost <= 0 ||
          obj_flags.cost <= 0)
        continue;

      // We have more than one of the same comp, let's merge them.
      addToComponentCharges(tComp->getComponentCharges());
      addToComponentMaxCharges(tComp->getComponentMaxCharges());
      obj_flags.cost += tComp->obj_flags.cost;
      --(*tComp);
      delete tComp;
    }

    if (tNum > getComponentCharges()) {
      sprintf(tString, "%s I don't have %d charges of that component.  Here %s the %d I do have.",
              ch->name, tNum, ((getComponentCharges() > 2) ? "are" : "is"),
              getComponentCharges());
      tKeeper->doTell(tString);
      tNum  = getComponentCharges();
      tCost = shopPrice(1, tShop, tChr, &tDiscount);
    }

    if (getComponentCharges() == tNum) {
      tObj = this;
      --(*tObj);
    } else {
      if (!(tObj = read_object(number, REAL))) {
        vlogf(LOG_MISC, "Shop with item not in db!  [%d]", number);
        return;
      }

      TComponent *tComponent;

      if ((tComponent = dynamic_cast<TComponent *>(tObj))) {
        tComponent->setComponentCharges(tNum);
        tComponent->setComponentMaxCharges(tNum);
      }

      addToComponentCharges(-tNum);
      addToComponentMaxCharges(-tNum);
      tObj->obj_flags.cost = tCost;
      obj_flags.cost -= tCost;
    }

    tObj->purchaseMe(ch, tKeeper, tCost, tShop);
    *ch += *tObj;
    ch->logItem(tObj, CMD_BUY);
    tValue++;
    tObj->recalcShopData(TRUE, tCost);

    sprintf(tString, "%s/%d", SHOPFILE_PATH, tShop);
    tKeeper->saveItems(tString);
  }

  if (!tValue)
    return;

  sprintf(tString, shop_index[tShop].message_buy,
          ch->name, tCost);
  tKeeper->doTell(tString);

  ch->sendTo(COLOR_OBJECTS, "You now have %s (*%d charges).\n\r",
          good_uncap(getName()).c_str(), tNum);
  act("$n buys $p.", FALSE, ch, this, NULL, TO_ROOM);
  ch->doSave(SILENT_YES);
}

void TComponent::sellMe(TBeing *ch, TMonster *tKeeper, int tShop)
{
  if (gamePort == PROD_GAMEPORT) {
    TObj::sellMe(ch, tKeeper, tShop);
    return;
  }

  char tString[256];
  int  tChr,
       tCost,
       tDiscount = 100;

  if (!shop_index[tShop].profit_sell) {
    sprintf(tString, shop_index[tShop].do_not_buy, ch->getName());
    tKeeper->doTell(tString);
    return;
  }

  if (obj_flags.cost <= 1 || isObjStat(ITEM_NEWBIE)) {
    sprintf(tString, "%s I'm sorry, I don't buy valueless items.", ch->getName());
    tKeeper->doTell(tString);
    return;
  }

  if (sellMeCheck(ch, tKeeper))
    return;

  tChr = ch->plotStat(STAT_CURRENT, STAT_CHA, 3, 18, 13);

  if (ch->doesKnowSkill(SKILL_SWINDLE)) {
    // make 5 separate rolls so chr goes up amount based on learning
    for (int tRunner = 0; tRunner < 5; tRunner++)
      if (bSuccess(ch, ch->getSkillValue(SKILL_SWINDLE), SKILL_SWINDLE))
        tChr += 1;
  }

  tChr   = min(18, tChr);
  tCost  = max(1, sellPrice(tShop, tChr, &tDiscount));

  if (tKeeper->getMoney() < tCost) {
    sprintf(tString, shop_index[tShop].missing_cash1, ch->getName());
    tKeeper->doTell(tString);
    return;
  }

  if (isRare() && obj_index[getItemIndex()].max_exist <= 10) {
    sprintf(tString, "%s Wow!  This is one of those limited items.", ch->name);
    tKeeper->doTell(tString);
    sprintf(tString, "%s You should really think about auctioning it.", ch->name);
    tKeeper->doTell(tString);
  }

  act("$n sells $p.", FALSE, ch, this, 0, TO_ROOM);

  sprintf(tString, shop_index[tShop].message_sell, ch->getName(), tCost);
  tKeeper->doTell(tString);

  ch->sendTo(COLOR_OBJECTS, "The shopkeeper now has %s.\n\r", good_uncap(getName()).c_str());
  ch->logItem(this, CMD_SELL);

  sellMeMoney(ch, tKeeper, tCost, tShop);
  recalcShopData(FALSE, tCost);

  if (ch->isAffected(AFF_GROUP) && ch->desc &&
           IS_SET(ch->desc->autobits, AUTO_SPLIT) &&
          (ch->master || ch->followers)) {
    sprintf(tString, "%d", tCost);
    ch->doSplit(tString, false);
  }

  // exception to the eq-sharing log, if person sells item to shop as char#1
  // permit them to buy it as char#2
  // this means the "owners" log should be reset on sell
  delete [] owners;
  owners = NULL;

  --(*this);

  if (shop_producing(this, tShop)) {
    delete this;
  } else
    *tKeeper += *this;

  sprintf(tString, "%s/%d", SHOPFILE_PATH, tShop);
  tKeeper->saveItems(tString);
  ch->doSave(SILENT_YES);
}
