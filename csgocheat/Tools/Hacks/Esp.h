#pragma once
#include "../../stdafx.h"
#include "CreateMoveETC.h"

class Esp
{
public:
	/*static void Run( )
	{
		Watermark();

		if( !Hacks.LocalPlayer )
			return;

		//if( !Settings.GetBoolSetting( Tab_Visuals, Visuals_Enable ) )
		//	return;

		if( !Interfaces.pEngine->IsInGame() || !Interfaces.pEngine->IsConnected() )
			return;

		static bool bCrosshair;

		if ( Settings.GetBoolSetting( Tab_Visuals, Visuals_Crosshair ) )
		{
			Crosshair();
			if ( !bCrosshair )
			{
				Interfaces.pEngine->ClientCmd_Unrestricted( "crosshair 0", 0 );
				bCrosshair = true;
			}
		}
		else
		{
			if ( bCrosshair )
			{
				Interfaces.pEngine->ClientCmd_Unrestricted( "crosshair 1", 0 );
				bCrosshair = false;
			}
		}

		for ( int i = 0; i < Interfaces.pEntList->GetHighestEntityIndex( ); i++ )
		{
			CBaseEntity* pEntity = Interfaces.pEntList->GetClientEntity( i );
			if( !pEntity )
				continue;

			if( pEntity->IsWeapon () )
				WorldESP(pEntity);
			else
				PlayerESP( pEntity );
		}
	}*/

	static void ThirdPerson()
	{
		static bool bSpoofed = false;
		static bool bThirdperson = false;
		MEMCHECK;

		static ConVar* sv_cheat;
		static SpoofedConvar* big_cheta;

		if( !bSpoofed )
		{
			sv_cheat = Interfaces.g_ICVars->FindVar( "sv_cheats" );
			big_cheta = new SpoofedConvar( sv_cheat );
			bSpoofed = true;
		}

		if( bSpoofed )
		{
			if( !bThirdperson && Settings.GetMenuSetting( Tab_Visuals, Visuals_ThirdPerson ) && Hacks.LocalPlayer->GetHealth() > 0 )
			{
				big_cheta->SetBool( true );
				Interfaces.pEngine->ClientCmd_Unrestricted( "thirdperson", 0 );
				bThirdperson = true;
			}
			else if( bThirdperson && !Settings.GetMenuSetting( Tab_Visuals, Visuals_ThirdPerson ) )
			{
				big_cheta->SetBool( false );
				Interfaces.pEngine->ClientCmd_Unrestricted( "firstperson", 0 );
				bThirdperson = false;
			}
			if( Hacks.LocalPlayer->GetHealth() <= 0 )
			{
				Interfaces.pEngine->ClientCmd_Unrestricted( "firstperson", 0 );
				bThirdperson = false;
			}
		}
	}

	static void Watermark()
	{
		int W, H;
		Interfaces.pEngine->GetScreenSize( W, H );
		//RECT rect = GetTextSize1 ( Hacks.Font_Watermark, L"無敗" );
		Interfaces.pSurface->DrawT( W * 0.15, 20, CColor( 255, 255, 255, 120 ), Hacks.Font_Watermark, false, "UNDEFEATED V2" );
	}

	static void Crosshair()
	{
		int W, H, cW, cH;
		Interfaces.pEngine->GetScreenSize( W, H );

		cW = W / 2;
		cH = H / 2;

		int dX = W / Hacks.FOV;
		int dY = H / Hacks.FOV;
		int drX, drY;

		if( Settings.GetSetting( Tab_Visuals, Visuals_CrosshairDynamic ) )
		{
			if( Settings.GetSetting( Tab_Visuals, Visuals_NoRecoil ) )
			{
				drX = cW - ( int )( dX * ( Hacks.LocalPlayer->GetPunchAngle().y * 2 ) );
				drY = cH + ( int )( dY * ( Hacks.LocalPlayer->GetPunchAngle().x * 2 ) );
			}
			else
			{
				drX = cW - ( int )( dX * ( ( ( Hacks.LocalPlayer->GetPunchAngle().y * 2.f ) * 0.45f ) + Hacks.LocalPlayer->GetPunchAngle().y ) );
				drY = cH + ( int )( dY * ( ( ( Hacks.LocalPlayer->GetPunchAngle().x * 2.f ) * 0.45f ) + Hacks.LocalPlayer->GetPunchAngle().x ) );
			}
		}
		else
		{
			drX = cW;
			drY = cH;
		}

		Interfaces.pSurface->DrawSetColor( 0, 0, 0, 255 );
		Interfaces.pSurface->DrawFilledRect( drX - 4, drY - 2, drX - 4 + 8, drY - 2 + 4 );
		Interfaces.pSurface->DrawFilledRect( drX - 2, drY - 4, drX - 2 + 4, drY - 4 + 8 );

		Interfaces.pSurface->DrawSetColor( 255, 255, 255, 255 );
		Interfaces.pSurface->DrawFilledRect( drX - 3, drY - 1, drX - 3 + 6, drY - 1 + 2 );
		Interfaces.pSurface->DrawFilledRect( drX - 1, drY - 3, drX - 1 + 2, drY - 3 + 6 );
	}

	static void DrawInfo( RECT rect, CBaseEntity* pPlayer )
	{
		player_info_t info;
		static class Text
		{
		public:
			string text;
			int side;
			int Font;

			Text( string text, int side, int Font ) : text( text ), side( side ), Font( Font )
			{
			}
		};
		std::vector< Text > texts;
		if( pPlayer->IsHostage() )
		{
			if( Settings.GetSetting( Tab_Visuals, Visuals_Hostage ) )
				texts.push_back( Text( "Hostage", 2, Hacks.Font_ESP ) );
		}
		else if( Interfaces.pEngine->GetPlayerInfo( pPlayer->GetIndex(), &info ) )
		{
			if( Settings.GetSetting( Tab_Visuals, Visuals_Name ) )
				texts.push_back( Text( pPlayer->GetName(), 0, Hacks.Font_ESP ) );

			CBaseCombatWeapon* weapon = pPlayer->GetActiveBaseCombatWeapon();
			if( weapon )
			{
				if( Settings.GetSetting( Tab_Visuals, Visuals_Weapons ) && weapon )
					texts.push_back( Text( weapon->GetGunIcon(), 2, Hacks.Font_Weapons ) );
				if( Settings.GetSetting( Tab_Visuals, Visuals_Flashed ) && pPlayer->IsFlashed() )
					texts.push_back( Text( "Flashed", 2, Hacks.Font_ESP ) );
				//if( Settings.GetSetting( Tab_Visuals, Visuals_ArmorBar ) )
				//	texts.push_back( Text( pPlayer->GetArmorIcon(), 1, Hacks.Font_Weapons ) );
			}
			//delete[]weapon;
		}
		else if( pPlayer->IsWeapon() )
		{
			CBaseCombatWeapon* weapon = static_cast< CBaseCombatWeapon* >( pPlayer );

			if( weapon )
				texts.push_back( Text( ( char* )weapon->GetGunIcon(), 0, Hacks.Font_Weapons ) );
			int iClip = weapon->ammo();
			string s = iClip >= 0 && iClip <= 200 && weapon->IsGun() ? std::to_string( iClip ) : "--";
			if( weapon )
				texts.push_back( Text( strdup( ( s/* + "/" + std::to_string ( weapon->ammo2 () )*/ ).c_str() ), 2, Hacks.Font_ESP ) );
			//delete[]weapon;
		}

		int middle = ( ( rect.right - rect.left ) / 2 ) + rect.left;
		int Top[3] = { rect.top,rect.top, rect.bottom };
		for( auto text : texts )
		{
			RECT nameSize = GetTextSize( text.Font, ( char* )text.text.c_str() );
			switch( text.side )
			{
				case 0:
					Top[ 0 ] -= nameSize.bottom + 1;
					Interfaces.pSurface->DrawT( middle - ( nameSize.right / 2 ), Top[ 0 ], CColor( 255, 255, 255, 255 ), text.Font, false, ( char* )text.text.c_str() );
					break;
				case 1:
					Interfaces.pSurface->DrawT( rect.right + 2, Top[ 1 ], CColor( 255, 255, 255, 255 ), text.Font, false, ( char* )text.text.c_str() );
					Top[ 1 ] += nameSize.bottom + 1;
					break;
				case 2:
					Interfaces.pSurface->DrawT( middle - ( nameSize.right / 2 ), Top[ 2 ], CColor( 255, 255, 255, 255 ), text.Font, false, ( char* )text.text.c_str() );
					Top[ 2 ] += nameSize.bottom + 1;
					break;
			}
		}
	}

	static void DoEsp()
	{
		static bool bCrosshair;

		ThirdPerson();

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

		int width = 0;
		int height = 0;
		Interfaces.pEngine->GetScreenSize( width, height );
		for( auto i = 0; i <= Interfaces.pEntList->GetHighestEntityIndex(); i++ )
		{
			CBaseEntity* pEntity = Interfaces.pEntList->GetClientEntity( i );
			if( pEntity == nullptr )
				continue;
			if( pEntity == Hacks.LocalPlayer )
				continue;
			if( pEntity->IsDormant() )
				continue;
			player_info_t info;
			if( Interfaces.pEngine->GetPlayerInfo( pEntity->GetIndex(), &info ) )
			{
				CColor clr = pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam() ? CColor( 50, 150, 255, 255 ) : CColor( 235, 235, 235, 255 );
				if( pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam() && !Settings.GetSetting( Tab_Visuals, Visuals_EspTeam ) )
					continue;
				if( !pEntity->isAlive() )
					continue;

				bool PVS = false;
				RECT rect = DynamicBox( pEntity, PVS );

				DrawInfo( rect, pEntity );

				if( Settings.GetSetting( Tab_Visuals, Visuals_BoxEnabled ) )
					DrawBox( rect, clr );
				if( Settings.GetSetting( Tab_Visuals, Visuals_AimLines ) )
					DrawSnapLine( Vector( rect.left + ( ( rect.right - rect.left ) / 2 ), rect.bottom, 0 ), clr );
				if( Settings.GetSetting( Tab_Visuals, Visuals_Skeltal ) )
					Skeleton( pEntity );
				if( Settings.GetSetting( Tab_Visuals, Visuals_HealthBar ) )
					DrawHealth( rect, pEntity );
				if( Settings.GetSetting( Tab_Visuals, Visuals_EngineRadar ) )
					*pEntity->IsSpottedPtr() = true;
				//if ( Settings.GetSetting( Tab_Visuals, Visuals_Dlights ) ) DLight(pEntity, clr);
			}
			if( Settings.GetSetting( Tab_Visuals, Visuals_DroppedWeapons ) && pEntity->IsWeapon() )
			{
				auto weapon = static_cast< CBaseCombatWeapon* >( pEntity );
				auto plr = Interfaces.pEntList->GetClientEntityFromHandle( ( HANDLE )weapon->GetOwnerHandle() );
				Vector vOrig3D = pEntity->GetAbsOrigin();
				Vector vOrig;
				Interfaces.g_pDebugOverlay->ScreenPosition( vOrig3D, vOrig );
				player_info_t info;
				if( plr == nullptr || plr->isValidPlayer() || !plr->isAlive() || !( plr->GetHealth() > 0 ) || !( Interfaces.pEngine->GetPlayerInfo( plr->GetIndex(), &info ) ) )
				{
					if( !( vOrig3D.y >= -5 && vOrig3D.y <= 5 && vOrig3D.x >= -5 && vOrig3D.x <= 5 ) )
					{
						bool k = false;
						if( *weapon->GetItemDefinitionIndex() != WEAPON_C4 )
						{
							RECT rect = DynamicBox( pEntity, k );
							//DrawBox(rect, CColor(255, 255, 255, 255));
							DrawInfo( rect, pEntity );
						}
						else
						{
							bool k = false;
							Interfaces.pSurface->DrawT( vOrig.x, vOrig.y - 5, CColor( 255, 255, 255, 255 ), 0xA1, true, "j" );
						}
					}
				}
				//delete[]weapon;
			}
			if( Settings.GetSetting( Tab_Visuals, Visuals_Hostage ) , pEntity->IsHostage() )
			{
				CColor clr = CColor( 255, 255, 255, 255 );
				bool t;
				RECT rect = DynamicBox( pEntity, t );
				DrawInfo( rect, pEntity );
			}
			else if( Settings.GetSetting( Tab_Visuals, Visuals_C4 ) && strstr( pEntity->GetClientClass()->m_pNetworkName, "CPlantedC4" ) )
			{
				if( !pEntity->BombDefused() )
				{
					float flTime = pEntity->BombTimer() - Interfaces.pGlobalVars->curtime;
					if( flTime > 0.f )
					{
						Vector vPos3D = pEntity->GetAbsOrigin();
						Vector vPos;
						Interfaces.g_pDebugOverlay->ScreenPosition( vPos3D, vPos );

						bool bCanDefuse = ( Hacks.LocalPlayer->HasKit() && flTime > 5.5f ) || flTime > 10.5f;
						Interfaces.pSurface->DrawT( vPos.x, vPos.y - 5, CColor( 255, 255, 255, 255 ), 0xA1, false, "j" );
						Interfaces.pSurface->DrawT( vPos.x + 25, vPos.y, bCanDefuse ? CColor( 255, 255, 255, 255 ) : CColor( 255, 0, 0, 255 ), Hacks.Font_ESP, false, "%.2f", flTime );

						if( Hacks.LocalPlayer->GetHealth() > 0 )
						{
							float flArmor = Hacks.LocalPlayer->GetArmor();
							float flDistance = Hacks.LocalPlayer->GetAbsOrigin().DistTo( pEntity->GetAbsOrigin() );
							float a = 450.7f;
							float b = 75.68f;
							float c = 789.2f;
							float d = ( ( flDistance - b ) / c );

							float flDmg = a * exp( -d * d );
							float flDamage = flDmg;

							if( flArmor > 0 )
							{
								float flNew = flDmg * 0.5f;
								float flArmor = ( flDmg - flNew ) * 0.5f;

								if( flArmor > static_cast< float >( flArmor ) )
								{
									flArmor = static_cast< float >( flArmor ) * ( 1.f / 0.5f );
									flNew = flDmg - flArmor;
								}

								flDamage = flNew;
							}

							int x, y;
							Interfaces.pEngine->GetScreenSize( x, y );

							if( flDamage > 1 )
								Interfaces.pSurface->DrawT( x / 2, y * 0.1, flDamage >= Hacks.LocalPlayer->GetHealth() ? CColor( 255, 0, 0, 255 ) : CColor( 255, 255, 255, 255 ), Hacks.Font_Watermark, true, "THE BOMB WILL EXPLODE IN %.1f SECONDS DEALING %.f DAMAGE ", flTime, flDamage );
						}
					}
				}
			}
			//delete[]pEntity;
		}
	}

	static void WorldESP( CBaseEntity* pEntity )
	{
	}

	static RECT DynamicBox( CBaseEntity* pPlayer, bool& PVS )
	{
		Vector trans = pPlayer->GetVecOrigin();

		Vector min;
		Vector max;

		min = pPlayer->BBMin();
		max = pPlayer->BBMax();

		Vector pointList[] = {
			Vector( min.x, min.y, min.z ),
			Vector( min.x, max.y, min.z ),
			Vector( max.x, max.y, min.z ),
			Vector( max.x, min.y, min.z ),
			Vector( max.x, max.y, max.z ),
			Vector( min.x, max.y, max.z ),
			Vector( min.x, min.y, max.z ),
			Vector( max.x, min.y, max.z )
		};

		Vector Distance = pointList[ 0 ] - pointList[ 1 ];
		int dst = Distance.Length();
		dst /= 1.3f;
		Vector angs;
		Misc::CalcAngle( trans, Hacks.LocalPlayer->GetEyePosition(), angs );

		Vector all[8];
		angs.y += 45;
		for( int i = 0; i < 4; i++ )
		{
			g_Math.angleVectors( angs, all[ i ] );
			all[ i ] *= dst;
			all[ i + 4 ] = all[ i ];
			all[ i ].z = max.z;
			all[ i + 4 ].z = min.z;
			VectorAdd( all[ i ], trans, all[ i ] );
			VectorAdd( all[ i + 4 ], trans, all[ i + 4 ] );
			angs.y += 90;
		}

		Vector flb, brt, blb, frt, frb, brb, blt, flt;
		PVS = true;

		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 3 ], flb ) )
			PVS = false;
		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 0 ], blb ) )
			PVS = false;
		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 2 ], frb ) )
			PVS = false;
		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 6 ], blt ) )
			PVS = false;
		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 5 ], brt ) )
			PVS = false;
		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 4 ], frt ) )
			PVS = false;
		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 1 ], brb ) )
			PVS = false;
		if( !Interfaces.g_pDebugOverlay->ScreenPosition( all[ 7 ], flt ) )
			PVS = false;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

		float left = flb.x;
		float top = flb.y;
		float right = flb.x;
		float bottom = flb.y;

		for( int i = 0; i < 8; i++ )
		{
			if( left > arr[ i ].x )
				left = arr[ i ].x;
			if( top > arr[ i ].y )
				top = arr[ i ].y;
			if( right < arr[ i ].x )
				right = arr[ i ].x;
			if( bottom < arr[ i ].y )
				bottom = arr[ i ].y;
		}
		RECT rect;
		rect.left = left;
		rect.bottom = bottom;
		rect.right = right;
		rect.top = top;
		return rect;
	}

	static void Skeleton( CBaseEntity* pEntity )
	{
		CColor col = CColor( 0, 200, 255, 255 );
		if( pEntity->GetTeam() == 2 )
			col = CColor( 202, 137, 255, 255 );
		if( Settings.GetSetting( Tab_Visuals, Visuals_Skeltal ) )
		{
			studiohdr_t* pStudioHdr = Interfaces.g_pModelInfo->GetStudioModel( pEntity->GetModel() );

			if( !pStudioHdr )
				return;

			Vector vParent, vChild, sParent, sChild;

			for( int j = 0; j < pStudioHdr->numbones; j++ )
			{
				mstudiobone_t* pBone = pStudioHdr->GetBone( j );

				if( pBone && ( pBone->flags & BONE_USED_BY_HITBOX ) && ( pBone->parent != -1 ) )
				{
					vChild = pEntity->GetBonePos( j );
					vParent = pEntity->GetBonePos( pBone->parent );
					Interfaces.g_pDebugOverlay->ScreenPosition( vParent, sParent );
					Interfaces.g_pDebugOverlay->ScreenPosition( vChild, sChild );
					int red = 0;
					int green = 0;
					int blue = 0;
					int alpha = 0;
					col.GetColor( red, green, blue, alpha );
					Interfaces.pSurface->DrawSetColor( red, green, blue, alpha );
					Interfaces.pSurface->DrawLine( sParent[ 0 ], sParent[ 1 ], sChild[ 0 ], sChild[ 1 ] );
				}
				//delete[]pBone;
			}
		}
	}

	static void DrawSnapLine( Vector to, CColor clr )
	{
		int width = 0;
		int height = 0;
		Interfaces.pEngine->GetScreenSize( width, height );
		Vector From( ( width / 2 ), height - 1, 0 );
		DrawLine( From.x, From.y, to.x, to.y, clr );
	}

	static void DrawBox( RECT rect, CColor Color )
	{
		DrawOutlinedRect( rect.left - 1, rect.top - 1, rect.right + 1, rect.bottom + 1, CColor( 0, 0, 0, 150 ) );
		DrawOutlinedRect( rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, CColor( 0, 0, 0, 125 ) );
		DrawOutlinedRect( rect.left, rect.top, rect.right, rect.bottom, Color );
	}

	static void DrawHealth( RECT rect, CBaseEntity* pPlayer )
	{
		float HealthValue = pPlayer->GetHealth();
		int Red = 255 - ( HealthValue * 2.55 );
		int Green = HealthValue * 2.55;
		float height = ( rect.bottom - rect.top ) * ( HealthValue / 100 );
		DrawRect( rect.left - 4, rect.top - 1, rect.left - 2, rect.bottom + 1, CColor( 0, 0, 0, 150 ) );
		DrawRect( rect.left - 4, rect.bottom - height, rect.left - 3, rect.bottom, CColor( Red, Green, 0, 255 ) );
	}

	static void ArmorBar( Vector bot, Vector top, float armor, int off )
	{
		float h = ( bot.y - top.y );
		float offset = ( h / 4.f ) + 2 + off * 6;

		UINT hp = h - ( UINT )( ( h * armor ) / 100 );

		Interfaces.pSurface->DrawSetColor( 0, 0, 0, 150 );
		Interfaces.pSurface->DrawOutlinedRect( ( top.x - offset ) - 1, top.y - 1, ( top.x - offset ) - 1 + 5, top.y - 1 + h + 2 );
		Interfaces.pSurface->DrawSetColor( 30, 144, 255, 255 );
		Interfaces.pSurface->DrawFilledRect( ( top.x - offset ), top.y + hp, ( top.x - offset ) + 3, top.y + hp + h - hp + 0 );
	}

	static void IWBox( Vector vTopScreen, Vector vScreen, float h, float w, CColor clr )
	{
	}

	static void DLight( CBaseEntity* pEntity, CColor clr )
	{
		dlight_t* pDlight = Interfaces.g_pEffects->CL_AllocDlight( pEntity->GetIndex() );

		pDlight->origin = pEntity->GetAbsOrigin();
		pDlight->radius = 250.f;
		pDlight->color = clr;
		pDlight->die = Interfaces.pGlobalVars->curtime + 0.1f;
		pDlight->decay = 40.f;
		//delete[]pDlight;
	}
};
