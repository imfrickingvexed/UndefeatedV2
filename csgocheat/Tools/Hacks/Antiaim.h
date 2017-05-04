#pragma once
#include "stdafx.h"
#include "../Menu/Menu.h"
#include "Misc.h"
#include "Aimbot.h"
#include "../Utils/LocalInfo.h"
#include "../Menu/SettingsManager.h"

class AntiAim
{
public:
	bool start = false;
	bool jitter = false;
	bool flip = false;
	bool ShouldAA = true;
	int current = 0;
private:
	bool CanFireNext( CBaseEntity* pEntity )
	{
		CBaseCombatWeapon* pWeapon = pEntity->GetActiveBaseCombatWeapon();
		float flServerTime = pEntity->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;
		float flNextShot = pWeapon->NextPrimaryAttack() - flServerTime;
		return flNextShot == 1;
	}

	void AtTargets( Vector& viewangles )
	{
		if( Settings.GetSetting( Tab_Ragebot, AntiAim_AtTargets ) != 1 )
		{
			if( Aimbot.Target != Vector( 0, 0, 0 ) )
			{
				Misc::CalcAngle( Hacks.LocalPlayer->GetVecOrigin(), Aimbot.Target + Hacks.LocalPlayer->GetVecOrigin(), viewangles );
			}
			else if( Settings.GetSetting( Tab_Ragebot, AntiAim_AtTargets ) == 2 )
				ShouldAA = false;
		}
	}

	void Fakelag()
	{
		static int Ticks = 6;
		static Vector pos = Hacks.LocalPlayer->GetVecOrigin();
		int fakelag = Settings.GetSetting( Tab_Misc, Misc_FakeLag );
		switch( fakelag )
		{
			case 0:
			case 1:
				if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) )
				{
					if( Hacks.CurrentCmd->tick_count % 3 == 0 )
						Hacks.SendPacket = false;
				}
				break;
			case 2:
				if( !( LocalInfo.Choked > 12 ) )
					Hacks.SendPacket = false;
				break;
			case 3:
				if( !( ( pos - LocalInfo.LastPos ).Length2DSqr() > 4096.f ) && !( LocalInfo.Choked > 12 ) )
					Hacks.SendPacket = false;
				break;
			case 4:
				if( LocalInfo.Choked > Ticks )
				{
					if( Ticks == 12 )
						Ticks = 5;
					else
						Ticks = 12;
				}
				else
					Hacks.SendPacket = false;
				break;
			case 5:
				if( LocalInfo.Choked > Ticks )
				{
					Ticks += ( ( rand() % 4 ) + 2 ) * ( ( rand() % 2 ) + 2 );
					while( Ticks > 12 )
						Ticks -= 7;
				}
				else
					Hacks.SendPacket = false;
				break;
		}
		pos = Hacks.LocalPlayer->GetVecOrigin();
	}

	void SidewaysFake( int& Add )
	{
		if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) )
		{
			if( Hacks.SendPacket )
			{
				static bool t = false;
				Add = t ? 90 : 120;
				t = !t;
			}
		}
	}

	void BackFake( int& Add )
	{
		if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) && Hacks.SendPacket )
		{
			static bool t = false;
			Add = t ? -165 : 165;
			t = !t;
		}
	}

	void jittertime()
	{
		static clock_t start1_t1 = clock();
		double timeSoFar1 = ( double )( clock() - start1_t1 ) / CLOCKS_PER_SEC;
		if( timeSoFar1 < 0.05 )
			return;
		jitter = !jitter;
		start1_t1 = clock();
	}

	void fliptime()
	{
		static clock_t start_t1 = clock();
		double timeSoFar = ( double )( clock() - start_t1 ) / CLOCKS_PER_SEC;
		if( timeSoFar < 4.0 )
			return;
		start_t1 = clock();
	}

	void Axis( Vector& View )
	{
		int Add = 0;
		static int SpinYaw = 0;
		float JitterAng = 0;
		static float StoredAng = 0;
		jittertime();
		fliptime();
		//static int jitter = 0;
		static int last = 0;
		int help = {};
		int Fake = rand() % 3;
		static int Spin[2] = { 0, 0 };
		for( int& s : Spin )
			if( s > 180 )
				s -= 360;
			else if( s < -180 )
				s += 360;
		int yaw = Settings.GetSetting( Tab_Ragebot, AntiAim_Yaw );
		switch( yaw )
		{
				//Sideways Distortion
			case 2: //Sideways Distortion
				if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) )
				{
					if( Settings.GetSetting( Tab_Misc, Misc_AntiUntrusted ) )
					{
						if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
						{
							if( Hacks.SendPacket )
							{
								if( jitter )
									Add = -90;
								else
									Add = 90;
							}
							else
							{
								float flRandom = rand() % 5 + 1.f;
								switch( Hacks.CurrentCmd->tick_count % 4 )
								{
									case 0:
										Add = -170.f - flRandom;
										break;
									case 3:
									case 1:
										Add = 180.f;
										break;
									case 2:
										Add = 170.f + flRandom;
										break;
								}
							}
						}
						else
						{
							if( Hacks.SendPacket )
							{
								switch( Hacks.CurrentCmd->tick_count % 4 )
								{
									case 0:
										Add = -100.f;
										break;
									case 1:
										Add = 100.f;
										break;
									case 2:
										Add = 100.f;
										break;
									case 3:
										Add = -100.f;
										break;
								}
							}
							else
							{
								float flRandom = rand() % 32 + 28.f;
								switch( Hacks.CurrentCmd->tick_count % 4 )
								{
									case 0:
										Add = -100.f - flRandom;
										break;
									case 1:
										Add = 100.f + flRandom;
										break;
									case 2:
										Add = -100.f - flRandom;
										break;
									case 3:
										Add = 100.f + flRandom;
										break;
								}
							}
						}
					}
					else
					{
						//Nospread
						if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
						{
							if( Hacks.SendPacket )
							{
								switch( Hacks.CurrentCmd->tick_count % 4 )
								{
									case 0:
										Add = -100.f;
										break;
									case 3:
									case 1:
										Add = 180.f;
										break;
									case 2:
										Add = 100.f;
										break;
								}
							}
							else
							{
								if( !LocalInfo.Flags & FL_ONGROUND )
								{
									float flRandom = rand() % 32 + 28.f;
									switch( Hacks.CurrentCmd->tick_count % 4 )
									{
										case 0:
											Add = -100.f + flRandom;
											break;
										case 3:
										case 1:
											Add = 180.f;
											break;
										case 2:
											Add = 100.f + flRandom;
											break;
											break;
									}
								}
							}
						}
						else
						{
							return;
						}
					}
				}
				else
				{
					if( jitter )
						Add = -135;
					else
						Add = 135;
				}
				break;

				//180 or back
			case 3: //180 or back
				if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) )
				{
					if( Settings.GetSetting( Tab_Misc, Misc_AntiUntrusted ) )
					{
						//Matchmaking
						if( Hacks.SendPacket )
						{
							Add = -90;
						}
						else
						{
							Add = 180;
						}
					}
					else
					{
						//Nospread
						if( LocalInfo.Flags & FL_ONGROUND )
						{
							if( Hacks.SendPacket )
							{
								if( flip )
									Add = -90;
								else
									Add = 90;
							}
							else
							{
								Add = 180;
							}
						}
						else
						{
							if( Hacks.SendPacket )
							{
								Add = -90;
							}
							else
							{
								Add = 90;
							}
						}
					}
				}
				else
				{
					Add = 180;
				}
				break;

				//Crooked Distortion
			case 4: //Crooked Distortion
				if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) )
				{
					if( Hacks.SendPacket )
					{
						if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
						{
							if( !( Hacks.LocalPlayer->GetFlags() & FL_ONGROUND ) )
							{
								Add = 90;
							}
							else
							{
								Add = -90;
							}
						}
						else
						{
							Add = 170;
						}
					}
					else
					{
						if( !( Hacks.LocalPlayer->GetFlags() & FL_ONGROUND ) )
						{
							Add = -45;
						}
						else
						{
							if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
							{
								Add = -161;
							}
							else
							{
								Add = -20;
							}
						}
					}
				}
				else
				{
					if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
					{
						if( jitter )
							Add = 161;
						else
							Add = -161;
					}
					else
					{
						Add = 180;
					}
				}
				break;
				//Spin
			case 5: //Spin
				if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) )
				{
					if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
					{
						if( Hacks.SendPacket )
						{
							if( flip )
								SpinYaw += 35;
							else
								SpinYaw -= 35;
							View.y = SpinYaw;
						}
						else
						{
							float flRandom = rand() % 10 + 5.f;
							flRandom *= rand() % 2 == 0 ? -1.f : 1.f;
							switch( Hacks.CurrentCmd->tick_count % 4 )
							{
								case 0:
									JitterAng = -170.f;
									break;
								case 1:
									JitterAng = 180.f;
									break;
								case 2:
									JitterAng = 170.f;
									break;
								case 3:
									JitterAng = 180.f;
									break;
							}
							Add = JitterAng + flRandom;
						}
					}
					else
					{
						if( Hacks.SendPacket )
						{
							if( flip )
								SpinYaw += 35;
							else
								SpinYaw -= 35;
							StoredAng = SpinYaw;
							View.y = SpinYaw;
						}
						else
						{
							float flFake = rand() % 32 + 19.f;
							if( ( StoredAng ) < 0 )
								StoredAng += flFake;
							else if( ( StoredAng ) > 0 )
								StoredAng -= flFake;
							Add = StoredAng;
						}
					}
				}
				else
				{
					SpinYaw += 90;
					View.y = SpinYaw;
				}
				//Hacks.CurrentCmd->viewangles.y = SpinYaw;
				break;
				//180 Distortion
			case 6: //180 Distortion
				if( Settings.GetSetting( Tab_Ragebot, AntiAim_FakeHitbox ) )
				{
					if( Settings.GetSetting( Tab_Misc, Misc_AntiUntrusted ) )
					{
						if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
						{
							if( Hacks.SendPacket )
							{
								if( jitter )
									Add = -120;
								else
									Add = 120;
							}
							else
							{
								float flRandom = rand() % 5 + 1.f;
								switch( Hacks.CurrentCmd->tick_count % 4 )
								{
									case 0:
										Add = -170.f - flRandom;
										break;
									case 3:
									case 1:
										Add = 180.f;
										break;
									case 2:
										Add = 170.f + flRandom;
										break;
								}
							}
						}
						else
						{
							if( Hacks.SendPacket )
							{
								switch( Hacks.CurrentCmd->tick_count % 4 )
								{
								case 0:
									Add = -125.f;
									break;
								case 1:
									Add = -179.f;
									break;
								case 2:
									Add = 125.f;
									break;
								case 3:
									Add = 179.f;
									break;
								case 4:
									Add = -125.f;
									break;
								}
							}
							else
							{
								float flRandom = rand() % 33 + 30.f;
								switch( Hacks.CurrentCmd->tick_count % 4 )
								{
									case 0:
										Add = -125.f - flRandom;
										break;
									case 1:
										Add = -179.f - flRandom;
										break;
									case 2:
										Add = 125.f + flRandom;
										break;
									case 3:
										Add = 179.f - flRandom;
										break;
									case 4:
										Add = -125.f - flRandom;
										break;

								}
							}
						}
					}
					else
					{
						//Nospread
						if( Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1 )
						{
							if( Hacks.SendPacket )
							{
								SpinYaw += 45;
								View.y = SpinYaw;
							}
							else
							{
								if( !LocalInfo.Flags & FL_ONGROUND )
								{
									float flRandom = rand() % 32 + 28.f;
									switch( Hacks.CurrentCmd->tick_count % 4 )
									{
									case 0:
										Add = -125.f;
										break;
									case 1:
										Add = -179.f;
										break;
									case 2:
										Add = 125.f;
										break;
									case 3:
										Add = 179.f;
										break;
									case 4:
										Add = -125.f;
										break;
									}
								}
							}
						}
						else
						{
							return;
						}
					}
				}
				else
				{
					if( jitter )
						Add = -135;
					else
						Add = 135;
				}
				break;
			case 7:
				if (Settings.GetSetting(Tab_Ragebot, AntiAim_FakeHitbox))
				{
					static int base = 0;
					if (!(rand() % 40))
						base = rand() % 360;
					if (!Hacks.SendPacket)
					{
						View.y = base;
						if (LocalInfo.Flags & FL_ONGROUND)
						{
							switch (Hacks.CurrentCmd->tick_count % 2)
							{
							case 0:
								Add = 45;
								break;
							case 1:
								Add = 180;
								break;
							}
						}
						else
						{
							if (Hacks.LocalPlayer->GetVecVelocity().Length2D() > 1)
							{
								switch (Hacks.CurrentCmd->tick_count % 2)
								{
								case 0:
									Add = 165;
									break;
								case 1:
									Add = -90;
									break;
								}
							}
							else
							{
								switch (Hacks.CurrentCmd->tick_count % 2)
								{
								case 0:
									Add = -165;
									break;
								case 1:
									Add = 90;
									break;
								}
							}
						}
					}
					else
					{
						View.y = base;
						Add = 0;
					}
				}
				else
				{
					switch (Hacks.CurrentCmd->tick_count % 4)
					{
					case 0:
						Add = 90.f;
						break;
					case 3: case 1:
						Add = 160;
						break;
					case 2:
						Add = -170;
						break;
					}
				}
				break;
			case 8: //bait AA
			{
				CBaseEntity* pEntity = Aimbot.pTarget ? Aimbot.pTarget : Hacks.LocalPlayer;
				if (CanFireNext(pEntity))
				{
					// do bait thing
				}
				else
				{
					// do normal thing
				}
			}
		}
		Hacks.CurrentCmd->viewangles.y = View.y + Add;
		int pitch = Settings.GetSetting( Tab_Ragebot, AntiAim_Pitch );
		switch( pitch )
		{
			case 2:
				Hacks.CurrentCmd->viewangles.x = 89;
				break;
			case 3:
				Hacks.CurrentCmd->viewangles.x = 272.911758f;
				if( !Hacks.SendPacket )
					Hacks.CurrentCmd->viewangles.x = 89;
				break;
			case 4:
				Hacks.CurrentCmd->viewangles.x = 89;
				if( Hacks.LocalWeapon->isSniper() )
					Hacks.CurrentCmd->viewangles.x = 85;
				else if( Hacks.LocalWeapon->isRifle() || Hacks.LocalWeapon->isSmgW() )
					Hacks.CurrentCmd->viewangles.x = 80;
				else if( Hacks.LocalWeapon->isPistol() )
					Hacks.CurrentCmd->viewangles.x = 89;
				else if( Hacks.LocalWeapon->isMachineW() )
					Hacks.CurrentCmd->viewangles.x = 78.50;
				else if( Hacks.LocalWeapon->isShotgun() )
					Hacks.CurrentCmd->viewangles.x = 75.00;
				break;
			case 5:
				Hacks.CurrentCmd->viewangles.x = 179.99;
				break;
		}
	}

public:
	void Run()
	{
		if( !Settings.GetSetting( Tab_Ragebot, AntiAim_Enabled ) )
			return;
		if( Hacks.LocalPlayer->GetMoveType() == MOVETYPE_LADDER )
			return;
		if( Hacks.CurrentCmd->buttons & IN_USE )
			return;
		ShouldAA = true;
		Fakelag();
		Vector View = Hacks.CurrentCmd->viewangles;
		AtTargets( View );
		if( ShouldAA )
			Axis( View );
	}
} AA;
