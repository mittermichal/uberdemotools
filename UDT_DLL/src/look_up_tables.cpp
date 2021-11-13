#include "look_up_tables.hpp"
#include "timer.hpp"

#include <stdlib.h>
#include <string.h>


#define TABLE_END UDT_S16_MIN

#define TABLE_ENTRY(Table) Table, Table##_U2Q, Table##_Q2U

#define VALIDATE_TABLE_SIZE(Table) \
	static_assert(sizeof(Table) == sizeof(const s16*) * udtProtocol::Count * 3, #Table " invalid")

// can't validate the last table entry at compile time... *sigh*
// static_assert(Table[(sizeof(Table) / sizeof(Table[0])) - 1] == END_OF_LIST, #Table " invalid");
// maybe constexpr can help, but my compiler doesn't handle it
#define VALIDATE_TABLE_SIZES(Table, Count) \
	static_assert(sizeof(Table##_U2Q) == sizeof(s16) * Count, #Table "_U2Q invalid"); \
	static_assert(sizeof(Table##_Q2U) == sizeof(s16) * Count * 2, #Table "_Q2U invalid")


// @TODO: remove once processed
struct WolfEV
{
	enum Id
	{
		XXX_EV_NONE,
		EV_FOOTSTEP,
		EV_FOOTSTEP_METAL,
		EV_FOOTSTEP_WOOD,
		EV_FOOTSTEP_GRASS,
		EV_FOOTSTEP_GRAVEL,
		EV_FOOTSTEP_ROOF,
		EV_FOOTSTEP_SNOW,
		EV_FOOTSTEP_CARPET,
		EV_FOOTSPLASH,
		EV_FOOTWADE,
		EV_SWIM,
		EV_STEP_4,
		EV_STEP_8,
		EV_STEP_12,
		EV_STEP_16,
		EV_FALL_SHORT,
		EV_FALL_MEDIUM,
		EV_FALL_FAR,
		EV_FALL_NDIE,
		EV_FALL_DMG_10,
		EV_FALL_DMG_15,
		EV_FALL_DMG_25,
		EV_FALL_DMG_50,
		EV_JUMP_PAD,            // boing sound at origin, jump sound on player
		EV_JUMP,
		EV_WATER_TOUCH, // foot touches
		EV_WATER_LEAVE, // foot leaves
		EV_WATER_UNDER, // head touches
		EV_WATER_CLEAR, // head leaves
		EV_ITEM_PICKUP,         // normal item pickups are predictable
		EV_ITEM_PICKUP_QUIET,   // (SA) same, but don't play the default pickup sound as it was specified in the ent
		EV_GLOBAL_ITEM_PICKUP,  // powerup / team sounds are broadcast to everyone
		EV_NOITEM,
		EV_NOAMMO,
		EV_EMPTYCLIP,
		EV_FILL_CLIP,
		EV_MG42_FIXED, // JPW NERVE
		EV_WEAP_OVERHEAT,
		EV_CHANGE_WEAPON,
		EV_FIRE_WEAPON,
		EV_FIRE_WEAPONB,
		EV_FIRE_WEAPON_LASTSHOT,
		EV_FIRE_QUICKGREN,  // "Quickgrenade"
		EV_NOFIRE_UNDERWATER,
		EV_FIRE_WEAPON_MG42,
		EV_USE_ITEM0,
		EV_USE_ITEM1,
		EV_USE_ITEM2,
		EV_USE_ITEM3,
		EV_USE_ITEM4,
		EV_USE_ITEM5,
		EV_USE_ITEM6,
		EV_USE_ITEM7,
		EV_USE_ITEM8,
		EV_USE_ITEM9,
		EV_USE_ITEM10,
		EV_USE_ITEM11,
		EV_USE_ITEM12,
		EV_USE_ITEM13,
		EV_USE_ITEM14,
		EV_USE_ITEM15,
		EV_ITEM_RESPAWN,
		EV_ITEM_POP,
		EV_PLAYER_TELEPORT_IN,
		EV_PLAYER_TELEPORT_OUT,
		EV_GRENADE_BOUNCE,      // eventParm will be the soundindex
		EV_GENERAL_SOUND,
		EV_GLOBAL_SOUND,        // no attenuation
		EV_GLOBAL_CLIENT_SOUND, // DHM - Nerve :: no attenuation, only plays for specified client
		// OSPx
		EV_ANNOUNCER_SOUND,		// Deals with countdown // RtcwPro keep this last to avoid OSP demo errors
		// -OSPx
		EV_BULLET_HIT_FLESH,
		EV_BULLET_HIT_WALL,
		EV_MISSILE_HIT,
		EV_MISSILE_MISS,
		EV_RAILTRAIL,
		EV_VENOM,
		EV_VENOMFULL,
		EV_BULLET,              // otherEntity is the shooter
		EV_LOSE_HAT,            //----(SA)
		EV_GIB_HEAD,            // only blow off the head
		EV_PAIN,
		EV_CROUCH_PAIN,
		EV_DEATH1,
		EV_DEATH2,
		EV_DEATH3,
		EV_OBITUARY,
		EV_STOPSTREAMINGSOUND, // JPW NERVE swiped from sherman
		EV_POWERUP_QUAD,
		EV_POWERUP_BATTLESUIT,
		EV_POWERUP_REGEN,
		EV_GIB_PLAYER,          // gib a previously living player
		EV_DEBUG_LINE,
		EV_STOPLOOPINGSOUND,
		EV_TAUNT,
		EV_SMOKE,
		EV_SPARKS,
		EV_SPARKS_ELECTRIC,
		EV_BATS,
		EV_BATS_UPDATEPOSITION,
		EV_BATS_DEATH,
		EV_EXPLODE,     // func_explosive
		EV_EFFECT,      // target_effect
		EV_MORTAREFX,   // mortar firing
		// JPW NERVE
		EV_SPINUP,  // JPW NERVE panzerfaust preamble
		EV_TESTID1, // new particle test
		EV_TESTID2,
		EV_ENDTEST,
		// jpw
		EV_SNOW_ON,
		EV_SNOW_OFF,
		EV_MISSILE_MISS_SMALL,
		EV_MISSILE_MISS_LARGE,
		EV_WOLFKICK_HIT_FLESH,
		EV_WOLFKICK_HIT_WALL,
		EV_WOLFKICK_MISS,
		EV_SPIT_HIT,
		EV_SPIT_MISS,
		EV_SHARD,
		EV_JUNK,
		EV_EMITTER, //----(SA)	added // generic particle emitter that uses client-side particle scripts
		EV_OILPARTICLES,
		EV_OILSLICK,
		EV_OILSLICKREMOVE,
		EV_MG42EFX,
		EV_FLAMEBARREL_BOUNCE,
		EV_FLAKGUN1,
		EV_FLAKGUN2,
		EV_FLAKGUN3,
		EV_FLAKGUN4,
		EV_EXERT1,
		EV_EXERT2,
		EV_EXERT3,
		EV_SNOWFLURRY,
		EV_CONCUSSIVE,
		EV_DUST,
		EV_RUMBLE_EFX,
		EV_GUNSPARKS,
		EV_FLAMETHROWER_EFFECT,
		EV_SNIPER_SOUND,
		EV_POPUP,
		EV_POPUPBOOK,
		EV_GIVEPAGE,    //----(SA)	added
		EV_MG42BULLET_HIT_FLESH,    // Arnout: these two send the seed as well
		EV_MG42BULLET_HIT_WALL,
		EV_MAX_EVENTS   // just added as an 'endcap'
	};
};

// @TODO: remove once processed
struct WolfET
{
	enum Id
	{
		ET_GENERAL,
		ET_PLAYER,
		ET_ITEM,
		ET_MISSILE,
		ET_MOVER,
		ET_BEAM,
		ET_PORTAL,
		ET_SPEAKER,
		ET_PUSH_TRIGGER,
		ET_TELEPORT_TRIGGER,
		ET_INVISIBLE,
		ET_GRAPPLE,             // grapple hooked on wall
		ET_CONCUSSIVE_TRIGGER,  // JPW NERVE trigger for concussive dust particles
		ET_OID_TRIGGER,         // DHM - Nerve :: Objective Info Display
		ET_EXPLOSIVE_INDICATOR, // NERVE - SMF

		//---- (SA) Wolf
		ET_EXPLOSIVE,           // brush that will break into smaller bits when damaged
		ET_EF_TESLA,
		ET_EF_SPOTLIGHT,
		ET_EFFECT3,
		ET_ALARMBOX,
		ET_CORONA,
		ET_TRAP,

		ET_GAMEMODEL,           // misc_gamemodel.  similar to misc_model, but it's a dynamic model so we have LOD
		ET_FOOTLOCKER,  //----(SA)	added
		//---- end

		ET_FLAMEBARREL,
		ET_FP_PARTS,

		// FIRE PROPS
		ET_FIRE_COLUMN,
		ET_FIRE_COLUMN_SMOKE,
		ET_RAMJET,

		ET_FLAMETHROWER_CHUNK,      // DHM - NERVE :: Used in server side collision detection for flamethrower

		ET_EXPLO_PART,

		ET_PROP,
		ET_BAT,

		ET_AI_EFFECT,

		ET_CAMERA,
		ET_MOVERSCALED,

		ET_CORPSE,              // Arnout: dead player
		ET_SMOKER,              // Arnout: target_smoke entity

		ET_TEMPHEAD,            // Gordon: temporary head for clients for bullet traces

		ET_MG42_BARREL,         // Arnout: MG42 barrel

		ET_EVENTS               // any of the EV_* events can be added freestanding
		// by setting eType to ET_EVENTS + eventNum
		// this avoids having to set eFlags and eventNum
	};
};

// @TODO: remove once processed
struct WolfWP
{
	enum Id
	{
		WP_NONE,                // 0

		WP_KNIFE,               // 1
		// German weapons
		WP_LUGER,               // 2
		WP_MP40,                // 3
		WP_MAUSER,              // 4
		WP_FG42,                // 5
		WP_GRENADE_LAUNCHER,    // 6
		WP_PANZERFAUST,         // 7
		WP_VENOM,               // 8
		WP_FLAMETHROWER,        // 9
		WP_TESLA,               // 10
		WP_SPEARGUN,            // 11

		// weapon keys only go 1-0, so put the alternates above that (since selection will be a double click on the german weapon key)

		// American equivalents
		WP_KNIFE2,              // 12
		WP_COLT,                // 13	equivalent american weapon to german luger
		WP_THOMPSON,            // 14	equivalent american weapon to german mp40
		WP_GARAND,              // 15	equivalent american weapon to german mauser
		WP_BAR,                 // 16	equivalent american weapon to german fg42
		WP_GRENADE_PINEAPPLE,   // 17
		WP_ROCKET_LAUNCHER,     // 18	equivalent american weapon to german panzerfaust

		// secondary fire weapons
		WP_SNIPERRIFLE,         // 19
		WP_SNOOPERSCOPE,        // 20
		WP_VENOM_FULL,          // 21
		WP_SPEARGUN_CO2,        // 22
		WP_FG42SCOPE,           // 23	fg42 alt fire
		WP_BAR2,                // 24

		// more weapons
		WP_STEN,                // 25	silenced sten sub-machinegun
		WP_MEDIC_SYRINGE,       // 26	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
		WP_AMMO,                // 27	// JPW NERVE likewise
		WP_ARTY,                // 28
		WP_SILENCER,            // 29	// used to be sp5
		WP_AKIMBO,              // 30	//----(SA)	added

		// jpw
		WP_CROSS,               // 31
		WP_DYNAMITE,            // 32
		WP_DYNAMITE2,           // 33
		WP_PROX,                // 34

		WP_MONSTER_ATTACK1,     // 35	// generic monster attack, slot 1
		WP_MONSTER_ATTACK2,     // 36	// generic monster attack, slot 2
		WP_MONSTER_ATTACK3,     // 37	// generic monster attack, slot 2

		WP_SMOKETRAIL,          // 38

		WP_GAUNTLET,            // 39

		WP_SNIPER,              // 40
		WP_MORTAR,              // 41
		VERYBIGEXPLOSION,       // 42	// explosion effect for airplanes

		// NERVE - SMF - special weapons are here now
		WP_MEDKIT,              // 43
		WP_PLIERS,              // 44
		WP_SMOKE_GRENADE,       // 45
		// -NERVE - SMF
		WP_BINOCULARS,          // 46

		WP_NUM_WEAPONS          // 47   NOTE: this cannot be larger than 64 for AI/player weapons!
	};
};

// @TODO: remove once processed
struct WolfMOD
{
	enum Id
	{
		MOD_UNKNOWN,
		MOD_SHOTGUN,
		MOD_GAUNTLET,
		MOD_MACHINEGUN,
		MOD_GRENADE,
		MOD_GRENADE_SPLASH,
		MOD_ROCKET,
		MOD_ROCKET_SPLASH,
		MOD_RAILGUN,
		MOD_LIGHTNING,
		MOD_BFG,
		MOD_BFG_SPLASH,

		// (SA) modified wolf weap mods
		MOD_KNIFE,
		MOD_KNIFE2,
		MOD_KNIFE_STEALTH,
		MOD_LUGER,
		MOD_COLT,
		MOD_MP40,
		MOD_THOMPSON,
		MOD_STEN,
		MOD_MAUSER,
		MOD_SNIPERRIFLE,
		MOD_GARAND,
		MOD_SNOOPERSCOPE,
		MOD_SILENCER,   //----(SA)
		MOD_AKIMBO,     //----(SA)
		MOD_BAR,    //----(SA)
		MOD_FG42,
		MOD_FG42SCOPE,
		MOD_PANZERFAUST,
		MOD_ROCKET_LAUNCHER,
		MOD_GRENADE_LAUNCHER,
		MOD_VENOM,
		MOD_VENOM_FULL,
		MOD_FLAMETHROWER,
		MOD_TESLA,
		MOD_SPEARGUN,
		MOD_SPEARGUN_CO2,
		MOD_GRENADE_PINEAPPLE,
		MOD_CROSS,
		// end

		MOD_MORTAR,
		MOD_MORTAR_SPLASH,

		MOD_KICKED,
		MOD_GRABBER,

		MOD_DYNAMITE,
		MOD_DYNAMITE_SPLASH,
		MOD_AIRSTRIKE, // JPW NERVE
		MOD_SYRINGE,    // JPW NERVE
		MOD_AMMO,   // JPW NERVE
		MOD_ARTY,   // JPW NERVE

		MOD_WATER,
		MOD_SLIME,
		MOD_LAVA,
		MOD_CRUSH,
		MOD_TELEFRAG,
		MOD_FALLING,
		MOD_SUICIDE,
		MOD_TARGET_LASER,
		MOD_TRIGGER_HURT,
		MOD_GRAPPLE,
		MOD_EXPLOSIVE,
		MOD_POISONGAS,

		// RF, AI attacks
		MOD_ZOMBIESPIT,
		MOD_ZOMBIESPIT_SPLASH,
		MOD_ZOMBIESPIRIT,
		MOD_ZOMBIESPIRIT_SPLASH,

		MOD_LOPER_LEAP,
		MOD_LOPER_GROUND,
		MOD_LOPER_HIT,

		// JPW NERVE multiplayer class-specific MODs
		MOD_LT_AMMO,
		MOD_LT_AIRSTRIKE,
		MOD_ENGINEER,   // not sure if we'll use
		MOD_MEDIC,      // these like this or not
		//
		MOD_BAT,

		// OSPx
		MOD_ADMKILL,
		MOD_SELFKILL,
		MOD_ARTILLERY,
		MOD_SWITCHTEAM,
		MOD_NUM_MODS
		// -OSPx
	};
};


static s16 PowerUps_3_90_U2Q[udtPowerUpIndex::Count];
static s16 PowerUps_3_90_Q2U[udtPowerUpIndex::Count * 2];
static const s16 PowerUps_3_90[] =
{
	(s16)udtPowerUpIndex::QuadDamage, 1,
	(s16)udtPowerUpIndex::BattleSuit, 2,
	(s16)udtPowerUpIndex::Haste, 3,
	(s16)udtPowerUpIndex::Invisibility, 4,
	(s16)udtPowerUpIndex::Regeneration, 5,
	(s16)udtPowerUpIndex::Flight, 6,
	(s16)udtPowerUpIndex::RedFlag, 7,
	(s16)udtPowerUpIndex::BlueFlag, 8,
	TABLE_END
};
VALIDATE_TABLE_SIZES(PowerUps_3_90, udtPowerUpIndex::Count);

static s16 PowerUps_91_U2Q[udtPowerUpIndex::Count];
static s16 PowerUps_91_Q2U[udtPowerUpIndex::Count * 2];
static const s16 PowerUps_91[] =
{
	(s16)udtPowerUpIndex::QuadDamage, 5,
	(s16)udtPowerUpIndex::BattleSuit, 6,
	(s16)udtPowerUpIndex::Haste, 7,
	(s16)udtPowerUpIndex::Invisibility, 8,
	(s16)udtPowerUpIndex::Regeneration, 9,
	(s16)udtPowerUpIndex::Flight, 10,
	(s16)udtPowerUpIndex::RedFlag, 2,
	(s16)udtPowerUpIndex::BlueFlag, 3,
	(s16)udtPowerUpIndex::NeutralFlag, 4,
	(s16)udtPowerUpIndex::Scout, 12,
	(s16)udtPowerUpIndex::Guard, 13,
	(s16)udtPowerUpIndex::Doubler, 14,
	(s16)udtPowerUpIndex::ArmorRegeneration, 15,
	(s16)udtPowerUpIndex::Invulnerability, 11,
	TABLE_END
};
VALIDATE_TABLE_SIZES(PowerUps_91, udtPowerUpIndex::Count);

static const s16* PowerUpTables[] =
{
	TABLE_ENTRY(PowerUps_3_90), // 3
	TABLE_ENTRY(PowerUps_3_90), // 48
	TABLE_ENTRY(PowerUps_3_90), // 60 @TODO: RTCW
	TABLE_ENTRY(PowerUps_3_90), // 66
	TABLE_ENTRY(PowerUps_3_90), // 67
	TABLE_ENTRY(PowerUps_3_90), // 68
	TABLE_ENTRY(PowerUps_3_90), // 73
	TABLE_ENTRY(PowerUps_3_90), // 90
	TABLE_ENTRY(PowerUps_91) // 91
};
VALIDATE_TABLE_SIZE(PowerUpTables);

static s16 LifeStats_3_68_U2Q[udtLifeStatsIndex::Count];
static s16 LifeStats_3_68_Q2U[udtLifeStatsIndex::Count * 2];
static const s16 LifeStats_3_68[] =
{
	(s16)udtLifeStatsIndex::Health, 0,
	(s16)udtLifeStatsIndex::HoldableItem, 1,
	(s16)udtLifeStatsIndex::Weapons, 2,
	(s16)udtLifeStatsIndex::Armor, 3,
	(s16)udtLifeStatsIndex::MaxHealth, 6,
	TABLE_END
};
VALIDATE_TABLE_SIZES(LifeStats_3_68, udtLifeStatsIndex::Count);

static s16 LifeStats_73p_U2Q[udtLifeStatsIndex::Count];
static s16 LifeStats_73p_Q2U[udtLifeStatsIndex::Count * 2];
static const s16 LifeStats_73p[] =
{
	(s16)udtLifeStatsIndex::Health, 0,
	(s16)udtLifeStatsIndex::HoldableItem, 1,
	(s16)udtLifeStatsIndex::Weapons, 3,
	(s16)udtLifeStatsIndex::Armor, 4,
	(s16)udtLifeStatsIndex::MaxHealth, 7,
	TABLE_END
};
VALIDATE_TABLE_SIZES(LifeStats_73p, udtLifeStatsIndex::Count);

static const s16* LifeStatsTables[] =
{
	TABLE_ENTRY(LifeStats_3_68), // 3
	TABLE_ENTRY(LifeStats_3_68), // 48
	TABLE_ENTRY(LifeStats_3_68), // 60 @TODO: RTCW
	TABLE_ENTRY(LifeStats_3_68), // 66
	TABLE_ENTRY(LifeStats_3_68), // 67
	TABLE_ENTRY(LifeStats_3_68), // 68
	TABLE_ENTRY(LifeStats_73p), // 73
	TABLE_ENTRY(LifeStats_73p), // 90
	TABLE_ENTRY(LifeStats_73p) // 91
};
VALIDATE_TABLE_SIZE(LifeStatsTables);

static s16 PersStats_3_U2Q[udtPersStatsIndex::Count];
static s16 PersStats_3_Q2U[udtPersStatsIndex::Count * 2];
static const s16 PersStats_3[] =
{
	(s16)udtPersStatsIndex::Score, 0,
	(s16)udtPersStatsIndex::DamageGiven, 1,
	(s16)udtPersStatsIndex::Rank, 2,
	(s16)udtPersStatsIndex::Team, 3,
	(s16)udtPersStatsIndex::SpawnCount, 4,
	(s16)udtPersStatsIndex::LastAttacker, 7,
	(s16)udtPersStatsIndex::Deaths, 8,
	(s16)udtPersStatsIndex::Impressives, 9,
	(s16)udtPersStatsIndex::Excellents, 10,
	(s16)udtPersStatsIndex::Humiliations, 11,
	TABLE_END
};
VALIDATE_TABLE_SIZES(PersStats_3, udtPersStatsIndex::Count);

static s16 PersStats_48_68_U2Q[udtPersStatsIndex::Count];
static s16 PersStats_48_68_Q2U[udtPersStatsIndex::Count * 2];
static const s16 PersStats_48_68[] =
{
	(s16)udtPersStatsIndex::FlagCaptures, 14,
	(s16)udtPersStatsIndex::Score, 0,
	(s16)udtPersStatsIndex::DamageGiven, 1,
	(s16)udtPersStatsIndex::Rank, 2,
	(s16)udtPersStatsIndex::Team, 3,
	(s16)udtPersStatsIndex::SpawnCount, 4,
	(s16)udtPersStatsIndex::LastAttacker, 6,
	(s16)udtPersStatsIndex::LastTargetHealthAndArmor, 7,
	(s16)udtPersStatsIndex::Deaths, 8,
	(s16)udtPersStatsIndex::Impressives, 9,
	(s16)udtPersStatsIndex::Excellents, 10,
	(s16)udtPersStatsIndex::Defends, 11,
	(s16)udtPersStatsIndex::Assists, 12,
	(s16)udtPersStatsIndex::Humiliations, 13,
	TABLE_END
};
VALIDATE_TABLE_SIZES(PersStats_48_68, udtPersStatsIndex::Count);

static s16 PersStats_73p_U2Q[udtPersStatsIndex::Count];
static s16 PersStats_73p_Q2U[udtPersStatsIndex::Count * 2];
static const s16 PersStats_73p[] =
{
	(s16)udtPersStatsIndex::FlagCaptures, 13,
	(s16)udtPersStatsIndex::Score, 0,
	(s16)udtPersStatsIndex::DamageGiven, 1,
	(s16)udtPersStatsIndex::Rank, 2,
	(s16)udtPersStatsIndex::Team, 3,
	(s16)udtPersStatsIndex::SpawnCount, 4,
	(s16)udtPersStatsIndex::LastAttacker, 6,
	(s16)udtPersStatsIndex::LastTargetHealthAndArmor, 14,
	(s16)udtPersStatsIndex::Deaths, 7,
	(s16)udtPersStatsIndex::Impressives, 8,
	(s16)udtPersStatsIndex::Excellents, 9,
	(s16)udtPersStatsIndex::Defends, 10,
	(s16)udtPersStatsIndex::Assists, 11,
	(s16)udtPersStatsIndex::Humiliations, 12,
	TABLE_END
};
VALIDATE_TABLE_SIZES(PersStats_73p, udtPersStatsIndex::Count);

static const s16* PersStatsTables[] =
{
	TABLE_ENTRY(PersStats_3), // 3
	TABLE_ENTRY(PersStats_48_68), // 48
	TABLE_ENTRY(PersStats_48_68), // 60 @TODO: RTCW
	TABLE_ENTRY(PersStats_48_68), // 66
	TABLE_ENTRY(PersStats_48_68), // 67
	TABLE_ENTRY(PersStats_48_68), // 68
	TABLE_ENTRY(PersStats_73p), // 73
	TABLE_ENTRY(PersStats_73p), // 90
	TABLE_ENTRY(PersStats_73p) // 91
};
VALIDATE_TABLE_SIZE(PersStatsTables);

static s16 EntityTypes_3_U2Q[udtEntityType::Count];
static s16 EntityTypes_3_Q2U[udtEntityType::Count * 2];
static const s16 EntityTypes_3[] =
{
	(s16)udtEntityType::Event, 12,
	(s16)udtEntityType::General, 0,
	(s16)udtEntityType::Player, 1,
	(s16)udtEntityType::Item, 2,
	(s16)udtEntityType::Missile, 3,
	(s16)udtEntityType::Mover, 4,
	(s16)udtEntityType::Beam, 5,
	(s16)udtEntityType::Portal, 6,
	(s16)udtEntityType::Speaker, 7,
	(s16)udtEntityType::PushTrigger, 8,
	(s16)udtEntityType::TeleportTrigger, 9,
	(s16)udtEntityType::Invisible, 10,
	(s16)udtEntityType::Grapple, 11,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityTypes_3, udtEntityType::Count);

static s16 EntityTypes_48p_U2Q[udtEntityType::Count];
static s16 EntityTypes_48p_Q2U[udtEntityType::Count * 2];
static const s16 EntityTypes_48p[] =
{
	(s16)udtEntityType::Event, 13,
	(s16)udtEntityType::General, 0,
	(s16)udtEntityType::Player, 1,
	(s16)udtEntityType::Item, 2,
	(s16)udtEntityType::Missile, 3,
	(s16)udtEntityType::Mover, 4,
	(s16)udtEntityType::Beam, 5,
	(s16)udtEntityType::Portal, 6,
	(s16)udtEntityType::Speaker, 7,
	(s16)udtEntityType::PushTrigger, 8,
	(s16)udtEntityType::TeleportTrigger, 9,
	(s16)udtEntityType::Invisible, 10,
	(s16)udtEntityType::Grapple, 11,
	(s16)udtEntityType::Team, 12,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityTypes_48p, udtEntityType::Count);

#if 0 // ET
static s16 EntityTypes_84_U2Q[udtEntityType::Count];
static s16 EntityTypes_84_Q2U[udtEntityType::Count * 2];
static const s16 EntityTypes_84[] =
{
	(s16)udtEntityType::Event, 61,
	(s16)udtEntityType::General, 0,
	(s16)udtEntityType::Player, 1,
	(s16)udtEntityType::Item, 2,
	(s16)udtEntityType::Missile, 3,
	(s16)udtEntityType::Mover, 4,
	(s16)udtEntityType::Beam, 5,
	(s16)udtEntityType::Portal, 6,
	(s16)udtEntityType::Speaker, 7,
	(s16)udtEntityType::PushTrigger, 8,
	(s16)udtEntityType::TeleportTrigger, 9,
	(s16)udtEntityType::Invisible, 10,
	(s16)udtEntityType::Grapple, 11,
	(s16)udtEntityType::Team, 12,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityTypes_84, udtEntityType::Count);
#endif

static s16 EntityTypes_60_U2Q[udtEntityType::Count];
static s16 EntityTypes_60_Q2U[udtEntityType::Count * 2];
static const s16 EntityTypes_60[] =
{
	(s16)udtEntityType::Event, 40,
	(s16)udtEntityType::General, 0,
	(s16)udtEntityType::Player, 1,
	(s16)udtEntityType::Item, 2,
	(s16)udtEntityType::Missile, 3,
	(s16)udtEntityType::Mover, 4,
	(s16)udtEntityType::Beam, 5,
	(s16)udtEntityType::Portal, 6,
	(s16)udtEntityType::Speaker, 7,
	(s16)udtEntityType::PushTrigger, 8,
	(s16)udtEntityType::TeleportTrigger, 9,
	(s16)udtEntityType::Invisible, 10,
	(s16)udtEntityType::Grapple, 11,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityTypes_48p, udtEntityType::Count);

static const s16* EntityTypeTables[] =
{
	TABLE_ENTRY(EntityTypes_3), // 3
	TABLE_ENTRY(EntityTypes_48p), // 48
	TABLE_ENTRY(EntityTypes_60), // 60
	TABLE_ENTRY(EntityTypes_48p), // 66
	TABLE_ENTRY(EntityTypes_48p), // 67
	TABLE_ENTRY(EntityTypes_48p), // 68
	TABLE_ENTRY(EntityTypes_48p), // 73
	TABLE_ENTRY(EntityTypes_48p), // 90
	TABLE_ENTRY(EntityTypes_48p) // 91
};
VALIDATE_TABLE_SIZE(EntityTypeTables);

static s16 EntityFlagBits_3_U2Q[udtEntityFlag::Count];
static s16 EntityFlagBits_3_Q2U[udtEntityFlag::Count * 2];
static const s16 EntityFlagBits_3[] =
{
	(s16)udtEntityFlag::Dead, 0,
	(s16)udtEntityFlag::TeleportBit, 2,
	(s16)udtEntityFlag::AwardExcellent, 3,
	(s16)udtEntityFlag::AwardHumiliation, 6,
	(s16)udtEntityFlag::NoDraw, 7,
	(s16)udtEntityFlag::Firing, 8,
	(s16)udtEntityFlag::AwardCapture, 11,
	(s16)udtEntityFlag::Chatting, 12,
	(s16)udtEntityFlag::ConnectionInterrupted, 13,
	(s16)udtEntityFlag::HasVoted, 14,
	(s16)udtEntityFlag::AwardImpressive, 15,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityFlagBits_3, udtEntityFlag::Count);

static s16 EntityFlagBits_48_U2Q[udtEntityFlag::Count];
static s16 EntityFlagBits_48_Q2U[udtEntityFlag::Count * 2];
static const s16 EntityFlagBits_48[] =
{
	(s16)udtEntityFlag::Dead, 0,
	(s16)udtEntityFlag::TeleportBit, 2,
	(s16)udtEntityFlag::AwardExcellent, 3,
	(s16)udtEntityFlag::AwardHumiliation, 6,
	(s16)udtEntityFlag::NoDraw, 7,
	(s16)udtEntityFlag::Firing, 8,
	(s16)udtEntityFlag::AwardCapture, 11,
	(s16)udtEntityFlag::Chatting, 12,
	(s16)udtEntityFlag::ConnectionInterrupted, 13,
	(s16)udtEntityFlag::HasVoted, 14,
	(s16)udtEntityFlag::AwardImpressive, 15,
	(s16)udtEntityFlag::AwardDefense, 16,
	(s16)udtEntityFlag::AwardAssist, 17,
	(s16)udtEntityFlag::AwardDenied, 18,
	(s16)udtEntityFlag::HasTeamVoted, 19,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityFlagBits_48, udtEntityFlag::Count);

static s16 EntityFlagBits_66_90_U2Q[udtEntityFlag::Count];
static s16 EntityFlagBits_66_90_Q2U[udtEntityFlag::Count * 2];
static const s16 EntityFlagBits_66_90[] =
{
	(s16)udtEntityFlag::Dead, 0,
	(s16)udtEntityFlag::TeleportBit, 2,
	(s16)udtEntityFlag::AwardExcellent, 3,
	(s16)udtEntityFlag::PlayerEvent, 4,
	(s16)udtEntityFlag::AwardHumiliation, 6,
	(s16)udtEntityFlag::NoDraw, 7,
	(s16)udtEntityFlag::Firing, 8,
	(s16)udtEntityFlag::AwardCapture, 11,
	(s16)udtEntityFlag::Chatting, 12,
	(s16)udtEntityFlag::ConnectionInterrupted, 13,
	(s16)udtEntityFlag::HasVoted, 14,
	(s16)udtEntityFlag::AwardImpressive, 15,
	(s16)udtEntityFlag::AwardDefense, 16,
	(s16)udtEntityFlag::AwardAssist, 17,
	(s16)udtEntityFlag::AwardDenied, 18,
	(s16)udtEntityFlag::HasTeamVoted, 19,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityFlagBits_66_90, udtEntityFlag::Count);

static s16 EntityFlagBits_91_U2Q[udtEntityFlag::Count];
static s16 EntityFlagBits_91_Q2U[udtEntityFlag::Count * 2];
static const s16 EntityFlagBits_91[] =
{
	(s16)udtEntityFlag::Dead, 0,
	(s16)udtEntityFlag::TeleportBit, 2,
	(s16)udtEntityFlag::AwardExcellent, 3,
	(s16)udtEntityFlag::PlayerEvent, 4,
	(s16)udtEntityFlag::AwardHumiliation, 6,
	(s16)udtEntityFlag::NoDraw, 7,
	(s16)udtEntityFlag::Firing, 8,
	(s16)udtEntityFlag::AwardCapture, 11,
	(s16)udtEntityFlag::Chatting, 12,
	(s16)udtEntityFlag::ConnectionInterrupted, 13,
	(s16)udtEntityFlag::AwardImpressive, 15,
	(s16)udtEntityFlag::AwardDefense, 16,
	(s16)udtEntityFlag::AwardAssist, 17,
	(s16)udtEntityFlag::AwardDenied, 18,
	(s16)udtEntityFlag::Spectator, 14,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityFlagBits_91, udtEntityFlag::Count);

static const s16* EntityFlagBitTables[] =
{
	TABLE_ENTRY(EntityFlagBits_3), // 3
	TABLE_ENTRY(EntityFlagBits_48), // 48
	TABLE_ENTRY(EntityFlagBits_66_90), // 60 @TODO: RTCW
	TABLE_ENTRY(EntityFlagBits_66_90), // 66
	TABLE_ENTRY(EntityFlagBits_66_90), // 67
	TABLE_ENTRY(EntityFlagBits_66_90), // 68
	TABLE_ENTRY(EntityFlagBits_66_90), // 73
	TABLE_ENTRY(EntityFlagBits_66_90), // 90
	TABLE_ENTRY(EntityFlagBits_91) // 91
};
VALIDATE_TABLE_SIZE(EntityFlagBitTables);

static s16 EntityEvents_3_U2Q[udtEntityEvent::Count];
static s16 EntityEvents_3_Q2U[udtEntityEvent::Count * 2];
static const s16 EntityEvents_3[] =
{
	(s16)udtEntityEvent::Obituary, 58,
	(s16)udtEntityEvent::WeaponFired, 23,
	(s16)udtEntityEvent::ItemPickup, 19,
	(s16)udtEntityEvent::GlobalItemPickup, 20,
	(s16)udtEntityEvent::GlobalSound, 46,
	(s16)udtEntityEvent::ItemRespawn, 40,
	(s16)udtEntityEvent::ItemPop, 41,
	(s16)udtEntityEvent::PlayerTeleportIn, 42,
	(s16)udtEntityEvent::PlayerTeleportOut, 43,
	(s16)udtEntityEvent::BulletHitFlesh, 47,
	(s16)udtEntityEvent::BulletHitWall, 48,
	(s16)udtEntityEvent::MissileHit, 49,
	(s16)udtEntityEvent::MissileMiss, 50,
	(s16)udtEntityEvent::RailTrail, 51,
	(s16)udtEntityEvent::PowerUpQuad, 59,
	(s16)udtEntityEvent::PowerUpBattleSuit, 60,
	(s16)udtEntityEvent::PowerUpRegen, 61,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityEvents_3, udtEntityEvent::Count);

static s16 EntityEvents_48_68_U2Q[udtEntityEvent::Count];
static s16 EntityEvents_48_68_Q2U[udtEntityEvent::Count * 2];
static const s16 EntityEvents_48_68[] =
{
	(s16)udtEntityEvent::Obituary, 60,
	(s16)udtEntityEvent::WeaponFired, 23,
	(s16)udtEntityEvent::ItemPickup, 19,
	(s16)udtEntityEvent::GlobalItemPickup, 20,
	(s16)udtEntityEvent::GlobalSound, 46,
	(s16)udtEntityEvent::GlobalTeamSound, 47,
	(s16)udtEntityEvent::ItemRespawn, 40,
	(s16)udtEntityEvent::ItemPop, 41,
	(s16)udtEntityEvent::PlayerTeleportIn, 42,
	(s16)udtEntityEvent::PlayerTeleportOut, 43,
	(s16)udtEntityEvent::BulletHitFlesh, 48,
	(s16)udtEntityEvent::BulletHitWall, 49,
	(s16)udtEntityEvent::MissileHit, 50,
	(s16)udtEntityEvent::MissileMiss, 51,
	(s16)udtEntityEvent::MissileMissMetal, 52,
	(s16)udtEntityEvent::RailTrail, 53,
	(s16)udtEntityEvent::PowerUpQuad, 61,
	(s16)udtEntityEvent::PowerUpBattleSuit, 62,
	(s16)udtEntityEvent::PowerUpRegen, 63,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityEvents_48_68, udtEntityEvent::Count);

static s16 EntityEvents_73p_U2Q[udtEntityEvent::Count];
static s16 EntityEvents_73p_Q2U[udtEntityEvent::Count * 2];
static const s16 EntityEvents_73p[] =
{
	(s16)udtEntityEvent::Obituary, 58,
	(s16)udtEntityEvent::WeaponFired, 20,
	(s16)udtEntityEvent::ItemPickup, 15,
	(s16)udtEntityEvent::GlobalItemPickup, 16,
	(s16)udtEntityEvent::GlobalSound, 43,
	(s16)udtEntityEvent::GlobalTeamSound, 44,
	(s16)udtEntityEvent::ItemRespawn, 37,
	(s16)udtEntityEvent::ItemPop, 38,
	(s16)udtEntityEvent::PlayerTeleportIn, 39,
	(s16)udtEntityEvent::PlayerTeleportOut, 40,
	(s16)udtEntityEvent::BulletHitFlesh, 45,
	(s16)udtEntityEvent::BulletHitWall, 46,
	(s16)udtEntityEvent::MissileHit, 47,
	(s16)udtEntityEvent::MissileMiss, 48,
	(s16)udtEntityEvent::MissileMissMetal, 49,
	(s16)udtEntityEvent::RailTrail, 50,
	(s16)udtEntityEvent::PowerUpQuad, 59,
	(s16)udtEntityEvent::PowerUpBattleSuit, 60,
	(s16)udtEntityEvent::PowerUpRegen, 61,
	(s16)udtEntityEvent::QL_Overtime, 84,
	(s16)udtEntityEvent::QL_GameOver, 85,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityEvents_73p, udtEntityEvent::Count);

#if 0 // ET
static s16 EntityEvents_84_U2Q[udtEntityEvent::Count];
static s16 EntityEvents_84_Q2U[udtEntityEvent::Count * 2];
static const s16 EntityEvents_84[] =
{
	(s16)udtEntityEvent::Obituary, 70,
	(s16)udtEntityEvent::BulletHitFlesh, 57,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityEvents_84, udtEntityEvent::Count);
#endif

static s16 EntityEvents_60_U2Q[udtEntityEvent::Count];
static s16 EntityEvents_60_Q2U[udtEntityEvent::Count * 2];
static const s16 EntityEvents_60[] =
{
	(s16)udtEntityEvent::Obituary, 86,
	(s16)udtEntityEvent::WeaponFired, 40,
	(s16)udtEntityEvent::ItemPickup, 30,
	(s16)udtEntityEvent::GlobalItemPickup, 32,
	(s16)udtEntityEvent::GlobalSound, 68,
	(s16)udtEntityEvent::ItemRespawn, 62,
	(s16)udtEntityEvent::ItemPop, 63,
	(s16)udtEntityEvent::PlayerTeleportIn, 64,
	(s16)udtEntityEvent::PlayerTeleportOut, 65,
	(s16)udtEntityEvent::BulletHitFlesh, 71,
	(s16)udtEntityEvent::BulletHitWall, 72,
	(s16)udtEntityEvent::MissileHit, 73,
	(s16)udtEntityEvent::MissileMiss, 74,
	(s16)udtEntityEvent::RailTrail, 75,
	(s16)udtEntityEvent::PowerUpQuad, 88,
	(s16)udtEntityEvent::PowerUpBattleSuit, 89,
	(s16)udtEntityEvent::PowerUpRegen, 90,
	TABLE_END
};
VALIDATE_TABLE_SIZES(EntityEvents_3, udtEntityEvent::Count);

static const s16* EntityEventTables[] =
{
	TABLE_ENTRY(EntityEvents_3), // 3
	TABLE_ENTRY(EntityEvents_48_68), // 48
	TABLE_ENTRY(EntityEvents_60),
	TABLE_ENTRY(EntityEvents_48_68), // 66
	TABLE_ENTRY(EntityEvents_48_68), // 67
	TABLE_ENTRY(EntityEvents_48_68), // 68
	TABLE_ENTRY(EntityEvents_73p), // 73
	TABLE_ENTRY(EntityEvents_73p), // 90
	TABLE_ENTRY(EntityEvents_73p) // 91
};
VALIDATE_TABLE_SIZE(EntityEventTables);

static s16 ConfigStringIndices_3_U2Q[udtConfigStringIndex::Count];
static s16 ConfigStringIndices_3_Q2U[udtConfigStringIndex::Count * 2];
static const s16 ConfigStringIndices_3[] =
{
	(s16)udtConfigStringIndex::FirstPlayer, 544,
	(s16)udtConfigStringIndex::Intermission, 14,
	(s16)udtConfigStringIndex::LevelStartTime, 13,
	(s16)udtConfigStringIndex::WarmUpEndTime, 5,
	(s16)udtConfigStringIndex::FlagStatus, 15,
	(s16)udtConfigStringIndex::ServerInfo, 0,
	(s16)udtConfigStringIndex::SystemInfo, 1,
	(s16)udtConfigStringIndex::Scores1, 6,
	(s16)udtConfigStringIndex::Scores2, 7,
	(s16)udtConfigStringIndex::VoteTime, 8,
	(s16)udtConfigStringIndex::VoteString, 9,
	(s16)udtConfigStringIndex::VoteYes, 10,
	(s16)udtConfigStringIndex::VoteNo, 11,
	(s16)udtConfigStringIndex::GameVersion, 12,
	(s16)udtConfigStringIndex::ItemFlags, 27,
	(s16)udtConfigStringIndex::QL_TimeoutStartTime, 669,
	(s16)udtConfigStringIndex::QL_TimeoutEndTime, 670,
	(s16)udtConfigStringIndex::QL_ReadTeamClanName, 693,
	(s16)udtConfigStringIndex::QL_BlueTeamClanName, 694,
	(s16)udtConfigStringIndex::QL_RedTeamClanTag, 695,
	(s16)udtConfigStringIndex::QL_BlueTeamClanTag, 696,
	(s16)udtConfigStringIndex::CPMA_GameInfo, 672,
	(s16)udtConfigStringIndex::CPMA_RoundInfo, 710,
	(s16)udtConfigStringIndex::OSP_GamePlay, 806,
	TABLE_END
};
VALIDATE_TABLE_SIZES(ConfigStringIndices_3, udtConfigStringIndex::Count);

static s16 ConfigStringIndices_48_68_U2Q[udtConfigStringIndex::Count];
static s16 ConfigStringIndices_48_68_Q2U[udtConfigStringIndex::Count * 2];
static const s16 ConfigStringIndices_48_68[] =
{
	(s16)udtConfigStringIndex::FirstPlayer, 544,
	(s16)udtConfigStringIndex::Intermission, 22,
	(s16)udtConfigStringIndex::LevelStartTime, 21,
	(s16)udtConfigStringIndex::WarmUpEndTime, 5,
	(s16)udtConfigStringIndex::FlagStatus, 23,
	(s16)udtConfigStringIndex::ServerInfo, 0,
	(s16)udtConfigStringIndex::SystemInfo, 1,
	(s16)udtConfigStringIndex::Scores1, 6,
	(s16)udtConfigStringIndex::Scores2, 7,
	(s16)udtConfigStringIndex::VoteTime, 8,
	(s16)udtConfigStringIndex::VoteString, 9,
	(s16)udtConfigStringIndex::VoteYes, 10,
	(s16)udtConfigStringIndex::VoteNo, 11,
	(s16)udtConfigStringIndex::TeamVoteTime, 12,
	(s16)udtConfigStringIndex::TeamVoteString, 14,
	(s16)udtConfigStringIndex::TeamVoteYes, 16,
	(s16)udtConfigStringIndex::TeamVoteNo, 18,
	(s16)udtConfigStringIndex::GameVersion, 20,
	(s16)udtConfigStringIndex::ItemFlags, 27,
	(s16)udtConfigStringIndex::QL_TimeoutStartTime, 669,
	(s16)udtConfigStringIndex::QL_TimeoutEndTime, 670,
	(s16)udtConfigStringIndex::QL_ReadTeamClanName, 693,
	(s16)udtConfigStringIndex::QL_BlueTeamClanName, 694,
	(s16)udtConfigStringIndex::QL_RedTeamClanTag, 695,
	(s16)udtConfigStringIndex::QL_BlueTeamClanTag, 696,
	(s16)udtConfigStringIndex::CPMA_GameInfo, 672,
	(s16)udtConfigStringIndex::CPMA_RoundInfo, 710,
	(s16)udtConfigStringIndex::OSP_GamePlay, 806,
	TABLE_END
};
VALIDATE_TABLE_SIZES(ConfigStringIndices_48_68, udtConfigStringIndex::Count);

static s16 ConfigStringIndices_73_90_U2Q[udtConfigStringIndex::Count];
static s16 ConfigStringIndices_73_90_Q2U[udtConfigStringIndex::Count * 2];
static const s16 ConfigStringIndices_73_90[] =
{
	(s16)udtConfigStringIndex::FirstPlayer, 529,
	(s16)udtConfigStringIndex::Intermission, 14,
	(s16)udtConfigStringIndex::LevelStartTime, 13,
	(s16)udtConfigStringIndex::WarmUpEndTime, 5,
	(s16)udtConfigStringIndex::PauseStart, 669,
	(s16)udtConfigStringIndex::PauseEnd, 670,
	(s16)udtConfigStringIndex::FlagStatus, 658,
	(s16)udtConfigStringIndex::ServerInfo, 0,
	(s16)udtConfigStringIndex::SystemInfo, 1,
	(s16)udtConfigStringIndex::Scores1, 6,
	(s16)udtConfigStringIndex::Scores2, 7,
	(s16)udtConfigStringIndex::VoteTime, 8,
	(s16)udtConfigStringIndex::VoteString, 9,
	(s16)udtConfigStringIndex::VoteYes, 10,
	(s16)udtConfigStringIndex::VoteNo, 11,
	(s16)udtConfigStringIndex::GameVersion, 12,
	(s16)udtConfigStringIndex::ItemFlags, 15,
	(s16)udtConfigStringIndex::QL_TimeoutStartTime, 669,
	(s16)udtConfigStringIndex::QL_TimeoutEndTime, 670,
	(s16)udtConfigStringIndex::QL_ReadTeamClanName, 693,
	(s16)udtConfigStringIndex::QL_BlueTeamClanName, 694,
	(s16)udtConfigStringIndex::QL_RedTeamClanTag, 695,
	(s16)udtConfigStringIndex::QL_BlueTeamClanTag, 696,
	TABLE_END
};
VALIDATE_TABLE_SIZES(ConfigStringIndices_73_90, udtConfigStringIndex::Count);

#if 0 // ET
static s16 ConfigStringIndices_84_U2Q[udtConfigStringIndex::Count];
static s16 ConfigStringIndices_84_Q2U[udtConfigStringIndex::Count * 2];
static const s16 ConfigStringIndices_84[] =
{
	(s16)udtConfigStringIndex::FirstPlayer, 689,
	(s16)udtConfigStringIndex::Intermission, 12,
	(s16)udtConfigStringIndex::LevelStartTime, 11,
	(s16)udtConfigStringIndex::WarmUpEndTime, 5,
	(s16)udtConfigStringIndex::ServerInfo, 0,
	(s16)udtConfigStringIndex::SystemInfo, 1,
	(s16)udtConfigStringIndex::GameVersion, 10,
	TABLE_END
};
VALIDATE_TABLE_SIZES(ConfigStringIndices_84, udtConfigStringIndex::Count);
#endif

static s16 ConfigStringIndices_91_U2Q[udtConfigStringIndex::Count];
static s16 ConfigStringIndices_91_Q2U[udtConfigStringIndex::Count * 2];
static const s16 ConfigStringIndices_91[] =
{
	(s16)udtConfigStringIndex::FirstPlayer, 529,
	(s16)udtConfigStringIndex::Intermission, 14,
	(s16)udtConfigStringIndex::LevelStartTime, 13,
	(s16)udtConfigStringIndex::WarmUpEndTime, 5,
	(s16)udtConfigStringIndex::FirstPlacePlayerName, 659,
	(s16)udtConfigStringIndex::SecondPlacePlayerName, 660,
	(s16)udtConfigStringIndex::PauseStart, 669,
	(s16)udtConfigStringIndex::PauseEnd, 670,
	(s16)udtConfigStringIndex::FlagStatus, 658,
	(s16)udtConfigStringIndex::ServerInfo, 0,
	(s16)udtConfigStringIndex::SystemInfo, 1,
	(s16)udtConfigStringIndex::Scores1, 6,
	(s16)udtConfigStringIndex::Scores2, 7,
	(s16)udtConfigStringIndex::VoteTime, 8,
	(s16)udtConfigStringIndex::VoteString, 9,
	(s16)udtConfigStringIndex::VoteYes, 10,
	(s16)udtConfigStringIndex::VoteNo, 11,
	(s16)udtConfigStringIndex::GameVersion, 12,
	(s16)udtConfigStringIndex::ItemFlags, 15,
	(s16)udtConfigStringIndex::QL_RedTeamTimeoutsLeft, 671,
	(s16)udtConfigStringIndex::QL_BlueTeamTimeoutsLeft, 672,
	TABLE_END
};
VALIDATE_TABLE_SIZES(ConfigStringIndices_91, udtConfigStringIndex::Count);

static s16 ConfigStringIndices_60_U2Q[udtConfigStringIndex::Count];
static s16 ConfigStringIndices_60_Q2U[udtConfigStringIndex::Count * 2];
static const s16 ConfigStringIndices_60[] =
{
	(s16)udtConfigStringIndex::FirstPlayer, 576,
	(s16)udtConfigStringIndex::Intermission, 14,
	(s16)udtConfigStringIndex::LevelStartTime, 13,
	(s16)udtConfigStringIndex::WarmUpEndTime, 5,
	(s16)udtConfigStringIndex::ServerInfo, 0,
	(s16)udtConfigStringIndex::SystemInfo, 1,
	(s16)udtConfigStringIndex::Scores1, 6,
	(s16)udtConfigStringIndex::Scores2, 7,
	(s16)udtConfigStringIndex::VoteTime, 8,
	(s16)udtConfigStringIndex::VoteString, 9,
	(s16)udtConfigStringIndex::VoteYes, 10,
	(s16)udtConfigStringIndex::VoteNo, 11,
	TABLE_END
};
VALIDATE_TABLE_SIZES(ConfigStringIndices_60, udtConfigStringIndex::Count);

static const s16* ConfigStringIndexTables[] =
{
	TABLE_ENTRY(ConfigStringIndices_3), // 3
	TABLE_ENTRY(ConfigStringIndices_48_68), // 48
	TABLE_ENTRY(ConfigStringIndices_60), // 60
	TABLE_ENTRY(ConfigStringIndices_48_68), // 66
	TABLE_ENTRY(ConfigStringIndices_48_68), // 67
	TABLE_ENTRY(ConfigStringIndices_48_68), // 68
	TABLE_ENTRY(ConfigStringIndices_73_90), // 73
	TABLE_ENTRY(ConfigStringIndices_73_90), // 90
	TABLE_ENTRY(ConfigStringIndices_91) // 91
};
VALIDATE_TABLE_SIZE(ConfigStringIndexTables);

static s16 Teams_U2Q[udtTeam::Count];
static s16 Teams_Q2U[udtTeam::Count * 2];
static const s16 Teams[] =
{
	(s16)udtTeam::Free, 0,
	(s16)udtTeam::Red, 1,
	(s16)udtTeam::Blue, 2,
	(s16)udtTeam::Spectators, 3,
	TABLE_END
};
VALIDATE_TABLE_SIZES(Teams, udtTeam::Count);

static const s16* TeamTables[] =
{
	TABLE_ENTRY(Teams), // 3
	TABLE_ENTRY(Teams), // 48
	TABLE_ENTRY(Teams), // 60 @TODO: RTCW
	TABLE_ENTRY(Teams), // 66
	TABLE_ENTRY(Teams), // 67
	TABLE_ENTRY(Teams), // 68
	TABLE_ENTRY(Teams), // 73
	TABLE_ENTRY(Teams), // 90
	TABLE_ENTRY(Teams) // 91
};
VALIDATE_TABLE_SIZE(TeamTables);

static s16 GameTypes_3_U2Q[udtGameType::Count];
static s16 GameTypes_3_Q2U[udtGameType::Count * 2];
static const s16 GameTypes_3[] =
{
	(s16)udtGameType::FFA, 0,
	(s16)udtGameType::Duel, 1,
	(s16)udtGameType::TDM, 3,
	(s16)udtGameType::CTF, 4,
	TABLE_END
};
VALIDATE_TABLE_SIZES(GameTypes_3, udtGameType::Count);

static s16 GameTypes_48_68_U2Q[udtGameType::Count];
static s16 GameTypes_48_68_Q2U[udtGameType::Count * 2];
static const s16 GameTypes_48_68[] =
{
	(s16)udtGameType::FFA, 0,
	(s16)udtGameType::Duel, 1,
	(s16)udtGameType::TDM, 3,
	(s16)udtGameType::CTF, 4,
	(s16)udtGameType::OneFlagCTF, 5,
	(s16)udtGameType::Obelisk, 6,
	(s16)udtGameType::Harvester, 7,
	TABLE_END
};
VALIDATE_TABLE_SIZES(GameTypes_48_68, udtGameType::Count);

static s16 GameTypes_73p_U2Q[udtGameType::Count];
static s16 GameTypes_73p_Q2U[udtGameType::Count * 2];
static const s16 GameTypes_73p[] =
{
	(s16)udtGameType::FFA, 0,
	(s16)udtGameType::Duel, 1,
	(s16)udtGameType::Race, 2,
	(s16)udtGameType::RedRover, 12,
	(s16)udtGameType::TDM, 3,
	(s16)udtGameType::CA, 4,
	(s16)udtGameType::CTF, 5,
	(s16)udtGameType::OneFlagCTF, 6,
	(s16)udtGameType::Obelisk, 7,
	(s16)udtGameType::Harvester, 8,
	(s16)udtGameType::Domination, 10,
	(s16)udtGameType::CTFS, 11,
	(s16)udtGameType::FT, 9,
	TABLE_END
};
VALIDATE_TABLE_SIZES(GameTypes_73p, udtGameType::Count);

static const s16* GameTypeTables[] =
{
	TABLE_ENTRY(GameTypes_3), // 3
	TABLE_ENTRY(GameTypes_48_68), // 48
	TABLE_ENTRY(GameTypes_48_68), // 60 @TODO: RTCW
	TABLE_ENTRY(GameTypes_48_68), // 66
	TABLE_ENTRY(GameTypes_48_68), // 67
	TABLE_ENTRY(GameTypes_48_68), // 68
	TABLE_ENTRY(GameTypes_73p), // 73
	TABLE_ENTRY(GameTypes_73p), // 90
	TABLE_ENTRY(GameTypes_73p) // 91
};
VALIDATE_TABLE_SIZE(GameTypeTables);

static s16 FlagStatus_U2Q[udtFlagStatus::Count];
static s16 FlagStatus_Q2U[udtFlagStatus::Count * 2];
static const s16 FlagStatus[] =
{
	(s16)udtFlagStatus::InBase, 0,
	(s16)udtFlagStatus::Carried, 1,
	(s16)udtFlagStatus::Missing, 2,
	TABLE_END
};
VALIDATE_TABLE_SIZES(FlagStatus, udtFlagStatus::Count);

static const s16* FlagStatusTables[] =
{
	TABLE_ENTRY(FlagStatus), // 3
	TABLE_ENTRY(FlagStatus), // 48
	TABLE_ENTRY(FlagStatus), // 60 @TODO: RTCW
	TABLE_ENTRY(FlagStatus), // 66
	TABLE_ENTRY(FlagStatus), // 67
	TABLE_ENTRY(FlagStatus), // 68
	TABLE_ENTRY(FlagStatus), // 73
	TABLE_ENTRY(FlagStatus), // 90
	TABLE_ENTRY(FlagStatus) // 91
};
VALIDATE_TABLE_SIZE(FlagStatusTables);

static s16 Weapons_3_68_U2Q[udtWeapon::Count];
static s16 Weapons_3_68_Q2U[udtWeapon::Count * 2];
static const s16 Weapons_3_68[] =
{
	(s16)udtWeapon::Gauntlet, 1,
	(s16)udtWeapon::MachineGun, 2,
	(s16)udtWeapon::Shotgun, 3,
	(s16)udtWeapon::GrenadeLauncher, 4,
	(s16)udtWeapon::RocketLauncher, 5,
	(s16)udtWeapon::PlasmaGun, 8,
	(s16)udtWeapon::Railgun, 7,
	(s16)udtWeapon::LightningGun, 6,
	(s16)udtWeapon::BFG, 9,
	(s16)udtWeapon::GrapplingHook, 10,
	TABLE_END
};
VALIDATE_TABLE_SIZES(Weapons_3_68, udtWeapon::Count);

static s16 Weapons_73p_U2Q[udtWeapon::Count];
static s16 Weapons_73p_Q2U[udtWeapon::Count * 2];
static const s16 Weapons_73p[] =
{
	(s16)udtWeapon::Gauntlet, 1,
	(s16)udtWeapon::MachineGun, 2,
	(s16)udtWeapon::Shotgun, 3,
	(s16)udtWeapon::GrenadeLauncher, 4,
	(s16)udtWeapon::RocketLauncher, 5,
	(s16)udtWeapon::PlasmaGun, 8,
	(s16)udtWeapon::Railgun, 7,
	(s16)udtWeapon::LightningGun, 6,
	(s16)udtWeapon::BFG, 9,
	(s16)udtWeapon::NailGun, 11,
	(s16)udtWeapon::ChainGun, 13,
	(s16)udtWeapon::ProximityMineLauncher, 12,
	(s16)udtWeapon::HeavyMachineGun, 14,
	(s16)udtWeapon::GrapplingHook, 10,
	TABLE_END
};
VALIDATE_TABLE_SIZES(Weapons_73p, udtWeapon::Count);

static const s16* WeaponTables[] =
{
	TABLE_ENTRY(Weapons_3_68), // 3
	TABLE_ENTRY(Weapons_3_68), // 48
	TABLE_ENTRY(Weapons_3_68), // 60 @TODO: RTCW
	TABLE_ENTRY(Weapons_3_68), // 66
	TABLE_ENTRY(Weapons_3_68), // 67
	TABLE_ENTRY(Weapons_3_68), // 68
	TABLE_ENTRY(Weapons_73p), // 73
	TABLE_ENTRY(Weapons_73p), // 90
	TABLE_ENTRY(Weapons_73p) // 91
};
VALIDATE_TABLE_SIZE(WeaponTables);

static s16 MeansOfDeath_3_68_U2Q[udtMeanOfDeath::Count];
static s16 MeansOfDeath_3_68_Q2U[udtMeanOfDeath::Count * 2];
static const s16 MeansOfDeath_3_68[] =
{
	(s16)udtMeanOfDeath::Shotgun, 1,
	(s16)udtMeanOfDeath::Gauntlet, 2,
	(s16)udtMeanOfDeath::MachineGun, 3,
	(s16)udtMeanOfDeath::Grenade, 4,
	(s16)udtMeanOfDeath::GrenadeSplash, 5,
	(s16)udtMeanOfDeath::Rocket, 6,
	(s16)udtMeanOfDeath::RocketSplash, 7,
	(s16)udtMeanOfDeath::Plasma, 8,
	(s16)udtMeanOfDeath::PlasmaSplash, 9,
	(s16)udtMeanOfDeath::Railgun, 10,
	(s16)udtMeanOfDeath::Lightning, 11,
	(s16)udtMeanOfDeath::BFG, 12,
	(s16)udtMeanOfDeath::BFGSplash, 13,
	(s16)udtMeanOfDeath::Water, 14,
	(s16)udtMeanOfDeath::Slime, 15,
	(s16)udtMeanOfDeath::Lava, 16,
	(s16)udtMeanOfDeath::Crush, 17,
	(s16)udtMeanOfDeath::TeleFrag, 18,
	(s16)udtMeanOfDeath::Fall, 19,
	(s16)udtMeanOfDeath::Suicide, 20,
	(s16)udtMeanOfDeath::TargetLaser, 21,
	(s16)udtMeanOfDeath::TriggerHurt, 22,
	(s16)udtMeanOfDeath::Grapple, 23,
	TABLE_END
};
VALIDATE_TABLE_SIZES(MeansOfDeath_3_68, udtMeanOfDeath::Count);

static s16 MeansOfDeath_73p_U2Q[udtMeanOfDeath::Count];
static s16 MeansOfDeath_73p_Q2U[udtMeanOfDeath::Count * 2];
static const s16 MeansOfDeath_73p[] =
{
	(s16)udtMeanOfDeath::Shotgun, 1,
	(s16)udtMeanOfDeath::Gauntlet, 2,
	(s16)udtMeanOfDeath::MachineGun, 3,
	(s16)udtMeanOfDeath::Grenade, 4,
	(s16)udtMeanOfDeath::GrenadeSplash, 5,
	(s16)udtMeanOfDeath::Rocket, 6,
	(s16)udtMeanOfDeath::RocketSplash, 7,
	(s16)udtMeanOfDeath::Plasma, 8,
	(s16)udtMeanOfDeath::PlasmaSplash, 9,
	(s16)udtMeanOfDeath::Railgun, 10,
	(s16)udtMeanOfDeath::Lightning, 11,
	(s16)udtMeanOfDeath::BFG, 12,
	(s16)udtMeanOfDeath::BFGSplash, 13,
	(s16)udtMeanOfDeath::Water, 14,
	(s16)udtMeanOfDeath::Slime, 15,
	(s16)udtMeanOfDeath::Lava, 16,
	(s16)udtMeanOfDeath::Crush, 17,
	(s16)udtMeanOfDeath::TeleFrag, 18,
	(s16)udtMeanOfDeath::Fall, 19,
	(s16)udtMeanOfDeath::Suicide, 20,
	(s16)udtMeanOfDeath::TargetLaser, 21,
	(s16)udtMeanOfDeath::TriggerHurt, 22,
	(s16)udtMeanOfDeath::NailGun, 23,
	(s16)udtMeanOfDeath::ChainGun, 24,
	(s16)udtMeanOfDeath::ProximityMine, 25,
	(s16)udtMeanOfDeath::Kamikaze, 26,
	(s16)udtMeanOfDeath::Juiced, 27,
	(s16)udtMeanOfDeath::Grapple, 28,
	(s16)udtMeanOfDeath::TeamSwitch, 29,
	(s16)udtMeanOfDeath::Thaw, 30,
	(s16)udtMeanOfDeath::HeavyMachineGun, 32,
	TABLE_END
};
VALIDATE_TABLE_SIZES(MeansOfDeath_73p, udtMeanOfDeath::Count);

static s16 MeansOfDeath_60_U2Q[udtMeanOfDeath::Count];
static s16 MeansOfDeath_60_Q2U[udtMeanOfDeath::Count * 2];
static const s16 MeansOfDeath_60[] =
{
	// @TODO:
	TABLE_END
};
VALIDATE_TABLE_SIZES(MeansOfDeath_60, udtMeanOfDeath::Count);

static const s16* MeanOfDeathTables[] =
{
	TABLE_ENTRY(MeansOfDeath_3_68), // 3
	TABLE_ENTRY(MeansOfDeath_3_68), // 48
	TABLE_ENTRY(MeansOfDeath_60), // 60
	TABLE_ENTRY(MeansOfDeath_3_68), // 66
	TABLE_ENTRY(MeansOfDeath_3_68), // 67
	TABLE_ENTRY(MeansOfDeath_3_68), // 68
	TABLE_ENTRY(MeansOfDeath_73p), // 73
	TABLE_ENTRY(MeansOfDeath_73p), // 90
	TABLE_ENTRY(MeansOfDeath_73p) // 91
};
VALIDATE_TABLE_SIZE(MeanOfDeathTables);

static s16 Items_3_68_U2Q[udtItem::Count];
static s16 Items_3_68_Q2U[udtItem::Count * 2];
static const s16 Items_3_68[] =
{
	(s16)udtItem::AmmoBFG, 25,
	(s16)udtItem::AmmoBullets, 19,
	(s16)udtItem::AmmoCells, 21,
	(s16)udtItem::AmmoGrenades, 20,
	(s16)udtItem::AmmoLightning, 22,
	(s16)udtItem::AmmoRockets, 23,
	(s16)udtItem::AmmoShells, 18,
	(s16)udtItem::AmmoSlugs, 24,
	(s16)udtItem::HoldableMedkit, 27,
	(s16)udtItem::HoldableTeleporter, 26,
	(s16)udtItem::ItemArmorBody, 3,
	(s16)udtItem::ItemArmorCombat, 2,
	(s16)udtItem::ItemArmorShard, 1,
	(s16)udtItem::ItemEnviro, 29,
	(s16)udtItem::ItemFlight, 33,
	(s16)udtItem::ItemHaste, 30,
	(s16)udtItem::ItemHealth, 5,
	(s16)udtItem::ItemHealthLarge, 6,
	(s16)udtItem::ItemHealthMega, 7,
	(s16)udtItem::ItemHealthSmall, 4,
	(s16)udtItem::ItemInvis, 31,
	(s16)udtItem::ItemQuad, 28,
	(s16)udtItem::ItemRegen, 32,
	(s16)udtItem::FlagBlue, 35,
	(s16)udtItem::FlagRed, 34,
	(s16)udtItem::WeaponBFG, 16,
	(s16)udtItem::WeaponGauntlet, 8,
	(s16)udtItem::WeaponGrapplingHook, 17,
	(s16)udtItem::WeaponGrenadeLauncher, 11,
	(s16)udtItem::WeaponLightningGun, 13,
	(s16)udtItem::WeaponMachinegun, 10,
	(s16)udtItem::WeaponPlasmaGun, 15,
	(s16)udtItem::WeaponRailgun, 14,
	(s16)udtItem::WeaponRocketLauncher, 12,
	(s16)udtItem::WeaponShotgun, 9,
	TABLE_END
};
VALIDATE_TABLE_SIZES(Items_3_68, udtItem::Count);

static s16 Items_73_U2Q[udtItem::Count];
static s16 Items_73_Q2U[udtItem::Count * 2];
static const s16 Items_73[] =
{
	(s16)udtItem::AmmoBFG, 26,
	(s16)udtItem::AmmoBelt, 42,
	(s16)udtItem::AmmoBullets, 20,
	(s16)udtItem::AmmoCells, 22,
	(s16)udtItem::AmmoGrenades, 21,
	(s16)udtItem::AmmoLightning, 23,
	(s16)udtItem::AmmoMines, 41,
	(s16)udtItem::AmmoNails, 40,
	(s16)udtItem::AmmoRockets, 24,
	(s16)udtItem::AmmoShells, 19,
	(s16)udtItem::AmmoSlugs, 25,
	(s16)udtItem::HoldableInvulnerability, 39,
	(s16)udtItem::HoldableKamikaze, 37,
	(s16)udtItem::HoldableMedkit, 28,
	(s16)udtItem::HoldablePortal, 38,
	(s16)udtItem::HoldableTeleporter, 27,
	(s16)udtItem::ItemAmmoRegen, 46,
	(s16)udtItem::ItemArmorBody, 3,
	(s16)udtItem::ItemArmorCombat, 2,
	(s16)udtItem::ItemArmorJacket, 4,
	(s16)udtItem::ItemArmorShard, 1,
	(s16)udtItem::ItemBlueCube, 49,
	(s16)udtItem::ItemDoubler, 45,
	(s16)udtItem::ItemEnviro, 30,
	(s16)udtItem::ItemFlight, 34,
	(s16)udtItem::ItemGuard, 44,
	(s16)udtItem::ItemHaste, 31,
	(s16)udtItem::ItemHealth, 6,
	(s16)udtItem::ItemHealthLarge, 7,
	(s16)udtItem::ItemHealthMega, 8,
	(s16)udtItem::ItemHealthSmall, 5,
	(s16)udtItem::ItemInvis, 32,
	(s16)udtItem::ItemQuad, 29,
	(s16)udtItem::ItemRedCube, 48,
	(s16)udtItem::ItemRegen, 33,
	(s16)udtItem::ItemScout, 43,
	(s16)udtItem::FlagBlue, 36,
	(s16)udtItem::FlagNeutral, 47,
	(s16)udtItem::FlagRed, 35,
	(s16)udtItem::WeaponBFG, 17,
	(s16)udtItem::WeaponChaingun, 52,
	(s16)udtItem::WeaponGauntlet, 9,
	(s16)udtItem::WeaponGrapplingHook, 18,
	(s16)udtItem::WeaponGrenadeLauncher, 12,
	(s16)udtItem::WeaponLightningGun, 14,
	(s16)udtItem::WeaponMachinegun, 11,
	(s16)udtItem::WeaponNailgun, 50,
	(s16)udtItem::WeaponPlasmaGun, 16,
	(s16)udtItem::WeaponProxLauncher, 51,
	(s16)udtItem::WeaponRailgun, 15,
	(s16)udtItem::WeaponRocketLauncher, 13,
	(s16)udtItem::WeaponShotgun, 10,
	TABLE_END
};
VALIDATE_TABLE_SIZES(Items_73, udtItem::Count);

static s16 Items_90p_U2Q[udtItem::Count];
static s16 Items_90p_Q2U[udtItem::Count * 2];
static const s16 Items_90p[] =
{
	(s16)udtItem::AmmoBFG, 26,
	(s16)udtItem::AmmoBelt, 42,
	(s16)udtItem::AmmoBullets, 20,
	(s16)udtItem::AmmoCells, 22,
	(s16)udtItem::AmmoGrenades, 21,
	(s16)udtItem::AmmoHMG, 55,
	(s16)udtItem::AmmoLightning, 23,
	(s16)udtItem::AmmoMines, 41,
	(s16)udtItem::AmmoNails, 40,
	(s16)udtItem::AmmoPack, 56,
	(s16)udtItem::AmmoRockets, 24,
	(s16)udtItem::AmmoShells, 19,
	(s16)udtItem::AmmoSlugs, 25,
	(s16)udtItem::HoldableInvulnerability, 39,
	(s16)udtItem::HoldableKamikaze, 37,
	(s16)udtItem::HoldableMedkit, 28,
	(s16)udtItem::HoldablePortal, 38,
	(s16)udtItem::HoldableTeleporter, 27,
	(s16)udtItem::ItemAmmoRegen, 46,
	(s16)udtItem::ItemArmorBody, 3,
	(s16)udtItem::ItemArmorCombat, 2,
	(s16)udtItem::ItemArmorJacket, 4,
	(s16)udtItem::ItemArmorShard, 1,
	(s16)udtItem::ItemBlueCube, 49,
	(s16)udtItem::ItemDoubler, 45,
	(s16)udtItem::ItemEnviro, 30,
	(s16)udtItem::ItemFlight, 34,
	(s16)udtItem::ItemGuard, 44,
	(s16)udtItem::ItemHaste, 31,
	(s16)udtItem::ItemHealth, 6,
	(s16)udtItem::ItemHealthLarge, 7,
	(s16)udtItem::ItemHealthMega, 8,
	(s16)udtItem::ItemHealthSmall, 5,
	(s16)udtItem::ItemInvis, 32,
	(s16)udtItem::ItemKeyGold, 58,
	(s16)udtItem::ItemKeyMaster, 59,
	(s16)udtItem::ItemKeySilver, 57,
	(s16)udtItem::ItemQuad, 29,
	(s16)udtItem::ItemRedCube, 48,
	(s16)udtItem::ItemRegen, 33,
	(s16)udtItem::ItemScout, 43,
	(s16)udtItem::ItemSpawnArmor, 53,
	(s16)udtItem::FlagBlue, 36,
	(s16)udtItem::FlagNeutral, 47,
	(s16)udtItem::FlagRed, 35,
	(s16)udtItem::WeaponBFG, 17,
	(s16)udtItem::WeaponChaingun, 52,
	(s16)udtItem::WeaponGauntlet, 9,
	(s16)udtItem::WeaponGrapplingHook, 18,
	(s16)udtItem::WeaponGrenadeLauncher, 12,
	(s16)udtItem::WeaponHMG, 54,
	(s16)udtItem::WeaponLightningGun, 14,
	(s16)udtItem::WeaponMachinegun, 11,
	(s16)udtItem::WeaponNailgun, 50,
	(s16)udtItem::WeaponPlasmaGun, 16,
	(s16)udtItem::WeaponProxLauncher, 51,
	(s16)udtItem::WeaponRailgun, 15,
	(s16)udtItem::WeaponRocketLauncher, 13,
	(s16)udtItem::WeaponShotgun, 10,
	TABLE_END
};
VALIDATE_TABLE_SIZES(Items_90p, udtItem::Count);

static const s16* ItemTables[] =
{
	TABLE_ENTRY(Items_3_68), // 3
	TABLE_ENTRY(Items_3_68), // 48
	TABLE_ENTRY(Items_3_68), // @TODO: RTCW
	TABLE_ENTRY(Items_3_68), // 66
	TABLE_ENTRY(Items_3_68), // 67
	TABLE_ENTRY(Items_3_68), // 68
	TABLE_ENTRY(Items_73), // 73
	TABLE_ENTRY(Items_90p), // 90
	TABLE_ENTRY(Items_90p) // 91
};
VALIDATE_TABLE_SIZE(ItemTables);

static s16 PMTypes_U2Q[udtPlayerMovementType::Count];
static s16 PMTypes_Q2U[udtPlayerMovementType::Count * 2];
static const s16 PMTypes[] =
{
	(s16)udtPlayerMovementType::Normal, 0,
	(s16)udtPlayerMovementType::NoClip, 1,
	(s16)udtPlayerMovementType::Spectator, 2,
	(s16)udtPlayerMovementType::Dead, 3,
	(s16)udtPlayerMovementType::Freeze, 4,
	(s16)udtPlayerMovementType::Intermission, 5,
	(s16)udtPlayerMovementType::SPIntermission, 6,
	TABLE_END
};
VALIDATE_TABLE_SIZES(PMTypes, udtPlayerMovementType::Count);

static const s16* PMTypeTables[] =
{
	TABLE_ENTRY(PMTypes), // 3
	TABLE_ENTRY(PMTypes), // 48
	TABLE_ENTRY(PMTypes), // @TODO: RTCW
	TABLE_ENTRY(PMTypes), // 66
	TABLE_ENTRY(PMTypes), // 67
	TABLE_ENTRY(PMTypes), // 68
	TABLE_ENTRY(PMTypes), // 73
	TABLE_ENTRY(PMTypes), // 90
	TABLE_ENTRY(PMTypes) // 91
};
VALIDATE_TABLE_SIZE(PMTypeTables);

struct MagicNumberTableGroup
{
	MagicNumberTableGroup(udtMagicNumberType::Id type, const s16** tables, u32 count)
		: Tables(tables)
		, Count(count)
		, Type(type)
	{
	}

	const s16** Tables;
	u32 Count;
	udtMagicNumberType::Id Type;
};

static const MagicNumberTableGroup MagicNumberTables[udtMagicNumberType::Count] =
{
	MagicNumberTableGroup(udtMagicNumberType::PowerUpIndex, PowerUpTables, udtPowerUpIndex::Count),
	MagicNumberTableGroup(udtMagicNumberType::LifeStatsIndex, LifeStatsTables, udtLifeStatsIndex::Count),
	MagicNumberTableGroup(udtMagicNumberType::PersStatsIndex, PersStatsTables, udtPersStatsIndex::Count),
	MagicNumberTableGroup(udtMagicNumberType::EntityType, EntityTypeTables, udtEntityType::Count),
	MagicNumberTableGroup(udtMagicNumberType::EntityFlag, EntityFlagBitTables, udtEntityFlag::Count),
	MagicNumberTableGroup(udtMagicNumberType::EntityEvent, EntityEventTables, udtEntityEvent::Count),
	MagicNumberTableGroup(udtMagicNumberType::ConfigStringIndex, ConfigStringIndexTables, udtConfigStringIndex::Count),
	MagicNumberTableGroup(udtMagicNumberType::Team, TeamTables, udtTeam::Count),
	MagicNumberTableGroup(udtMagicNumberType::GameType, GameTypeTables, udtGameType::Count),
	MagicNumberTableGroup(udtMagicNumberType::FlagStatus, FlagStatusTables, udtFlagStatus::Count),
	MagicNumberTableGroup(udtMagicNumberType::Weapon, WeaponTables, udtWeapon::Count),
	MagicNumberTableGroup(udtMagicNumberType::MeanOfDeath, MeanOfDeathTables, udtMeanOfDeath::Count),
	MagicNumberTableGroup(udtMagicNumberType::Item, ItemTables, udtItem::Count),
	MagicNumberTableGroup(udtMagicNumberType::PlayerMovementType, PMTypeTables, udtPlayerMovementType::Count)
};


#define UNDEFINED UDT_S16_MIN


static int SortCallback(const void* a, const void* b)
{
	return *((const s16*)a + 1) - *((const s16*)b + 1);
}

void BuildLookUpTables()
{
	for(u32 mnt = 0; mnt < (u32)udtMagicNumberType::Count; ++mnt)
	{
		const MagicNumberTableGroup& tableGroup = MagicNumberTables[mnt];
		s16* prevTable_U2Q = NULL;
		s16* prevTable_Q2U = NULL;
		for(u32 p = 0; p < (u32)udtProtocol::Count; ++p)
		{
			const s16* const table = tableGroup.Tables[3 * p + 0];
			s16* const table_U2Q = (s16*)tableGroup.Tables[3 * p + 1];
			s16* const table_Q2U = (s16*)tableGroup.Tables[3 * p + 2];
			if(table_U2Q != prevTable_U2Q)
			{
				for(u32 i = 0; i < tableGroup.Count; ++i)
				{
					table_U2Q[i] = UNDEFINED;
				}
				for(u32 i = 0; i < tableGroup.Count; ++i)
				{
					const s16 newIdx = table[2 * i + 0];
					if(newIdx < 0)
					{
						break;
					}
					table_U2Q[newIdx] = table[2 * i + 1];
				}
			}
			if(table_Q2U != prevTable_Q2U)
			{
				for(u32 i = 0; i < tableGroup.Count; ++i)
				{
					s16 newIdx = (s16)i;
					s16 newVal = UNDEFINED;
					for(u32 j = 0; j < tableGroup.Count; ++j)
					{
						if(table[2 * j + 0] == newIdx)
						{
							newVal = table[2 * j + 1];
							break;
						}
					}
					table_Q2U[2 * i + 0] = newIdx;
					table_Q2U[2 * i + 1] = newVal;
				}
				qsort(table_Q2U, (size_t)tableGroup.Count, 2 * sizeof(s16), &SortCallback);
			}
			prevTable_U2Q = table_U2Q;
			prevTable_Q2U = table_Q2U;
		}
	}
}

struct idGameType68_CPMA
{
	enum Id
	{
		HM = -1,
		FFA = 0,
		Duel = 1,
		SP = 2,
		TDM = 3,
		CTF = 4,
		CA = 5,
		FT = 6,
		CTFS = 7,
		NTF = 8,
		TwoVsTwo = 9,
		Count
	};
};

static s32 GetUDTGameTypeCPMA(s32 gt)
{
	switch((idGameType68_CPMA::Id)gt)
	{
		case idGameType68_CPMA::HM: return (s32)udtGameType::HM;
		case idGameType68_CPMA::FFA: return (s32)udtGameType::FFA;
		case idGameType68_CPMA::Duel: return (s32)udtGameType::Duel;
		case idGameType68_CPMA::SP: return (s32)udtGameType::SP;
		case idGameType68_CPMA::TDM: return (s32)udtGameType::TDM;
		case idGameType68_CPMA::CTF: return (s32)udtGameType::CTF;
		case idGameType68_CPMA::CA: return (s32)udtGameType::CA;
		case idGameType68_CPMA::FT: return (s32)udtGameType::FT;
		case idGameType68_CPMA::CTFS: return (s32)udtGameType::CTFS;
		case idGameType68_CPMA::NTF: return (s32)udtGameType::NTF;
		case idGameType68_CPMA::TwoVsTwo: return (s32)udtGameType::TwoVsTwo;
		default: return UDT_S32_MIN;
	}
}

static s32 GetIdGameTypeCPMA(s32 gt)
{
	switch((udtGameType::Id)gt)
	{
		case udtGameType::HM: return (s32)idGameType68_CPMA::HM;
		case udtGameType::FFA: return (s32)idGameType68_CPMA::FFA;
		case udtGameType::Duel: return (s32)idGameType68_CPMA::Duel;
		case udtGameType::SP: return (s32)idGameType68_CPMA::SP;
		case udtGameType::TDM: return (s32)idGameType68_CPMA::TDM;
		case udtGameType::CTF: return (s32)idGameType68_CPMA::CTF;
		case udtGameType::CA: return (s32)idGameType68_CPMA::CA;
		case udtGameType::FT: return (s32)idGameType68_CPMA::FT;
		case udtGameType::CTFS: return (s32)idGameType68_CPMA::CTFS;
		case udtGameType::NTF: return (s32)idGameType68_CPMA::NTF;
		case udtGameType::TwoVsTwo: return (s32)idGameType68_CPMA::TwoVsTwo;
		default: return UDT_S32_MIN;
	}
}

struct idItem68_CPMA
{
	enum Id
	{
		ItemArmorJacket = 36,
		ItemBackpack,
		TeamCTFNeutralflag
	};
};

static s32 GetIdExtraItemCPMA(s32 item)
{
	switch((udtItem::Id)item)
	{
		case udtItem::ItemArmorJacket: return (s32)idItem68_CPMA::ItemArmorJacket;
		case udtItem::ItemBackpack: return (s32)idItem68_CPMA::ItemBackpack;
		case udtItem::FlagNeutral: return (s32)idItem68_CPMA::TeamCTFNeutralflag;
		default: return UDT_S32_MIN;
	}
}

static s32 GetUDTExtraItemCPMA(s32 item)
{
	switch((idItem68_CPMA::Id)item)
	{
		case idItem68_CPMA::ItemArmorJacket: return (s32)udtItem::ItemArmorJacket;
		case idItem68_CPMA::ItemBackpack: return (s32)udtItem::ItemBackpack;
		case idItem68_CPMA::TeamCTFNeutralflag: return (s32)udtItem::FlagNeutral;
		default: return UDT_S32_MIN;
	}
}

bool GetIdNumber(s32& idNumber, udtMagicNumberType::Id numberType, u32 udtNumber, udtProtocol::Id protocol, udtMod::Id mod)
{
	if((u32)numberType >= (u32)udtMagicNumberType::Count ||
	   (u32)protocol >= (u32)udtProtocol::Count)
	{
		return false;
	}

	if(numberType == udtMagicNumberType::GameType &&
	   protocol <= udtProtocol::Dm68 &&
	   mod == udtMod::CPMA)
	{
		const s32 result = GetIdGameTypeCPMA((s32)udtNumber);
		const bool success = result != UDT_S32_MIN;
		if(success) idNumber = result;
		return success;
	}

	if(numberType == udtMagicNumberType::Item &&
	   protocol <= udtProtocol::Dm68 &&
	   mod == udtMod::CPMA)
	{
		const s32 result = GetIdExtraItemCPMA((s32)udtNumber);
		if(result != UDT_S32_MIN)
		{
			idNumber = result;
			return true;
		}
	}

	const MagicNumberTableGroup* tableGroup = NULL;
	for(u32 i = 0; i < (u32)udtMagicNumberType::Count; ++i)
	{
		if(MagicNumberTables[i].Type == numberType)
		{
			tableGroup = &MagicNumberTables[i];
			break;
		}
	}

	if(tableGroup == NULL || udtNumber >= tableGroup->Count)
	{
		return false;
	}

	const s16 result = tableGroup->Tables[(u32)protocol * 3 + 1][udtNumber];
	if(result == UNDEFINED)
	{
		return false;
	}

	idNumber = (s32)result;

	return true;
}

static int BinarySearchCallback(const void* a, const void* b)
{
	return *(const s16*)a - *(const s16*)b;
}

bool GetUDTNumber(u32& udtNumber, udtMagicNumberType::Id numberType, s32 idNumber, udtProtocol::Id protocol, udtMod::Id mod)
{
	if((u32)numberType >= (u32)udtMagicNumberType::Count ||
	   (u32)protocol >= (u32)udtProtocol::Count)
	{
		return false;
	}

	if(numberType == udtMagicNumberType::GameType && 
	   protocol <= udtProtocol::Dm68 && 
	   mod == udtMod::CPMA)
	{
		const s32 result = GetUDTGameTypeCPMA(idNumber);
		const bool success = result != UDT_S32_MIN;
		if(success) udtNumber = (u32)result;
		return success;
	}

	if(numberType == udtMagicNumberType::Item && 
	   protocol <= udtProtocol::Dm68 &&
	   mod == udtMod::CPMA)
	{
		const s32 result = GetUDTExtraItemCPMA(idNumber);
		if(result != UDT_S32_MIN)
		{
			udtNumber = (u32)result;
			return true;
		}
	}

	const MagicNumberTableGroup* tableGroup = NULL;
	for(u32 i = 0; i < (u32)udtMagicNumberType::Count; ++i)
	{
		if(MagicNumberTables[i].Type == numberType)
		{
			tableGroup = &MagicNumberTables[i];
			break;
		}
	}

	if(tableGroup == NULL)
	{
		return false;
	}

	const s16* const table = tableGroup->Tables[(u32)protocol * 3 + 2];
	const void* const idNumberPtr = bsearch(&idNumber, table + 1, (size_t)tableGroup->Count, sizeof(s16) * 2, &BinarySearchCallback);
	if(idNumberPtr == NULL)
	{
		return false;
	}

	udtNumber = (u32)*((const s16*)idNumberPtr - 1);

	return true;
}

s32 GetIdNumber(udtMagicNumberType::Id numberType, u32 udtNumber, udtProtocol::Id protocol, udtMod::Id mod)
{
	s32 idNumber = UDT_S32_MIN;
	GetIdNumber(idNumber, numberType, udtNumber, protocol, mod);

	return idNumber;
}

s32 GetIdEntityStateFlagMask(udtEntityFlag::Id udtFlagId, udtProtocol::Id protocol)
{
	const s32 flagBit = GetIdNumber(udtMagicNumberType::EntityFlag, (u32)udtFlagId, protocol);
	if(flagBit == UDT_S32_MIN)
	{
		return 0;
	}

	return 1 << flagBit;
}
