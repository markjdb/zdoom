/*******************************
* B_Bot.h                      *
* Description:                 *
* Used with all b_*            *
*******************************/

#ifndef __B_BOT_H__
#define __B_BOT_H__

#include "c_cvars.h"
#include "tables.h"
#include "info.h"
#include "doomdef.h"
#include "d_ticcmd.h"
#include "r_defs.h"
#include "a_pickups.h"

#define FORWARDWALK		0x1900
#define FORWARDRUN		0x3200
#define SIDEWALK		0x1800
#define SIDERUN			0x2800

#define BOT_VERSION 0.97
//Switches-
#define BOT_RELEASE_COMPILE //Define this when compiling a version that should be released.

#define NOCOLOR     11
#define MAXTHINGNODES 100 //Path stuff (paths created between items).
#define SPAWN_DELAY 80  //Used to determine how many tics there are between each bot spawn when bot's are being spawned in a row (like when entering a new map).

#define BOTFILENAME "bots.cfg"

#define MAX_TRAVERSE_DIST 100000000 //10 meters, used within b_func.c
#define AVOID_DIST   45000000 //Try avoid incoming missiles once they reached this close
#define SAFE_SELF_MISDIST (140*FRACUNIT)    //Distance from self to target where it's safe to pull a rocket.
#define FRIEND_DIST  15000000 //To friend.
#define DARK_DIST  5000000 //Distance that bot can see enemies in the dark from.
#define WHATS_DARK  50 //light value thats classed as dark.
#define MAX_MONSTER_TARGET_DIST  50000000 //Too high can slow down the performance, see P_mobj.c
#define ENEMY_SCAN_FOV (120*ANGLE_1)
#define THINGTRYTICK 1000
#define MAXMOVEHEIGHT (32*FRACUNIT) //MAXSTEPMOVE but with jumping counted in.
#define GETINCOMBAT 35000000 //Max distance to item. if it's due to be icked up in a combat situation.
#define SHOOTFOV	(60*ANGLE_1)
#define AFTERTICS   (2*TICRATE) //Seconds that bot will be alert on an recent enemy. Ie not looking the other way
#define MAXROAM		(4*TICRATE) //When this time is elapsed the bot will roam after something else.
//monster mod
#define MSPAWN_DELAY 20//Tics between each spawn.
#define MMAXSELECT   100 //Maximum number of monsters that can be selected at a time.

struct FCheckPosition;

struct botskill_t
{
	int aiming;
	int perfection;
	int reaction;   //How fast the bot will fire after seeing the player.
	int isp;        //Instincts of Self Preservation. Personality
};

FArchive &operator<< (FArchive &arc, botskill_t &skill);

enum
{
	BOTINUSE_No,
	BOTINUSE_Waiting,
	BOTINUSE_Yes,
};

//Info about all bots in the bots.cfg
//Updated during each level start.
//Info given to bots when they're spawned.
struct botinfo_t
{
	botinfo_t *next;
	char *name;
	char *info;
	botskill_t skill;
	int inuse;
	int lastteam;
};

//Used to keep all the globally needed variables in nice order.
class FCajunMaster
{
public:
	~FCajunMaster();

	void ClearPlayer (int playernum, bool keepTeam);

	//(B_Game.c)
	void Main (int buf);
	void Init ();
	void End();
	bool SpawnBot (const char *name, int color = NOCOLOR);
	bool LoadBots ();
	void ForgetBots ();
	void TryAddBot (BYTE **stream, int player);
	void RemoveAllBots (bool fromlist);
	void DestroyAllBots ();

	//(B_Func.c)
	bool Check_LOS (AActor *mobj1, AActor *mobj2, angle_t vangle);

	//(B_Think.c)
	void WhatToGet (AActor *actor, AActor *item);

	//(B_move.c)
	void Roam (AActor *actor, ticcmd_t *cmd);
	bool Move (AActor *actor, ticcmd_t *cmd);
	bool TryWalk (AActor *actor, ticcmd_t *cmd);
	void NewChaseDir (AActor *actor, ticcmd_t *cmd);
	bool CleanAhead (AActor *thing, fixed_t x, fixed_t y, ticcmd_t *cmd);
	void TurnToAng (AActor *actor);
	void Pitch (AActor *actor, AActor *target);
	bool IsDangerous (sector_t *sec);

	TArray<FString> getspawned; //Array of bots (their names) which should be spawned when starting a game.
	BYTE freeze:1;			//Game in freeze mode.
	BYTE changefreeze:1;	//Game wants to change freeze mode.
	int botnum;
	botinfo_t *botinfo;
	int spawn_tries;
	int wanted_botnum;
	TObjPtr<AActor> firstthing;
	TObjPtr<AActor>	body1;
	TObjPtr<AActor> body2;

	bool	 m_Thinking;

private:
	//(B_Game.c)
	bool DoAddBot (BYTE *info, botskill_t skill);

	//(B_Func.c)
	bool Reachable (AActor *actor, AActor *target);
	void Dofire (AActor *actor, ticcmd_t *cmd);
	bool IsLeader (player_t *player);
	AActor *Choose_Mate (AActor *bot);
	AActor *Find_enemy (AActor *bot);
	void SetBodyAt (fixed_t x, fixed_t y, fixed_t z, int hostnum);
	fixed_t FakeFire (AActor *source, AActor *dest, ticcmd_t *cmd);
	angle_t FireRox (AActor *bot, AActor *enemy, ticcmd_t *cmd);
	bool SafeCheckPosition (AActor *actor, fixed_t x, fixed_t y, FCheckPosition &tm);

	//(B_Think.c)
	void Think (AActor *actor, ticcmd_t *cmd);
	void ThinkForMove (AActor *actor, ticcmd_t *cmd);
	void Set_enemy (AActor *actor);

protected:
	bool	 ctf;
	int		 loaded_bots;
	int		 t_join;
	bool	 observer; //Consoleplayer is observer.
};

class DBot : public DObject
{
	DECLARE_CLASS(DBot,DObject)
	HAS_OBJECT_POINTERS
public:
	DBot ();

	void Clear ();
	void Serialize (FArchive &arc);

	angle_t		angle;		// The wanted angle that the bot try to get every tic.
							//  (used to get a smooth view movement)
	TObjPtr<AActor>		dest;		// Move Destination.
	TObjPtr<AActor>		prev;		// Previous move destination.


	TObjPtr<AActor>		enemy;		// The dead meat.
	TObjPtr<AActor>		missile;	// A threatening missile that needs to be avoided.
	TObjPtr<AActor>		mate;		// Friend (used for grouping in teamplay or coop).
	TObjPtr<AActor>		last_mate;	// If bots mate disappeared (not if died) that mate is
							// pointed to by this. Allows bot to roam to it if
							// necessary.

	//Skills
	struct botskill_t	skill;

	//Tickers
	int			t_active;	// Open door, lower lift stuff, door must open and
							// lift must go down before bot does anything
							// radical like try a stuckmove
	int			t_respawn;
	int			t_strafe;
	int			t_react;
	int			t_fight;
	int			t_roam;
	int			t_rocket;

	//Misc booleans
	bool		first_shot;	// Used for reaction skill.
	bool		sleft;		// If false, strafe is right.
	bool		allround;
	bool		increase;

	fixed_t		oldx;
	fixed_t		oldy;
};


//Externs
extern FCajunMaster bglobal;

EXTERN_CVAR (Float, bot_flag_return_time)
EXTERN_CVAR (Int, bot_next_color)
EXTERN_CVAR (Bool, bot_allow_duds)
EXTERN_CVAR (Int, bot_maxcorpses)
EXTERN_CVAR (Bool, bot_observer)
EXTERN_CVAR (Bool, bot_watersplash)
EXTERN_CVAR (Bool, bot_chat)

#endif	// __B_BOT_H__




