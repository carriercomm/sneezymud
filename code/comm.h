//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#ifndef __COMM_H
#define __COMM_H

enum actToParmT {
     TO_ROOM,
     TO_VICT,
     TO_NOTVICT,
     TO_CHAR
};

void sendToAll(char *messg);
void sendToExcept(char *messg, TBeing *ch);
void sendToRoom(colorTypeT, const char *messg, int room);
void sendToRoom(const char *messg, int room);
void sendToRoomExcept(char *messg, int room, TBeing *ch);
void sendToRoomExceptTwo(char *, int, TBeing *, TBeing *);
void perform_to_all(char *messg, TBeing *ch);
void perform_complex(TBeing *, TBeing *, TObj *, TObj *, char *, byte, int);
void sendrpf(int, colorTypeT, TRoom *, const char *,...);
void sendrpf(int, TRoom *, const char *,...);
void sendrpf(colorTypeT, TRoom *, const char *,...);
void sendrpf(TRoom *, const char *,...);
void sendToOutdoor(colorTypeT, const char *, const char *);
void colorAct(colorTypeT, const char *, bool, const TThing *, const TThing *, const TThing *, actToParmT, const char * color = NULL, int = 0);
void act(const char *, bool, const TThing *, const TThing *, const TThing *, actToParmT, const char * color = NULL, int = 0);
void nukeMobs(int);
bool isEmpty(int);

const int PULSE_COMMAND     =0;
const int PULSE_TICK        =1;

// These things should be changed if machine changes to make ticks
// as even as possible. They have been tweaked to make the current Linux
// machine work correctly. They were 2* normal mode before tweaking
// desired goal is combat round <= 3 secs
// I cut these by 25% for v5.1 - Bat
// - To test real time to pulse conversion, look at bottom of gameLoop
// - 9600 pulses take 1237 secs : Bat 05/05/99  (low load)
// - 7200 pulses take 2744 secs : Bat 01/12/00  (avg load, 1 day uptime)

// July 2001 - new machine and stuff, pulse conversion stuff gives us
// between 9.2 and 9.6 pulses per second
// rounding up to ten, from our previous value of 4 (which we had balacned
// to for playtime and stuff) gives us 5/2 conversion ratio... we need to
// eat up that difference some how

// these are all factors of 3, so going 2/3 will be easy... 2/3 * 5/2 is...
// 5/3, duh i shouldn't have needed a calculator for that
// so we still have a factor of 5/3 to account for
// see statistics.cc for the remaining compensation in exp/damage rates


const int ONE_SECOND        =10;
const int PULSE_MOBACT      =(int)((float)ONE_SECOND * 1.2);
const int PULSE_TELEPORT    =(int)((float)ONE_SECOND * 1.2);
const int PULSE_COMBAT      =(int)((float)ONE_SECOND * 1.2);
const int PULSE_DROWNING    =(int)((float)ONE_SECOND * 3.6);
const int PULSE_SPEC_PROCS  =(int)((float)ONE_SECOND * 3.6);
const int PULSE_NOISES      =(int)((float)ONE_SECOND * 4.8);

// Altering PULSE_UPDATES will speed up ticks, but also causes "mud time"
// to totally recalculate (making it shorter will age people).
// use caution!
const int PULSE_UPDATE      =ONE_SECOND * 36;
const int PULSE_MUDHOUR     =PULSE_UPDATE * 4;

const int SECS_PER_UPDATE  = PULSE_UPDATE/ONE_SECOND;
const int SECS_PER_MUDHOUR  = PULSE_MUDHOUR/ONE_SECOND;

// currently, there are 4 partial calls per hour
const int SECS_PER_MUD_DAY   = (24*SECS_PER_MUDHOUR);
const int SECS_PER_MUD_MONTH = (28*SECS_PER_MUD_DAY);
const int SECS_PER_MUD_YEAR  = (12*SECS_PER_MUD_MONTH);

// updateAffects() is called on combat counter (socket.cc)
// this will tell us how many combat-calls needed to simulate a "tick"
// The reason for this is that we sometimes want to say "it takes a mud-hour"
// and the mud-hour must be represented as a number of combat-pulse calls
const int UPDATES_PER_TICK = (PULSE_UPDATE/PULSE_COMBAT);
const int UPDATES_PER_MUDHOUR = (PULSE_MUDHOUR/PULSE_COMBAT);

extern const char * const prompt_mesg[];
extern void signalSetup(void);
extern int noSpecials;

#endif





