//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
// $Log: sound.h,v $
// Revision 1.1  1999/09/12 17:24:04  sneezy
// Initial revision
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef __SOUND_H
#define __SOUND_H

extern const string & SOUND_TYPE_SOCIAL;
extern const string & SOUND_TYPE_NOISE;
extern const string & SOUND_TYPE_COMBAT;
extern const string & SOUND_TYPE_MAGIC;

extern const string & MUSIC_TYPE_COMBAT;
extern const string & MUSIC_TYPE_ZONE;
extern const string & MUSIC_TYPE_DEATH;

enum musicNumT {
     MUSIC_OFF,
     MUSIC_COMBAT_01,
     MUSIC_COMBAT_02,  // theme to conan
     MUSIC_COMBAT_03,
     MAX_MUSIC_NUM
};
const musicNumT MIN_MUSIC_NUM = musicNumT(0);

enum soundNumT {
     SOUND_OFF,
     SOUND_SNORE,
     SOUND_HORSE_1,
     SOUND_HORSE_2,
     SOUND_CATHISS,
     SOUND_DOGBARK_1,
     SOUND_DOGBARK_2,
     SOUND_YAWN_1,
     SOUND_YAWN_2,
     SOUND_YAWN_3,
     SOUND_YAWN_4,
     SOUND_THUNDER_1,
     SOUND_THUNDER_2,
     SOUND_THUNDER_3,
     SOUND_THUNDER_4,
     SOUND_THUNDER_5,
     SOUND_GIGGLE,
     SOUND_LAUGH,
     SOUND_BURP,
     SOUND_CLAP,
     SOUND_FART,
     SOUND_SNEEZE,
     SOUND_CACKLE,
     SOUND_DM_LAUGH,
     SOUND_SCREAM,
     SOUND_HIT_BLUNT_01,
     SOUND_HIT_BLUNT_02,
     SOUND_HIT_BLUNT_03,
     SOUND_HIT_BLUNT_04,
     SOUND_HIT_BLUNT_05,
     SOUND_HIT_BLUNT_06,
     SOUND_HIT_BLUNT_07,
     SOUND_HIT_BLUNT_08,
     SOUND_HIT_BLUNT_09,
     SOUND_HIT_BLUNT_10,
     SOUND_HIT_BLUNT_11,
     SOUND_HIT_BLUNT_12,
     SOUND_HIT_BLUNT_13,
     SOUND_HIT_BLUNT_14,
     SOUND_HIT_BLUNT_15,
     SOUND_HIT_BLUNT_16,
     SOUND_HIT_BLUNT_17,
     SOUND_HIT_BLUNT_18,
     SOUND_HIT_SLASH_01,
     SOUND_HIT_SLASH_02,
     SOUND_HIT_SLASH_03,
     SOUND_HIT_SLASH_04,
     SOUND_HIT_PIERCE_01,
     SOUND_HIT_WHIP_01,
     SOUND_HIT_WHIP_02,
     SOUND_MISS_01,
     SOUND_MISS_02,
     SOUND_MISS_03,
     SOUND_MISS_04,
     SOUND_MISS_05,
     SOUND_MISS_06,
     SOUND_MISS_07,
     SOUND_MISS_08,
     SOUND_MISS_09,
     SOUND_MISS_10,
     SOUND_PATHETIC_01,
     SOUND_PATHETIC_02,
     SOUND_PATHETIC_03,
     SOUND_PATHETIC_04,
     SOUND_DEATH_CRY_01,
     SOUND_DEATH_CRY_02,
     SOUND_DEATH_CRY_03,
     SOUND_DEATH_CRY_04,
     SOUND_DEATH_CRY_05,
     SOUND_DEATH_CRY_06,
     SOUND_DEATH_CRY_07,
     SOUND_DEATH_CRY_08,
     SOUND_DEATH_CRY_09,
     SOUND_DEATH_CRY_10,
     SOUND_DEATH_CRY_11,
     SOUND_DONT_KILL_ME,
     SOUND_BACKSTAB_01,
     SOUND_BACKSTAB_02,
     SOUND_DOOROPEN_01,
     SOUND_DOOROPEN_02,
     SOUND_DOORCLOSE_01,
     SOUND_DOORCLOSE_02,
     SOUND_DOORCLOSE_03,
     SOUND_DOORCLOSE_04,
     SOUND_BANZAI_01,
     SOUND_BANZAI_02,
     SOUND_BANZAI_03,
     SOUND_BANZAI_04,
     SOUND_WATER_DROP,
     SOUND_WATER_GURGLE,
     SOUND_WATER_WAVE,
     SOUND_BOING,
     SOUND_COMEBACK,
     SOUND_NONE_PASS,
     SOUND_FLEE_01,
     SOUND_FLEE_02,
     SOUND_FLEE_03,
     SOUND_CRIT_01,
     SOUND_CRIT_02,
     SOUND_CRIT_03,
     SOUND_CRIT_04,
     SOUND_CRIT_05,
     SOUND_CRIT_06,
     SOUND_CRIT_07,
     SOUND_CRIT_08,
     SOUND_CRIT_09,
     SOUND_CRIT_10,
     SOUND_CRIT_11,
     SOUND_CRIT_12,
     SOUND_CRIT_13,
     SOUND_CRIT_14,
     SOUND_CRIT_15,
     SOUND_CRIT_16,
     SOUND_CRIT_17,
     SOUND_CRIT_18,
     SOUND_CRIT_19,
     SOUND_CRIT_20,
     SOUND_CRIT_21,
     SOUND_CRIT_22,
     SOUND_CRIT_23,
     SOUND_CRIT_24,
     SOUND_CRIT_25,
     SOUND_CRIT_26,
     SOUND_CRIT_27,
     SOUND_CRIT_28,
     SOUND_CRIT_29,
     SOUND_CRIT_30,
     SOUND_CRIT_31,
     SOUND_CRIT_32,
     SOUND_CRIT_33,
     SOUND_CRIT_34,
     SOUND_CRIT_35,
     SOUND_CRIT_36,
     SOUND_CRIT_37,
     SOUND_CRIT_38,
     SOUND_CRIT_39,
     SOUND_CRIT_40,
     SOUND_CRIT_41,
     SOUND_CRIT_42,
     SOUND_CRIT_43,
     SOUND_SHOCK,
     SOUND_YAHOO,
     SOUND_CHEWED_UP,
     SOUND_DISAGREE,
     SOUND_FOODPOISON,
     SOUND_BRING_DEAD,
     SOUND_EGOBLAST_1,
     SOUND_EGOBLAST_2,
     SOUND_CAST_FAIL_01,
     SOUND_CAST_FAIL_02,
     SOUND_RAIN_START,
     SOUND_SPELL_ACCELERATE,
     SOUND_SPELL_ANIMATE_DEAD,
     SOUND_SPELL_ARMOR,
     SOUND_SPELL_ASTRAL_WALK,
     SOUND_SPELL_BLESS,
     SOUND_SPELL_CALL_LIGHTNING,
     SOUND_SPELL_GRANITE_FIST,
     SOUND_SPELL_HASTE,
     SOUND_SPELL_HELLFIRE,
     SOUND_SPELL_METEOR_SWARM,
     SOUND_SPELL_MYSTIC_DART,
     SOUND_SPELL_PILLAR_OF_SALT,
     SOUND_SPELL_PORTAL,
     SOUND_SPELL_RAIN_BRIMSTONE,
     SOUND_SPELL_SANCTUARY,
     SOUND_SPELL_SPONTANEOUS_COMBUST,
     SOUND_SPELL_TELEPORT,
     SOUND_SPELL_WORD_OF_RECALL,
     SOUND_SPELL_SORCERERS_GLOBE,
     SOUND_SPELL_SLING_SHOT,
     SOUND_SPELL_STUNNING_ARROW,
     MAX_SOUND_NUM
};
const soundNumT MIN_SOUND_NUM = soundNumT(0);

extern soundNumT pickRandSound(soundNumT, soundNumT);
extern musicNumT pickRandMusic(musicNumT, musicNumT);

#endif
