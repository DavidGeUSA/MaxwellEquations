#ifndef __MEMMAN_H__
#define __MEMMAN_H__
/*******************************************************************
	Author: David Ge (dge893@gmail.com, aka Wei Ge)
	Last modified: 03/31/2018
	Allrights reserved by David Ge

	Modifications
	Date            Author      Description
	---------------------------------------------
	2021-03-06      David Ge    Linux compatibility

********************************************************************/

#include "ErrorMemoryMan.h"

#ifdef __linux__ 
#define LODWORD(l)           ((unsigned long)(((size_t)(l)) & 0xffffffff))
#define HIDWORD(l)           ((unsigned long)((((size_t)(l)) >> 32) & 0xffffffff))
#elif _WIN32
#define LODWORD(l)           ((DWORD)(((size_t)(l)) & 0xffffffff))
#define HIDWORD(l)           ((DWORD)((((size_t)(l)) >> 32) & 0xffffffff))
#else

#endif

extern unsigned long osErrorCode;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

void RememberOSerror();
void ClearOSerror();
unsigned long GetLastWinError();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
