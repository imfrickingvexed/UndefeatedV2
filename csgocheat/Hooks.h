#pragma once
#include <vector>
#include <algorithm>
#include "stdafx.h"
#include "resource.h"
#include "SDK/SDK Headers/CGameEvent.h"

void __fastcall Hooked_Createmove( CInput* thisptr, void* _EAX, int sequence_number, float input_sample_frametime, bool active );

void __stdcall Hooked_PaintTraverse( unsigned int vguiPanel, bool forceRepaint, bool allowForce );

void __stdcall Hooked_Frame_Stage_Notify( ClientFrameStage_t curStage );

void __stdcall Hooked_OverrideView( CViewSetup* pSetup );

LRESULT __stdcall Hooked_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void __fastcall Hooked_DrawModelExecute( void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld );

void __stdcall Hooked_ClientCmdUnrestricted( const char* szCmdString, char flag );

void __stdcall Hooked_PlaySound( const char* pSample );

void Init();

//void __stdcall Hooked_OverrideMouseInput(float *x, float* y);

extern VTHookManager VMTClientMode;
extern VTHookManager VMTGameEvent;

class CHackManager
{
private:
	/* Typedefs */
	typedef void (__thiscall *iPaintTraverse)( void*, unsigned int, bool, bool );

	typedef void (__stdcall *iCreateMoveFn)( CInput*, void*, int, float, bool );

	typedef void (__stdcall *iFrameStageNotifyFn)( ClientFrameStage_t );

	typedef void (__stdcall *iOverRideViewFn)( CViewSetup* );

	typedef void (__thiscall* IDrawModelExecute)( void*, void*, void*, const ModelRenderInfo_t&, matrix3x4* );

	typedef void (__stdcall* ClientCmdUnresticted)( const char* szCmdString, char flag );

	typedef bool ( __cdecl* ServerRankRevealAllFn)( float* );

	typedef void ( __stdcall* PlaySoundFn )( const char* pSample );

	typedef void ( __stdcall* IsReadyFn )();

	//typedef void(__stdcall* iOverrideMouseInput)(float *x, float *y);

public:
	std::vector<IGameEventListener2*> listeners = {};

	/* Original Functions */
	DWORD oCreateMove;
	iPaintTraverse oPaintTraverse;
	iFrameStageNotifyFn oFrameStageNotify;
	IDrawModelExecute oDrawModelExecute;
	iOverRideViewFn oOverrideView;
	ClientCmdUnresticted oClientCmdUnresticted;
	PlaySoundFn oPlaySound;
	ServerRankRevealAllFn ServerRankRevealEx;
	IsReadyFn IsReadyEx;
	//iOverrideMouseInput oOverrideMouseInput;

	/* Holding Varialbles */
	CInput::CUserCmd* CurrentCmd;
	CBaseEntity* LocalPlayer;
	CBaseCombatWeapon* LocalWeapon;
	Vector LastAngles;
	bool SendPacket = true;
	float FOV;

	DWORD Font_MenuTitle;
	DWORD Font_HUD;
	DWORD Font_HUD_Big;
	DWORD Font_Watermark;
	DWORD Font_Tab;
	DWORD Font_Logo;
	DWORD Font_ESP;
	DWORD Font_Controls;
	DWORD Font_Icons;
	DWORD Font_Weapons;
	DWORD Font_CSGO;

	/* Hooking */
	void Hook()
	{
		Font_MenuTitle = Interfaces.pSurface->Create_Font();
		Font_Watermark = Interfaces.pSurface->Create_Font();
		Font_Tab = Interfaces.pSurface->Create_Font();
		Font_ESP = Interfaces.pSurface->Create_Font();
		Font_Logo = Interfaces.pSurface->Create_Font();
		Font_Controls = Interfaces.pSurface->Create_Font();
		Font_Icons = Interfaces.pSurface->Create_Font();
		Font_Weapons = Interfaces.pSurface->Create_Font();
		Font_HUD = Interfaces.pSurface->Create_Font();
		Font_HUD_Big = Interfaces.pSurface->Create_Font();

		Interfaces.pSurface->SetFontGlyphSet( Font_Controls, "Tahoma", 13, 400, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
		Interfaces.pSurface->SetFontGlyphSet( Font_Tab, "Tahoma", 22, 700, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE );
		Interfaces.pSurface->SetFontGlyphSet( Font_MenuTitle, "Tahoma", 12, 600, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE );
		Interfaces.pSurface->SetFontGlyphSet( Font_Watermark, "Tahoma", 12, 600, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
		Interfaces.pSurface->SetFontGlyphSet( Font_Logo, "Tahoma", 48, 600, 0, 0, FONTFLAG_ANTIALIAS );
		Interfaces.pSurface->SetFontGlyphSet( Font_ESP, "Tahoma", 11, 600, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
		Interfaces.pSurface->SetFontGlyphSet( Font_HUD, "Tahoma", 16, 600, 0, 0, FONTFLAG_ANTIALIAS );
		Interfaces.pSurface->SetFontGlyphSet( Font_HUD_Big, "Tahoma", 36, 700, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE );

		HRSRC hResInst = FindResource( nullptr, MAKEINTRESOURCE( IDR_FONT1 ), RT_FONT );
		HGLOBAL hGlobal = LoadResource( nullptr, hResInst );
		size_t __LEN = SizeofResource( nullptr, hResInst );
		DWORD dwFonts;
		Interfaces.__FNTHANDLE = AddFontMemResourceEx( nullptr, __LEN, nullptr, &dwFonts );
		auto __FONT = CreateFont( 16, 0, 0, 0, 500, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "undefeated" );

		Interfaces.pSurface->SetFontGlyphSet( Font_Icons, "undefeated", 24, 500, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE );
		Interfaces.pSurface->SetFontGlyphSet( Font_Weapons, "undefeated", 12, 500, 0, 0, FONTFLAG_ANTIALIAS );

		// Hooks
		if( Interfaces.pClient )
		{
			VMTClient.bInitialize( reinterpret_cast< DWORD** >( Interfaces.pClient ) );
			oCreateMove = VMTClient.dwHookMethod( reinterpret_cast< DWORD >( &Hooked_Createmove ), 21 );
			oFrameStageNotify = reinterpret_cast< iFrameStageNotifyFn >( VMTClient.dwHookMethod( reinterpret_cast< DWORD >( &Hooked_Frame_Stage_Notify ), 36 ) );
		}

		if( Interfaces.pEngine )
		{
			VMTEngine.bInitialize( reinterpret_cast< DWORD** >( Interfaces.pEngine ) );
			oClientCmdUnresticted = reinterpret_cast< ClientCmdUnresticted >( VMTEngine.dwHookMethod( reinterpret_cast< DWORD >( &Hooked_ClientCmdUnrestricted ), 114 ) );
		}

		if( Interfaces.pSurface )
		{
			VMTPanel.bInitialize( reinterpret_cast< unsigned long** >( Interfaces.pPanel ) );
			oPaintTraverse = reinterpret_cast< iPaintTraverse >( VMTPanel.dwHookMethod( reinterpret_cast< DWORD >( &Hooked_PaintTraverse ), 41 ) );
		}

		if( Interfaces.g_pModelRender )
		{
			VMTModelRender.bInitialize( reinterpret_cast< DWORD** >( Interfaces.g_pModelRender ) );
			oDrawModelExecute = reinterpret_cast< IDrawModelExecute >( VMTModelRender.dwHookMethod( reinterpret_cast< DWORD >( &Hooked_DrawModelExecute ), 21 ) );
		}

		if( Interfaces.pClientMode )
		{
			VMTClientMode.bInitialize( reinterpret_cast< DWORD** >( Interfaces.pClientMode ) );
			oOverrideView = reinterpret_cast< iOverRideViewFn >( VMTClientMode.dwHookMethod( reinterpret_cast< DWORD >( &Hooked_OverrideView ), 18 ) );
		}

		if( Interfaces.pSurface )
		{
			VMTSurface.bInitialize( ( DWORD** )Interfaces.pSurface );
			oPlaySound = ( PlaySoundFn )VMTSurface.dwHookMethod( ( DWORD )&Hooked_PlaySound, 82 );
		}

		while( !( INIT::Window = FindWindowA( "Valve001", nullptr ) ) )
			Sleep( 100 );
		if( INIT::Window )
			INIT::OldWindow = ( WNDPROC )SetWindowLongPtr( INIT::Window, GWL_WNDPROC, ( LONG_PTR )Hooked_WndProc );

		Init();
	}

	void UnHook()
	{
		VMTClient.UnHook();
		Sleep( 150 );
		VMTClientMode.UnHook();
		Sleep( 150 );
		VMTEngine.UnHook();
		Sleep( 150 );
		VMTModelRender.UnHook();
		Sleep( 150 );
		VMTPanel.UnHook();
		Sleep( 150 );
		VMTGameEvent.UnHook();
		Sleep( 150 );
		VMTSurface.UnHook();
		Sleep( 150 );
		for (auto listener : listeners)
			Interfaces.g_GameEventMgr->RemoveListener(listener);
	}
};
