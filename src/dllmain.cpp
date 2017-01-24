#include "stdafx.h"
#include <stdio.h>

void SoulReaverHook();


FARPROC p[7] = { 0 };

BOOL hooked = FALSE;

void LoadFunctions() {

	char * sz_buffer = new char[255];
	char * sz_systemDirectory = new char[255];
	memset(sz_systemDirectory, 0, 255);

	GetSystemDirectory(sz_systemDirectory, 255);

	sprintf(sz_buffer, "%s\\dinput.dll", sz_systemDirectory);

	HMODULE hL = LoadLibrary(sz_buffer);
	if (hL == 0)
	{
		return;
	}

	p[0] = GetProcAddress(hL, "DirectInputCreateA");
	p[1] = GetProcAddress(hL, "DirectInputCreateEx");
	p[2] = GetProcAddress(hL, "DirectInputCreateW");
	p[3] = GetProcAddress(hL, "DllCanUnloadNow");
	p[4] = GetProcAddress(hL, "DllGetClassObject");
	p[5] = GetProcAddress(hL, "DllRegisterServer");
	p[6] = GetProcAddress(hL, "DllUnregisterServer");

}


//DirectInputCreateA
extern "C" __declspec(naked) void __stdcall __E__0__()
{
	if (!hooked)
	{
		SoulReaverHook();
		hooked = true;
	}
	__asm
	{
		jmp p[0];
	}
}
// DirectInputCreateEx
extern "C" __declspec(naked) void __stdcall __E__1__()
{
	__asm
	{
		jmp p[1];
	}
}
// DirectInputCreateW
extern "C" __declspec(naked) void __stdcall __E__2__()
{
	__asm
	{
		jmp p[2];
	}
}
// DllCanUnloadNow
extern "C" __declspec(naked) void __stdcall __E__3__()
{
	__asm
	{
		jmp p[3];
	}
}
// DllGetClassObject
extern "C" __declspec(naked) void __stdcall __E__4__()
{
	__asm
	{
		jmp p[4];
	}
}
//DllRegisterServer
extern "C" __declspec(naked) void __stdcall __E__5__()
{
	__asm
	{
		jmp p[5];
	}
}
//DllUnregisterServer
extern "C" __declspec(naked) void __stdcall __E__6__()
{
	__asm
	{
		jmp p[6];
	}
}


typedef struct DIJOYSTATE {
	LONG    lX;                     /* x-axis position              */
	LONG    lY;                     /* y-axis position              */
	LONG    lZ;                     /* z-axis position              */
	LONG    lRx;                    /* x-axis rotation              */
	LONG    lRy;                    /* y-axis rotation              */
	LONG    lRz;                    /* z-axis rotation              */
	LONG    rglSlider[2];           /* extra axes positions         */
	DWORD   rgdwPOV[4];             /* POV directions               */
	BYTE    rgbButtons[32];         /* 32 buttons                   */
} DIJOYSTATE, *LPDIJOYSTATE;

void SoulReaverGamePadFix()
{

	int result;

	DIJOYSTATE *dijoystate;

	__asm
	{
		push edx;
		push 0x50;
		mov dijoystate, EDX;
		MOV ECX, DWORD PTR DS : [EAX];
		PUSH EAX;
		CALL DWORD PTR DS : [ECX + 0x24];
		mov result, eax
	}

	if (dijoystate->lX > -500 && dijoystate->lX < 500)
	{
		dijoystate->lX = 0;
	}

	if (dijoystate->lY > -500 && dijoystate->lY < 500)
	{
		dijoystate->lY = 0;
	}

	__asm mov eax, result;
}

void SoulReaverHook()
{
	DWORD dwOldProtect, dwNewProtect, dwOldCall, dwNewCall, dwNewAddress, dwAddress;

	dwAddress = 0x00478201;
	dwNewAddress = (DWORD)&SoulReaverGamePadFix;

	BYTE opCALL[9] = { 0xE8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90 };
	dwNewCall = dwNewAddress - dwAddress - 5;
	memcpy(&opCALL[1], &dwNewCall, 4);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)dwAddress, 9, PAGE_EXECUTE_WRITECOPY, &dwOldProtect);
	memcpy((LPVOID)dwAddress, &opCALL, 9);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)dwAddress, 9, dwOldProtect, &dwNewProtect);

}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		LoadFunctions();
		break;

	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}