/*
  jflp.c - Jason's Freelancer Patch code support.

  Jason Hood, 31 May, 2009.

  This patch removes the following warnings from FLSpew:
  * couldnt find material 163618903
	Generated when a waypoint is selected.  I was unable to fix it, so just
	remove the warning, instead.
  * get_zone(N) failed
	Generated when a job sector has no name, which is quite frequently.
  * Disconnecting equipment with FATE_UNKNOWN, using FATE_DISAPPEAR
	Generated when a wreck drops its weapons.  This is a bug in Freelancer.exe,
	since it doesn't detect the "fate_loot" type.
  * Used Hostile Pick Assistance
	Not sure exactly when this is generated (or even why), but it's something
	to do with targetting enemy fighters.

  2 June, 2009:
  + replace the line number of the "Deform::start_aim" notice with the
	timestamp of the event (in milliseconds; it also replaces the string with
	"(null)", in order to fit the new code).

  26 & 27 August, 2009:
  + add proper stat displays for Powerplants, Engines, Scanners, Tractor Beams
	and Armor.	Not used by the vanilla game, but most mods display them.
	Also add a blank line after "Stats".

  7 to 9 September, 2009:
  + provide widescreen support.

  v1.01, 22 September, 2009:
  + allow roll to work during mouse flight.

  v1.10, 2 & 3 October, 2009:
  - fixed widescreen support;
  * use En Dash instead of Hyphen-Minus for the trade lane arrow;
  + added 8-way strafing;
  + put the sector letter first (as in "Sector C4");
  + add conditional code for cameras.ini anti-cheat (CAMERA_AC).

  v1.11, 22 October, 2009:
  + if MultiCruise is being used, include cruise speed in Engine stats.

  5 November, 21 December, 2009:
  + stat display will recognise the name containing a non-breaking space and
	will display "n/a" instead.

  21 February, 2010:
  + define NO_STATS to remove the additional stats altogether.

  v1.12, 6 December, 2010:
  - adjust fovh & hvaspect in thorn scripts for widescreen support;
  - fixed Alt+Tab with the Nav Map open in widescreen;
  - fixed widescreen mouse sensitivity.

  v1.20, 6 to 11 July, 2011:
  * remove the defines, use options instead: NO_STATS becomes "nostats" &
	CAMERA_AC becomes "nofovx";
  * fovx is always calculated, unless "!" is added ("fovx = 70!");
  * adjust cameras when changing resolution;
  - fixed fonts when resolution changes ratio;
  - fixed widescreen news ticker;
  - removed "cockpit aspect" warning;
  + working full screen/windowed toggle (Alt+Enter);
  + added options "no4way" and "no8way" to disable enhanced strafing;
  + allow viewing of server info, even for different versions.

  v1.21, 25 July, 2012:
  - fixed ADDR_FONT4a (added 1 to the already-added address);
  + removed the "ArchDB::Get" random mission spew warning;
  * use a specific base address.

  v1.22, 22 June, 2018:
  - retrieve the base address of "dacom.dll" & "thorn.dll".

  v1.23, 11 October, 2019:
  - fixed ratio of widescreen cinematics.

  v1.24, 31 August, 2020:
  - fixed widescreen text clipping in the Contact List.

  v1.25, 2 September, 2020:
  - fixed widescreen crosshair size.

  Build (VC 6):
	rc jflp.rc
	cl /nologo /W3 /O2 /GF /MD /LD jflp.c jflp.res /link /base:0x6100000 /filealign:512
*/
typedef unsigned int uint;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <math.h>

#pragma comment(lib, "user32.lib")

#define PI 3.14159265358979

#define NAKED __declspec(naked)
#define STDCALL __stdcall

DWORD dummy;
#define ProtectX(addr, size) \
	VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &dummy)
#define ProtectW(addr, size) \
	VirtualProtect(addr, size, PAGE_READWRITE, &dummy)

#define RELOFS(from, to) \
	*(PDWORD)(from) = (DWORD)(to) - (DWORD)(from)-4

#define REL2ABS(addr) ((DWORD)(addr) + *(PDWORD)(addr) + 4)

#define NEWOFS(from, to)      \
	to##_Old = REL2ABS(from); \
	RELOFS(from, to##_Hook)

#define INDIRECT(from, to)                 \
	(DWORD) to##_Old = **(PDWORD *)(from); \
	to##_New = (DWORD)to##_Hook;           \
	*(PDWORD *)(from) = &to##_New

// Stats ---------------------------------------------------------------------

void Stats_Hook(void);

const DWORD stats_jump_table[] = {
	(DWORD)Stats_Hook, // Power
	(DWORD)Stats_Hook, // Engine
	(DWORD)Stats_Hook, // Scanner
	(DWORD)Stats_Hook, // Tractor
	(DWORD)Stats_Hook, // Armor
	(DWORD)Stats_Hook, // ShieldGenerator
	0x484DA9,		   // Thruster
	0x484377,		   // Gun 0x484377
	0x4840E3,		   // MineDropper 0x484A6B
	(DWORD)Stats_Hook, // CounterMeasureDropper
	0x484C9C,		   // RepairKit
	0x484D23,		   // ShieldBattery
	0x484A6B,		   // Munition & Mine
	0x4851E5,		   // CounterMeasure
	0x485495,		   // default
};

const BYTE stats_jump_index[] = {
	0,	// Power
	1,	// Engine
	14, // Shield
	5,	// ShieldGenerator
	6,	// Thruster
	14, // Launcher
	7,	// Gun
	8,	// MineDropper
	9,	// CounterMeasureDropper
	2,	// Scanner
	14, // Light
	3,	// Tractor
	14, // AttachedFXEquip
	14, // InternalFXEquip
	14, // RepairDroid
	10, // RepairKit
	11, // ShieldBattery
	14, // CloakingDevice
	14, // TradeLaneEquip
	14, // Projectile
	12, // Munition
	12, // Mine
	13, // CounterMeasure
	4	// Armor
};

const BYTE stats_BOOL_index[] = {
	0, // Power
	0, // Engine
	1, // Shield
	0, // ShieldGenerator
	0, // Thruster
	1, // Launcher
	0, // Gun
	0, // MineDropper
	0, // CounterMeasureDropper
	0, // Scanner
	1, // Light
	0, // Tractor
	1, // AttachedFXEquip
	1, // InternalFXEquip
	1, // RepairDroid
	0, // RepairKit
	0, // ShieldBattery
	1, // CloakingDevice
	1, // TradeLaneEquip
	1, // Projectile
	0, // Munition
	0, // Mine
	0, // CounterMeasure
	0  // Armor
};

struct Vector
{
	float x;
	float y;
	float z;
};

struct Root
{
	/*  1 */ uint vtable;
	/*  2 */ uint iArchId;
	/*  3 */ char *szName;
	/*  4 */ uint iArchType;
	/*  5 */ uint iIdsName;
	/*  6 */ uint iIdsInfo;
	/*  7 */ float fHitPoints;
	/*  8 */ float fMass;
	/*  9 */ int iExplosionArchId;
	/* 10 */ float fExplosionResistance;
	/* 11x4 */ union
	{
		float fRotationInertia[3];
		struct Vector vRotationInertia;
	};
	/* 14 */ BOOL bHasRotationInertia;
	BOOL bPhantomPhysics;
	/* 15 */ uint iDunno1;
	/* 16 */ uint iDunno2;
	/* 17 */ void *anim;
	/* 18 */ uint iArray[5]; // not sure about this.
};

struct Equipment
{
	BYTE padding[sizeof(struct Root)];
	/* 23 */ float fVolume;
	/* 24 */ uint bUseCount;
	/* 25 */ uint iUnitsPerContainer;
	/* 26 */ uint iTractoredExplosionId;
	/* 27 */ uint bLootable;
};

struct EqObj
{
	BYTE padding[sizeof(struct Root)];
	/* 23 */ void *cg;
	/* 24 */ uint iDunno1;
	/* 25 */ uint iDunno2;
	/* 26 */ uint iDunno3;
	/* 27 */ uint iDunno4;
	/* 28 */ uint iDunno5;
	/* 29 */ uint iDunno6;
	/* 30 */ uint iDunno7;
	/* 31 */ uint iDunno9;
	/* 32 */ BOOL b128;
	BOOL bDockingCamera;
	/* 33 */ uint iDunno10;
	/* 34 */ uint iDunno11;
	/* 35 */ uint iDunno12;
};

struct AttachedEquipment
{
	BYTE padding[sizeof(struct Equipment)];
	/* 28 */ uint iSeparationExplosionId;
	/* 29 */ uint iDebrisTypeId;
	/* 30 */ float fChildImpulse;
	/* 31 */ float fParentImpulse;
	/* 32 */ char *szHpChild;
	/* 33 */ int iToughness;
};

struct Armor
{
	BYTE padding[sizeof(struct Equipment)];
	/* 28 */ float fHitPointsScale;
};

struct CloakingDevice
{
	BYTE padding[sizeof(struct AttachedEquipment)];
	/* 34 */ float fPowerUsage;
	/* 35 */ float fCloakinTime;
	/* 36 */ float fCloakoutTime;
	/* 37 */ uint iCloakinFxId;
	/* 38 */ uint iCloakoutFxId;
};

struct Engine
{
	BYTE padding[sizeof(struct Equipment)];
	/* 28 */ float fMaxForce;
	/* 29 */ float fLinearDrag;
	/* 30 */ float fPowerUsage;
	/* 31 */ float fCruisePowerUsage;
	/* 32 */ float fCruiseChargeTime;
	/* 33 */ BOOL bIndestructible;
	/* 34 */ float fReverseFraction;
};

struct Explosion
{
	/*  0 */ uint iID;
	/*  1 */ float fRadius;
	/*  2 */ float fImpulse;
	/*  3 */ float fHullDamage;
	/*  4 */ float fEnergyDamage;
	// and some other stuff
};

struct Launcher
{
	BYTE padding[sizeof(struct AttachedEquipment)];
	/* 34 */ float fDamagePerFire;
	/* 35 */ float fPowerUsage;
	/* 36 */ float fRefireDelay;
	/* 37 */ float fMuzzleVelocity;
	/* 38 */ uint iProjectileArchId;
	/* 39 */ char *szUseAnimation;
};

struct Gun
{
	BYTE padding[sizeof(struct Launcher)];
	/* 40 */ float fDispersionAngle;
	/* 41 */ float fTurnRate;
	/* 42 */ float fGunAzimuth[2];
	/* 44 */ float fGunElevation[2];
	/* 46 */ BOOL bUseGunAzimuth;
	BOOL bUseGunElevation;
	BOOL bAutoTurret;
	/* 47 */ uint stdVectorHpTypes[4]; // st6::vector?
};

struct MineDropper
{
	BYTE padding[sizeof(struct Launcher)];
	// no fields here
};

struct Power
{
	BYTE padding[sizeof(struct Equipment)];
	/* 28 */ float fCapacity;
	/* 29 */ float fChargeRate;
	/* 30 */ float fThrustCapacity;
	/* 31 */ float fThrustChargeRate;
};

struct Projectile
{
	BYTE padding[sizeof(struct Equipment)];
	/* 28 */ float fLifeTime;
	/* 29 */ float fOwnerSafeTime;
	/* 30 */ BOOL bRequiresAmmo;
	BOOL bForceGunOri;
	/* 31 */ uint iLootAppearanceId;
};

struct CounterMeasure
{
	BYTE padding[sizeof(struct Projectile)];
	/* 32 */ float fLinearDrag;
	/* 33 */ float fRange;
	/* 34 */ float fDiversionPctg;
};

struct CounterMeasureDropper
{
	BYTE padding[sizeof(struct Launcher)];
	/* 40 */ float fAiRange;
};

struct Munition
{
	BYTE padding[sizeof(struct Projectile)];
	/* 32 */ float fHullDamage;
	/* 33 */ float fEnergyDamage;
	/* 34 */ uint iWeaponTypeId;
	/* 35 */ uint iMotorId;
	/* 36 */ uint iSeeker; // 2 = lock, dumb = 1, ? = 0
	/* 37 */ float fTimeToLock;
	/* 38 */ float fSeekerRange;
	/* 39 */ float fSeekerFovDeg;
	/* 40 */ float fMaxAngularVelocity;
	/* 41 */ float fDetonationDist;
	/* 42 */ uint iHpType;
	/* 43 */ BOOL bCruiseDisruptor;
};

struct Mine
{
	BYTE padding[sizeof(struct Projectile)];
	/* 32 */ float fLinearDrag;
	/* 33 */ float fDetonationDist;
	/* 34 */ float fSeekerDist;
	/* 35 */ float fAcceleration;
	/* 36 */ float fTopSpeed;
};

struct Scanner
{
	BYTE padding[sizeof(struct Equipment)];

	/* 28 */ float fPowerUsage;
	/* 29 */ float fRange;
	/* 30 */ float fCargoScanRange;
};

struct ShieldGenerator
{
	BYTE padding[sizeof(struct AttachedEquipment)];
	/* 34 */ uint iHpType;
	/* 35 */ uint iShieldTypeId;
	/* 36 */ float fRegenerationRate;
	/* 37 */ float fMaxCapacity;
	/* 38 */ float fConstantPowerDraw;
	/* 39 */ float fRebuildPowerDraw;
	/* 40 */ float fOfflineThreshold;
	float fOfflineRebuildTime;
	/* 41 */ uint stdVectorHpShieldType[3];
};

struct Thruster
{
	BYTE padding[sizeof(struct AttachedEquipment)];
	/* 34 */ float fPowerUsage;
	/* 35 */ float fMaxForce;
};

struct Tractor
{
	BYTE padding[sizeof(struct Equipment)];
	/* 28 */ float fMaxLength;
	/* 29 */ float fReachSpeed;
};

HMODULE multicruise = INVALID_HANDLE_VALUE;
LPVOID cruise_map, cruise_map_end;
float *cruising_speed;
DWORD find_eng;

void Find_MultiCruise()
{
	if (multicruise == INVALID_HANDLE_VALUE)
	{
		multicruise = GetModuleHandle("MultiCruise.dll");
		if (multicruise != NULL)
		{
			if (*(PDWORD)((PBYTE)multicruise + 0x1534) == (DWORD)multicruise + 0xCD0C)
			{
				cruise_map = (PBYTE)multicruise + 0xCD0C;
				cruise_map_end = (PBYTE)multicruise + 0xCD10;
				cruising_speed = (float *)((PBYTE)multicruise + 0xCD04);
				find_eng = (DWORD)multicruise + 0x14A0;
			}
			else
			{
				multicruise = NULL;
			}
		}
	}
}

float Cruise_Speed(UINT eng)
{
	LPVOID end;

	if (!multicruise)
		return 0;

	__asm {
	lea	edx, eng
	lea	eax, end
	push	edx
	push	eax
	mov	ecx, cruise_map
	call	find_eng
	mov	eax, end
	mov	ecx, cruise_map_end
	mov	edx, cruising_speed
	cmp	eax, [ecx]
	je	done
	lea	edx, [eax+0x10]
  done:
	fld	dword ptr [edx]
	}
}

LPWSTR buf = (LPWSTR)0x66fc60; // internal WCHAR buf[4096]

UINT(*GetString)
(LPVOID rsrc, // handle to resources.dll, not actually used
 UINT res,	  // resource id (ids_name)
 LPWSTR buf,  // buffer for string
 UINT len)	  // length of buffer
	= (LPVOID)0x4347e0;

void get_stat_line(BYTE flags, UINT *len, UINT name_id, LPCWSTR value_fmt,
				   UINT suffix_id, ...)
{
	if (flags & 1)
		*len += GetString(NULL, name_id, buf + *len, 128);

	if (flags & 2)
	{
		if (flags & 4)
		{
			wcscpy(buf + *len, L"n/a");
			*len += 3;
		}
		else
		{
			va_list args;
			va_start(args, suffix_id);
			*len += _vswprintf(buf + *len, value_fmt, args);
			va_end(args);
			if (suffix_id)
				*len += GetString(NULL, suffix_id, buf + *len, 128);
		}
	}
	buf[(*len)++] = '\n';
}

UINT STDCALL Stats(int idx, LPBYTE equip, BYTE name, BYTE value)
{
	BYTE flags = name + (value << 1);
	UINT len = 0;
	LPCWSTR ifmt = L"%d";
	LPCWSTR f0fmt = L"%.0f";
	LPCWSTR f2fmt = L"%.2f";
	LPCWSTR f0fmtPercent = L"%.0f%%";
	LPCWSTR f2fmtS = L" %.2fs";
	LPCWSTR f0fmtSpace = L" %.0f";
	LPCWSTR ufmt = L"%u";

	// If the name contains a non-breaking space, display the stats as "n/a".
	if (value)
	{
		GetString(NULL, *(UINT *)(equip + 0x14), buf, 128);
		if (wcschr(buf, L'�') != NULL)
			flags |= 4;
	}

	switch (idx)
	{
	case 0: // Power
	{
		// Energy Capacity:   1000 GJ
		// Regeneration Rate: 95 GW
		struct Power *power = (struct Power *)equip;
		get_stat_line(flags, &len, 1758, ifmt, 0, (int)power->fCapacity);
		get_stat_line(flags, &len, 1753, ifmt, 0, (int)power->fChargeRate);
		// buf[len++] = '\n';
		// get_stat_line(flags, &len, 1698, ifmt, 0, (int)power->fThrustCapacity);
		// get_stat_line(flags, &len, 1753, ifmt, 0, (int)power->fThrustChargeRate);
	}
	break;
	case 1: // Engine
	{
		// Top Speed:	     80 m/s
		// Reverse Speed:      16 m/s
		// Energy Drain:	     0 GW
		//(Cruise Speed:	     300 m/s)
		// Cruise Charge Time: 5.00s
		// Cruise Energy Drain: 20 GW
		struct Engine *engine = (struct Engine *)equip;
		float top_speed, cruise_speed;
		Find_MultiCruise();
		if (value)
		{
			top_speed = engine->fMaxForce / engine->fLinearDrag;
			cruise_speed = Cruise_Speed(*(PUINT)(equip + 8));
		}
		get_stat_line(flags, &len, 945, ifmt, 1760, (int)top_speed);
		get_stat_line(flags, &len, 1699, ifmt, 1760, (int)(top_speed * engine->fReverseFraction));
		// get_stat_line(flags, &len, 1755, ifmt, 0, (int)engine->power_usage);
		// if (multicruise)
		// get_stat_line(flags, &len, 1703, ifmt, 1760, (int)cruise_speed);
		// else
		// buf[len++] = '\n';
		get_stat_line(flags, &len, 1762, f2fmt, 1764, engine->fCruiseChargeTime);
		get_stat_line(flags, &len, 1763, ifmt, 0, (int)engine->fCruisePowerUsage);
	}
	break;
	case 2: // Scanner
	{
		// Range: 	   2500m
		// Cargo Scan Range: 2000m
		struct Scanner *scanner = (struct Scanner *)equip;
		get_stat_line(flags, &len, 1745, ifmt, 1759, (int)scanner->fRange);
		get_stat_line(flags, &len, 1697, ifmt, 1759, (int)scanner->fCargoScanRange);
		// get_stat_line(flags, &len, 1748, f0fmtSpace, 0, (int)scanner->fPowerUsage);
	}
	break;
	case 3: // Tractor
	{
		// Range:        1500m
		// Extend Speed: 2000 m/s
		struct Tractor *tractor = (struct Tractor *)equip;
		get_stat_line(flags, &len, 1745, ifmt, 1759, (int)tractor->fMaxLength);
		get_stat_line(flags, &len, 1696, ifmt, 1760, (int)tractor->fReachSpeed);
	}
	break;
	case 4: // Armor
	{
		// Armor Improvement: 17%
		struct Armor *armor = (struct Armor *)equip;
		get_stat_line(flags, &len, 1695, f0fmtPercent, 0, (armor->fHitPointsScale - 1) * 100);
	}
	break;
	case 5: // ShieldGenerator
	{
		// Shield Capacity: 3570 u
		// Regeneration Rate: 320 u/s
		// Rebuild Time 9.45s
		// Rebuild Energy Drain: 41 GW
		// Constant Energy Drain: 271 GW
		struct ShieldGenerator *shield = (struct ShieldGenerator *)equip;
		get_stat_line(flags, &len, 1752, f0fmt, 0, shield->fMaxCapacity * (1.0f - shield->fOfflineThreshold));
		get_stat_line(flags, &len, 1753, f0fmt, 0, shield->fRegenerationRate);
		get_stat_line(flags, &len, 479579, f2fmtS, 0, shield->fOfflineRebuildTime);
		get_stat_line(flags, &len, 479577, f0fmtSpace, 0, shield->fRebuildPowerDraw);
		get_stat_line(flags, &len, 479578, f0fmtSpace, 0, shield->fConstantPowerDraw);
	}
	break;
	case 7: // Gun
	{
		// Hull Damage Per Shot: 250
		// Hull Average DPS: 1000/s
		// Shield Damage Per Shot: 251
		// Shield Average DPS: 500/s
		// Range: 742m
		// Refire Rate: 3.03/s
		// Projectile Speed: 1776 m/s
		// Spread: 2° (If present)
		// Energy Usage: 49 GW (If present)
		// Sustained Energy Usage: 490 KW (If present)
		// Turn Rate: 15°/s (If present)
		// Mount: Gimballed, 40°
		struct Launcher *lnch = (struct Launcher *)equip;
		struct Gun *gn = (struct Gun *)equip;
		struct Projectile *prj = (struct Projectile *)equip;
		struct Munition *mun = (struct Munition *)equip;
		struct Root *rt = (struct Root *)equip;
		get_stat_line(flags, &len, 1743, f0fmt, 0, mun->fHullDamage);
		get_stat_line(flags, &len, 1744, f0fmt, 0, mun->fEnergyDamage);
		get_stat_line(flags, &len, 1745, f0fmt, 1759, (prj->fLifeTime * lnch->fMuzzleVelocity));
		get_stat_line(flags, &len, 1746, f0fmt, 1760, lnch->fMuzzleVelocity);
		get_stat_line(flags, &len, 1747, f2fmt, 0, (1.0f / lnch->fRefireDelay));
		get_stat_line(flags, &len, 1748, f0fmt, 0, lnch->fPowerUsage);
		get_stat_line(flags, &len, 459437, f0fmtSpace, 0, (gn->fDispersionAngle * 180.0f / 3.141592653f));
		get_stat_line(flags, &len, 977, f2fmt, 0, (mun->fMaxAngularVelocity));
		// get_stat_line(flags, &len, 0, ufmt, 0, (rt->vtable));
	}
	break;
	case 8: // MineDropper
	{
		// Hull Damage Per Shot: 250
		// Shield Damage Per Shot: 251
		// Explosion Radius: 30m
		// Refire Rate: 3.03/s
		// Top Speed: 30 m/s
		// Lifetime: 10.50s
		// Seeker Distance: 100m
		// Energy Usage: 25 GW (If present)
		struct Launcher *lnch = (struct Launcher *)equip;
		struct Explosion *expl = (struct Explosion *)equip;
		struct Mine *mine = (struct Mine *)equip;
		struct Projectile *prj = (struct Projectile *)equip;
		get_stat_line(flags, &len, 1743, f0fmt, 0, expl->fHullDamage);
		get_stat_line(flags, &len, 1744, f0fmt, 0, expl->fEnergyDamage);
		get_stat_line(flags, &len, 1744, f0fmt, 0, expl->fRadius);
		get_stat_line(flags, &len, 1744, f0fmt, 0, mine->fTopSpeed);
		get_stat_line(flags, &len, 1744, f0fmt, 0, mine->fSeekerDist);
		get_stat_line(flags, &len, 1745, f0fmt, 1759, prj->fLifeTime);
		get_stat_line(flags, &len, 1746, f0fmt, 1760, lnch->fPowerUsage);
	}
	break;
	case 9: // CounterMeasureDropper
	{
		// Decoy Effectiveness: 25%
		// Decoy Range: 1000m
		// Projectile Speed: 1500 m/s
		// Refire Rate: 8.33/s
		// Energy Usage: 5 GW
		struct CounterMeasure *cm = (struct CounterMeasure *)equip;
		struct Launcher *cmdr = (struct Launcher *)equip;
		get_stat_line(flags, &len, 1751, f0fmtPercent, 0, cm->fDiversionPctg);
		get_stat_line(flags, &len, 1750, f0fmt, 1759, cm->fRange);
		get_stat_line(flags, &len, 1748, f0fmt, 0, cmdr->fPowerUsage);
		get_stat_line(flags, &len, 1747, f2fmt, 0, (1.0f / cmdr->fRefireDelay));
		get_stat_line(flags, &len, 1746, f0fmt, 1760, cmdr->fMuzzleVelocity);
	}
	break;
	}

	return len;
}

NAKED
void Stats_Hook(void)
{
	__asm {
	push	dword ptr [esp+0x22c]
	push	dword ptr [esp+0x22c]
	push	edi
	push	eax
	call	Stats
	push	0
	push	dword ptr [esp+0x224]
	push	0x4854c0
	ret
	}
}

// Widescreen ----------------------------------------------------------------

#define WIDTH ((PDWORD)0x679bc8)
#define HEIGHT ((PDWORD)0x679bcc)

float ratio, old_ratio;
DWORD width43;
float WinCamera_fovx = 27.216f;
float Default_fovx = 35.000f;
BOOL nofovx;

// Given the 4:3 value of fovx, determine the value for the current resolution.
float calc_fovx(float deg)
{
	return (float)(atan(ratio / 2 / (4.0f / 3 / 2 / tan(deg * PI / 180))) * 180 / PI);
}

// Called when the resolution is set.
BOOL Resolution(void)
{
#define FOVX_WIN ((float *)0x59d5fd)
#define CRATIO ((float *)0x5c9020)
#define BAR_WIDTH ((double *)0x5d7e58)

	BOOL changed = FALSE;

	ratio = (float)*WIDTH / *HEIGHT;
	if (ratio != old_ratio)
	{
		old_ratio = ratio;
		*FOVX_WIN = calc_fovx(WinCamera_fovx); // set the default value for WinCamera
		*BAR_WIDTH = 4.0f / 3 / 8 / ratio;	   // set the width of the gauges
		*CRATIO = ratio * 1.39f;			   // cinematic ratio (1.85 / 1.33)

		// No need to re-read the cameras on startup.
		if (width43 != 0)
			changed = TRUE;
		width43 = *HEIGHT * 4 / 3;
	}

	return changed;
}

DWORD Resolution_Old;

NAKED
void Resolution_Hook(void)
{
	__asm {
	call	Resolution
	test	al, al
	jz	done
	mov	eax, 0x549830 // read the cameras
	mov	ecx, 0x678b98
	call	eax
  done:
	jmp	Resolution_Old
	align	16
	}
}

float (*Fovx_Old)(LPCSTR); // atof
DWORD Fovx_New;

// Called when the fovx value is read from cameras.ini.
float STDCALL Fovx(LPCSTR fovx, BOOL WinCamera)
{
	float f = Fovx_Old(fovx);
	if (f == 0 || nofovx || strchr(fovx, '!') == NULL)
		return calc_fovx((WinCamera) ? WinCamera_fovx : Default_fovx) * 2;
	return f;
}

NAKED
void Fovx_Hook(void)
{
	__asm {
	xor	ebx, ebx
	cmp	esi, 0x67dbf8 // WinCamera
	sete	bl
	push	ebx
	push	eax
	call	Fovx
	ret
	}
}

// If hvaspect in a thorn is 1.33, change it to the ratio and adjust
// fovh accordingly.

void STDCALL Fovh(float *camera)
{
	int hvaspect = (int)(camera[3] * 100);
	if (hvaspect == 133)
	{
		camera[3] = ratio;
		camera[0] = calc_fovx(camera[0]);
	}
	else if (hvaspect == 185)
	{
		camera[3] = *CRATIO;
		camera[0] = calc_fovx(camera[0]);
	}
}

NAKED
void Fovh_Hook(void)
{
	__asm {
	add	ecx, 0xD4
	push	ecx
	call	Fovh
	ret
	}
}

// Use the desktop resolution as the default and add it to the list, removing
// 800x600.  Put it last, since Freelancer tests every width, ignoring height
// (so if you have 1280x800 and 1280x960, the one that occurs last is chosen,
// irrespective of what you actually selected).
void Patch_Dimensions(void)
{
#define ADDR_DEFAULT ((PBYTE)0x56223f)
#define ADDR_LIST ((PBYTE)0x4a9835)

	RECT desktop;
	PBYTE addr;

	static int res_width[] = {800, 1024, 1152, 1280, 1600};
	static int res_height[] = {600, 768, 864, 960, 1200};
	int i;

	ProtectX(ADDR_DEFAULT, 11);
	ProtectX(ADDR_LIST, 0x5e);

	GetWindowRect(GetDesktopWindow(), &desktop);
	*(DWORD *)ADDR_DEFAULT = desktop.right;
	*(DWORD *)(ADDR_DEFAULT + 7) = desktop.bottom;

	for (i = 0; i < 5; ++i)
		if (desktop.right == res_width[i] && desktop.bottom == res_height[i])
			break;
	if (i == 5)
	{
		memcpy(res_width, res_width + 1, 4 * sizeof(int));
		memcpy(res_height, res_height + 1, 4 * sizeof(int));
		res_width[4] = desktop.right;
		res_height[4] = desktop.bottom;
		for (addr = ADDR_LIST, i = 0; i < 5; addr += 10, ++i)
		{
			*(DWORD *)addr = res_width[i];
			*(DWORD *)(addr + 50) = res_height[i];
		}
	}
}

// Full screen / windowed toggle ---------------------------------------------

typedef BOOL (*set_res_)(HWND hWnd, LPBYTE new_res, LPBYTE old_res);
set_res_ set_res = (set_res_)0x4249c0;
#define res ((PBYTE)0x679bc0)
#define hWnd (*(HWND *)0x6679f4)

BOOL STDCALL ToggleFullScreen(UINT dummy)
{
	BYTE new_res[16 * 4];

	memcpy(new_res, res, sizeof(new_res));
	new_res[0x25] ^= 1; // toggle full screen flag
	set_res(hWnd, new_res, res);
	return TRUE;
}

// Roll during mouse flight --------------------------------------------------

DWORD Roll_Old, Roll_New;

// Copy the roll value generated by the key to the mouse flight value.
NAKED
void Roll_Hook(void)
{
	__asm {
	xor	eax, eax
	cmp	dword ptr [edx], 0x80000000 // No X?
	jne	roll
	cmp	dword ptr [edx+4], 0x80000000 // No Y?
	jne	roll
	mov	eax, [edx+8] // Must be Z
  roll:
	mov	ds:[0x4c7f14], eax
	jmp	Roll_Old
	align	16
	}
}

// 8-way strafe --------------------------------------------------------------

BOOL no4way, no8way;

void left_up(void);
void left_down(void);
void right_up(void);
void right_down(void);

DWORD strafe_table_10[] = {
	0x062BBD3A, // left
	0x062BBD68, // right
	0x062BBD81, // up
	0x062BBD9A, // down
	(DWORD)left_up,
	(DWORD)left_down,
	(DWORD)right_up,
	(DWORD)right_down,
};

DWORD strafe_table_11[] = {
	0x062BB9BA, // left
	0x062BB9E8, // right
	0x062BBA01, // up
	0x062BBA1A, // down
	(DWORD)left_up,
	(DWORD)left_down,
	(DWORD)right_up,
	(DWORD)right_down,
};

DWORD *strafe_table;
DWORD PhySys_Controller_push;

NAKED
void SetStrafe(void)
{
	__asm {
	mov	edx, [ecx+0x18]
        test	eax, eax
        jz	ok
        dec	edx
        jz	left
        dec	edx
        jz	right
        dec	edx
        jz	up
        dec	edx
        jnz	ok
												// down:
        cmp	eax, 3
        jae	ok
        lea	eax, [eax+eax+4]
        jmp	ok
  up:
        cmp	eax, 3
        jae	ok
        lea	eax, [eax+eax+3]
        jmp	ok
  right:
        cmp	eax, 2
        jbe	ok
        add	eax, 4
        jmp	ok
  left:
        cmp	eax, 2
        jbe	ok
        add	eax, 2
  ok:
        mov	[ecx+0x18], eax
	ret
	}
}

NAKED
void controller(void)
{
	__asm {
	mov	ecx, [esp+0x14+8]
	mov	edx, [esp+0x18+8]
	mov	eax, [esp+0x1C+8]
	mov	[esp+0x08+8], ecx
	mov	ecx, [esi+0x14]
	mov	[esp+0x10+8], eax
	mov	eax, [edi]
	mov	[esp+0x0C+8], edx
	mov	edx, [ecx+0x88]
	fld	dword ptr [edx+0x0124]
	push	eax
	fld	dword ptr [esp+0x0C+8]
	lea	ecx, [esp+0x0C+8]
	fmul	st(0), st(1)
	push	ecx
	mov	ecx, esi
	fstp	dword ptr [esp+0x10+8]
	fld	dword ptr [esp+0x14+8]
	fmul	st(0), st(1)
	fstp	dword ptr [esp+0x14+8]
	fld	dword ptr [esp+0x18+8]
	fmul	st(0), st(1)
	fstp	dword ptr [esp+0x18+8]
	fstp	st(0)
	call	PhySys_Controller_push
	ret
	}
}

NAKED
void up(void)
{
	__asm {
	mov	eax, [esi+0x14]
	fld	dword ptr [eax+0x0C]
	add	eax, 0x08
	fstp	dword ptr [esp+0x14+4]
	fld	dword ptr [eax+0x10]
	fstp	dword ptr [esp+0x18+4]
	fld	dword ptr [eax+0x1c]
	fstp	dword ptr [esp+0x1C+4]
	call	controller
	ret
	}
}

NAKED
void down(void)
{
	__asm {
	mov	eax, [esi+0x14]
	fld	dword ptr [eax+0x0C]
	add	eax, 0x08
	fld	dword ptr [eax+0x10]
	fld	dword ptr [eax+0x1C]
	fstp	dword ptr [esp+0x28+4]
	fxch	st(1)
	fchs
	fstp	dword ptr [esp+0x14+4]
	fchs
	fstp	dword ptr [esp+0x18+4]
	fld	dword ptr [esp+0x28+4]
	fchs
	fstp	dword ptr [esp+0x1C+4]
	call	controller
	ret
	}
}

NAKED
void left_up(void)
{
	__asm {
        call	up
        mov	eax, strafe_table
        jmp	dword ptr [eax]
	}
}

NAKED
void left_down(void)
{
	__asm {
        call	down
        mov	eax, strafe_table
        jmp	dword ptr [eax]
	}
}

NAKED
void right_up(void)
{
	__asm {
        call	up
        mov	eax, strafe_table
        jmp	dword ptr [eax+4]
	}
}

NAKED
void right_down(void)
{
	__asm {
        call	down
        mov	eax, strafe_table
	jmp	dword ptr [eax+4]

	align	16
	}
}

// Server Info ---------------------------------------------------------------

DWORD Connect_Old, ConnectDbl_Old;

NAKED
void Connect_Hook(void)
{
	__asm {
	push	eax
	call	Connect_Old
	test	al, al
	jz	done // nothing selected
	mov	ecx, [esi+0xC4]
	mov	eax, 0x58d0b0
	push	dword ptr [esp+4+8]
	call	eax // get the selected server
	mov	eax, [eax+8]
	test	eax, eax
	jz	done
	mov	eax, [eax+0x94] // its version
	cmp	eax, ds:[0x67ecb8] // client's version
	je	done
	mov	dword ptr [esp], 0x56f78a
  done:
	ret	4
	}
}

NAKED
void ConnectDbl_Hook(void)
{
	__asm {
	mov	eax, [eax+0x8c]
	cmp	eax, ds:[0x67ecb8]
	jne	done
	jmp	ConnectDbl_Old
  done:
	xor	al, al
	ret	4
	}
}

// ---------------------------------------------------------------------------

void Patch_Common(void)
{
#define ADDR_VERS10 (0x62c1485 - 0x6260000 + common)
#define ADDR_FATE10 (0x6293727 - 0x6260000 + common)
#define ADDR_ADBGET10 (0x62f9536 - 0x6260000 + common)
#define ADDR_ZONE10 (0x633c26c - 0x6260000 + common)
#define ADDR_STRAFE10 (0x62bbcc9 - 0x6260000 + common)
#define ADDR_CNTRLR10 (0x62bbd28 - 0x6260000 + common)

#define ADDR_VERS11 (0x62c14e5 - 0x6260000 + common)
#define ADDR_FATE11 (0x6293487 - 0x6260000 + common)
#define ADDR_ADBGET11 (0x62f95b6 - 0x6260000 + common)
#define ADDR_ZONE11 (0x633c34c - 0x6260000 + common)
#define ADDR_STRAFE11 (0x62bb949 - 0x6260000 + common)
#define ADDR_CNTRLR11 (0x62bb9a8 - 0x6260000 + common)

	PBYTE addr_fate, addr_zone, addr_arch, addr_strafe, addr_cntrlr;

	PBYTE common = (PBYTE)GetModuleHandle("common.dll");
	if (*(DWORD *)ADDR_VERS11 == 0x639F3CC)
	{
		addr_fate = ADDR_FATE11;
		addr_zone = ADDR_ZONE11;
		addr_arch = ADDR_ADBGET11;
		addr_strafe = ADDR_STRAFE11;
		addr_cntrlr = ADDR_CNTRLR11;
		strafe_table = strafe_table_11;
		PhySys_Controller_push = 0x6285d60;
	}
	else if (*(DWORD *)ADDR_VERS10 == 0x639F39C)
	{
		addr_fate = ADDR_FATE10;
		addr_zone = ADDR_ZONE10;
		addr_arch = ADDR_ADBGET10;
		addr_strafe = ADDR_STRAFE10;
		addr_cntrlr = ADDR_CNTRLR10;
		strafe_table = strafe_table_10;
		PhySys_Controller_push = 0x6286230;
	}
	else
		return;

	// Change the fate test to below 3 is bad, below/equal 5 is good.
	ProtectX(addr_fate, 6);
	memcpy(addr_fate, "\x72\x05\x83\xf8\x05\x76", 6);

	// Skip the zone message altogether.
	ProtectX(addr_zone, 1);
	*addr_zone = 0x28;

	// Skip the ArchDB::Get message from random missions (it searched for ship,
	// then solar, then equipment, displaying a not found message for each).
	ProtectX(addr_arch, 0x48);
	addr_arch[0] = addr_arch[1] =
		addr_arch[0x46] = addr_arch[0x47] = 0x90;

	ProtectX(addr_strafe, 0x75);
	if (no4way)
	{
		addr_cntrlr[2] = 1;
	}
	else if (!no8way)
	{
		// Hook in the 8-way strafe code.
		addr_strafe[0] = 0xe8;
		*(DWORD *)(addr_strafe + 1) = (PBYTE)SetStrafe - addr_strafe - 5;
		addr_cntrlr[2] = 7;
		*(DWORD *)(addr_cntrlr + 14) = (DWORD)strafe_table;
	}
}

NAKED
void RendComp_Patch(void)
{
	__asm {
	mov	edx, [ebp]
	lea	eax, [esp+0x28]
	cmp	edx, 163618903
	ret
	}
}

void Patch_RendComp(void)
{
#define ADDR_MATERIAL (0x6c318be - 0x6c20000 + rendcomp)

	PBYTE rendcomp = (PBYTE)GetModuleHandle("rendcomp.dll");
	if (*ADDR_MATERIAL == 0x8b)
	{
		// Add in a test for the ignored material.
		static BYTE code[] =
			{
				0xE8, 0x00, 0x00, 0x00, 0x00, // call RendComp_Patch
				0x74, 0x2D,					  // jz	skip_log
				0x52};
		*(DWORD *)(code + 1) = (PBYTE)RendComp_Patch - ADDR_MATERIAL - 5;
		ProtectX(ADDR_MATERIAL, sizeof(code));
		memcpy(ADDR_MATERIAL, code, sizeof(code));
	}
}

void Patch(LPSTR opt)
{
#define ADDR_ASSIST ((PWORD)0x4ede00)
#define ADDR_DEFORM ((PBYTE)0x452648)
#define ADDR_STATS1 ((PBYTE)0x47c21f)
#define ADDR_STATS2 ((PBYTE)0x4840c9)
#define ADDR_STATSNL1 ((PBYTE)0x47c355)
#define ADDR_STATSNL2 ((PBYTE)0x47c501)
#define ADDR_RES ((PDWORD)(0x424ad4 + 1))
#define ADDR_43 ((PBYTE)0x4af1e7)
#define ADDR_COCKPIT1 ((PBYTE)0x5176cc)
#define ADDR_COCKPIT2 ((PBYTE)0x5188e7)
#define ADDR_FOVX ((PDWORD)(0x40f617 + 2))
#define ADDR_FOVY ((PDWORD)(0x40f60c + 2))
#define ADDR_MOUSE ((PDWORD *)(0x41d9c3 + 2))
#define ADDR_MOUSE1 ((PDWORD *)(0x41fb6a + 2))
#define ADDR_MOUSE2 ((PDWORD *)(0x41fbde + 2))
#define ADDR_MOUSE3 ((PDWORD *)(0x41fdb7 + 2))
#define ADDR_MOUSE4 ((PDWORD *)(0x41fe23 + 2))
#define ADDR_NAVMAP ((PDWORD *)(0x499a3b + 1))
#define ADDR_NAVMAP1 ((float **)(0x495b88 + 2))
#define ADDR_FOVH ((PBYTE)(0x6f41537 - 0x6f20000 + thorn))
#define ADDR_FONT0 ((PDWORD *)(0x412e8b + 2)) // determines shadow offset
#define ADDR_FONT1a ((PDWORD *)(0x415c26 + 1))
#define ADDR_FONT1b ((PDWORD *)(0x415cc5 + 2))
#define ADDR_FONT1c ((PDWORD *)(0x415db3 + 1))
#define ADDR_FONT2a ((PDWORD *)(0x416136 + 1))
#define ADDR_FONT2b ((PDWORD *)(0x416158 + 2))
#define ADDR_FONT2c ((PDWORD *)(0x4161fc + 2))
#define ADDR_FONT2d ((PDWORD *)(0x41637d + 2))
#define ADDR_FONT3a ((PDWORD *)(0x4165c0 + 1))
#define ADDR_FONT3b ((PDWORD *)(0x416662 + 1))
#define ADDR_FONT3c ((PDWORD *)(0x416788 + 2))
#define ADDR_FONT4a ((PDWORD *)(0x416870 + 1))
#define ADDR_FONT4b ((PDWORD *)(0x416912 + 1))
#define ADDR_FONT4c ((PDWORD *)(0x416a38 + 2))
#define ADDR_FONT5a ((PDWORD *)(0x416b40 + 1))
#define ADDR_FONT5b ((PDWORD *)(0x416be2 + 1))
#define ADDR_FONT5c ((PDWORD *)(0x416d08 + 2))
#define ADDR_FONT6a ((PDWORD *)(0x416e10 + 1))
#define ADDR_FONT6b ((PDWORD *)(0x416e95 + 1))
#define ADDR_FONT6c ((PDWORD *)(0x416ead + 1))
#define ADDR_FONT7a ((PDWORD *)(0x417130 + 1))
#define ADDR_FONT7b ((PDWORD *)(0x4171b5 + 1))
#define ADDR_FONT7c ((PDWORD *)(0x4171cd + 1))
#define ADDR_FONT8a ((PDWORD *)(0x417300 + 1))
#define ADDR_FONT8b ((PDWORD *)(0x417385 + 1))
#define ADDR_FONT8c ((PDWORD *)(0x41739d + 1))
#define ADDR_FONT9a ((PDWORD *)(0x4d34fa + 2)) // contact list clipping
#define ADDR_FONT9b ((PDWORD *)(0x4d37d6 + 2))
#define ADDR_TICKER ((PDWORD *)(0x4a5cbc + 2))
#define ADDR_XHAIR1 ((PDWORD *)(0x4f211f + 2))
#define ADDR_XHAIR2 ((PDWORD *)(0x4f2141 + 2))
#define ADDR_ROLL1 ((PDWORD)(0x4c7940 + 2))
#define ADDR_ROLL2 ((PDWORD)0x4c7f14)
#define ADDR_ARROW ((LPWSTR)0x5cef58)
#define ADDR_LETTER1a ((PBYTE)0x48dcf5) // object in space
#define ADDR_LETTER1b ((PBYTE)0x48dd01)
#define ADDR_LETTER2a ((PBYTE)0x41d1e6) // mission description
#define ADDR_LETTER2b ((PBYTE)0x41d250)
#define ADDR_LETTER3a ((PBYTE)0x4baee8) // Neural Net
#define ADDR_LETTER3b ((PBYTE)0x4baf52)
#define ADDR_FULLWIN1 ((PBYTE)0x5b2497)
#define ADDR_FULLWIN2 ((PDWORD)0x5b254c)
#define ADDR_SELECT ((PWORD)0x5711a1)
#define ADDR_CONNECTDBL ((PDWORD)(0x571aea + 1))
#define ADDR_CONNECT ((PDWORD)(0x56f778 + 1))

	PBYTE thorn = (PBYTE)GetModuleHandle("thorn.dll");

	// Skip the "Used Hostile Pick Assistance" message.
	ProtectX(ADDR_ASSIST, 2);
	*ADDR_ASSIST = 0x26EB;

	ProtectX(ADDR_DEFORM, 10);
	memcpy(ADDR_DEFORM, "\x6A\x00"	   // push 0 ;"(null)"
						"\x8B\x76\x04" // mov  esi, [esi+4]
						"\xFF\x76\x08" // push dword[esi+8]
						"\x8B\xFF",
		   10); // mov  edi, edi

	if (strstr(opt, "nostats") == NULL)
	{
		// Replace the equipment stats jump table.
		ProtectX(ADDR_STATS1, 26);
		ProtectX(ADDR_STATS2, 19);
		ADDR_STATS1[2] = ADDR_STATS2[2] = -11;
		ADDR_STATS1[5] = ADDR_STATS2[5] = 23;
		*(DWORD *)(ADDR_STATS2 + 15) = (DWORD)stats_jump_index;
		*(DWORD *)(ADDR_STATS2 + 22) = (DWORD)stats_jump_table;
		*(DWORD *)(ADDR_STATS1 + 15) = (DWORD)stats_BOOL_index;
	}

	// Add a blank line after the equipment's "Stats".
	ProtectX(ADDR_STATSNL1, 20);
	ProtectX(ADDR_STATSNL2, 20);
	*(DWORD *)(ADDR_STATSNL1 + 1) =
		*(DWORD *)(ADDR_STATSNL1 + 16) =
			*(DWORD *)(ADDR_STATSNL2 + 1) =
				*(DWORD *)(ADDR_STATSNL2 + 16) = 0x5d0a08;

	// Support widescreen displays.
	ProtectX(ADDR_RES, 4);
	ProtectX(ADDR_43, 1);
	ProtectX(ADDR_FOVX, 4);
	ProtectX(ADDR_MOUSE, 4);
	ProtectX(ADDR_MOUSE1, 4);
	ProtectX(ADDR_MOUSE2, 4);
	ProtectX(ADDR_MOUSE3, 4);
	ProtectX(ADDR_MOUSE4, 4);
	ProtectX(ADDR_NAVMAP, 4);
	ProtectX(ADDR_NAVMAP1, 4);
	ProtectX(ADDR_FOVH, 8);
	ProtectX(FOVX_WIN, 4);
	ProtectX(CRATIO, 4);
	ProtectW(BAR_WIDTH, 4);
	ProtectX(ADDR_COCKPIT1, 1);
	ProtectX(ADDR_COCKPIT2, 1);
	ProtectX(ADDR_FONT0, 4);
	ProtectX(ADDR_FONT1a, 4);
	// ProtectX( ADDR_FONT1b,   4 );
	// ProtectX( ADDR_FONT1c,   4 );
	ProtectX(ADDR_FONT2a, 4);
	// ProtectX( ADDR_FONT2b,   4 );
	// ProtectX( ADDR_FONT2c,   4 );
	// ProtectX( ADDR_FONT2d,   4 );
	// ProtectX( ADDR_FONT3a,   4 );
	// ProtectX( ADDR_FONT3b,   4 );
	// ProtectX( ADDR_FONT3c,   4 );
	// ProtectX( ADDR_FONT4a,   4 );
	// ProtectX( ADDR_FONT4b,   4 );
	// ProtectX( ADDR_FONT4c,   4 );
	// ProtectX( ADDR_FONT5a,   4 );
	// ProtectX( ADDR_FONT5b,   4 );
	// ProtectX( ADDR_FONT5c,   4 );
	// ProtectX( ADDR_FONT6a,   4 );
	// ProtectX( ADDR_FONT6b,   4 );
	// ProtectX( ADDR_FONT6c,   4 );
	ProtectX(ADDR_FONT7a, 4);
	// ProtectX( ADDR_FONT7b,   4 );
	// ProtectX( ADDR_FONT7c,   4 );
	// ProtectX( ADDR_FONT8a,   4 );
	// ProtectX( ADDR_FONT8b,   4 );
	// ProtectX( ADDR_FONT8c,   4 );
	ProtectX(ADDR_FONT9a, 4);
	// ProtectX( ADDR_FONT9b,   4 );
	ProtectX(ADDR_XHAIR1, 4);
	// ProtectX( ADDR_XHAIR2,   4 );
	ProtectX(ADDR_TICKER, 4);

	*ADDR_43 =									// ignore 4:3 resolution test
		*ADDR_COCKPIT1 = *ADDR_COCKPIT2 = 0xEB; // ignore cockpit aspect warning
	NEWOFS(ADDR_RES, Resolution);
	INDIRECT(ADDR_FOVX, Fovx);

	nofovx = (strstr(opt, "nofovx") != NULL);
	if (nofovx)
	{
		// Prevent fovy from working, too.
		// ProtectX( ADDR_FOVY, 4 );
		RELOFS(ADDR_FOVY, 0x40f857);
	}

	// Use the equivalent 4:3 width instead of the actual width.
	*ADDR_MOUSE =
		*ADDR_MOUSE1 = *ADDR_MOUSE2 = *ADDR_MOUSE3 = *ADDR_MOUSE4 =
			*ADDR_NAVMAP =
				*ADDR_FONT0 =
					*ADDR_FONT1a = *ADDR_FONT1b = *ADDR_FONT1c =
						*ADDR_FONT2a = *ADDR_FONT2b = *ADDR_FONT2c = *ADDR_FONT2d =
							*ADDR_FONT3a = *ADDR_FONT3b = *ADDR_FONT3c =
								*ADDR_FONT4a = *ADDR_FONT4b = *ADDR_FONT4c =
									*ADDR_FONT5a = *ADDR_FONT5b = *ADDR_FONT5c =
										*ADDR_FONT6a = *ADDR_FONT6b = *ADDR_FONT6c =
											*ADDR_FONT7a = *ADDR_FONT7b = *ADDR_FONT7c =
												*ADDR_FONT8a = *ADDR_FONT8b = *ADDR_FONT8c =
													*ADDR_FONT9a = *ADDR_FONT9b =
														*ADDR_XHAIR1 = *ADDR_XHAIR2 =
															*ADDR_TICKER = &width43;
	// The Nav Map seems to want the original 4:3 WinCamera value.
	*ADDR_NAVMAP1 = &WinCamera_fovx;
	// Adjust the cameraprops fovh value used in thorn scripts.
	*ADDR_FOVH = 0xE8;
	*(DWORD *)(ADDR_FOVH + 1) = (PBYTE)Fovh_Hook - ADDR_FOVH - 5;
	ADDR_FOVH[5] = 0xC2;
	ADDR_FOVH[6] = 0x04;
	ADDR_FOVH[7] = 0x00;
	Patch_Dimensions();

	// Make USER_FULLSCREEN work.
	ProtectX(ADDR_FULLWIN1, 1);
	// ProtectX( ADDR_FULLWIN2, 4 );
	*ADDR_FULLWIN1 = 0x33;
	*ADDR_FULLWIN2 = (DWORD)ToggleFullScreen;

	// Support rolling during mouse flight.
	ProtectX(ADDR_ROLL1, 4);
	ProtectX(ADDR_ROLL2, 4);
	INDIRECT(ADDR_ROLL1, Roll);

	// Use "�>" instead of "->" (provided it hasn't already been patched).
	if (*ADDR_ARROW == '-' && ADDR_ARROW[1] == '>')
	{
		ProtectW(ADDR_ARROW, 2);
		*ADDR_ARROW = 0x2013;
	}

	// Put the sector letter first.
	ProtectX(ADDR_LETTER1a, 0x0C);
	ProtectX(ADDR_LETTER2a, 0x70);
	ProtectX(ADDR_LETTER3a, 0x70);
	*ADDR_LETTER1a = 0x4c;
	*ADDR_LETTER1b = 0x30;
	*ADDR_LETTER2a = *ADDR_LETTER3a = 0x57;
	*ADDR_LETTER2b = *ADDR_LETTER3b = 0x53;

	// Allow viewing of server info for different versions.
	ProtectX(ADDR_SELECT, 2);
	ProtectX(ADDR_CONNECT, 4);
	// ProtectX( ADDR_CONNECTDBL, 4 );
	*ADDR_SELECT = 0x01B0;
	NEWOFS(ADDR_CONNECT, Connect);
	NEWOFS(ADDR_CONNECTDBL, ConnectDbl);

	if (strstr(opt, "no4way") != NULL)
	{
		no4way = TRUE;
		no8way = TRUE;
	}
	if (strstr(opt, "no8way") != NULL)
		no8way = TRUE;

	Patch_Common();
	Patch_RendComp();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Process options by reading the line stored from dacom.ini.  The ini
		// parser strips comments and adds spaces around equal signs; the DLL parser
		// stops at the first space/tab.  The entire line is MAX_PATH chars
		// (including NUL).
		LPSTR opt = "";
		// Find the LoadLibrary return address in the stack.
		DWORD dacom = 0x65b2689 - 0x65b0000 + (DWORD)GetModuleHandle("dacom.dll");
		PDWORD esp = (PDWORD)&hinstDLL + 0x730 / 4;
		int i = 0, d = 1;
		for (;;)
		{
			if (esp[i] == dacom)
			{
				opt = (LPSTR)(esp + i) + 0x2254;
				opt += strlen(opt) + 1; // skip the DLL and its NUL
				_strlwr(opt);
				break;
			}
			i += d;
			if (i == 256)
			{
				i = d = -1;
			}
			else if (i == -256)
			{
				break;
			}
		}
		Patch(opt);
	}
	return TRUE;
}
