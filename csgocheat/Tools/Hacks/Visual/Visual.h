#pragma once
#include "../../../stdafx.h"

CColor TeamVisible, TeamInvisible, EnemyVisible, EnemyInvisible;
CColor Black( 0, 0, 0, 255 );
CColor White( 255, 255, 255, 255 );
CColor Red( 255, 0, 0, 255 );
CColor Blue( 0, 0, 255, 255 );
CColor Green( 0, 255, 0, 255 );
CColor Yellow( 255, 255, 0, 255 );
CColor Grey( 128, 128, 128, 255 );
CColor Pink( 255, 20, 147, 255 );
CColor Sky( 0, 191, 255, 255 );
CColor Cyan( 0, 255, 255, 255 );
CColor Purple( 160, 32, 240, 255 );


class CVisual
{
public:
	static void DrawString( int font, int x, int y, int align, CColor color, const char* text )
	{
		char buf[1024];
		sprintf( buf, text );

		wchar_t wbuf[1024];
		MultiByteToWideChar( CP_UTF8, 0, buf, 256, wbuf, 256 );

		Interfaces.pSurface->DrawString( font, x, y, align, color, wbuf );
	}

	static void Run()
	{
		Watermark();

		//if( !Sets.Visuals.Enabled )
		//	return;

		if( !Interfaces.pEngine->IsInGame() || !Interfaces.pEngine->IsConnected() )
			return;

		static bool bCrosshair = false;

		if( Settings.GetSetting( Tab_Visuals, Visuals_Crosshair ) )
		{
			Crosshair();
			if( !bCrosshair )
			{
				Interfaces.pEngine->ClientCmd_Unrestricted( "crosshair 0", 0 );
				bCrosshair = true;
			}
		}
		else
		{
			if( bCrosshair )
			{
				Interfaces.pEngine->ClientCmd_Unrestricted( "crosshair 1", 0 );
				bCrosshair = false;
			}
		}
		//hmm not sure what u want here vxd or what this is so ill leave it commented out
		/*
		if ( Sets.Colors.UseColors )
		{
		TeamVisible = MathFunctions::HueToColor( Sets.Colors.Colors[ 0 ] );
		TeamInvisible = MathFunctions::HueToColor( Sets.Colors.Colors[ 1 ] );
		EnemyVisible = MathFunctions::HueToColor( Sets.Colors.Colors[ 2 ] );
		EnemyInvisible = MathFunctions::HueToColor( Sets.Colors.Colors[ 3 ] );
		}
		else
		{
		TeamVisible = Blue;
		TeamInvisible = Green;
		EnemyVisible = Yellow;
		EnemyInvisible = Red;
		}
		*/ //fix <<

		for( int i = 0; i < Interfaces.pEntList->GetHighestEntityIndex(); i++ )
		{
			CBaseEntity* pEnt = Interfaces.pEntList->GetClientEntity( i );
			if( !pEnt )
				continue;
			if( pEnt->GetClientClass()->m_ClassID == 35 )
				PlayerESP( i );
			else
				WorldESP( i );
		}
	}

	static void WorldESP( int index )
	{
	}

	static void PlayerESP( int index )
	{
		CBaseEntity* pEntity = Interfaces.pEntList->GetClientEntity( index );

		if( !pEntity )
			return;

		if( !pEntity->isValidPlayer() )
			return;

		if( pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam() && !Settings.GetSetting( Tab_Visuals, Visuals_EspTeam ) )
			return;
		//fix below
		if( pEntity->GetTeam() != Hacks.LocalPlayer->GetTeam() && !Settings.GetSetting( Tab_Visuals, Visuals_BoxEnabled ) )
			return;
		//end
		Vector vMax = pEntity->BBMax();
		Vector vPos, vPos3D;
		Vector vTop, vTop3D;
		Vector vHead, vHead3D;

		vPos3D = pEntity->GetAbsOrigin();
		vTop3D = vPos3D + Vector( 0, 0, vMax.z );
		vHead3D = pEntity->GetBonePos( 8 );

		if( !Interfaces.g_pDebugOverlay->ScreenPosition( vPos3D, vPos ) || !Interfaces.g_pDebugOverlay->ScreenPosition( vTop3D, vTop ) || !Interfaces.g_pDebugOverlay->ScreenPosition( vHead3D, vHead ) )
			return;

		float flHeight = vPos.y - vTop.y;
		float flWidth = flHeight / 4.5f;
		bool bVisible;

		Ray_t ray;
		trace_t tr;
		ray.Init( Hacks.LocalPlayer->GetEyePosition(), pEntity->GetBonePos( 8 ) );
		CTraceFilter filter;
		filter.pSkip = Hacks.LocalPlayer;
		Interfaces.pTrace->TraceRay( ray, MASK_SHOT, &filter, &tr );

		bVisible = tr.m_pEnt == pEntity;

		CColor clrVisible = pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam() ? TeamVisible : EnemyVisible;
		CColor clrInvisible = pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam() ? TeamInvisible : EnemyInvisible;

		if( Settings.GetSetting( Tab_Visuals, Visuals_Skeltal ) )
			Skeleton( pEntity, White );

		if( Settings.GetSetting( Tab_Visuals, Visuals_AimLines ) )
			Snapline( pEntity->GetAbsOrigin(), bVisible ? clrVisible : clrInvisible );

		// finish ESP ( boxes, fill, dLight, Name, wepName, health, armor, flashed, bomb / defuser etc. )
	}

	static void Watermark()
	{
		int iScreenW, iScreenH;
		Interfaces.pEngine->GetScreenSize( iScreenW, iScreenH );
		//DrawString(Hacks.Font_ESP, iScreenW * 0.1, iScreenH * 0.05, FONT_LEFT, White, "UNDEFEATED V2");
	}

	static void Skeleton( CBaseEntity* pEntity, CColor Color )
	{
		studiohdr_t* pStudioModel = Interfaces.g_pModelInfo->GetStudioModel( pEntity->GetModel() );
		if( pStudioModel )
		{
			static matrix3x4 pBoneToWorldOut[128];
			if( pEntity->SetupBones( pBoneToWorldOut, 128, 256, Interfaces.pGlobalVars->curtime ) )
			{
				for( int i = 0; i < pStudioModel->numbones; i++ )
				{
					mstudiobone_t* pBone = pStudioModel->GetBone( i );
					if( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
						continue;

					Vector vBonePos1;
					if( !Interfaces.g_pDebugOverlay->ScreenPosition( Vector( pBoneToWorldOut[ i ][ 0 ][ 3 ], pBoneToWorldOut[ i ][ 1 ][ 3 ], pBoneToWorldOut[ i ][ 2 ][ 3 ] ), vBonePos1 ) )
						continue;

					Vector vBonePos2;
					if( !Interfaces.g_pDebugOverlay->ScreenPosition( Vector( pBoneToWorldOut[ pBone->parent ][ 0 ][ 3 ], pBoneToWorldOut[ pBone->parent ][ 1 ][ 3 ], pBoneToWorldOut[ pBone->parent ][ 2 ][ 3 ] ), vBonePos2 ) )
						continue;

					Interfaces.pSurface->DrawSetColor( Color._color[ 0 ], Color._color[ 1 ], Color._color[ 2 ], Color._color[ 3 ] );
					Interfaces.pSurface->DrawLine( ( int )vBonePos1.x, ( int )vBonePos1.y, ( int )vBonePos2.x, ( int )vBonePos2.y );
				}
			}
		}
	}

	static void Snapline( Vector vTo, CColor Color )
	{
		int iScreenW, iScreenH;
		Interfaces.pEngine->GetScreenSize( iScreenW, iScreenH );

		Vector vFrom = Vector( iScreenW / 2, iScreenH, 0 );

		Interfaces.pSurface->DrawSetColor( Color._color[ 0 ], Color._color[ 1 ], Color._color[ 2 ], Color._color[ 3 ] );
		Interfaces.pSurface->DrawLine( vFrom.x, vFrom.y, vTo.x, vTo.y ); //pSnapline
	}

	static void Crosshair()
	{
	}
};
