//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// spell_num : conversion between internal enums and external ints for 
//    spell numbers
//
//////////////////////////////////////////////////////////////////////////


#include "stdsneezy.h"
#if 0
    case SPELL_FIND_FAMILIAR:
      return 35;
    case SPELL_CHAIN_LIGHTNING:
      return 96; // not coded
    case SPELL_DETECT_POISON:
      return 139; // NOT CODED;
    case SPELL_HAIL_STORM:
      return 169; // not coded;
    case SPELL_HOLOCAUST:
      return 170; // not coded;
    case SPELL_HARM_FULL:
      return 174; // not coded (maybe good);
    case SPELL_HARM_CRITICAL_SPRAY:
      return 175; // not coded;
    case SPELL_HARM_SPRAY:
      return 176; // not coded;
    case SPELL_HARM_FULL_SPRAY:
      return 177; // not coded;
    case SKILL_BUTCHER:
      return 275; // not coded;
    case SKILL_MASS_FORAGE:
      return 309;
    case SKILL_TAN:
      return 311;
    case SPELL_DETECT_POISON_DEIKHAN:
      return 331; // not coded;
    case SKILL_HOLY_WEAPONS:
      return 358; // not coded;
    case SPELL_SYNO_CASTER:
      return 370;
    case SPELL_HOLY_LIGHT:
      return 383; // not coded;
    case SKILL_CASTING:
      return 598;
    case SKILL_PRAYING:
      return 604;
    case SKILL_CLAW:
      return 607;
    case SKILL_CLEAVE:
      return 608;
    case SKILL_SLASH2:
      return 609;
    case SKILL_SLICE:
      return 610;
    case SKILL_SMASH:
      return 614;
    case SKILL_CRUSH:
      return 615;
    case SKILL_BLUDGEON:
      return 616;
    case SKILL_SMITE2:
      return 617;
    case SKILL_HIT:
      return 618;
    case SKILL_WHIP:
      return 619;
    case SKILL_PUMMEL:
      return 620;
    case SKILL_FLAIL:
      return 621;
    case SKILL_BEAT:
      return 622;
    case SKILL_THRASH:
      return 623;
    case SKILL_THUMP:
      return 624;
    case SKILL_WALLOP:
      return 625;
    case SKILL_BATTER:
      return 626;
    case SKILL_STRIKE:
      return 627;
    case SKILL_CLUB:
      return 628;
    case SKILL_POUND:
      return 629;
    case SKILL_STAB:
      return 634;
    case SKILL_STING:
      return 635;
    case SKILL_BITE:
      return 636;
    case SKILL_PIERCE2:
      return 637;
    case SKILL_THRUST:
      return 638;
    case SKILL_SPEAR:
      return 639;
    case SKILL_SHORT_BOW:
      return 644;
    case SKILL_LONG_BOW:
      return 645;
    case SKILL_CROSSBOW:
      return 646;
    case SKILL_THROW:
      return 647;
    case SKILL_ARMOR_USE:
      return 653;
    case SKILL_DEFEND:
      return 662;
#endif

int mapSpellnumToFile(spellNumT stt)
{
  switch (stt) {
    case TYPE_UNDEFINED:
      return -1;
    case SPELL_GUST:
      return 0;
    case SPELL_SLING_SHOT:
      return 1;
    case SPELL_GUSHER:
      return 2;
    case SPELL_HANDS_OF_FLAME:
      return 3;
    case SPELL_MYSTIC_DARTS:
      return 4;
    case SPELL_FLARE:
      return 5;
    case SPELL_SORCERERS_GLOBE:
      return 6;
    case SPELL_FAERIE_FIRE:
      return 7;
    case SPELL_ILLUMINATE:
      return 8;
    case SPELL_DETECT_MAGIC:
      return 9;
    case SPELL_STUNNING_ARROW:
      return 10;
    case SPELL_MATERIALIZE:
      return 11;
    case SPELL_PROTECTION_FROM_EARTH:
      return 12;
    case SPELL_PROTECTION_FROM_AIR:
      return 13;
    case SPELL_PROTECTION_FROM_FIRE:
      return 14;
    case SPELL_PROTECTION_FROM_WATER:
      return 15;
    case SPELL_PROTECTION_FROM_ELEMENTS:
      return 16;
    case SPELL_PEBBLE_SPRAY:
      return 17;
    case SPELL_ARCTIC_BLAST:
      return 18;
    case SPELL_COLOR_SPRAY:
      return 19;
    case SPELL_INFRAVISION:
      return 20;
    case SPELL_IDENTIFY:
      return 21;
    case SPELL_POWERSTONE:
      return 22;
    case SPELL_FAERIE_FOG:
      return 23;
    case SPELL_TELEPORT:
      return 24;
    case SPELL_SENSE_LIFE:
      return 25;
    case SPELL_CALM:
      return 26;
    case SPELL_ACCELERATE:
      return 27;
    case SPELL_DUST_STORM:
      return 28;
    case SPELL_LEVITATE:
      return 29;
    case SPELL_FEATHERY_DESCENT:
      return 30;
    case SPELL_STEALTH:
      return 31;
    case SPELL_GRANITE_FISTS:
      return 32;
    case SPELL_ICY_GRIP:
      return 33;
    case SPELL_GILLS_OF_FLESH:
      return 34;
    case SPELL_TELEPATHY:
      return 36;
    case SPELL_FEAR:
      return 37;
    case SPELL_SLUMBER:
      return 38;
    case SPELL_CONJURE_EARTH:
      return 39;
    case SPELL_CONJURE_AIR:
      return 40;
    case SPELL_CONJURE_FIRE:
      return 41;
    case SPELL_CONJURE_WATER:
      return 42;
    case SPELL_DISPEL_MAGIC:
      return 43;
    case SPELL_ENHANCE_WEAPON:
      return 44;
    case SPELL_GALVANIZE:
      return 45;
    case SPELL_DETECT_INVISIBLE:
      return 46;
    case SPELL_DISPEL_INVISIBLE:
      return 47;
    case SPELL_TORNADO:
      return 48;
    case SPELL_SAND_BLAST:
      return 49;
    case SPELL_ICE_STORM:
      return 50;
    case SPELL_BLAST_OF_FURY:
      return 51;
    case SPELL_ACID_BLAST:
      return 52;
    case SPELL_FIREBALL:
      return 53;
    case SPELL_FARLOOK:
      return 54;
    case SPELL_FALCON_WINGS:
      return 55;
    case SPELL_INVISIBILITY:
      return 56;
    case SPELL_ENSORCER:
      return 57;
    case SPELL_EYES_OF_FERTUMAN:
      return 58;
    case SPELL_COPY:
      return 59;
    case SPELL_HASTE:
      return 60;
    case SPELL_IMMOBILIZE:
      return 66;
    case SPELL_SUFFOCATE:
      return 67;
    case SPELL_FLY:
      return 68;
    case SPELL_ANTIGRAVITY:
      return 69;
    case SPELL_DIVINATION:
      return 73;
    case SPELL_SHATTER:
      return 74;
    case SKILL_SCRIBE:
      return 75;
    case SPELL_SPONTANEOUS_GENERATION:
      return 76;
    case SPELL_METEOR_SWARM:
      return 80;
    case SPELL_LAVA_STREAM:
      return 81;
    case SPELL_STONE_SKIN:
      return 82;
    case SPELL_TRAIL_SEEK:
      return 83;
    case SPELL_INFERNO:
      return 87;
    case SPELL_HELLFIRE:
      return 88;
    case SPELL_FLAMING_FLESH:
      return 89;
    case SPELL_FLAMING_SWORD:
      return 90;
    case SPELL_ENERGY_DRAIN:
      return 94;
    case SPELL_ATOMIZE:
      return 95;
    case SPELL_ANIMATE:
      return 97;
    case SPELL_BIND:
      return 98;
    case SPELL_ETHER_GATE:
      return 99;
    case SPELL_FUMBLE:
      return 102;
    case SPELL_TRUE_SIGHT:
      return 103;
    case SPELL_CLOUD_OF_CONCEALMENT:
      return 104;
    case SPELL_POLYMORPH:
      return 105;
    case SPELL_SILENCE:
      return 106;
    case SPELL_WATERY_GRAVE:
      return 110;
    case SPELL_TSUNAMI:
      return 111;
    case SPELL_BREATH_OF_SARAHAGE:
      return 112;
    case SPELL_PLASMA_MIRROR:
      return 113;
    case SPELL_GARMULS_TAIL:
      return 114;
    case SPELL_HEAL_LIGHT:
      return 126;
    case SPELL_HARM_LIGHT:
      return 127;
    case SPELL_CREATE_FOOD:
      return 128;
    case SPELL_CREATE_WATER:
      return 129;
    case SPELL_ARMOR:
      return 130;
    case SPELL_BLESS:
      return 131;
    case SPELL_CLOT:
      return 132;
    case SPELL_RAIN_BRIMSTONE:
      return 133;
    case SPELL_HEAL_SERIOUS:
      return 134;
    case SPELL_HARM_SERIOUS:
      return 135;
    case SPELL_STERILIZE:
      return 136;
    case SPELL_EXPEL:
      return 137;
    case SPELL_CURE_DISEASE:
      return 138;
    case SPELL_CURSE:
      return 140;
    case SPELL_REMOVE_CURSE:
      return 141;
    case SPELL_CURE_POISON:
      return 142;
    case SPELL_HEAL_CRITICAL:
      return 143;
    case SPELL_SALVE:
      return 144;
    case SPELL_POISON:
      return 145;
    case SPELL_HARM_CRITICAL:
      return 146;
    case SPELL_INFECT:
      return 147;
    case SPELL_REFRESH:
      return 148;
    case SPELL_NUMB:
      return 149;
    case SPELL_DISEASE:
      return 150;
    case SPELL_FLAMESTRIKE:
      return 151;
    case SPELL_PLAGUE_LOCUSTS:
      return 152;
    case SPELL_CURE_BLINDNESS:
      return 153;
    case SPELL_SUMMON:
      return 154;
    case SPELL_HEAL:
      return 155;
    case SPELL_PARALYZE_LIMB:
      return 156;
    case SPELL_WORD_OF_RECALL:
      return 157;
    case SPELL_HARM:
      return 158;
    case SPELL_BLINDNESS:
      return 159;
    case SPELL_PILLAR_SALT:
      return 165;
    case SPELL_EARTHQUAKE:
      return 166;
    case SPELL_CALL_LIGHTNING:
      return 167;
    case SPELL_SPONTANEOUS_COMBUST:
      return 168;
    case SPELL_BLEED:
      return 178;
    case SPELL_PARALYZE:
      return 179;
    case SPELL_BONE_BREAKER:
      return 180;
    case SPELL_WITHER_LIMB:
      return 181;
    case SPELL_SANCTUARY:
      return 185;
    case SPELL_CURE_PARALYSIS:
      return 186;
    case SPELL_SECOND_WIND:
      return 187;
    case SPELL_HEROES_FEAST:
      return 192;
    case SPELL_ASTRAL_WALK:
      return 193;
    case SPELL_PORTAL:
      return 194;
    case SPELL_HEAL_FULL:
      return 198;
    case SPELL_HEAL_CRITICAL_SPRAY:
      return 199;
    case SPELL_HEAL_SPRAY:
      return 200;
    case SPELL_HEAL_FULL_SPRAY:
      return 201;
    case SPELL_RESTORE_LIMB:
      return 202;
    case SPELL_KNIT_BONE:
      return 203;
    case SKILL_KICK:
      return 217;
    case SKILL_BASH:
      return 218;
    case SKILL_HEADBUTT:
      return 219;
    case SKILL_RESCUE:
      return 220;
    case SKILL_SMYTHE:
      return 221;
    case SKILL_DISARM:
      return 222;
    case SKILL_BERSERK:
      return 223;
    case SKILL_SWITCH_OPP:
      return 224;
    case SKILL_BODYSLAM:
      return 225;
    case SKILL_KNEESTRIKE:
      return 226;
    case SKILL_SHOVE:
      return 229;
    case SKILL_RETREAT:
      return 230;
    case SKILL_GRAPPLE:
      return 234;
    case SKILL_STOMP:
      return 235;
    case SKILL_DOORBASH:
      return 238;
    case SKILL_DEATHSTROKE:
      return 239;
    case SKILL_HIKING:
      return 266;
    case SKILL_KICK_RANGER:
      return 267;
    case SKILL_FORAGE:
      return 268;
    case SKILL_SEEKWATER:
      return 269;
    case SKILL_TRANSFORM_LIMB:
      return 270;
    case SKILL_BEAST_SOOTHER:
      return 271;
    case SKILL_TRACK:
      return 272;
    case SPELL_ROOT_CONTROL:
      return 273;
    case SKILL_BASH_RANGER:
      return 274;
    case SKILL_RESCUE_RANGER:
      return 276;
    case SKILL_BEFRIEND_BEAST:
      return 277;
    case SKILL_TRANSFIX:
      return 278;
    case SKILL_SKIN:
      return 279;
    case SKILL_DUAL_WIELD:
      return 280;
    case SPELL_LIVING_VINES:
      return 281;
    case SKILL_BEAST_SUMMON:
      return 282;
    case SKILL_BARKSKIN:
      return 283;
    case SKILL_SWITCH_RANGER:
      return 287;
    case SKILL_RETREAT_RANGER:
      return 288;
    case SPELL_STICKS_TO_SNAKES:
      return 292;
    case SPELL_STORMY_SKIES:
      return 293;
    case SPELL_TREE_WALK:
      return 294;
    case SKILL_BEAST_CHARM:
      return 298;
    case SPELL_SHAPESHIFT:
      return 299;
    case SKILL_CONCEALMENT:
      return 303;
    case SKILL_APPLY_HERBS:
      return 304;
    case SKILL_DIVINATION:
      return 308;
    case SKILL_ENCAMP:
      return 310;
    case SKILL_KICK_DEIKHAN:
      return 325;
    case SPELL_HEAL_LIGHT_DEIKHAN:
      return 326;
    case SKILL_CHIVALRY:
      return 327;
    case SPELL_ARMOR_DEIKHAN:
      return 328;
    case SPELL_BLESS_DEIKHAN:
      return 329;
    case SKILL_BASH_DEIKHAN:
      return 330;
    case SPELL_EXPEL_DEIKHAN:
      return 332;
    case SPELL_CLOT_DEIKHAN:
      return 333;
    case SPELL_RAIN_BRIMSTONE_DEIKHAN:
      return 334;
    case SPELL_STERILIZE_DEIKHAN:
      return 335;
    case SPELL_REMOVE_CURSE_DEIKHAN:
      return 336;
    case SPELL_CURSE_DEIKHAN:
      return 337;
    case SKILL_RESCUE_DEIKHAN:
      return 338;
    case SKILL_SMITE:
      return 339;
    case SPELL_INFECT_DEIKHAN:
      return 340;
    case SPELL_CURE_DISEASE_DEIKHAN:
      return 341;
    case SPELL_CREATE_FOOD_DEIKHAN:
      return 342;
    case SPELL_CREATE_WATER_DEIKHAN:
      return 343;
    case SPELL_HEAL_SERIOUS_DEIKHAN:
      return 344;
    case SPELL_CURE_POISON_DEIKHAN:
      return 345;
    case SKILL_CHARGE:
      return 346;
    case SPELL_HARM_SERIOUS_DEIKHAN:
      return 347;
    case SPELL_POISON_DEIKHAN:
      return 348;
    case SKILL_DISARM_DEIKHAN:
      return 349;
    case SPELL_HEAL_CRITICAL_DEIKHAN:
      return 350;
    case SPELL_HARM_CRITICAL_DEIKHAN:
      return 351;
    case SPELL_HARM_LIGHT_DEIKHAN:
      return 352;
    case SKILL_SWITCH_DEIKHAN:
      return 355;
    case SKILL_RETREAT_DEIKHAN:
      return 356;
    case SKILL_SHOVE_DEIKHAN:
      return 357;
    case SKILL_RIDE:
      return 359;  // was 575
    case SKILL_CALM_MOUNT:
      return 360;
    case SKILL_TRAIN_MOUNT:
      return 361;
    case SKILL_ADVANCED_RIDING:
      return 362;
    case SKILL_RIDE_DOMESTIC:
      return 363;
    case SKILL_RIDE_NONDOMESTIC:
      return 364;
    case SKILL_RIDE_WINGED:
      return 365;
    case SKILL_RIDE_EXOTIC:
      return 366;
    case SPELL_HEROES_FEAST_DEIKHAN:
      return 367;
    case SPELL_REFRESH_DEIKHAN:
      return 368;
    case SPELL_SYNOSTODWEOMER:
      return 369;
    case SPELL_SALVE_DEIKHAN:
      return 374;
    case SKILL_LAY_HANDS:
      return 375;
    case SPELL_HARM_DEIKHAN:
      return 379;
    case SPELL_NUMB_DEIKHAN:
      return 380;
    case SPELL_EARTHQUAKE_DEIKHAN:
      return 381;
    case SPELL_CALL_LIGHTNING_DEIKHAN:
      return 382;
    case SKILL_YOGINSA:
      return 397;
    case SKILL_CINTAI:
      return 398;
    case SKILL_OOMLAT:
      return 399;
    case SKILL_KICK_MONK:
      return 400;
    case SKILL_ADVANCED_KICKING:
      return 401;
    case SKILL_DISARM_MONK:
      return 402;
    case SKILL_GROUNDFIGHTING:
      return 403;
    case SKILL_CHOP:
      return 404;
    case SKILL_SPRINGLEAP:
      return 405;
    case SKILL_DUFALI:
      return 406;
    case SKILL_RETREAT_MONK:
      return 407;
    case SKILL_SNOFALTE:
      return 408;
    case SKILL_COUNTER_MOVE:
      return 409;
    case SKILL_SWITCH_MONK:
      return 410;
    case SKILL_JIRIN:
      return 411; 
    case SKILL_KUBO:
      return 412;
    case SKILL_CATFALL:
      return 413;
    case SKILL_WOHLIN:
      return 423;
    case SKILL_VOPLAT:
      return 424;
    case SKILL_BLINDFIGHTING:
      return 425;
    case SKILL_CHI:
      return 426;
    case SKILL_QUIV_PALM:
      return 435;
    case SKILL_CRIT_HIT:
      return 436;
    case SKILL_FEIGN_DEATH:
      return 448;
    case SKILL_BLUR:
      return 449;
    case SKILL_HURL:
      return 461;
    case SKILL_SHOULDER_THROW:
      return 462;
    case SKILL_BAREHAND_SPEC:
      return 474;
    case SKILL_SWINDLE:
      return 482;
    case SKILL_SNEAK:
      return 483;
    case SKILL_STABBING:
      return 484;
    case SKILL_RETREAT_THIEF:
      return 485;
    case SKILL_KICK_THIEF:
      return 486;
    case SKILL_PICK_LOCK:
      return 487;
    case SKILL_BACKSTAB:
      return 488;
    case SKILL_SEARCH:
      return 489;
    case SKILL_SPY:
      return 490;
    case SKILL_SWITCH_THIEF:
      return 491;
    case SKILL_STEAL:
      return 492;
    case SKILL_DETECT_TRAP:
      return 493;
    case SKILL_SUBTERFUGE:
      return 494;
    case SKILL_DISARM_TRAP:
      return 495;
    case SKILL_CUDGEL:
      return 496;
    case SKILL_HIDE:
      return 497;
    case SKILL_POISON_WEAPON:
      return 498;
    case SKILL_DISGUISE:
      return 499;
    case SKILL_DODGE_THIEF:
      return 500;
    case SKILL_GARROTTE:
      return 501;
    case SKILL_SET_TRAP_CONT:
      return 502;
    case SKILL_DUAL_WIELD_THIEF:
      return 503;
    case SKILL_DISARM_THIEF:
      return 504;
    case SKILL_COUNTER_STEAL:
      return 505;
    case SKILL_SET_TRAP_DOOR:
      return 506;
    case SKILL_SET_TRAP_MINE:
      return 507;
    case SKILL_SET_TRAP_GREN:
      return 508;
    case SPELL_CACAODEMON:
      return 556;
    case SPELL_CREATE_GOLEM:
      return 557;
    case SPELL_DANCING_BONES:
      return 558;
    case SPELL_CONTROL_UNDEAD:
      return 559;
    case SPELL_RESURRECTION:
      return 560;
    case SPELL_VOODOO:
      return 561;
    case SKILL_BREW:
      return 562;
    case SPELL_VAMPIRIC_TOUCH:
      return 563;
    case SPELL_LIFE_LEECH:
      return 564;
    case SKILL_TURN:
      return 565;
    case SKILL_SIGN:
      return 576;
    case SKILL_SWIM:
      return 577;
    case SKILL_CONS_UNDEAD:
      return 578;
    case SKILL_CONS_VEGGIE:
      return 579;
    case SKILL_CONS_DEMON:
      return 580;
    case SKILL_CONS_ANIMAL:
      return 581;
    case SKILL_CONS_REPTILE:
      return 582;
    case SKILL_CONS_PEOPLE:
      return 583;
    case SKILL_CONS_GIANT:
      return 584;
    case SKILL_CONS_OTHER:
      return 585;
    case SKILL_READ_MAGIC:
      return 586;
    case SKILL_BANDAGE:
      return 587;
    case SKILL_CLIMB:
      return 588;
    case SKILL_FAST_HEAL:
      return 589;
    case SKILL_EVALUATE:
      return 590;
    case SKILL_TACTICS:
      return 591;
    case SKILL_DISSECT:
      return 592;
    case SKILL_DEFENSE:
      return 593;
    case SKILL_OFFENSE:
      return 594;
    case SKILL_WHITTLE:
      return 663;
    case SKILL_WIZARDRY:
      return 596;
    case SKILL_MEDITATE:
      return 597;
    case SKILL_DEVOTION:
      return 601;
    case SKILL_PENANCE:
      return 602;
    case SKILL_SLASH_SPEC:
      return 611;
    case SKILL_BLUNT_SPEC:
      return 630;
    case SKILL_PIERCE_SPEC:
      return 640;
    case SKILL_RANGED_SPEC:
      return 648;
    case SKILL_FAST_LOAD:
      return 649;
    case SKILL_SLASH_PROF:
      return 654;
    case SKILL_BOW:
      return 655;
    case SKILL_PIERCE_PROF:
      return 656;
    case SKILL_BLUNT_PROF:
      return 657;
    case SKILL_SHARPEN:
      return 658;
    case SKILL_DULL:
      return 659;
    case SKILL_BAREHAND_PROF:
      return 660;
    case SKILL_ATTUNE:
      return 661;
    case SKILL_STAVECHARGE:
      return 662;
    case AFFECT_DISEASE:
      return 2500;
    case AFFECT_COMBAT:
      return 2501;
    case AFFECT_PET:
      return 2502;
    case AFFECT_TRANSFORMED_HANDS:
      return 2503;
    case AFFECT_TRANSFORMED_ARMS:
      return 2504;
    case AFFECT_TRANSFORMED_LEGS:
      return 2505;
    case AFFECT_TRANSFORMED_HEAD:
      return 2506;
    case AFFECT_TRANSFORMED_NECK:
      return 2507;
    case LAST_TRANSFORMED_LIMB:
      return 2508;
    case AFFECT_DUMMY:
      return 2509;
    case AFFECT_DRUNK:
      return 2510;
    case AFFECT_NEWBIE:
      return 2511;
    case AFFECT_SKILL_ATTEMPT:
      return 2512;
    case AFFECT_FREE_DEATHS:
      return 2513;
    case AFFECT_TEST_FIGHT_MOB:
      return 2514;
    case AFFECT_DRUG:
      return 2515;
    case AFFECT_ORPHAN_PET:
      return 2516;
    case AFFECT_PLAYERKILL:
      return 2517;
    case AFFECT_HORSEOWNED:
      return 2518;
    case AFFECT_THRALL:
      return 2519;
    case AFFECT_CHARM:
      return 2520;
    case SPELL_FIRE_BREATH:
    case SPELL_CHLORINE_BREATH:
    case SPELL_FROST_BREATH:
    case SPELL_ACID_BREATH:
    case SPELL_LIGHTNING_BREATH:
    case LAST_BREATH_WEAPON:
    case MAX_SKILL:
    case DAMAGE_NORMAL:
    case DAMAGE_CAVED_SKULL:
    case DAMAGE_BEHEADED:
    case DAMAGE_DISEMBOWLED_HR:
    case DAMAGE_DISEMBOWLED_VR:
    case DAMAGE_STOMACH_WOUND:
    case DAMAGE_HACKED:
    case DAMAGE_IMPALE:
    case DAMAGE_STARVATION:
    case DAMAGE_FALL:
    case DAMAGE_HEMORRAGE:
    case DAMAGE_DROWN:
    case DAMAGE_DRAIN:
    case DAMAGE_DISRUPTION:
    case DAMAGE_SUFFOCATION:
    case DAMAGE_RAMMED:
    case DAMAGE_WHIRLPOOL:
    case DAMAGE_ELECTRIC:
    case DAMAGE_ACID:
    case DAMAGE_GUST:
    case DAMAGE_EATTEN:
    case DAMAGE_KICK_HEAD:
    case DAMAGE_KICK_SOLAR:
    case DAMAGE_HEADBUTT_THROAT:
    case DAMAGE_HEADBUTT_BODY:
    case DAMAGE_HEADBUTT_CROTCH:
    case DAMAGE_HEADBUTT_LEG:
    case DAMAGE_HEADBUTT_FOOT:
    case DAMAGE_HEADBUTT_JAW:
    case DAMAGE_TRAP_SLEEP:
    case DAMAGE_TRAP_TELEPORT:
    case DAMAGE_TRAP_FIRE:
    case DAMAGE_TRAP_POISON:
    case DAMAGE_TRAP_ACID:
    case DAMAGE_TRAP_TNT:
    case DAMAGE_TRAP_ENERGY:
    case DAMAGE_TRAP_BLUNT:
    case DAMAGE_TRAP_PIERCE:
    case DAMAGE_TRAP_SLASH:
    case DAMAGE_TRAP_FROST:
    case DAMAGE_TRAP_DISEASE:
    case DAMAGE_ARROWS:
    case DAMAGE_FIRE:
    case DAMAGE_FROST:
    case DAMAGE_HEADBUTT_SKULL:
    case DAMAGE_COLLISION:
    case DAMAGE_KICK_SHIN:
    case DAMAGE_KNEESTRIKE_FOOT:
    case DAMAGE_KNEESTRIKE_SHIN:
    case DAMAGE_KNEESTRIKE_KNEE:
    case DAMAGE_KNEESTRIKE_THIGH:
    case DAMAGE_KNEESTRIKE_CROTCH:
    case DAMAGE_KNEESTRIKE_SOLAR:
    case DAMAGE_KNEESTRIKE_CHIN:
    case DAMAGE_KNEESTRIKE_FACE:
    case DAMAGE_KICK_SIDE:
    case TYPE_HIT:
    case TYPE_BLUDGEON:
    case TYPE_WHIP:
    case TYPE_CRUSH:
    case TYPE_SMASH:
    case TYPE_SMITE:
    case TYPE_PUMMEL:
    case TYPE_FLAIL:
    case TYPE_BEAT:
    case TYPE_THRASH:
    case TYPE_THUMP:
    case TYPE_WALLOP:
    case TYPE_BATTER:
    case TYPE_STRIKE:
    case TYPE_CLUB:
    case TYPE_POUND:
    case TYPE_PIERCE:
    case TYPE_BITE:
    case TYPE_STING:
    case TYPE_STAB:
    case TYPE_THRUST:
    case TYPE_SPEAR:
    case TYPE_BEAK:
    case TYPE_SLASH:
    case TYPE_CLAW:
    case TYPE_CLEAVE:
    case TYPE_SLICE:
    case TYPE_AIR:
    case TYPE_EARTH:
    case TYPE_FIRE:
    case TYPE_WATER:
    case TYPE_BEAR_CLAW:
    case TYPE_KICK:
    case TYPE_MAUL:
    case TYPE_MAX_HIT:
    case LAST_ODDBALL_AFFECT:
      break;
  }
  forceCrash("Bogus value (%d) in mapSpellNumToFile", stt);
  return -1;
}

spellNumT mapFileToSpellnum(int stt)
{
  switch (stt) {
    case -1:
      return TYPE_UNDEFINED;
    case 0:
      return SPELL_GUST;
    case 1:
      return SPELL_SLING_SHOT;
    case 2:
      return SPELL_GUSHER;
    case 3:
      return SPELL_HANDS_OF_FLAME;
    case 4:
      return SPELL_MYSTIC_DARTS;
    case 5:
      return SPELL_FLARE;
    case 6:
      return SPELL_SORCERERS_GLOBE;
    case 7:
      return SPELL_FAERIE_FIRE;
    case 8:
      return SPELL_ILLUMINATE;
    case 9:
      return SPELL_DETECT_MAGIC;
    case 10:
      return SPELL_STUNNING_ARROW;
    case 11:
      return SPELL_MATERIALIZE;
    case 12:
      return SPELL_PROTECTION_FROM_EARTH;
    case 13:
      return SPELL_PROTECTION_FROM_AIR;
    case 14:
      return SPELL_PROTECTION_FROM_FIRE;
    case 15:
      return SPELL_PROTECTION_FROM_WATER;
    case 16:
      return SPELL_PROTECTION_FROM_ELEMENTS;
    case 17:
      return SPELL_PEBBLE_SPRAY;
    case 18:
      return SPELL_ARCTIC_BLAST;
    case 19:
      return SPELL_COLOR_SPRAY;
    case 20:
      return SPELL_INFRAVISION;
    case 21:
      return SPELL_IDENTIFY;
    case 22:
      return SPELL_POWERSTONE;
    case 23:
      return SPELL_FAERIE_FOG;
    case 24:
      return SPELL_TELEPORT;
    case 25:
      return SPELL_SENSE_LIFE;
    case 26:
      return SPELL_CALM;
    case 27:
      return SPELL_ACCELERATE;
    case 28:
      return SPELL_DUST_STORM;
    case 29:
      return SPELL_LEVITATE;
    case 30:
      return SPELL_FEATHERY_DESCENT;
    case 31:
      return SPELL_STEALTH;
    case 32:
      return SPELL_GRANITE_FISTS;
    case 33:
      return SPELL_ICY_GRIP;
    case 34:
      return SPELL_GILLS_OF_FLESH;
    case 36:
      return SPELL_TELEPATHY;
    case 37:
      return SPELL_FEAR;
    case 38:
      return SPELL_SLUMBER;
    case 39:
      return SPELL_CONJURE_EARTH;
    case 40:
      return SPELL_CONJURE_AIR;
    case 41:
      return SPELL_CONJURE_FIRE;
    case 42:
      return SPELL_CONJURE_WATER;
    case 43:
      return SPELL_DISPEL_MAGIC;
    case 44:
      return SPELL_ENHANCE_WEAPON;
    case 45:
      return SPELL_GALVANIZE;
    case 46:
      return SPELL_DETECT_INVISIBLE;
    case 47:
      return SPELL_DISPEL_INVISIBLE;
    case 48:
      return SPELL_TORNADO;
    case 49:
      return SPELL_SAND_BLAST;
    case 50:
      return SPELL_ICE_STORM;
    case 51:
      return SPELL_BLAST_OF_FURY;
    case 52:
      return SPELL_ACID_BLAST;
    case 53:
      return SPELL_FIREBALL;
    case 54:
      return SPELL_FARLOOK;
    case 55:
      return SPELL_FALCON_WINGS;
    case 56:
      return SPELL_INVISIBILITY;
    case 57:
      return SPELL_ENSORCER;
    case 58:
      return SPELL_EYES_OF_FERTUMAN;
    case 59:
      return SPELL_COPY;
    case 60:
      return SPELL_HASTE;
    case 66:
      return SPELL_IMMOBILIZE;
    case 67:
      return SPELL_SUFFOCATE;
    case 68:
      return SPELL_FLY;
    case 69:
      return SPELL_ANTIGRAVITY;
    case 73:
      return SPELL_DIVINATION;
    case 74:
      return SPELL_SHATTER;
    case 75:
      return SKILL_SCRIBE;
    case 76:
      return SPELL_SPONTANEOUS_GENERATION;
    case 80:
      return SPELL_METEOR_SWARM;
    case 81:
      return SPELL_LAVA_STREAM;
    case 82:
      return SPELL_STONE_SKIN;
    case 83:
      return SPELL_TRAIL_SEEK;
    case 87:
      return SPELL_INFERNO;
    case 88:
      return SPELL_HELLFIRE;
    case 89:
      return SPELL_FLAMING_FLESH;
    case 90:
      return SPELL_FLAMING_SWORD;
    case 94:
      return SPELL_ENERGY_DRAIN;
    case 95:
      return SPELL_ATOMIZE;
    case 97:
      return SPELL_ANIMATE;
    case 98:
      return SPELL_BIND;
    case 99:
      return SPELL_ETHER_GATE;
    case 102:
      return SPELL_FUMBLE;
    case 103:
      return SPELL_TRUE_SIGHT;
    case 104:
      return SPELL_CLOUD_OF_CONCEALMENT;
    case 105:
      return SPELL_POLYMORPH;
    case 106:
      return SPELL_SILENCE;
    case 110:
      return SPELL_WATERY_GRAVE;
    case 111:
      return SPELL_TSUNAMI;
    case 112:
      return SPELL_BREATH_OF_SARAHAGE;
    case 113:
      return SPELL_PLASMA_MIRROR;
    case 114:
      return SPELL_GARMULS_TAIL;
    case 126:
      return SPELL_HEAL_LIGHT;
    case 127:
      return SPELL_HARM_LIGHT;
    case 128:
      return SPELL_CREATE_FOOD;
    case 129:
      return SPELL_CREATE_WATER;
    case 130:
      return SPELL_ARMOR;
    case 131:
      return SPELL_BLESS;
    case 132:
      return SPELL_CLOT;
    case 133:
      return SPELL_RAIN_BRIMSTONE;
    case 134:
      return SPELL_HEAL_SERIOUS;
    case 135:
      return SPELL_HARM_SERIOUS;
    case 136:
      return SPELL_STERILIZE;
    case 137:
      return SPELL_EXPEL;
    case 138:
      return SPELL_CURE_DISEASE;
    case 140:
      return SPELL_CURSE;
    case 141:
      return SPELL_REMOVE_CURSE;
    case 142:
      return SPELL_CURE_POISON;
    case 143:
      return SPELL_HEAL_CRITICAL;
    case 144:
      return SPELL_SALVE;
    case 145:
      return SPELL_POISON;
    case 146:
      return SPELL_HARM_CRITICAL;
    case 147:
      return SPELL_INFECT;
    case 148:
      return SPELL_REFRESH;
    case 149:
      return SPELL_NUMB;
    case 150:
      return SPELL_DISEASE;
    case 151:
      return SPELL_FLAMESTRIKE;
    case 152:
      return SPELL_PLAGUE_LOCUSTS;
    case 153:
      return SPELL_CURE_BLINDNESS;
    case 154:
      return SPELL_SUMMON;
    case 155:
      return SPELL_HEAL;
    case 156:
      return SPELL_PARALYZE_LIMB;
    case 157:
      return SPELL_WORD_OF_RECALL;
    case 158:
      return SPELL_HARM;
    case 159:
      return SPELL_BLINDNESS;
    case 165:
      return SPELL_PILLAR_SALT;
    case 166:
      return SPELL_EARTHQUAKE;
    case 167:
      return SPELL_CALL_LIGHTNING;
    case 168:
      return SPELL_SPONTANEOUS_COMBUST;
    case 178:
      return SPELL_BLEED;
    case 179:
      return SPELL_PARALYZE;
    case 180:
      return SPELL_BONE_BREAKER;
    case 181:
      return SPELL_WITHER_LIMB;
    case 185:
      return SPELL_SANCTUARY;
    case 186:
      return SPELL_CURE_PARALYSIS;
    case 187:
      return SPELL_SECOND_WIND;
    case 192:
      return SPELL_HEROES_FEAST;
    case 193:
      return SPELL_ASTRAL_WALK;
    case 194:
      return SPELL_PORTAL;
    case 198:
      return SPELL_HEAL_FULL;
    case 199:
      return SPELL_HEAL_CRITICAL_SPRAY;
    case 200:
      return SPELL_HEAL_SPRAY;
    case 201:
      return SPELL_HEAL_FULL_SPRAY;
    case 202:
      return SPELL_RESTORE_LIMB;
    case 203:
      return SPELL_KNIT_BONE;
    case 217:
      return SKILL_KICK;
    case 218:
      return SKILL_BASH;
    case 219:
      return SKILL_HEADBUTT;
    case 220:
      return SKILL_RESCUE;
    case 221:
      return SKILL_SMYTHE;
    case 222:
      return SKILL_DISARM;
    case 223:
      return SKILL_BERSERK;
    case 224:
      return SKILL_SWITCH_OPP;
    case 225:
      return SKILL_BODYSLAM;
    case 226:
      return SKILL_KNEESTRIKE;
    case 229:
      return SKILL_SHOVE;
    case 230:
      return SKILL_RETREAT;
    case 234:
      return SKILL_GRAPPLE;
    case 235:
      return SKILL_STOMP;
    case 238:
      return SKILL_DOORBASH;
    case 239:
      return SKILL_DEATHSTROKE;
    case 266:
      return SKILL_HIKING;
    case 267:
      return SKILL_KICK_RANGER;
    case 268:
      return SKILL_FORAGE;
    case 269:
      return SKILL_SEEKWATER;
    case 270:
      return SKILL_TRANSFORM_LIMB;
    case 271:
      return SKILL_BEAST_SOOTHER;
    case 272:
      return SKILL_TRACK;
    case 273:
      return SPELL_ROOT_CONTROL;
    case 274:
      return SKILL_BASH_RANGER;
    case 276:
      return SKILL_RESCUE_RANGER;
    case 277:
      return SKILL_BEFRIEND_BEAST;
    case 278:
      return SKILL_TRANSFIX;
    case 279:
      return SKILL_SKIN;
    case 280:
      return SKILL_DUAL_WIELD;
    case 281:
      return SPELL_LIVING_VINES;
    case 282:
      return SKILL_BEAST_SUMMON;
    case 283:
      return SKILL_BARKSKIN;
    case 287:
      return SKILL_SWITCH_RANGER;
    case 288:
      return SKILL_RETREAT_RANGER;
    case 292:
      return SPELL_STICKS_TO_SNAKES;
    case 293:
      return SPELL_STORMY_SKIES;
    case 294:
      return SPELL_TREE_WALK;
    case 298:
      return SKILL_BEAST_CHARM;
    case 299:
      return SPELL_SHAPESHIFT;
    case 303:
      return SKILL_CONCEALMENT;
    case 304:
      return SKILL_APPLY_HERBS;
    case 308:
      return SKILL_DIVINATION;
    case 310:
      return SKILL_ENCAMP;
    case 325:
      return SKILL_KICK_DEIKHAN;
    case 326:
      return SPELL_HEAL_LIGHT_DEIKHAN;
    case 327:
      return SKILL_CHIVALRY;
    case 328:
      return SPELL_ARMOR_DEIKHAN;
    case 329:
      return SPELL_BLESS_DEIKHAN;
    case 330:
      return SKILL_BASH_DEIKHAN;
    case 332:
      return SPELL_EXPEL_DEIKHAN;
    case 333:
      return SPELL_CLOT_DEIKHAN;
    case 334:
      return SPELL_RAIN_BRIMSTONE_DEIKHAN;
    case 335:
      return SPELL_STERILIZE_DEIKHAN;
    case 336:
      return SPELL_REMOVE_CURSE_DEIKHAN;
    case 337:
      return SPELL_CURSE_DEIKHAN;
    case 338:
      return SKILL_RESCUE_DEIKHAN;
    case 339:
      return SKILL_SMITE;
    case 340:
      return SPELL_INFECT_DEIKHAN;
    case 341:
      return SPELL_CURE_DISEASE_DEIKHAN;
    case 342:
      return SPELL_CREATE_FOOD_DEIKHAN;
    case 343:
      return SPELL_CREATE_WATER_DEIKHAN;
    case 344:
      return SPELL_HEAL_SERIOUS_DEIKHAN;
    case 345:
      return SPELL_CURE_POISON_DEIKHAN;
    case 346:
      return SKILL_CHARGE;
    case 347:
      return SPELL_HARM_SERIOUS_DEIKHAN;
    case 348:
      return SPELL_POISON_DEIKHAN;
    case 349:
      return SKILL_DISARM_DEIKHAN;
    case 350:
      return SPELL_HEAL_CRITICAL_DEIKHAN;
    case 351:
      return SPELL_HARM_CRITICAL_DEIKHAN;
    case 352:
      return SPELL_HARM_LIGHT_DEIKHAN;
    case 355:
      return SKILL_SWITCH_DEIKHAN;
    case 356:
      return SKILL_RETREAT_DEIKHAN;
    case 357:
      return SKILL_SHOVE_DEIKHAN;
    case 359:
      return SKILL_RIDE;
    case 360:
      return SKILL_CALM_MOUNT;
    case 361:
      return SKILL_TRAIN_MOUNT;
    case 362:
      return SKILL_ADVANCED_RIDING;
    case 363:
      return SKILL_RIDE_DOMESTIC;
    case 364:
      return SKILL_RIDE_NONDOMESTIC;
    case 365:
      return SKILL_RIDE_WINGED;
    case 366:
      return SKILL_RIDE_EXOTIC;
    case 367:
      return SPELL_HEROES_FEAST_DEIKHAN;
    case 368:
      return SPELL_REFRESH_DEIKHAN;
    case 369:
      return SPELL_SYNOSTODWEOMER;
    case 374:
      return SPELL_SALVE_DEIKHAN;
    case 375:
      return SKILL_LAY_HANDS;
    case 379:
      return SPELL_HARM_DEIKHAN;
    case 380:
      return SPELL_NUMB_DEIKHAN;
    case 381:
      return SPELL_EARTHQUAKE_DEIKHAN;
    case 382:
      return SPELL_CALL_LIGHTNING_DEIKHAN;
    case 397:
      return SKILL_YOGINSA;
    case 398:
      return SKILL_CINTAI;
    case 399:
      return SKILL_OOMLAT;
    case 400:
      return SKILL_KICK_MONK;
    case 401:
      return SKILL_ADVANCED_KICKING;
    case 402:
      return SKILL_DISARM_MONK;
    case 403:
      return SKILL_GROUNDFIGHTING;
    case 404:
      return SKILL_CHOP;
    case 405:
      return SKILL_SPRINGLEAP;
    case 406:
      return SKILL_DUFALI;
    case 407:
      return SKILL_RETREAT_MONK;
    case 408:
      return SKILL_SNOFALTE;
    case 409:
      return SKILL_COUNTER_MOVE;
    case 410:
      return SKILL_SWITCH_MONK;
    case 411:
      return SKILL_JIRIN;
    case 412:
      return SKILL_KUBO;
    case 413:
      return SKILL_CATFALL;
    case 423:
      return SKILL_WOHLIN;
    case 424:
      return SKILL_VOPLAT;
    case 425:
      return SKILL_BLINDFIGHTING;
    case 426:
      return SKILL_CHI;
    case 435:
      return SKILL_QUIV_PALM;
    case 436:
      return SKILL_CRIT_HIT;
    case 448:
      return SKILL_FEIGN_DEATH;
    case 449:
      return SKILL_BLUR;
    case 461:
      return SKILL_HURL;
    case 462:
      return SKILL_SHOULDER_THROW;
    case 474:
      return SKILL_BAREHAND_SPEC;
    case 482:
      return SKILL_SWINDLE;
    case 483:
      return SKILL_SNEAK;
    case 484:
      return SKILL_STABBING;
    case 485:
      return SKILL_RETREAT_THIEF;
    case 486:
      return SKILL_KICK_THIEF;
    case 487:
      return SKILL_PICK_LOCK;
    case 488:
      return SKILL_BACKSTAB;
    case 489:
      return SKILL_SEARCH;
    case 490:
      return SKILL_SPY;
    case 491:
      return SKILL_SWITCH_THIEF;
    case 492:
      return SKILL_STEAL;
    case 493:
      return SKILL_DETECT_TRAP;
    case 494:
      return SKILL_SUBTERFUGE;
    case 495:
      return SKILL_DISARM_TRAP;
    case 496:
      return SKILL_CUDGEL;
    case 497:
      return SKILL_HIDE;
    case 498:
      return SKILL_POISON_WEAPON;
    case 499:
      return SKILL_DISGUISE;
    case 500:
      return SKILL_DODGE_THIEF;
    case 501:
      return SKILL_GARROTTE;
    case 502:
      return SKILL_SET_TRAP_CONT;
    case 503:
      return SKILL_DUAL_WIELD_THIEF;
    case 504:
      return SKILL_DISARM_THIEF;
    case 505:
      return SKILL_COUNTER_STEAL;
    case 506:
      return SKILL_SET_TRAP_DOOR;
    case 507:
      return SKILL_SET_TRAP_MINE;
    case 508:
      return SKILL_SET_TRAP_GREN;
    case 556:
      return SPELL_CACAODEMON;
    case 557:
      return SPELL_CREATE_GOLEM;
    case 558:
      return SPELL_DANCING_BONES;
    case 559:
      return SPELL_CONTROL_UNDEAD;
    case 560:
      return SPELL_RESURRECTION;
    case 561:
      return SPELL_VOODOO;
    case 562:
      return SKILL_BREW;
    case 563:
      return SPELL_VAMPIRIC_TOUCH;
    case 564:
      return SPELL_LIFE_LEECH;
    case 565:
      return SKILL_TURN;
    case 576:
      return SKILL_SIGN;
    case 577:
      return SKILL_SWIM;
    case 578:
      return SKILL_CONS_UNDEAD;
    case 579:
      return SKILL_CONS_VEGGIE;
    case 580:
      return SKILL_CONS_DEMON;
    case 581:
      return SKILL_CONS_ANIMAL;
    case 582:
      return SKILL_CONS_REPTILE;
    case 583:
      return SKILL_CONS_PEOPLE;
    case 584:
      return SKILL_CONS_GIANT;
    case 585:
      return SKILL_CONS_OTHER;
    case 586:
      return SKILL_READ_MAGIC;
    case 587:
      return SKILL_BANDAGE;
    case 588:
      return SKILL_CLIMB;
    case 589:
      return SKILL_FAST_HEAL;
    case 590:
      return SKILL_EVALUATE;
    case 591:
      return SKILL_TACTICS;
    case 592:
      return SKILL_DISSECT;
    case 593:
      return SKILL_DEFENSE;
    case 594:
      return SKILL_OFFENSE;
    case 663:
      return SKILL_WHITTLE;
    case 596:
      return SKILL_WIZARDRY;
    case 597:
      return SKILL_MEDITATE;
    case 601:
      return SKILL_DEVOTION;
    case 602:
      return SKILL_PENANCE;
    case 611:
      return SKILL_SLASH_SPEC;
    case 630:
      return SKILL_BLUNT_SPEC;
    case 640:
      return SKILL_PIERCE_SPEC;
    case 648:
      return SKILL_RANGED_SPEC;
    case 649:
      return SKILL_FAST_LOAD;
    case 654:
      return SKILL_SLASH_PROF;
    case 655:
      return SKILL_BOW;
    case 656:
      return SKILL_PIERCE_PROF;
    case 657:
      return SKILL_BLUNT_PROF;
    case 658:
      return SKILL_SHARPEN;
    case 659:
      return SKILL_DULL;
    case 660:
      return SKILL_BAREHAND_PROF;
    case 661:
      return SKILL_ATTUNE;
    case 662:
      return SKILL_STAVECHARGE;
    case 2500:
      return AFFECT_DISEASE;
    case 2501:
      return AFFECT_COMBAT;
    case 2502:
      return AFFECT_PET;
    case 2503:
      return AFFECT_TRANSFORMED_HANDS;
    case 2504:
      return AFFECT_TRANSFORMED_ARMS;
    case 2505:
      return AFFECT_TRANSFORMED_LEGS;
    case 2506:
      return AFFECT_TRANSFORMED_HEAD;
    case 2507:
      return AFFECT_TRANSFORMED_NECK;
    case 2508:
      return LAST_TRANSFORMED_LIMB;
    case 2509:
      return AFFECT_DUMMY;
    case 2510:
      return AFFECT_DRUNK;
    case 2511:
      return AFFECT_NEWBIE;
    case 2512:
      return AFFECT_SKILL_ATTEMPT;
    case 2513:
      return AFFECT_FREE_DEATHS;
    case 2514:
      return AFFECT_TEST_FIGHT_MOB;
    case 2515:
      return AFFECT_DRUG;
    case 2516:
      return AFFECT_ORPHAN_PET;
    case 2517:
      return AFFECT_PLAYERKILL;
    case 2518:
      return AFFECT_HORSEOWNED;
    case 2519:
      return AFFECT_THRALL;
    case 2520:
      return AFFECT_CHARM;
    default:
      break;
  }
  forceCrash("Bad value (%d) in mapFileToSpellnum", stt);
  return MAX_SKILL;
}

spellNumT & operator++(spellNumT &c, int)
{
  return c = (c == MAX_SKILL) ? MIN_SPELL : spellNumT(c+1);
}

// this guy is primarily so we can convert TYPES_ to a 0-based array
spellNumT & operator-=(spellNumT &c, spellNumT num)
{
  return c = (c < num) ? spellNumT(0) : spellNumT(c-num);
}

bool applyTypeShouldBeSpellnum(applyTypeT att)
{
  return (att == APPLY_SPELL);
}
