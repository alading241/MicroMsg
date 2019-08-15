#include <windows.h>
#include <ntsecapi.h>
#include<string>

#include "Dispatch_SELF_Login.h"
#include "../../Common/DebugLog.h"
#include"../HookPort.h"
#include "../Inject/RemoteInjection.h"
#include"../Common.h"
#include"../pipe.h"



//
//Global
//
__pfnWxLogin pfnWxLoginForHeadUrl = NULL;
__pfnWxLogin pfnWxLoginForWxid = NULL;



//
//Dispatch_SELF_Login Functions
//
VOID
WINAPI
OnWxLoginForHeadUrl()
{
#ifdef Dbg
	DebugLog(DbgInfo, "OnWxLoginForHeadUrl...");
#endif

	DWORD* saveEBP = 0;
	__asm
	{
		mov saveEBP,ebp
	}
	saveEBP = (DWORD*)*saveEBP;

	WCHAR *wxBuf = (WCHAR*)*(DWORD*)((char*)saveEBP - 0x60);
	DWORD len = GetWBufferLen(wxBuf);
	if (len> 0)
	{
		std::string r=MyWideCharToMultiByte((BYTE*)wxBuf, len);
#ifdef Dbg
		DebugLog(DbgInfo, (char*)r.c_str());
#endif
		pipe_start_thread((LPVOID)r.c_str());
	}
	__asm
	{
		mov esp,ebp
		pop ebp
		jmp pfnWxLoginForHeadUrl;
	}
}

VOID
WINAPI
OnWxLoginForWxid()
{
#ifdef Dbg
	DebugLog(DbgInfo, "OnWxLoginForWxid...");
#endif

	DWORD* saveEBP = 0;
	__asm
	{
		mov saveEBP, ebp
	}
	saveEBP = (DWORD*)*saveEBP;

	//ȡwxid
	CHAR *wxBuf = (CHAR*)*(DWORD*)((char*)saveEBP + 0x8);
	DWORD len = strlen(wxBuf);
	if (len> 0)
	{
#ifdef Dbg
		DebugLog(DbgInfo, wxBuf);
#endif
		pipe_start_thread((LPVOID)wxBuf);
	}
	//ȡ΢�ź�
	wxBuf = (CHAR*)*(DWORD*)((char*)saveEBP + 0x70);
	len = strlen(wxBuf);
	if (len> 0)
	{
#ifdef Dbg
		DebugLog(DbgInfo, wxBuf);
#endif
		pipe_start_thread((LPVOID)wxBuf);
	}

	__asm
	{
		mov esp, ebp
		pop ebp
		jmp pfnWxLoginForWxid;
	}
}