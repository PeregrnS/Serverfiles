#pragma once

#include "Locale_inc.h"

bool		LocaleService_IsYMIR();
bool		LocaleService_IsJAPAN();
bool		LocaleService_IsENGLISH();
bool		LocaleService_IsHONGKONG();
bool		LocaleService_IsTAIWAN();
bool		LocaleService_IsNEWCIBN();
bool		LocaleService_IsEUROPE();
bool		LocaleService_IsWorldEdition();

unsigned	LocaleService_GetCodePage();
const char* LocaleService_GetName();
const char*	LocaleService_GetLocaleName();
const char*	LocaleService_GetLocalePath();
#if defined(__BL_MULTI_LANGUAGE__)
void		LocaleService_LoadLocaConfig();
#endif
const char*	LocaleService_GetSecurityKey();
BOOL		LocaleService_IsLeadByte( const char chByte );
int			LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength );

void		LocaleService_ForceSetLocale(const char* name, const char* localePath);
void		LocaleService_LoadConfig(const char* fileName);
bool		LocaleService_LoadGlobal(HINSTANCE hInstance);
unsigned	LocaleService_GetLastExp(int level);
int			LocaleService_GetSkillPower(unsigned level);

// CHEONMA
void		LocaleService_SetCHEONMA(bool isEnable);
bool		LocaleService_IsCHEONMA();
// END_OF_CHEONMA
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
