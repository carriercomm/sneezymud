//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////


#ifndef __EXTERN_H
#define __EXTERN_H

#ifndef __BEING_H
#error Please include BEING.H prior to inclusion of EXTERN.H
#endif

extern "C" {
#if 0
// disabled functions
extern int max_stat(race_t, statTypeT);
extern int min_stat(race_t, statTypeT);
#endif
extern wearSlotT slot_from_bit(int);
extern void cleanCharBuf(sstring &);
extern int split_string(const sstring &str, const sstring &sep, vector<sstring> &argv);
extern void sendrp_exceptf(TRoom *, TBeing *, const char *,...);
extern const sstring how_good(int);
extern sstring sprintbit(unsigned long, const sstring []);
extern bool is_exact_spellname(const sstring &, const sstring &);
extern bool is_exact_name(const sstring &, const sstring &);
extern int get_number(char **);
extern void printLimitedInRent(void);
extern void updateRentFiles(void);
extern bool noteLimitedItems(FILE *, const sstring &, unsigned char, bool);
extern const int thaco[128];
extern const byte ac_percent_pos[MAX_WEAR];
extern int AddToCharHeap(TBeing *heap[50], int *, int total[50], TBeing *);
extern int MobCountInRoom(const TThing *);
extern bool WizLock;
extern bool WizGoto;
extern bool WizShout;
extern bool WizInvis;
extern bool Twink;
extern bool Sleep;
extern bool NewbiePK;
extern bool AllowPcMobs;
extern bool TestCode1;
extern bool TestCode2;
extern bool TestCode3;
extern bool TestCode4;
extern bool TestCode5;
extern bool TestCode6;  
extern bool timeQueries;
extern bool gameLoopTiming;
extern bool Silence;
extern bool WizBuild;
extern bool Gravity;
extern bool QuestCode;
extern bool QuestCode2;
extern bool QuestCode3;
extern bool QuestCode4;
extern bool Shutdown;
extern long timeTill;
extern time_t Uptime;
extern bool fireInGrimhaven;
extern int clearpath(int room, dirTypeT direc);
extern const sstring DescDamage(double dam);
extern const sstring DescMoves(double a);
extern const sstring ac_for_score(int a);
extern const sstring DescRatio(double f);
extern const sstring DescAttacks(double a);
extern int MountEgoCheck(TBeing *, TBeing *);
extern bool getall(const sstring &, sstring &);
extern int getabunch(const sstring &, sstring &);
extern bool is_number(const sstring &);

extern const sstring heraldcolors[];
extern const sstring heraldcodes[];
extern const sstring card_names[14];
extern const sstring scandirs[];
extern const sstring home_terrains[];
extern const sstring editor_types_oedit[];
extern const sstring editor_types_medit[];
extern const sstring immunity_names[MAX_IMMUNES];
extern const sstring extra_bits[];
extern const sstring affected_bits[];
extern const sstring dirs[];
extern const sstring dirs_to_leading[];
extern const sstring dirs_to_blank[];
extern const sstring connected_types[MAX_CON_STATUS];
extern const sstring body_flags[];
extern const sstring fullness[];
extern const sstring attack_modes[];
extern const sstring attr_player_bits[];
extern const sstring wear_bits[MAX_ITEM_WEARS];
extern const sstring room_bits[MAX_ROOM_BITS];
extern const sstring exit_bits[MAX_DOOR_CONDITIONS];
//extern const sstring sector_types[];
extern const sstring action_bits[];
extern const sstring player_bits[];
extern const sstring position_types[];
extern const sstring material_groups[];
extern const sstring portal_self_enter_mess[MAX_PORTAL_TYPE];
extern const sstring portal_other_enter_mess[MAX_PORTAL_TYPE];
extern const sstring portal_self_exit_mess[MAX_PORTAL_TYPE];
extern const sstring deities[MAX_DEITIES];
extern const sstring bodyParts[MAX_WEAR+1];
extern const sstring trap_effects[MAX_TRAP_EFF];
extern const sstring chest_bits[];
extern const sstring door_types[];
extern const sstring color_options[10];
extern const sstring disc_names[];
extern const sstring auto_name[MAX_AUTO];
extern const sstring exits[];
extern const sstring corpse_flags[MAX_CORPSE_FLAGS];
extern const sstring illegalnames[];
extern const sstring month_name[12];
extern const sstring weekdays[7];
extern const sstring whitespace;
extern int numberhosts;
extern sstring lockmess;
extern const byte sharpness[];
extern const byte attack_mode_bonus[];
extern const int corpse_volume[];
extern const struct race_perc race_size[];
extern const double race_vol_constants[MAX_WEAR];
extern const int TrapDir[];
extern float power_level_number(int);
extern int personalize_object(TBeing *deity, TBeing *, int virt, int decay);
extern int resize_personalize_object(TBeing *deity, TBeing *, int virt, int decay);
extern bool safe_to_save_shop_stuff(TMonster *);
extern bool safe_to_be_in_system(const sstring &);
extern sstring sprinttype(int type, const sstring []);
extern int vsystem(const sstring &);
extern bool load_char(const sstring &name, charFile *);
extern int game_loop(int s);
extern bool safe_to_be_in_system_filepath(const sstring &);
extern void RoomLoad(TBeing *, int, int, bool);
extern int noise(const TBeing *);
extern void change_hands(TBeing *, const char *);
extern void appendPlayerName(TBeing *, TBeing *);
extern void setCombatStats(TBeing *, TBeing *, PolyType, spellNumT);
extern void SwitchStuff(TBeing *, TBeing *, bool setStats = TRUE);
extern int lycanthropeTransform(TBeing *);
extern void DisguiseStuff(TBeing *, TBeing *);
extern void CreateOneRoom(int);
extern void gain_exp(TBeing *, double gain, int rawdamage);
extern int check_sinking_obj(TObj *obj, int room);
extern int ctoi(char c);
extern void update_time(void);
extern void do_components(int pulse);
extern void extract_edit_char(TMonster *);
extern void obj_edit(TBeing *, const sstring &arg);
extern void room_edit(TBeing *, const sstring &arg);
extern void mob_edit(TBeing *, const sstring &);
extern void seditCore(TBeing *, sstring &);
extern void help_edit(TBeing *, sstring &arg);
extern void call_room_specials(void);
extern int DetermineExp(TBeing *mob);
extern void load_one_room(FILE * fl, TRoom *rp);
extern const dirTypeT rev_dir[];
extern void allocate_room(int);
extern void buildCommandArray();
extern void assign_objects(void);
extern void assign_rooms(void);
extern void buildSpellArray();
extern void buildSpellDamArray();
extern void buildTerrainDamMap();
extern void buildWeatherDamMap();
extern void buildComponentArray();
extern int init_game_stats();
extern void save_game_stats();
extern void fixup_players(void);
extern void bootSocialMessages(void);
extern void bootTheShops();
extern void processShopFiles(void);
extern void processCorpseFiles(void);
extern void processRepairFiles(void);
extern void updateSavedRoomItems(void);
extern void updateSavedRoom(sstring &fname);
extern TObj *get_object_in_equip(TBeing *, sstring &, TObj *[], int *, int);
extern sstring fold(sstring &);
extern bool has_prereqs(TBeing *, int);
extern bool should_be_logged(const TBeing *);
extern void stripFrontBytes(sstring &, int);
extern void BatoprsResetCharFlags (TBeing *);
extern bool trade_with(const TObj *, int);
extern void order_high_to_low(int *num1, int *num2, int *num3);
extern bool isanumber(const sstring &);
// extern void roll_abilities(TBeing *);
extern void zero_stats(TBeing *);
extern int CheckStorageChar (TBeing *, TBeing *);
extern bool raw_save_char(const sstring &, charFile *);
extern void set_killer_flag(TBeing *, TBeing *);
extern int SpaceForSkills(TBeing *);
extern sstring add_bars(const sstring &sstring);
extern dirTypeT can_see_linear(const TBeing *, const TBeing *targ, int *rng, dirTypeT *dr);
extern dirTypeT choose_exit_in_zone(int in_room, int tgt_room, int depth);
extern dirTypeT choose_exit_global(int in_room, int tgt_room, int depth);
extern void SetupCreateEngineData();
extern sstring nextToken(char, unsigned int, sstring &);
extern void MakeRoomNoise(TMonster *, int room, const sstring &local_snd, const sstring &distant_snd);
extern void MakeNoise(int room, const sstring &local_snd, const sstring &distant_snd);
extern bool has_healthy_body(TBeing *);
extern int RecGetObjRoom(const TThing *);
extern int ObjFromCorpse(TObj *c);
extern void break_bone(TBeing *, wearSlotT which);
extern void dirwalk(const sstring &dir, void (*fcn) (const sstring &));
extern void dirwalk_fullname(const sstring &dir, void (*fcn) (const sstring &));
extern void dirwalk_subs_fullname(const sstring &dir, void (*fcn) (const sstring &));
extern void argument_split_2(const sstring &, sstring &, sstring &);
extern int RecCompObjNum(const TObj *o, int obj_num);
extern sstring dsearch(const sstring &);
extern TOpal *find_biggest_powerstone(const TBeing *);
extern void wipeCorpseFile(const sstring &);
extern void wipeRentFile(const sstring &);
extern void wipeFollowersFile(const sstring &);
extern void wipePlayerFile(const sstring &);
extern void handleCorrupted(const sstring &, sstring &);
extern void store_mail(const sstring &, const sstring &, const sstring &);
extern void setup_dir(FILE * fl, int room, dirTypeT dir, TRoom * = NULL);
extern sstring hostLogList[MAX_BAN_HOSTS];
extern int numberLogHosts;
extern long roomCount;
extern int script_on_command(TBeing *, sstring &, int);
extern void initWhittle();
extern vector<zoneData>zone_table;
extern void DeleteHatreds(const TBeing *, const sstring &);
extern void DeleteFears(const TBeing *, const sstring &);
extern int SHARP_PRICE(TObj *);
extern bool UtilProcs(int);
extern bool GuildProcs(int);

extern void list_char_in_room(TThing *list, TBeing *ch);

// ch can not be const due to listMe()
extern void list_thing_in_room(const TThing *list, TBeing *ch);
extern void list_thing_on_heap(const TThing *, TBeing *ch, bool);

// ch can not be const, due to showTo
extern void list_in_heap(const TThing *list, TBeing *ch, bool show_all, int perc);

extern void update_world_weather();
extern bool pierceType(spellNumT);
extern bool bluntType(spellNumT);
extern bool slashType(spellNumT);
extern int generic_dispel_magic(TBeing *, TBeing *, int, immortalTypeT, safeTypeT = SAFE_NO);
extern int genericChaseSpirits(TBeing *, TBeing *, int, immortalTypeT, safeTypeT = SAFE_NO);
extern bool file_to_sstring(const sstring &name, sstring &buf, concatT concat = CONCAT_NO);
extern const sstring skill_diff(byte);
extern immuneTypeT getTypeImmunity(spellNumT type);
extern TPCorpse *pc_corpse_list;
#ifdef SUN
extern void bzero(sstring &, int);
extern int mkdir(const sstring &, short unsigned int);
extern int strcasecmp(char *, char *);
extern int setitimer(int, struct itimerval *, struct itimerval *);
extern int gettimeofday(struct timeval *, struct timezone *);
extern int sigsetmask(int);
extern long lrand48(void);
extern int strncasecmp(char *, char *, int);
#endif

#ifndef _CRYPT_H
#if defined(LINUX) || defined(SUN) || defined(SOLARIS)
extern char *crypt(const char *, const char *);
#endif
#endif

#if defined(SUN)
extern int bind(int, const struct sockaddr *, int namelen);
extern int gethostname(char *, int);
extern int accept(int, struct sockaddr *, int *);
extern int getpeername(int, struct sockaddr *, int *);
extern int getsockname(int, struct sockaddr *, int *);
extern int listen(int, int);
extern int socket(int, int, int);
#endif

#if defined(SUN)
extern int setsockopt(int, int, int, const char *, int);
extern int getsockopt(int, int, int, char *buf, int *);
#endif

extern const int spec_skill_array[50];
unsigned int CountBits(unsigned int);
extern bool exit_ok(roomDirData *, TRoom **);
extern spellNumT searchForSpellNum(const sstring &arg, exactTypeT exact);
extern bool thingsInRoomVis(TThing *, TRoom *);
extern int get(TBeing *, TThing *, TThing *, getTypeT, bool);
extern void portal_flag_change(TPortal *, unsigned int, const sstring &, setRemT); 
extern const sstring numberAsString(int);
extern void readStringNoAlloc(FILE *);
extern void reset_zone(int, bool);
extern void loadsetCheck(TBeing *, int, int, wearSlotT, const sstring &);
extern void room_iterate(TRoom *[], void (*func) (int, TRoom *, sstring &, struct show_room_zone_struct *), sstring &, void *);
extern void do_where_thing(const TBeing *, const TThing *, bool, sstring &);
extern bool canSeeThruDoor(const roomDirData *);
extern bool hasDigit(sstring &);
extern void countAccounts(const sstring &arg);
extern int repair_number;
extern int tics;
extern void count_repair_items(const sstring &name);
extern int roomOfObject(const TThing *t);
extern char lcb[256];
extern int getWeather(int);
extern const sstring describeTime();
extern void mudTimePassed(time_t t2, time_t t1, time_info_data *);
extern void realTimePassed(time_t t2, time_t t1, time_info_data *);
extern void assign_item_info();
extern void assignTerrainInfo();
extern int gamePort;   // the port we are running on
extern void mud_assert(int, const char *,...);
extern int determineDissectionItem(TBaseCorpse *, int *, sstring &, sstring &, TBeing *);
extern int determineSkinningItem(TBaseCorpse *, int *, sstring &, sstring &);
extern struct attack_hit_type attack_hit_text[];
extern struct attack_hit_type attack_hit_text_twink[];
extern void processAllInput();
extern void setPrompts(fd_set);
extern void afterPromptProcessing(fd_set);
extern bool is_daytime();
extern bool is_nighttime();
extern wearSlotT mapFileToSlot(int);
extern int mapSlotToFile(wearSlotT);
extern positionTypeT mapFileToPos(int);
extern int mapPosToFile(positionTypeT);
extern itemTypeT mapFileToItemType(int);
extern int mapItemTypeToFile(itemTypeT);
extern bool notBreakSlot(wearSlotT, bool);
extern bool notBleedSlot(wearSlotT);
extern bool illegalEmail(sstring &, Descriptor *, silentTypeT);
extern const struct class_info classInfo[MAX_CLASSES];
extern const struct disc_names_data discNames[MAX_DISCS];
extern const struct racial_health_type racial_health[MAX_RACIAL_TYPES];
extern const ubyte slot_chance[MAX_BODY_TYPES][MAX_WEAR];
extern TBeing *FindTBeingDiffZoneSameRace(TBeing *ch);
extern int check_size_restrictions(const TBeing *ch, const TObj *o, wearSlotT slot, const TBeing *);
extern bool hitInnocent(const TBeing *, const TThing *, const TThing *);
extern double get_doubling_level(float);
extern double mob_exp(float);
extern int kills_to_level(int);
extern int secs_to_level(int);
extern double getExpClassLevel(classIndT, int);
extern int total_ac_obj(TObj *o);
extern char LOWER(char c);
extern char UPPER(char c);
extern char ISNEWL(char ch) ;
extern int combatRound(double);
extern int compareWeights(const float, const float);
extern void pissOff(TMonster *irritated, TBeing *reason);
extern int get_range_actual_damage(TBeing *ch, TBeing *victim, TObj *o, int dam, spellNumT attacktype);
extern void generic_sell(TBeing *ch, TMonster *keeper, TObj *obj, int shop_nr);
extern void generic_num_sell(TBeing *ch, TMonster *keeper, TObj *obj, int shop_nr, int num);
extern TRoom *room_find_or_create(int);
class TNote;
extern TNote *createNote(sstring &);
extern sstring secsToString(time_t num);
extern sstring talenDisplay(int);
extern sstring volumeDisplay(int);
extern TThing *unequip_char_for_save(TBeing *ch, wearSlotT pos);
extern bool isCritPart(wearSlotT);
extern bool hideThisSpell(spellNumT);
extern void test_fight_death(TBeing *, TBeing *, int);
extern sstring shutdown_or_reboot();
extern void raw_write_out_object(const TObj *o, FILE *fp, unsigned int vnum);
extern void bootPulse(const sstring &, bool = true);
extern void readDissectionFile();
extern void sendAutoTips();
extern int sstringncmp(const sstring, const sstring, unsigned int);
extern void generate_obj_index();
extern void generate_mob_index();
extern void generic_cleanup();
extern int listAccount(sstring, sstring &);
extern bool genericBless(TBeing *, TBeing *, int, bool);
extern bool genericDisease(TBeing *, int);
extern void genericCurse(TBeing *, TBeing *, int, spellNumT);
extern sstring displayDifficulty(spellNumT skill);
extern void generic_dirlist(const sstring &, const TBeing *);
extern int doLiqSpell(TBeing *, TBeing *, liqTypeT, int);
extern int doObjSpell(TBeing *, TBeing *, TMagicItem *, TObj *, const sstring &, spellNumT);
extern double getSkillDiffModifier(spellNumT);
extern void getSkillLevelRange(spellNumT, int &, int &, int);
extern int getSpellCost(spellNumT spell, int lev, int learn);
extern int getSpellCasttime(spellNumT spell);
extern void nukeLdead(TBeing *);
extern dirTypeT getDirFromChar(const sstring);
extern dirTypeT getDirFromCmd(cmdTypeT);
extern dirTypeT mapFileToDir(int);
extern int mapDirToFile(dirTypeT);
extern sectorTypeT mapFileToSector(int);
extern int mapSectorToFile(sectorTypeT);
extern discNumT mapFileToDisc(int);
extern int mapDiscToFile(discNumT);
extern applyTypeT mapFileToApply(int);
extern int mapApplyToFile(applyTypeT);
extern void processStringForClient(sstring &);
extern void repoCheck(TMonster *mob, int rnum);
extern void repoCheckForRent(TBeing *ch, TObj *obj, bool corpse);
extern double balanceCorrectionForLevel(double);
extern double getLevMod(ush_int, unsigned int);
extern unsigned int rent_credit(ush_int, unsigned int, unsigned int);
extern int levelLuckModifier(float);
extern bool isDissectComponent(int);
extern bool isInkComponent(int);
extern bool isBrewComponent(int);
extern wizPowerT mapFileToWizPower(int);
extern int mapWizPowerToFile(wizPowerT);
extern bool checkAttuneUsage(TBeing *, int *, int *, TVial **, TSymbol *);
extern const sstring getWizPowerName(wizPowerT); 
extern void setWizPowers(const TBeing *, TBeing *, const sstring &);
extern void remWizPowers(const TBeing *, TBeing *, const sstring &);
extern void assign_drink_types();
extern void assign_drug_info();
extern drugTypeT mapFileToDrug(int);
extern int mapDrugToFile(drugTypeT);
extern int mapSpellnumToFile(spellNumT);
extern spellNumT mapFileToSpellnum(int);
extern bool applyTypeShouldBeSpellnum(applyTypeT);
extern void stSpaceOut(sstring &);
extern bool has_key(TBeing *ch, int key);
extern int bogusAccountName(const sstring &arg);
extern const sstring LimbHealth(double a);
extern doorTrapT mapFileToDoorTrap(int);
extern int mapDoorTrapToFile(doorTrapT);
extern int age_mod_for_stat(int age_num, statTypeT whichStat);
extern sstring describeDuration(const TBeing *, int);
extern bool is_ok(TMonster *, TBeing *, int);
extern int compareDetermineMessage(const int tDrift, const int tValue);
extern bool in_range(int, int, int);
extern int openQueue();
extern void closeQueue();
extern void mudSendMessage(int, int, const sstring &);
extern void recvTextHandler(const sstring &);
extern void mudRecvMessage();
extern void perform_violence(int pulse);
extern const sstring RandomWord();
}

// these needs C++ linkage to avoid conflict with functions in stdlib
extern int remove(TBeing *, TThing *);
extern int atoi(const sstring &);
extern int atoi_safe(const sstring);
extern double atof_safe(const sstring);
extern int GetApprox(int, int);
extern double GetApprox(double, int);

#endif

