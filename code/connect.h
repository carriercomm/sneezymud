#ifndef __CONNECT_H
#define __CONNECT_H

#include "obj_drug.h"

const unsigned short int PROMPT_HIT               = (1<<0);
const unsigned short int PROMPT_MANA              = (1<<1);
const unsigned short int PROMPT_MOVE              = (1<<2);
const unsigned short int PROMPT_GOLD              = (1<<3);
const unsigned short int PROMPT_EXP               = (1<<4);
const unsigned short int PROMPT_NAME              = (1<<5);
const unsigned short int PROMPT_OPPONENT          = (1<<6);
const unsigned short int PROMPT_CONDITION         = (1<<7);
const unsigned short int PROMPT_COND_LDR          = (1<<8);
const unsigned short int PROMPT_ROOM              = (1<<9);
const unsigned short int PROMPT_COLOR             = (1<<10);
const unsigned short int PROMPT_TANK              = (1<<11);
const unsigned short int PROMPT_TANK_OTHER        = (1<<12);
const unsigned short int PROMPT_BUILDER_ASSISTANT = (1<<13);
const unsigned short int PROMPT_EXPTONEXT_LEVEL   = (1<<14);
const unsigned short int PROMPT_VTANSI_BAR        = (1<<15);

enum termTypeT {
     TERM_NONE,  //         = 0;
     TERM_VT100,  //        = 1;
     TERM_ANSI   //         = 2;
};

enum connectStateT {
       CON_PLYNG,
       CON_NME,
       CON_NMECNF,
       CON_PWDNRM,
       CON_PWDCNF,
       CON_QSEX,
       CON_RMOTD,
       CON_QCLASS,
       CON_PWDNCNF,
       CON_WIZLOCK,
       CON_QRACE,
       CON_DELETE,
       CON_STAT_COMBAT,
       CON_STAT_COMBAT2,
       CON_QHANDS,
       CON_DISCON,
       CON_NEWACT,
       CON_ACTPWD,
       CON_NEWLOG,
       CON_NEWACTPWD,
       CON_EMAIL,
       CON_TERM,
       CON_CONN,
       CON_NEWPWD,
       CON_OLDPWD,
       CON_RETPWD,
       CON_DELCHAR,
       CON_ACTDELCNF,
       CON_EDITTING,
       CON_DISCLAIMER,
       CON_TIME,
       CON_CHARDELCNF,
       CON_DISCLAIMER2,
       CON_DISCLAIMER3,
       CON_WIZLOCKNEW,
       CON_STAT_LEARN,
       CON_STAT_UTIL,
       CON_CREATE_DONE,
       CON_STATS_START,
       CON_ENTER_DONE,
       CON_STATS_RULES,
       CON_STATS_RULES2,
       CON_LISTSERV,
       CON_HOME_HUMAN,
       CON_HOME_ELF,
       CON_HOME_DWARF,
       CON_HOME_GNOME,
       CON_HOME_OGRE,
       CON_HOME_HOBBIT,
// if adding more here, update connected_types array as well
       MAX_CON_STATUS,
// these are intentionally higher than MAX_CON
       CON_REDITING,
       CON_OEDITING,
       CON_MEDITING,
       CON_HELP,
       CON_WRITING,
       CON_SEDITING
};

class TAccount;
class TPerson;
class TSocket;

// This stuff has to be here because we include connect.h
// in being.h, so it tries to use these before they are declared.

class commText
{
  private:
    char *text;
    commText *next;
  public:
    char * getText() {
      return text;
    }
    void setText(char * n) {
      text = n;
    }
    commText * getNext() {
      return next;
    }
    void setNext( commText * n) {
      next = n;
    }
   
    commText();
    commText(const commText &a);
    commText & operator=(const commText &a);
    ~commText();
};

class textQ
{
  private:
    commText *begin;
    commText *end;

  public:
    commText * getBegin() {
      return begin;
    }
    void setBegin(commText *n) {
      begin = n;
    }
    commText * getEnd() {
      return end;
    }
    void setEnd(commText *n) {
      end = n;
    }

  private:
    textQ() {} // prevent use
  public:
    textQ(bool);
    textQ(const textQ &a);
    textQ & operator=(const textQ &a);
    ~textQ();

    bool takeFromQ(char *dest);
    void putInQ(const char *txt);
};

class editStuff
{
  public:
    int x, y;        // Current x andy position on the screen for cursor
    int bottom, end; // Bottom of text, and end of current line
    char **lines;    // Keeps up with text typed in

    editStuff();
    editStuff(const editStuff &a);
    ~editStuff();
};
    
class careerData
{
  public:
    unsigned int kills;            // keep up with kills I've made
    unsigned int group_kills;            // keep up with kills I've made
    unsigned int deaths;           // total deaths I've suffered
    double exp;                // total xp ever gained
    unsigned int crit_hits;
    unsigned int crit_hits_suff;
    unsigned int crit_misses;
    unsigned int crit_kills;
    unsigned int crit_kills_suff;
    unsigned int crit_beheads;
    unsigned int crit_beheads_suff;
    unsigned int crit_sev_limbs;
    unsigned int crit_sev_limbs_suff;
    unsigned int crit_cranial_pierce;
    unsigned int crit_cranial_pierce_suff;
    unsigned int crit_broken_bones;
    unsigned int crit_broken_bones_suff;
    unsigned int crit_crushed_skull;
    unsigned int crit_crushed_skull_suff;
    unsigned int crit_crushed_nerve;
    unsigned int crit_crushed_nerve_suff;
    unsigned int crit_voice;
    unsigned int crit_voice_suff;
    unsigned int crit_eye_pop;
    unsigned int crit_eye_pop_suff;
    unsigned int crit_lung_punct;
    unsigned int crit_lung_punct_suff;
    unsigned int crit_impale;
    unsigned int crit_impale_suff;
    unsigned int arena_victs;
    unsigned int arena_loss;
    unsigned int hits[MAX_ATTACK_MODE_TYPE];
    unsigned int swings[MAX_ATTACK_MODE_TYPE];
    unsigned int dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int dam_received[MAX_ATTACK_MODE_TYPE];
    unsigned int crit_cleave_two;
    unsigned int crit_cleave_two_suff;
    unsigned int crit_disembowel;
    unsigned int crit_disembowel_suff;
    unsigned int crit_eviscerate;
    unsigned int crit_eviscerate_suff;
    unsigned int crit_kidney;
    unsigned int crit_kidney_suff;
    unsigned int crit_genitalia;
    unsigned int crit_genitalia_suff;
    unsigned int skill_success_attempts;
    unsigned int skill_success_pass;
    unsigned int spell_success_attempts;
    unsigned int spell_success_pass;
    unsigned int prayer_success_attempts;
    unsigned int prayer_success_pass;
    unsigned int pets_bought;
    unsigned int pet_levels_bought;
    unsigned int stuck_in_foot;
    unsigned int ounces_of_blood;
    time_t hit_level40;
    time_t hit_level50;

    careerData();
    ~careerData();

    void setToZero() {
      kills = group_kills = 0;
      exp = 0.0;
      deaths = 0;
      crit_kills = crit_misses = crit_hits = 0;
      crit_kills_suff = crit_hits_suff = 0;
      crit_beheads = crit_sev_limbs = crit_cranial_pierce = 0;
      crit_beheads_suff = crit_sev_limbs_suff = crit_cranial_pierce_suff = 0;
      crit_broken_bones = crit_crushed_skull = 0;
      crit_broken_bones_suff = crit_crushed_skull_suff = 0;
      crit_cleave_two = crit_cleave_two_suff = 0;
      crit_disembowel = crit_disembowel_suff = 0;
      crit_crushed_nerve = crit_crushed_nerve_suff = 0;
      crit_voice = crit_voice_suff = 0;
      crit_eye_pop = crit_eye_pop_suff = 0;
      crit_lung_punct = crit_lung_punct_suff = 0;
      crit_impale = crit_impale_suff = 0;
      crit_eviscerate = crit_eviscerate_suff = 0;
      crit_kidney = crit_kidney_suff = 0;
      crit_genitalia = crit_genitalia_suff = 0;
      arena_victs = arena_loss = 0;
      skill_success_attempts = 0;
      skill_success_pass = 0;
      spell_success_attempts = 0;
      spell_success_pass = 0;
      prayer_success_attempts = 0;
      prayer_success_pass = 0;

      hit_level40 = hit_level50 = 0;

      int i;
      for (i= 0; i < MAX_ATTACK_MODE_TYPE; i++) {
        hits[i] = 0;
        swings[i] = 0;
        dam_done[i] = 0;
        dam_received[i] = 0;
      }
      pets_bought = 0;
      pet_levels_bought = 0;
      stuck_in_foot = 0;
      ounces_of_blood = 0;
      
    }
};

class sessionData
{
  public:
    time_t connect;
    int kills;
    int groupKills;
    double xp;
    double perc;
    byte group_share;
    bool amGroupTank;
    unsigned int hits[MAX_ATTACK_MODE_TYPE];
    unsigned int swings[MAX_ATTACK_MODE_TYPE];
    unsigned int rounds[MAX_ATTACK_MODE_TYPE];
    unsigned int swings_received[MAX_ATTACK_MODE_TYPE];
    unsigned int hits_received[MAX_ATTACK_MODE_TYPE];
    unsigned int rounds_received[MAX_ATTACK_MODE_TYPE];
    unsigned int level_attacked[MAX_ATTACK_MODE_TYPE];
    unsigned int potential_dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int potential_dam_received[MAX_ATTACK_MODE_TYPE];
    unsigned int combat_dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int combat_dam_received[MAX_ATTACK_MODE_TYPE];
    unsigned int skill_dam_done[MAX_ATTACK_MODE_TYPE];
    unsigned int skill_dam_received[MAX_ATTACK_MODE_TYPE];
    int mod_done[MAX_ATTACK_MODE_TYPE];
    int mod_received[MAX_ATTACK_MODE_TYPE];
    unsigned int skill_success_attempts;
    unsigned int skill_success_pass;
    unsigned int spell_success_attempts;
    unsigned int spell_success_pass;
    unsigned int prayer_success_attempts;
    unsigned int prayer_success_pass;
 
    sessionData();
    ~sessionData();

    void setToZero() {
      connect = time(0);
      kills = 0;
      groupKills = 0;
      xp = 0.0;
      perc = 0.0;
      group_share = 1;
      amGroupTank = false;
      skill_success_attempts = 0;
      skill_success_pass = 0;
      spell_success_attempts = 0;
      spell_success_pass = 0;
      prayer_success_attempts = 0;
      prayer_success_pass = 0;

      attack_mode_t i;
      for (i= ATTACK_NORMAL; i < MAX_ATTACK_MODE_TYPE; i++) {
        hits[i] = 0;
        swings[i] = 0;
        rounds[i] = 0;
        combat_dam_done[i] = 0;
        combat_dam_received[i] = 0;
        potential_dam_done[i] = 0;
        potential_dam_received[i] = 0;
        skill_dam_done[i] = 0;
        skill_dam_received[i] = 0;
        swings_received[i] = 0;
        hits_received[i] = 0;
        rounds_received[i] = 0;
        level_attacked[i] = 0;
        mod_done[i] = 0;
        mod_received[i] = 0;
      }
    }
};

class promptData
{
  public:
    unsigned short int type;
    char hpColor[20];
    char manaColor[20];
    char moveColor[20];
    char moneyColor[20];
    char expColor[20];
    char oppColor[20];
    char roomColor[20];
    char tankColor[20];
    char *prompt;
//    double xptnl[MAX_CLASSES];  getExpClassLevel is same for all classes
    double xptnl;

    promptData();
    ~promptData();
};

// Descriptor class
class Descriptor
{
  public:
    TSocket *socket;
    editStuff edit;
    char host[100];                // hostname                   
    char pwd[12];                 // password                   
    connectStateT connected;                // mode of 'connectedness'    
    int wait;                     // wait for how many loops    
    char *showstr_head;           // for paging through texts  
    byte tot_pages;               // for tracking paged info
    byte cur_page;                //       -
    char **str;                   // for the modify-str system
    int max_str;
    int prompt_mode;              // control of prompt-printing 
    char m_raw[4096];               // buffer for raw input    
    textQ output;                 // q of strings to send    
    textQ input;                  // q of unprocessed input  
    sessionData session;          // data for this session
    careerData career;            // data for career
    drugData drugs[MAX_DRUG];
    unsigned int autobits;
    unsigned int best_rent_credit;
    char last_teller[128];
    TBeing *character;            // linked to char (might be a poly)
    TAccount *account;            // linked to account
    TPerson *original;            // original char (always a person)
    snoopData snoop;              // to snoop people           
    Descriptor *next;             // link to next descriptor    
    char *pagedfile;              // what file is getting paged 
    char name[20];                // dummy field (idea, bug, mail use it)
    TObj *obj;                    // for object editor
    TMonster *mob;                // for monster editor 
    aliasData alias[16];          // aliases for players
    char history[10][MAX_INPUT_LENGTH];
    betData bet;
    cBetData bet_opt;
    byte screen_size;
    byte point_roll;
    time_t talkCount;
    bool m_bIsClient;
    sh_int bad_login;              // login catches for hackers 
    int severity;
    int office;
    int blockastart;
    int blockaend;
    int blockbstart;
    int blockbend;
    lastChangeData last;
    ubyte deckSize;
    char delname[20];
    promptData prompt_d;
    unsigned long plr_act;
    unsigned int plr_color;
    colorSubT plr_colorSub;
    unsigned int plr_colorOff;

    // Functions
  private:
    Descriptor();  // prevent default constructor from being used
  public:
    Descriptor(TSocket *);
    Descriptor(const Descriptor &);
    Descriptor & operator=(const Descriptor &a);
    ~Descriptor();

    int outputProcessing();
    int inputProcessing();
    void flush();
    void flushInput();
    int sendLogin(const char *);
    bool checkForMultiplay();
    bool checkForAccount(char *, bool silent = FALSE);
    bool checkForCharacter(char *);
    int doAccountStuff(char *);
    int clientCreateAccount(char *);
    int clientCreateChar(char *);
    bool isEditing();
    void Edit(char *);
    void deleteAccount();
    void menuWho();
    void saveAccount();
    int doAccountMenu(const char *);
    void add_to_history_list(const char *);
    int nanny(const char *);
    void sendMotd(int);
    void go_back_menu(connectStateT);
    void EchoOn();
    void EchoOff();
    void sendHomeList();
    void sendStartStatList();
    void sendDoneScreen();
    const char *getStatDescription(int);
    void sendStatList(int, int);
    void sendStatRules(int);
    void sendRaceList();
    void sendClassList(int);
    bool start_page_file(const char *, const char *);
    bool canChooseClass(int, bool multi = FALSE, bool triple = FALSE);
    int client_nanny(char *);
    void writeToQ(const char *arg);
    void clientf(const char *msg,...);
    bool page_file(const char *);
    void page_string(const char *, showNowT = SHOWNOW_NO, allowReplaceT allow = ALLOWREP_NO);
    void show_string(const char *, showNowT, allowReplaceT);
    const string badClassMessage(int Class, bool multi = FALSE, bool triple = FALSE);
#if 0
    char *badRaceMessage(int race);
#endif
    void send_client_motd();
    void send_client_inventory();
    void send_client_room_people();
    void send_client_room_objects();
    void send_client_prompt(int, int);
    void send_client_exits();
    int read_client(char *);
    void string_add(char *);
    void fdSocketClose(int);
    void saveAll();
    void worldSend(const char *, TBeing *);
    void sendShout(TBeing *, const char *);
    void updateScreenAnsi(unsigned int update);
    void updateScreenVt100(unsigned int update);
    int move(int, int);
    void send_bug(const char *, const char *);
    void add_comment(const char *, const char *);
    void cleanUpStr();
    void beep() {
      writeToQ("");
    }

    const string doColorSub() const;
    const string ansi_color_bold(const char *s) const;
    const string ansi_color_bold(const char *s, unsigned int) const;
    const string ansi_color(const char *s) const;
    const string ansi_color(const char *s, unsigned int) const;
    bool hasColor() const;
    bool hasColorVt() const;
    const char *highlight(char *s) const;
    const char *whiteBold() const;
    const char *blackBold() const;
    const char *redBold() const;
    const char *underBold() const;
    const char *blueBold() const;
    const char *cyanBold() const;
    const char *greenBold() const;
    const char *orangeBold() const;
    const char *purpleBold() const;
    const char *white() const;
    const char *black() const;
    const char *red() const;
    const char *under() const;
    const char *bold() const;
    const char *norm() const;
    const char *blue() const;
    const char *cyan() const;
    const char *green() const;
    const char *orange() const;
    const char *purple() const;
    const char *invert() const;
    const char *flash() const;
    const char *BlackOnBlack() const;
    const char *BlackOnWhite() const;
    const char *WhiteOnBlue() const;
    const char *WhiteOnCyan() const;
    const char *WhiteOnGreen() const;
    const char *WhiteOnOrange() const;
    const char *WhiteOnPurple() const;
    const char *WhiteOnRed() const;
};

#endif
