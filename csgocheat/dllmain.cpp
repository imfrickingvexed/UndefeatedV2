// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		CreateThread( nullptr, 0, static_cast< LPTHREAD_START_ROUTINE >( CheatMain ), nullptr, 0, nullptr );

		INIT::Dll = hModule;

		return TRUE;
	}
	return FALSE;
}
