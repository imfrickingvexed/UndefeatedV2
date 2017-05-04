#pragma once
#include "stdafx.h"
#include <chrono>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

class Misc
{
	typedef bool ( __cdecl* ServerRankRevealAllFn)( float* );

public:
	static vec_t Normalize_y( vec_t ang )
	{
		while( ang < -180.0f )
			ang += 360.0f;
		while( ang > 180.0f )
			ang -= 360.0f;
		return ang;
	}

	static void setName( const char* name )
	{
		auto namevar = Interfaces.g_ICVars->FindVar( "name" );
		*reinterpret_cast< int* >( reinterpret_cast< DWORD >( &namevar->fnChangeCallback ) + 0xC ) = 0;
		namevar->SetValue( name );
	}

	static void ServerRankReveal()
	{
		if( !Interfaces.pEngine->IsInGame() || !Interfaces.pEngine->IsConnected() )
			return;

		if( GetAsyncKeyState( VK_TAB ) == 0 )
			return;

		static float fArray[ 3 ] = { 0.f, 0.f, 0.f };

		static DWORD ServerRankReveal = ( DWORD )Utils.PatternSearch( "client.dll", ( PBYTE )"\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????x????", NULL, NULL );//Utils.PFindPattern( "client.dll", "55 8B EC 8B 0D ? ? ? ? 68 ? ? ? ? ");
		if( ServerRankReveal == 0 )
			ServerRankReveal = ( DWORD )Utils.PatternSearch( "client.dll", ( PBYTE )"\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????x????", NULL, NULL );//Utils.PFindPattern( "client.dll", "55 8B EC 8B 0D ? ? ? ? 68 ? ? ? ? ");

		if( ServerRankReveal != 0 )
		{
			Hacks.ServerRankRevealEx = ( ServerRankRevealAllFn )( ServerRankReveal );
			Hacks.ServerRankRevealEx( fArray );
		}
	}

	static void SetClanTag( const char* tag, const char* name )
	{
		static auto pSetClanTag = reinterpret_cast< void(__fastcall*)( const char*, const char* ) >( ( DWORD )( Utils.PatternSearch( "engine.dll", ( PBYTE )"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx", NULL, NULL ) ) );
		pSetClanTag( tag, name );
	}

	static int TIME_TO_TICKS( int dt )
	{
		return ( int )( 0.5f + ( float )( dt ) / Interfaces.pGlobalVars->interval_per_tick );
	}

	static float GetNetworkLatency()
	{
		INetChannelInfo* nci = Interfaces.pEngine->GetNetChannelInfo();
		if( nci )
		{
			return nci->GetAvgLatency( FLOW_OUTGOING );
		}
		return 0.0f;
	}

	static void ClantagChanger()
	{
		if( !Interfaces.pEngine->IsConnected() || !Interfaces.pEngine->IsInGame() )
			return;

		long currentTime_ms = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() ).count();
		static long timeStamp = currentTime_ms;
		if( currentTime_ms - timeStamp < 350 )
			return;

		timeStamp = currentTime_ms;
		if( Settings.GetSetting( Tab_Misc, Misc_ClanTagSpammer ) )
		{
			static int Cycle = 0;
			std::vector< string > Strings = { string( "UNDEFEATED" ), string( "UNDEFEATED" ), string( "無敗" ), string( "SUICIDE.TEAM" ), string( "SLAP.TEAM" ) };

			int iSwitch = Settings.GetMenuSetting( Tab_Misc, Misc_ClanTagSelection );

			string String = Strings[ iSwitch ];

			if( Settings.GetMenuSetting( Tab_Misc, Misc_ClanTagAnimate ) )
			{
				int iMinor = Strings[ iSwitch ].size();
				int iMajor = iMinor + 1;

				Cycle++;
				if( Cycle > iMinor )
					Cycle = -iMajor;
				String = string( iMinor, ' ' );
				for( int i = 0; i < Strings[ iSwitch ].size(); i++ )
				{
					int New = i + Cycle;
					if( New >= 0 && New < iMinor )
						String[ New ] = Strings[ iSwitch ][ i ];
				}
			}
			SetClanTag( String.c_str(), Hacks.LocalPlayer->GetName().c_str() );
		}
	}

	static void ThirdPerson( void )
	{
		static bool bThirdPerson = false;
		static bool bSpoofed = false;
		static ConVar* not_sv_cheats;
		static SpoofedConvar* big_cheta;

		if( !bSpoofed )
		{
			not_sv_cheats = Interfaces.g_ICVars->FindVar( "sv_cheats" );
			big_cheta = new SpoofedConvar( not_sv_cheats );
			big_cheta->SetBool( TRUE );
			bSpoofed = true;
		}

		if( Hacks.LocalPlayer->isAlive() && ( !bThirdPerson && Settings.GetSetting( Tab_Visuals, Visuals_ThirdPerson ) ) )
		{
			Interfaces.pEngine->ClientCmd_Unrestricted( "thirdperson", 0 );
			bThirdPerson = true;
		}
		else
		{
			Interfaces.pEngine->ClientCmd_Unrestricted( "firstperson", 0 );
			big_cheta->SetBool( FALSE );
			bThirdPerson = false;
		}
	}

	static double GetNumberOfTicksChoked( CBaseEntity* pEntity )
	{
		double flSimulationTime = pEntity->GetSimulationTime();
		double flSimDiff = ( ( double )Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick ) - flSimulationTime;
		return TIME_TO_TICKS( max(0.0f, flSimDiff) );
	}

	static bool Shooting()
	{
		if( Hacks.LocalWeapon->IsKnife() )
		{
			if( Hacks.CurrentCmd->buttons & IN_ATTACK || Hacks.CurrentCmd->buttons & IN_ATTACK2 )
				return true;
		}
		else if( Hacks.LocalWeapon->IsNade() )
		{
			CBaseCSGrenade* csGrenade = ( CBaseCSGrenade* )Hacks.LocalWeapon;
			if( csGrenade->GetThrowTime() > 0.f )
			{
				return true;
			}
		}
		else if( Hacks.CurrentCmd->buttons & IN_ATTACK && bullettime() )
		{
			if( *Hacks.LocalWeapon->GetItemDefinitionIndex() == weapon_revolver && Settings.GetSetting( Tab_Ragebot, Ragebot_AimbotEnabled ) )
			{
				if( Hacks.LocalWeapon->GetPostponeFireReadyTime() - GetServerTime() <= 0.05f )
				{
					return true;
				}
			}
			else
				return true;
		}
		return false;
	}

	static void DrawScope()
	{
		if( Settings.GetSetting( Tab_Visuals, Visuals_NoScope ) && Hacks.LocalWeapon )
			if( Hacks.LocalWeapon->isSniper() )
				if( Hacks.LocalPlayer->m_bIsScoped() )
				{
					int width = 0;
					int height = 0;
					Interfaces.pEngine->GetScreenSize( width, height );
					DrawRect( ( width / 2 ), 0, ( width / 2 ) + 1, height, CColor( 0, 0, 0, 255 ) );
					DrawRect( 0, ( height / 2 ), width, ( height / 2 ) + 1, CColor( 0, 0, 0, 255 ) );
				}
	}

	static void CircleStrafer( float& OldAngle )
	{
		static int Angle = 0;
		if( OldAngle - Angle > 360 )
			Angle -= 360;
		static bool shouldspin = false;
		static bool enabled = false;
		static bool check = false;
		if( GetAsyncKeyState( ( int )Settings.GetSetting( Tab_Misc, Misc_CircleStrafeKey ) ) )
		{
			if( !check )
				enabled = !enabled;
			check = true;
		}
		else
			check = false;
		if( enabled )
		{
			shouldspin = true;
			Hacks.CurrentCmd->buttons |= IN_JUMP;
			Hacks.CurrentCmd->buttons |= IN_DUCK;
		}
		if( shouldspin )
		{
			Vector Dir;
			g_Math.angleVectors( Vector( 0, Angle, 0 ), Dir );
			Dir *= 8218;
			Ray_t ray;
			CTraceWorldOnly filter;
			trace_t trace;
			ray.Init( Hacks.LocalPlayer->GetEyePosition(), Hacks.LocalPlayer->GetVecOrigin() + Dir );
			Interfaces.pTrace->TraceRay( ray, MASK_SHOT, &filter, &trace );
			auto temp = 3.4f / ( ( trace.endpos - Hacks.LocalPlayer->GetVecOrigin() ).Length() / 100.f );
			if( temp < 3.4f )
				temp = 3.4f;
			if( enabled )
			{
				Angle += temp;
				Hacks.CurrentCmd->sidemove = -450;
			}
			else
			{
				if( OldAngle - Angle < temp )
				{
					Angle = OldAngle;
					shouldspin = false;
				}
				else
					Angle += temp;
			}
			OldAngle = Angle;
		}
		else
			Angle = OldAngle;
	}

	static Vector Normalize( Vector& angs )
	{
		while( angs.y < -180.0f )
			angs.y += 360.0f;
		while( angs.y > 180.0f )
			angs.y -= 360.0f;
		if( angs.x > 89.0f )
			angs.x = 89.0f;
		if( angs.x < -89.0f )
			angs.x = -89.0f;
		angs.z = 0;
		return angs;
	}

	static bool AimStep( Vector angSource, Vector& angDestination )
	{
		Vector angDelta = Normalize( angDestination - angSource );
		if( angDelta.Abs() > 40.f )
		{
			angDestination = Normalize( angSource + angDelta / angDelta.Abs() * 40.f );
			return false;
		}
		return true;
	}

	static float GetServerTime()
	{
		return ( float )Hacks.LocalPlayer->GetTickBase() * Interfaces.pGlobalVars->interval_per_tick;
	}

	static bool bullettime()
	{
		if( !Hacks.LocalWeapon )
			return false;
		float flNextPrimaryAttack = Hacks.LocalWeapon->NextPrimaryAttack();

		return flNextPrimaryAttack <= GetServerTime();
	}

	static void MoveFix( CInput::CUserCmd* cmd, Vector& realvec )
	{
		Vector vMove( cmd->forwardmove, cmd->sidemove, cmd->upmove );
		float flSpeed = sqrt( vMove.x * vMove.x + vMove.y * vMove.y ), flYaw;
		Vector vMove2;
		g_Math.vectorAnglesVec( vMove, vMove2 );
		Normalize( vMove2 );
		flYaw = DEG2RAD(cmd->viewangles.y - realvec.y + vMove2.y);
		cmd->forwardmove = cos( flYaw ) * flSpeed;
		cmd->sidemove = sin( flYaw ) * flSpeed;
		if( 90 < abs( cmd->viewangles.x ) && 180 > abs( cmd->viewangles.x ) )
			cmd->forwardmove *= -1;
	}

	static void CalcAngle( Vector src, Vector dst, Vector& angles )
	{
		Vector delta = src - dst;
		double hyp = delta.Length2D();
		angles.y = atan( delta.y / delta.x ) * 57.295779513082f;
		angles.x = atan( delta.z / hyp ) * 57.295779513082f;
		if( delta.x >= 0.0 )
			angles.y += 180.0f;
		angles.z = 0;
	}

	static int FovTo( Vector From, Vector To )
	{
		From -= To;
		Normalize( From );
		return ( abs( From.y ) + abs( From.x ) );
	}
};
