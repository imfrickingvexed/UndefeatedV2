#pragma once
#include "../../stdafx.h"

//Don't change the location of max value, always at the end.
//And also add new vars to bottom of lists or it will cause problems in cfgs.
//Skeltal is the cutest
enum MenuTabs
{
	Tab_Ragebot = 0,
	Tab_Visuals,
	Tab_Misc,
	Tab_Config,
	MAXVALUEMENU
};

enum Config
{
	Config_save,
	Config_load,
	Config_type,
};

enum RagebotTab
{
	Ragebot_AimbotEnabled,
	Ragebot_Selection,
	Ragebot_Hitbox,
	Ragebot_Multipoint,
	Ragebot_PS,
	Ragebot_Multibox,
	Ragebot_Velocity,
	Ragebot_AutoShoot,
	Ragebot_Autoscope,
	Ragebot_SilentAim,
	Ragebot_Hitchance,
	Ragebot_MinDamage,
	Ragebot_AcceptKill,
	AntiAim_Enabled,
	AntiAim_Pitch,
	AntiAim_Yaw,
	AntiAim_AtTargets,
	AntiAim_FakeHitbox,
	AntiAim_EdgeDetection,
	Ragebot_Resolver,
	Ragebot_Accuracy,
	Ragebot_PreferBodyAim,
	Ragebot_PositionAdjustment,
	Ragebot_GOTV,
	Ragebot_MultipointType,
	MAXVALUERAGE
};

enum Visualstab
{
	Visuals_Enable,
	Visuals_BoxEnabled,
	Visuals_BoxType,
	Visuals_EspTeam,
	Visuals_HealthBar,
	Visuals_ArmorBar,
	Visuals_Name,
	Visuals_Flashed,
	Visuals_Weapons,
	Visuals_AimLines,
	Visuals_Skeltal,
	Visuals_EngineRadar,
	Visuals_Hostage,
	Visuals_DroppedWeapons,
	Visuals_NoRecoil,
	Visuals_NoFlash,
	Visuals_NoSmoke,
	Visuals_ThirdPerson,
	Visuals_NoScope,
	Visuals_Chams,
	Visuals_ChamsXQZ,
	Visuals_ChamsTeam,
	Visuals_ChamsGuns,
	Visuals_XQZ,
	Visuals_Crosshair,
	Visuals_CrosshairDynamic,
	Visuals_C4,
	Visuals_Flags,
	Visuals_Dlights,
	MAXVALUEVISUALS
};

enum MiscTab
{
	Misc_WeaponConfigs,
	Misc_WhichWeapon,
	Misc_AntiUntrusted,
	Misc_AutoJump,
	Misc_AutoStrafe,
	Misc_AutoStraferMode,
	Misc_ClanTagSpammer,
	Misc_ClanTagSelection,
	Misc_ClanTagAnimate,
	Misc_AutoAccept,
	Misc_FakeLag,
	Misc_Aimstep,
	Misc_ZStrafe,
	Misc_ZStrafeKey,
	Misc_CircleStrafe,
	Misc_CircleStrafeKey,
	Misc_UI,
	Misc_KnifeBot,
	Misc_Fov,
	MAXVALUEMISC
};

enum weapongroups
{
	Group_Default = 0,
	Group_Rifle,
	Group_Pistol,
	Group_Dak,
	Group_AWP,
	Group_Scout,
	Group_Shotgun,
	Group_SMG,
	Group_Heavy,

	MAXVALUEWEAPONGROUPS
};

class SettingsManger
{
public:
	bool weaponconfigs();

	static void SetSetting( int Tab, int Setting, double Value );

	static double GetSetting( int Tab, int Setting );

	double GetPlayer( int index, int Tab, int Setting );

	static double GetMenuSetting( int Tab, int Setting );
};

extern SettingsManger Settings;
