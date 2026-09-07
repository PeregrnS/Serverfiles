#include "StdAfx.h"
#include "PythonSystem.h"
#include "PythonApplication.h"

// #define DEFAULT_VALUE_ALWAYS_SHOW_NAME		true
#define DEFAULT_VALUE_ALWAYS_SHOW_NAME        1

bool g_bShowRainBowConfig = true;

void CPythonSystem::SetInterfaceHandler(PyObject * poHandler)
{
//	if (m_poInterfaceHandler)
//		Py_DECREF(m_poInterfaceHandler);

	m_poInterfaceHandler = poHandler;

//	if (m_poInterfaceHandler)
//		Py_INCREF(m_poInterfaceHandler);
}

void CPythonSystem::DestroyInterfaceHandler()
{
	m_poInterfaceHandler = NULL;
}

void CPythonSystem::SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight)
{
	m_WindowStatus[iIndex].isVisible	= iVisible;
	m_WindowStatus[iIndex].isMinimized	= iMinimized;
	m_WindowStatus[iIndex].ixPosition	= ix;
	m_WindowStatus[iIndex].iyPosition	= iy;
	m_WindowStatus[iIndex].iHeight		= iHeight;
}

void CPythonSystem::GetDisplaySettings()
{
	memset(m_ResolutionList, 0, sizeof(TResolution) * RESOLUTION_MAX_NUM);
	m_ResolutionCount = 0;

	LPDIRECT3D9EX lpD3D = CPythonGraphic::Instance().GetD3D();

	D3DADAPTER_IDENTIFIER9 d3dAdapterIdentifier;
	D3DDISPLAYMODEEX d3ddmDesktop;
	ZeroMemory(&d3ddmDesktop, sizeof(d3ddmDesktop));
	d3ddmDesktop.Size = sizeof(D3DDISPLAYMODEEX);

	lpD3D->GetAdapterIdentifier(0, 0, &d3dAdapterIdentifier);
	D3DDISPLAYROTATION desktopRotation = D3DDISPLAYROTATION_IDENTITY;
	lpD3D->GetAdapterDisplayModeEx(0, &d3ddmDesktop, &desktopRotation);

	D3DDISPLAYMODEFILTER modeFilter;
	ZeroMemory(&modeFilter, sizeof(modeFilter));
	modeFilter.Size = sizeof(modeFilter);
	modeFilter.Format = d3ddmDesktop.Format;
	modeFilter.ScanLineOrdering = D3DSCANLINEORDERING_UNKNOWN;

	DWORD dwNumAdapterModes = lpD3D->GetAdapterModeCountEx(0, &modeFilter);

	for (UINT iMode = 0; iMode < dwNumAdapterModes; iMode++)
	{
		D3DDISPLAYMODEEX DisplayMode;
		ZeroMemory(&DisplayMode, sizeof(DisplayMode));
		DisplayMode.Size = sizeof(D3DDISPLAYMODEEX);
		if (FAILED(lpD3D->EnumAdapterModesEx(0, &modeFilter, iMode, &DisplayMode)))
			continue;
		DWORD bpp = 0;

		if (DisplayMode.Width < 800 || DisplayMode.Height < 600)
			continue;

		if (DisplayMode.Format == D3DFMT_R5G6B5)
			bpp = 16;
		else if (DisplayMode.Format == D3DFMT_X8R8G8B8)
			bpp = 32;
		else
			continue;

		int check_res = false;

		for (int i = 0; !check_res && i < m_ResolutionCount; ++i)
		{
			if (m_ResolutionList[i].bpp != bpp ||
				m_ResolutionList[i].width != DisplayMode.Width ||
				m_ResolutionList[i].height != DisplayMode.Height)
				continue;

			int check_fre = false;

			for (int j = 0; j < m_ResolutionList[i].frequency_count; ++j)
			{
				if (m_ResolutionList[i].frequency[j] == DisplayMode.RefreshRate)
				{
					check_fre = true;
					break;
				}
			}

			if (!check_fre)
				if (m_ResolutionList[i].frequency_count < FREQUENCY_MAX_NUM)
					m_ResolutionList[i].frequency[m_ResolutionList[i].frequency_count++] = DisplayMode.RefreshRate;

			check_res = true;
		}

		if (!check_res)
		{
			if (m_ResolutionCount < RESOLUTION_MAX_NUM)
			{
				m_ResolutionList[m_ResolutionCount].width			= DisplayMode.Width;
				m_ResolutionList[m_ResolutionCount].height			= DisplayMode.Height;
				m_ResolutionList[m_ResolutionCount].bpp				= bpp;
				m_ResolutionList[m_ResolutionCount].frequency[0]	= DisplayMode.RefreshRate;
				m_ResolutionList[m_ResolutionCount].frequency_count	= 1;

				++m_ResolutionCount;
			}
		}
	}
}

int	CPythonSystem::GetResolutionCount()
{
	return m_ResolutionCount;
}

int CPythonSystem::GetFrequencyCount(int index)
{
	if (index >= m_ResolutionCount)
		return 0;

    return m_ResolutionList[index].frequency_count;
}

bool CPythonSystem::GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp)
{
	if (index >= m_ResolutionCount)
		return false;

	*width = m_ResolutionList[index].width;
	*height = m_ResolutionList[index].height;
	*bpp = m_ResolutionList[index].bpp;
	return true;
}

bool CPythonSystem::GetFrequency(int index, int freq_index, OUT DWORD *frequncy)
{
	if (index >= m_ResolutionCount)
		return false;

	if (freq_index >= m_ResolutionList[index].frequency_count)
		return false;

	*frequncy = m_ResolutionList[index].frequency[freq_index];
	return true;
}

int	CPythonSystem::GetResolutionIndex(DWORD width, DWORD height, DWORD bit)
{
	DWORD re_width, re_height, re_bit;
	int i = 0;

	while (GetResolution(i, &re_width, &re_height, &re_bit))
	{
		if (re_width == width)
			if (re_height == height)
				if (re_bit == bit)
					return i;
		i++;
	}

	return 0;
}

int	CPythonSystem::GetFrequencyIndex(int res_index, DWORD frequency)
{
	DWORD re_frequency;
	int i = 0;

	while (GetFrequency(res_index, i, &re_frequency))
	{
		if (re_frequency == frequency)
			return i;

		i++;
	}

	return 0;
}

DWORD CPythonSystem::GetWidth()
{
	return m_Config.width;
}

DWORD CPythonSystem::GetHeight()
{
	return m_Config.height;
}
DWORD CPythonSystem::GetBPP()
{
	return m_Config.bpp;
}
DWORD CPythonSystem::GetFrequency()
{
	return m_Config.frequency;
}

bool CPythonSystem::IsNoSoundCard()
{
	return m_Config.bNoSoundCard;
}

bool CPythonSystem::IsSoftwareCursor()
{
	return m_Config.is_software_cursor;
}

float CPythonSystem::GetMusicVolume()
{
	return m_Config.music_volume;
}

int CPythonSystem::GetSoundVolume()
{
	return m_Config.voice_volume;
}

void CPythonSystem::SetMusicVolume(float fVolume)
{
	m_Config.music_volume = fVolume;
}

void CPythonSystem::SetSoundVolumef(float fVolume)
{
	m_Config.voice_volume = int(5 * fVolume);
}

#ifdef ENABLE_FOV_OPTION
void CPythonSystem::SetFOVLevel(float fFOV)
{
	m_Config.iFOVLevel = fMINMAX(30.0f, fFOV, 120.0f);
}

float CPythonSystem::GetFOVLevel()
{
	return m_Config.iFOVLevel;
}
#endif

int CPythonSystem::GetDistance()
{
	return m_Config.iDistance;
}

int CPythonSystem::GetShadowLevel()
{
	return m_Config.iShadowLevel;
}

void CPythonSystem::SetShadowLevel(unsigned int level)
{
	m_Config.iShadowLevel = MIN(level, 5);
	CPythonBackground::instance().RefreshShadowLevel();
}

int CPythonSystem::IsSaveID()
{
	return m_Config.isSaveID;
}

const char * CPythonSystem::GetSaveID()
{
	return m_Config.SaveID;
}

bool CPythonSystem::isViewCulling()
{
	return m_Config.is_object_culling;
}

void CPythonSystem::SetSaveID(int iValue, const char * c_szSaveID)
{
	if (iValue != 1)
		return;

	m_Config.isSaveID = iValue;
	strncpy(m_Config.SaveID, c_szSaveID, sizeof(m_Config.SaveID) - 1);
}

CPythonSystem::TConfig * CPythonSystem::GetConfig()
{
	return &m_Config;
}

void CPythonSystem::SetConfig(TConfig * pNewConfig)
{
	m_Config = *pNewConfig;
}

void CPythonSystem::SetDefaultConfig()
{
	memset(&m_Config, 0, sizeof(m_Config));

	m_Config.width				= 1024;
	m_Config.height				= 768;
	m_Config.bpp				= 32;

#if defined( LOCALE_SERVICE_WE_JAPAN )
	m_Config.bWindowed			= true;
#else
	m_Config.bWindowed			= false;
#endif

	m_Config.is_software_cursor	= false;
	m_Config.is_object_culling	= true;
	m_Config.iDistance			= 3;

	m_Config.gamma				= 3;
	m_Config.music_volume		= 1.0f;
	m_Config.voice_volume		= 5;

	m_Config.bDecompressDDS		= 0;
	m_Config.bSoftwareTiling	= 0;
	m_Config.iShadowLevel		= 3;
#ifdef ENABLE_FOV_OPTION
	m_Config.iFOVLevel			= 30.0f;
#endif
	m_Config.bViewChat			= true;
	m_Config.bAlwaysShowName	= DEFAULT_VALUE_ALWAYS_SHOW_NAME;
	m_Config.bShowDamage		= true;
	m_Config.bShowRainBow = true;
	m_Config.bShowSalesText		= true;
	m_Config.bShowTitleText = true;
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
	m_Config.bShowMobAIFlag		= true;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
	m_Config.bShowMobLevel		= true;
#endif
#ifdef ENABLE_RENDER_TARGET_PREVIEW
	m_Config.bPreviewModel 		= true;
	m_Config.bIsRenderRotation 	= true;
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	m_Config.fPrivateShopViewDistance = 1.0f;
#endif
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
	m_Config.bAnonymousCountryMode = false;
	m_Config.bShowCountryFlag = true;
	m_Config.bShowEmpireFlag = true;
#endif
#ifdef ENABLE_NEW_GAMEOPTION
	m_Config.questLetter = true;
	m_Config.affectIcons = true;
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
	m_Config.m_fStoneScale = 0.0f;
#endif
#ifdef ENABLE_MAP_OBJECT_OPTIMIZATION
	m_Config.fobjectDist = 25600.0;
#endif
}

bool CPythonSystem::IsWindowed()
{
	return m_Config.bWindowed;
}

bool CPythonSystem::IsViewChat()
{
	return m_Config.bViewChat;
}

void CPythonSystem::SetViewChatFlag(int iFlag)
{
	m_Config.bViewChat = iFlag == 1 ? true : false;
}

int CPythonSystem::IsAlwaysShowName()
{
	return m_Config.bAlwaysShowName;
}
void CPythonSystem::SetAlwaysShowNameFlag(int iFlag)
{
	m_Config.bAlwaysShowName = iFlag;
}

// bool CPythonSystem::IsAlwaysShowName()
// {
// 	return m_Config.bAlwaysShowName;
// }

// void CPythonSystem::SetAlwaysShowNameFlag(int iFlag)
// {
// 	m_Config.bAlwaysShowName = iFlag == 1 ? true : false;
// }

bool CPythonSystem::IsShowDamage()
{
	return m_Config.bShowDamage;
}

void CPythonSystem::SetShowDamageFlag(int iFlag)
{
	m_Config.bShowDamage = iFlag == 1 ? true : false;
}

//
bool CPythonSystem::IsShowRainBow()
{
	return m_Config.bShowRainBow;
}

void CPythonSystem::SetShowRainBowFlag(int iFlag)
{
	m_Config.bShowRainBow = iFlag == 1 ? true : false;

	g_bShowRainBowConfig = m_Config.bShowRainBow;
}


bool CPythonSystem::IsShowTitleText()
{
	return m_Config.bShowTitleText;
}

void CPythonSystem::SetShowTitleTextFlag(int iFlag)
{
	m_Config.bShowTitleText = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsShowSalesText()
{
	return m_Config.bShowSalesText;
}

void CPythonSystem::SetShowSalesTextFlag(int iFlag)
{
	m_Config.bShowSalesText = iFlag == 1 ? true : false;
}

#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
bool CPythonSystem::IsShowMobAIFlag()
{
	return m_Config.bShowMobAIFlag;
}

void CPythonSystem::SetShowMobAIFlagFlag(int iFlag)
{
	m_Config.bShowMobAIFlag = iFlag == 1 ? true : false;
}
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
bool CPythonSystem::IsShowMobLevel()
{
	return m_Config.bShowMobLevel;
}

void CPythonSystem::SetShowMobLevelFlag(int iFlag)
{
	m_Config.bShowMobLevel = iFlag == 1 ? true : false;
}
#endif

#ifdef ENABLE_RENDER_TARGET_PREVIEW
bool CPythonSystem::IsPreviewModel()
{
	return m_Config.bPreviewModel;
}

void CPythonSystem::SetPreviewModel(int iFlag)
{
	m_Config.bPreviewModel = iFlag == 1 ? true : false;
}

bool CPythonSystem::IsRenderRotation()
{
	return m_Config.bIsRenderRotation;
}

void CPythonSystem::SetRenderRotation(int iFlag)
{
	m_Config.bIsRenderRotation = iFlag == 1 ? true : false;
}
#endif

bool CPythonSystem::IsAutoTiling()
{
	if (m_Config.bSoftwareTiling == 0)
		return true;

	return false;
}

void CPythonSystem::SetSoftwareTiling(bool isEnable)
{
	if (isEnable)
		m_Config.bSoftwareTiling=1;
	else
		m_Config.bSoftwareTiling=2;
}

bool CPythonSystem::IsSoftwareTiling()
{
	if (m_Config.bSoftwareTiling==1)
		return true;

	return false;
}

bool CPythonSystem::IsUseDefaultIME()
{
	return m_Config.bUseDefaultIME;
}

bool CPythonSystem::LoadConfig()
{
	FILE * fp = NULL;

	if (NULL == (fp = fopen("metin2.cfg", "rt")))
		return false;

	char buf[256];
	char command[256];
	char value[256];

	while (fgets(buf, 256, fp))
	{
		if (sscanf(buf, " %s %s\n", command, value) == EOF)
			break;

		if (!stricmp(command, "WIDTH"))
			m_Config.width = atoi(value);
		else if (!stricmp(command, "HEIGHT"))
			m_Config.height = atoi(value);
		else if (!stricmp(command, "BPP"))
			m_Config.bpp = atoi(value);
		else if (!stricmp(command, "FREQUENCY"))
			m_Config.frequency = atoi(value);
		else if (!stricmp(command, "SOFTWARE_CURSOR"))
			m_Config.is_software_cursor = atoi(value) ? true : false;
		else if (!stricmp(command, "OBJECT_CULLING"))
			m_Config.is_object_culling = atoi(value) ? true : false;
		else if (!stricmp(command, "VISIBILITY"))
			m_Config.iDistance = atoi(value);
		else if (!stricmp(command, "MUSIC_VOLUME")) {
			if(strchr(value, '.') == 0) { // Old compatiability
				m_Config.music_volume = pow(10.0f, (-1.0f + (((float) atoi(value)) / 5.0f)));
				if(atoi(value) == 0)
					m_Config.music_volume = 0.0f;
			} else
				m_Config.music_volume = atof(value);
		} else if (!stricmp(command, "VOICE_VOLUME"))
			m_Config.voice_volume = (char) atoi(value);
		else if (!stricmp(command, "GAMMA"))
			m_Config.gamma = atoi(value);
		else if (!stricmp(command, "IS_SAVE_ID"))
			m_Config.isSaveID = atoi(value);
		else if (!stricmp(command, "SAVE_ID"))
			strncpy(m_Config.SaveID, value, 20);
		else if (!stricmp(command, "WINDOWED"))
		{
			m_Config.bWindowed = atoi(value) == 1 ? true : false;
		}
		else if (!stricmp(command, "USE_DEFAULT_IME"))
			m_Config.bUseDefaultIME = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SOFTWARE_TILING"))
			m_Config.bSoftwareTiling = atoi(value);
		else if (!stricmp(command, "SHADOW_LEVEL"))
			m_Config.iShadowLevel = atoi(value);
		else if (!stricmp(command, "DECOMPRESSED_TEXTURE"))
			m_Config.bDecompressDDS = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "NO_SOUND_CARD"))
			m_Config.bNoSoundCard = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "VIEW_CHAT"))
			m_Config.bViewChat = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "ALWAYS_VIEW_NAME"))
		{
			// m_Config.bAlwaysShowName = atoi(value) == 1 ? true : false;
			m_Config.bAlwaysShowName = atoi(value);
		}
		else if (!stricmp(command, "SHOW_DAMAGE"))
			m_Config.bShowDamage = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_RAINBOW"))
			m_Config.bShowRainBow = atoi(value) == 1 ? true : false;
#ifdef ENABLE_FOV_OPTION
		else if (!stricmp(command, "FIELD_OF_VIEW"))
			m_Config.iFOVLevel = atoi(value);
#endif
		else if (!stricmp(command, "SHOW_SALESTEXT"))
			m_Config.bShowSalesText = atoi(value) == 1 ? true : false;

		else if (!stricmp(command, "SHOW_TITLE"))
			m_Config.bShowTitleText = atoi(value) == 1 ? true : false;

#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
		else if (!stricmp(command, "SHOW_MOBAIFLAG"))
			m_Config.bShowMobAIFlag = atoi(value) == 1 ? true : false;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
		else if (!stricmp(command, "SHOW_MOBLEVEL"))
			m_Config.bShowMobLevel = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_RENDER_TARGET_PREVIEW
		else if (!stricmp(command, "PREVIEW_MODEL"))
		{
			m_Config.bPreviewModel = atoi(value) == 1 ? true : false;
		}
		else if (!stricmp(command, "RENDER_ROTATION"))
		{
			m_Config.bIsRenderRotation = atoi(value) == 1 ? true : false;
		}
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		else if (!stricmp(command, "PRIVATE_SHOP_VIEW_DISTANCE"))
			m_Config.fPrivateShopViewDistance = atof(value);
#endif
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
		else if (!stricmp(command, "ANONYMOUS_MODE"))
			m_Config.bAnonymousCountryMode = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_COUNTRY_FLAG"))
			m_Config.bShowCountryFlag = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "SHOW_EMPIRE_FLAG"))
			m_Config.bShowEmpireFlag = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_NEW_GAMEOPTION
		else if (!stricmp(command, "QUEST_LETTER"))
			m_Config.questLetter = atoi(value) == 1 ? true : false;
		else if (!stricmp(command, "AFFECT_ICONS"))
			m_Config.affectIcons = atoi(value) == 1 ? true : false;
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
		else if (!stricmp(command, "STONE_SCALE"))
			m_Config.m_fStoneScale = atof(value);
#endif
#ifdef ENABLE_MAP_OBJECT_OPTIMIZATION
		else if (!stricmp(command, "OBJECT_DIST"))
			m_Config.fobjectDist = atof(value) > 25600.0 ? 25600.0 : atof(value);
#endif
#if defined(__BL_GRAPHIC_ON_OFF__)
		else if (!stricmp(command, "EFFECT_LEVEL"))
			CPythonGraphicOnOff::Instance().SetEffectOnOffLevel(atoi(value));
		else if (!stricmp(command, "PRIVATE_SHOP_LEVEL"))
			CPythonGraphicOnOff::Instance().SetPrivateShopOnOffLevel(atoi(value));
		else if (!stricmp(command, "DROP_ITEM_LEVEL"))
			CPythonGraphicOnOff::Instance().SetDropItemOnOffLevel(atoi(value));
		else if (!stricmp(command, "PET_STATUS"))
			CPythonGraphicOnOff::Instance().SetPetOnOffStatus(atoi(value));
		else if (!stricmp(command, "NPC_NAME_STATUS"))
			CPythonGraphicOnOff::Instance().SetNPCNameOnOffStatus(atoi(value));
#endif
	}

	if (m_Config.bWindowed)
	{
		unsigned screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
		unsigned screen_height = GetSystemMetrics(SM_CYFULLSCREEN);

		if (m_Config.width >= screen_width)
		{
			m_Config.width = screen_width;
		}
		if (m_Config.height >= screen_height)
		{
			m_Config.height = screen_height;
		}
	}

	m_OldConfig = m_Config;

	fclose(fp);

//	Tracef("LoadConfig: Resolution: %dx%d %dBPP %dHZ Software Cursor: %d, Music/Voice Volume: %d/%d Gamma: %d\n",
//		m_Config.width,
//		m_Config.height,
//		m_Config.bpp,
//		m_Config.frequency,
//		m_Config.is_software_cursor,
//		m_Config.music_volume,
//		m_Config.voice_volume,
//		m_Config.gamma);

	g_bShowRainBowConfig = m_Config.bShowRainBow;

	return true;
}

bool CPythonSystem::SaveConfig()
{
	FILE *fp;

	if (NULL == (fp = fopen("metin2.cfg", "wt")))
		return false;

	fprintf(fp, "WIDTH						%d\n"
				"HEIGHT						%d\n"
				"BPP						%d\n"
				"FREQUENCY					%d\n"
				"SOFTWARE_CURSOR			%d\n"
				"OBJECT_CULLING				%d\n"
				"VISIBILITY					%d\n"
				"MUSIC_VOLUME				%.3f\n"
				"VOICE_VOLUME				%d\n"
				"GAMMA						%d\n"
				"IS_SAVE_ID					%d\n"
				"SAVE_ID					%s\n"
				"DECOMPRESSED_TEXTURE		%d\n",
				m_Config.width,
				m_Config.height,
				m_Config.bpp,
				m_Config.frequency,
				m_Config.is_software_cursor,
				m_Config.is_object_culling,
				m_Config.iDistance,
				m_Config.music_volume,
				m_Config.voice_volume,
				m_Config.gamma,
				m_Config.isSaveID,
				m_Config.SaveID,
				m_Config.bDecompressDDS);

	if (m_Config.bWindowed == 1)
		fprintf(fp, "WINDOWED				%d\n", m_Config.bWindowed);
	if (m_Config.bViewChat == 0)
		fprintf(fp, "VIEW_CHAT				%d\n", m_Config.bViewChat);
	if (m_Config.bAlwaysShowName != DEFAULT_VALUE_ALWAYS_SHOW_NAME)
		fprintf(fp, "ALWAYS_VIEW_NAME		%d\n", m_Config.bAlwaysShowName);
	if (m_Config.bShowDamage == 0)
		fprintf(fp, "SHOW_DAMAGE		%d\n", m_Config.bShowDamage);

	if (m_Config.bShowRainBow == 0)
		fprintf(fp, "SHOW_RAINBOW		%d\n", m_Config.bShowRainBow);

	if (m_Config.bShowTitleText == 0)
		fprintf(fp, "SHOW_TITLE		%d\n", m_Config.bShowTitleText);

	if (m_Config.bShowSalesText == 0)
		fprintf(fp, "SHOW_SALESTEXT		%d\n", m_Config.bShowSalesText);
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
	if (m_Config.bShowMobAIFlag == 0)
		fprintf(fp, "SHOW_MOBAIFLAG		%d\n", m_Config.bShowMobAIFlag);
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
	if (m_Config.bShowMobLevel == 0)
		fprintf(fp, "SHOW_MOBLEVEL		%d\n", m_Config.bShowMobLevel);
#endif
#ifdef ENABLE_RENDER_TARGET_PREVIEW
	if (m_Config.bPreviewModel == 0)
	{
		fprintf(fp, "PREVIEW_MODEL			%d\n", m_Config.bPreviewModel);
	}
	if (m_Config.bIsRenderRotation == 0)
	{
		fprintf(fp, "RENDER_ROTATION		%d\n", m_Config.bIsRenderRotation);
	}
#endif
	fprintf(fp, "USE_DEFAULT_IME		%d\n", m_Config.bUseDefaultIME);
	fprintf(fp, "SOFTWARE_TILING		%d\n", m_Config.bSoftwareTiling);
	fprintf(fp, "SHADOW_LEVEL			%d\n", m_Config.iShadowLevel);
#ifdef ENABLE_FOV_OPTION
	fprintf(fp, "FIELD_OF_VIEW            %.1f\n", m_Config.iFOVLevel);
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	fprintf(fp, "PRIVATE_SHOP_VIEW_DISTANCE			%.3f\n", m_Config.fPrivateShopViewDistance);
#endif
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
	fprintf(fp, "ANONYMOUS_MODE\t\t\t%d\n", m_Config.bAnonymousCountryMode);
	fprintf(fp, "SHOW_COUNTRY_FLAG		%d\n", m_Config.bShowCountryFlag);
	fprintf(fp, "SHOW_EMPIRE_FLAG		%d\n", m_Config.bShowEmpireFlag);
#endif
#ifdef ENABLE_NEW_GAMEOPTION
	fprintf(fp, "QUEST_LETTER				%d\n", m_Config.questLetter);
	fprintf(fp, "AFFECT_ICONS				%d\n", m_Config.affectIcons);
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
	fprintf(fp, "STONE_SCALE			%f\n", m_Config.m_fStoneScale);
#endif
#ifdef ENABLE_MAP_OBJECT_OPTIMIZATION
	fprintf(fp, "OBJECT_DIST\t%.5f\n", m_Config.fobjectDist);
#endif
#if defined(__BL_GRAPHIC_ON_OFF__)
	fprintf(fp, "EFFECT_LEVEL\t\t\t%d\n", CPythonGraphicOnOff::Instance().GetEffectOnOffLevel());
	fprintf(fp, "PRIVATE_SHOP_LEVEL\t\t%d\n", CPythonGraphicOnOff::Instance().GetPrivateShopOnOffLevel());
	fprintf(fp, "DROP_ITEM_LEVEL\t\t%d\n", CPythonGraphicOnOff::Instance().GetDropItemOnOffLevel());
	fprintf(fp, "PET_STATUS\t\t\t\t%d\n", CPythonGraphicOnOff::Instance().GetPetOnOffStatus());
	fprintf(fp, "NPC_NAME_STATUS\t\t%d\n", CPythonGraphicOnOff::Instance().GetNPCNameOnOffStatus());
#endif
	fprintf(fp, "\n");

	fclose(fp);
	return true;
}

bool CPythonSystem::LoadInterfaceStatus()
{
	FILE * File;
	File = fopen("interface.cfg", "rb");

	if (!File)
		return false;

	fread(m_WindowStatus, 1, sizeof(TWindowStatus) * WINDOW_MAX_NUM, File);
	fclose(File);
	return true;
}

void CPythonSystem::SaveInterfaceStatus()
{
	if (!m_poInterfaceHandler)
		return;

	PyCallClassMemberFunc(m_poInterfaceHandler, "OnSaveInterfaceStatus", Py_BuildValue("()"));

	FILE * File;

	File = fopen("interface.cfg", "wb");

	if (!File)
	{
		TraceError("Cannot open interface.cfg");
		return;
	}

	fwrite(m_WindowStatus, 1, sizeof(TWindowStatus) * WINDOW_MAX_NUM, File);
	fclose(File);
}

bool CPythonSystem::isInterfaceConfig()
{
	return m_isInterfaceConfig;
}

const CPythonSystem::TWindowStatus & CPythonSystem::GetWindowStatusReference(int iIndex)
{
	return m_WindowStatus[iIndex];
}

void CPythonSystem::ApplyConfig()
{
	if (m_OldConfig.gamma != m_Config.gamma)
	{
		float val = 1.0f;

		switch (m_Config.gamma)
		{
			case 0: val = 0.4f;	break;
			case 1: val = 0.7f; break;
			case 2: val = 1.0f; break;
			case 3: val = 1.2f; break;
			case 4: val = 1.4f; break;
		}

		CPythonGraphic::Instance().SetGamma(val);
	}

	if (m_OldConfig.is_software_cursor != m_Config.is_software_cursor)
	{
		if (m_Config.is_software_cursor)
			CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_SOFTWARE);
		else
			CPythonApplication::Instance().SetCursorMode(CPythonApplication::CURSOR_MODE_HARDWARE);
	}

	m_OldConfig = m_Config;

	ChangeSystem();
}

void CPythonSystem::ChangeSystem()
{
	// Shadow
	/*
	if (m_Config.is_shadow)
		CScreen::SetShadowFlag(true);
	else
		CScreen::SetShadowFlag(false);
	*/
	CSoundManager& rkSndMgr = CSoundManager::Instance();
	/*
	float fMusicVolume;
	if (0 == m_Config.music_volume)
		fMusicVolume = 0.0f;
	else
		fMusicVolume= (float)pow(10.0f, (-1.0f + (float)m_Config.music_volume / 5.0f));
		*/
	rkSndMgr.SetMusicVolume(m_Config.music_volume);

	/*
	float fVoiceVolume;
	if (0 == m_Config.voice_volume)
		fVoiceVolume = 0.0f;
	else
		fVoiceVolume = (float)pow(10.0f, (-1.0f + (float)m_Config.voice_volume / 5.0f));
	*/
	rkSndMgr.SetSoundVolumeGrade(m_Config.voice_volume);
}

#if defined(ENABLE_PICK_FILTER)
#include <fstream>

CPythonSystem::CPickUpFilter::CPickUpFilter()
{
	///// READ ////
	std::ifstream inputFile;
	inputFile.open(cPickUpFilterFileName, std::ios::binary);
	if (inputFile.is_open())
	{
		inputFile.read(reinterpret_cast<char*>(bPickFilter),	sizeof(bool) * EPICKFILTER::EPICKFILTER_MAX);
		inputFile.read(reinterpret_cast<char*>(bPickSize),		sizeof(bool) * ESIZE::ESIZE_MAX);
		inputFile.read(reinterpret_cast<char*>(&bModeAll),		sizeof(bool));
		inputFile.read(reinterpret_cast<char*>(&m_bRefineMin),	sizeof(BYTE));
		inputFile.read(reinterpret_cast<char*>(&m_bRefineMax),	sizeof(BYTE));
		inputFile.read(reinterpret_cast<char*>(&m_lLevelMin),	sizeof(long));
		inputFile.read(reinterpret_cast<char*>(&m_lLevelMax),	sizeof(long));

		inputFile.close();
	}
	else
	{
		///// SET DEFAULT ////
		std::fill(std::begin(bPickFilter), std::end(bPickFilter), true);
		std::fill(std::begin(bPickSize), std::end(bPickSize), true);

		bModeAll = false;
		
		m_bRefineMin = 0;
		m_bRefineMax = 9;

		m_lLevelMin = 0;
		m_lLevelMax = 999;
	}
}

CPythonSystem::CPickUpFilter::~CPickUpFilter()
{
	///// SAVE ////
	std::ofstream outputFile;
	outputFile.open(cPickUpFilterFileName, std::ios::binary);
	if (outputFile.is_open())
	{
		outputFile.write(reinterpret_cast<char*>(bPickFilter),		sizeof(bool) * EPICKFILTER::EPICKFILTER_MAX);
		outputFile.write(reinterpret_cast<char*>(bPickSize),		sizeof(bool) * ESIZE::ESIZE_MAX);
		outputFile.write(reinterpret_cast<char*>(&bModeAll),		sizeof(bool));
		outputFile.write(reinterpret_cast<char*>(&m_bRefineMin),	sizeof(BYTE));
		outputFile.write(reinterpret_cast<char*>(&m_bRefineMax),	sizeof(BYTE));
		outputFile.write(reinterpret_cast<char*>(&m_lLevelMin),		sizeof(long));
		outputFile.write(reinterpret_cast<char*>(&m_lLevelMax),		sizeof(long));

		outputFile.close();
	}
	else
	{
		Tracenf("CPickUpFilter::~CPickUpFilter() Cannot create a file for save settings.");
	}
}

void CPythonSystem::CPickUpFilter::SetFilter(size_t sIndex, bool b)
{
	if (sIndex >= EPICKFILTER::EPICKFILTER_MAX)
	{
		Tracenf("CPickUpFilter::SetFilter(Index=%d) : Out of range", sIndex);
		return;
	}

	bPickFilter[sIndex] = b;
}

void CPythonSystem::CPickUpFilter::SetSize(size_t sIndex, bool b)
{
	if (sIndex >= ESIZE::ESIZE_MAX)
	{
		Tracenf("CPickUpFilter::SetSize(Index=%d) : Out of range", sIndex);
		return;
	}

	if (sIndex == ESIZE::BIG && b == false)
		SetFilter(EPICKFILTER::SUB_WEAPON_TWO_HANDED, false);

	bPickSize[sIndex] = b;
}

void CPythonSystem::CPickUpFilter::SetRefine(BYTE min, BYTE max)
{
	m_bRefineMin = static_cast<BYTE>(MINMAX(0, min, 9));
	m_bRefineMax = static_cast<BYTE>(MINMAX(0, max, 9));
}

void CPythonSystem::CPickUpFilter::SetLevel(long min, long max)
{
	m_lLevelMin = MINMAX(0, min, 999);
	m_lLevelMax = MINMAX(0, max, 999);
}

void CPythonSystem::CPickUpFilter::SetModeAll(bool b)
{
	bModeAll = b;
}

bool CPythonSystem::CPickUpFilter::CanPickItem(DWORD dwIID)
{
	CItemData* pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(CPythonItem::Instance().GetVirtualNumberOfGroundItem(dwIID), &pItemData))
	{
		Tracenf("CPickUpFilter::CanPickItem(dwIID=%d) : Non-exist item.", dwIID);
		return true;
	}

	if (CheckRefine(pItemData) == false)
		return false;

	if (CheckLevel(pItemData) == false)
		return false;

	if (CheckSize(pItemData) == false)
		return false;

	if (CheckType(pItemData) == false)
		return false;
	
	return true;
}

std::pair<BYTE, BYTE> CPythonSystem::CPickUpFilter::GetRefine()
{
	return std::make_pair(m_bRefineMin, m_bRefineMax);
}

std::pair<long, long> CPythonSystem::CPickUpFilter::GetLevel()
{
	return std::make_pair(m_lLevelMin, m_lLevelMax);
}

bool CPythonSystem::CPickUpFilter::GetFilter(size_t sIndex) const
{
	if (sIndex >= EPICKFILTER::EPICKFILTER_MAX)
	{
		Tracenf("CPickUpFilter::GetFilter(Index=%d) : Out of range", sIndex);
		return false;
	}

	return bPickFilter[sIndex];
}

bool CPythonSystem::CPickUpFilter::GetSize(size_t sIndex) const
{
	if (sIndex >= ESIZE::ESIZE_MAX)
	{
		Tracenf("CPickUpFilter::GetSize(Index=%d) : Out of range", sIndex);
		return false;
	}

	return bPickSize[sIndex];
}

bool CPythonSystem::CPickUpFilter::IsModeAll() const
{
	return bModeAll;
}

bool CPythonSystem::CPickUpFilter::CheckRefine(const CItemData* pItem) const
{
	const BYTE bRefineLevel = static_cast<BYTE>(pItem->GetRefine());
	if (bRefineLevel >= m_bRefineMin && bRefineLevel <= m_bRefineMax)
		return true;

	return false;
}

bool CPythonSystem::CPickUpFilter::CheckLevel(const CItemData* pItem) const
{
	CItemData::TItemLimit ItemLimit;
	for (BYTE i = 0; i < CItemData::ITEM_LIMIT_MAX_NUM; i++)
	{
		if (!pItem->GetLimit(i, &ItemLimit))
			continue;

		if (ItemLimit.bType != CItemData::LIMIT_LEVEL)
			continue;

		const long lLimitLevel = ItemLimit.lValue;
		return (lLimitLevel >= m_lLevelMin && lLimitLevel <= m_lLevelMax);
	}

	return true;
}

bool CPythonSystem::CPickUpFilter::CheckSize(const CItemData* pItem) const
{
	return GetSize(pItem->GetSize() - 1);
}

bool CPythonSystem::CPickUpFilter::CheckType(const CItemData* pItem) const
{
	const BYTE bType = pItem->GetType();
	const BYTE bSubType = pItem->GetSubType();

	switch (bType)
	{
	case CItemData::EItemType::ITEM_TYPE_WEAPON:
		switch (bSubType)
		{
		case CItemData::EWeaponSubTypes::WEAPON_SWORD:
			return GetFilter(EPICKFILTER::SUB_WEAPON_SWORD);

		case CItemData::EWeaponSubTypes::WEAPON_DAGGER:
			return GetFilter(EPICKFILTER::SUB_WEAPON_DAGGER);

		case CItemData::EWeaponSubTypes::WEAPON_BOW:
			return GetFilter(EPICKFILTER::SUB_WEAPON_BOW);

		case CItemData::EWeaponSubTypes::WEAPON_TWO_HANDED:
			return GetFilter(EPICKFILTER::SUB_WEAPON_TWO_HANDED);

		case CItemData::EWeaponSubTypes::WEAPON_BELL:
			return GetFilter(EPICKFILTER::SUB_WEAPON_BELL);

		case CItemData::EWeaponSubTypes::WEAPON_FAN:
			return GetFilter(EPICKFILTER::SUB_WEAPON_FAN);

		case CItemData::EWeaponSubTypes::WEAPON_ARROW:
			return GetFilter(EPICKFILTER::SUB_WEAPON_ARROW);

		/*case CItemData::EWeaponSubTypes::WEAPON_MOUNT_SPEAR:
			return GetFilter(EPICKFILTER::SUB_WEAPON_MOUNT_SPEAR);*/
		}
		break;

	case CItemData::EItemType::ITEM_TYPE_ARMOR:
		switch (bSubType)
		{
		case CItemData::EArmorSubTypes::ARMOR_BODY:
			return GetFilter(EPICKFILTER::SUB_ARMOR_BODY);

		case CItemData::EArmorSubTypes::ARMOR_HEAD:
			return GetFilter(EPICKFILTER::SUB_ARMOR_HEAD);

		case CItemData::EArmorSubTypes::ARMOR_SHIELD:
			return GetFilter(EPICKFILTER::SUB_ARMOR_SHIELD);

		case CItemData::EArmorSubTypes::ARMOR_WRIST:
			return GetFilter(EPICKFILTER::SUB_ARMOR_WRIST);

		case CItemData::EArmorSubTypes::ARMOR_FOOTS:
			return GetFilter(EPICKFILTER::SUB_ARMOR_FOOTS);

		case CItemData::EArmorSubTypes::ARMOR_NECK:
			return GetFilter(EPICKFILTER::SUB_ARMOR_NECK);

		case CItemData::EArmorSubTypes::ARMOR_EAR:
			return GetFilter(EPICKFILTER::SUB_ARMOR_EAR);
		}
		break;

	case CItemData::EItemType::ITEM_TYPE_METIN:
		return GetFilter(EPICKFILTER::TYPE_METIN);

	case CItemData::EItemType::ITEM_TYPE_ELK:
		return GetFilter(EPICKFILTER::TYPE_YANG);

	case CItemData::EItemType::ITEM_TYPE_SKILLBOOK:
		return GetFilter(EPICKFILTER::TYPE_SKILLBOOK);

	case CItemData::EItemType::ITEM_TYPE_GIFTBOX:
		return GetFilter(EPICKFILTER::TYPE_GIFTBOX);

	case CItemData::EItemType::ITEM_TYPE_BELT:
		return GetFilter(EPICKFILTER::TYPE_BELT);

	case CItemData::EItemType::ITEM_TYPE_POLYMORPH:
		return GetFilter(EPICKFILTER::TYPE_POLY);

	case CItemData::EItemType::ITEM_TYPE_RING:
		return GetFilter(EPICKFILTER::TYPE_RING);
	
	case CItemData::EItemType::ITEM_TYPE_USE:
		switch (bSubType)
		{
		case CItemData::EUseSubTypes::USE_POTION:
		case CItemData::EUseSubTypes::USE_ABILITY_UP:
		case CItemData::EUseSubTypes::USE_POTION_NODELAY:
		case CItemData::EUseSubTypes::USE_POTION_CONTINUE:
			return GetFilter(EPICKFILTER::SUB_POTION);
		}
		break;

	case CItemData::EItemType::ITEM_TYPE_MATERIAL:
		return GetFilter(EPICKFILTER::TYPE_MATERIAL);
	}

	return true;
}
#endif

void CPythonSystem::Clear()
{
	SetInterfaceHandler(NULL);
}

CPythonSystem::CPythonSystem()
{
	memset(&m_Config, 0, sizeof(TConfig));

	m_poInterfaceHandler = NULL;

	SetDefaultConfig();

	LoadConfig();
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
	m_dwFilterEmpireFlag = 0;
	LoadChatFilterSettings();
#endif

	ChangeSystem();

	if (LoadInterfaceStatus())
		m_isInterfaceConfig = true;
	else
		m_isInterfaceConfig = false;
}

CPythonSystem::~CPythonSystem()
{
	assert(m_poInterfaceHandler==NULL && "CPythonSystem MUST CLEAR!");
}

#ifdef ENABLE_HWID_BAN
#include <iomanip>
#include "picosha2.h"
#include "smbios.cpp"
TCHAR* registry_read(LPCTSTR subkey, LPCTSTR name, unsigned long type)
{
	HKEY key;
	TCHAR value[255];
	unsigned long value_length = 255;
	RegOpenKey(HKEY_LOCAL_MACHINE, subkey, &key);
	RegQueryValueEx(key, name, nullptr, &type, (LPBYTE)&value, &value_length);
	RegCloseKey(key);
	return value;
}

uint32_t CPythonSystem::getVolumeHash()
{
	CHAR windowsDirectory[MAX_PATH];
	CHAR volumeName[8] = { 0 };
	unsigned long serialNum = 0;

	if (!GetWindowsDirectory(windowsDirectory, sizeof(windowsDirectory)))
	{
		windowsDirectory[0] = L'C';
	}

	volumeName[0] = windowsDirectory[0];
	volumeName[1] = ':';
	volumeName[2] = '\\';
	volumeName[3] = '\0';

	GetVolumeInformation(volumeName, nullptr, 0, &serialNum, 0, nullptr, nullptr, 0);

	return serialNum;
}

const char* CPythonSystem::getCpuInfos()
{
	SYSTEM_INFO kSystemInfo;
	GetSystemInfo(&kSystemInfo);

	std::string stTemp;
	char szNum[15 + 1];
#define AddNumber(num) _itoa_s(num, szNum, 10), stTemp += szNum
	AddNumber(kSystemInfo.wProcessorArchitecture);
	AddNumber(kSystemInfo.dwNumberOfProcessors);
	AddNumber(kSystemInfo.dwProcessorType);
	AddNumber(kSystemInfo.wProcessorLevel);
	AddNumber(kSystemInfo.wProcessorRevision);
	AddNumber(IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_CHANNELS_ENABLED));
	AddNumber(IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE));
	AddNumber(IsProcessorFeaturePresent(PF_FLOATING_POINT_EMULATED));
	AddNumber(IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_PAE_ENABLED));
	AddNumber(IsProcessorFeaturePresent(PF_RDTSC_INSTRUCTION_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE));
	AddNumber(IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE));
#undef AddNumber

	return stTemp.c_str();
}

const char* CPythonSystem::getMachineName()
{
	static char computerName[1024];
	unsigned long size = 1024;
	GetComputerName(computerName, &size);
	return &(computerName[0]);
}

const char* CPythonSystem::getBiosDate()
{
	static char buf[1024];
	strncpy(buf, registry_read("HARDWARE\\DESCRIPTION\\System\\BIOS", "BIOSReleaseDate", REG_SZ), sizeof(buf));
	return buf;
}

const char* CPythonSystem::getMainboardName()
{
	static char buf[1024];
	strncpy(buf, registry_read("HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardProduct", REG_SZ), sizeof(buf));
	return buf;
}

const char* CPythonSystem::getGPUName()
{
	static char buf[1024];
	buf[0] = '\0';

	IDirect3D9Ex* pD3D = NULL;
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D)) || !pD3D)
		return buf;

	const UINT adapterCount = pD3D->GetAdapterCount();
	if (adapterCount > 0)
	{
		D3DADAPTER_IDENTIFIER9 id = {};
		if (SUCCEEDED(pD3D->GetAdapterIdentifier(0, 0, &id)))
			strncpy(buf, id.Description, sizeof(buf) - 1);
	}
	pD3D->Release();
	return buf;
}

const char* CPythonSystem::GetHWID()
{
	static const unsigned long TargetLength = 64;

	std::stringstream stream;

#ifdef HDD_COMPONENT
	stream << getVolumeHash();
#endif
#ifdef CPU_COMPONENT
	stream << getCpuInfos();
#endif
#ifdef PC_NAME_COMPONENT
	stream << getMachineName();
#endif
#ifdef BIOS_DATE_COMPONENT
	stream << getBiosDate();
#endif
#ifdef MAINBOARD_NAME_COMPONENT
	stream << getMainboardName();
#endif
#ifdef GPU_NAME_COMPONENT
	stream << getGPUName();
#endif

	auto string = stream.str();

	while (string.size() < TargetLength)
	{
		string = string + string;
	}

	if (string.size() > TargetLength)
	{
		string = string.substr(0, TargetLength);
	}

	return generateHash(string);
}

const char* CPythonSystem::generateHash(const std::string& bytes)
{
	static char s_szHWIDBuffer[HWID_MAX_LEN];
	static char chars[] = "0123456789ABCDEF";
	std::stringstream stream;

	auto size = bytes.size();
	for (unsigned long i = 0; i < size; ++i)
	{
		unsigned char ch = ~((unsigned char)((int16_t)bytes[i] +
											 (int16_t)bytes[(i + 1) % size] +
											 (int16_t)bytes[(i + 2) % size] +
											 (int16_t)bytes[(i + 3) % size])) * (i + 1);

		stream << chars[(ch >> 4) & 0x0F] << chars[ch & 0x0F];
	}

	strncpy(s_szHWIDBuffer, stream.str().c_str(), sizeof(s_szHWIDBuffer));

	return s_szHWIDBuffer;
}
#endif

#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
void CPythonSystem::SetAnonymousCountryMode(bool isEnable)
{
	m_Config.bAnonymousCountryMode = isEnable;
}

bool CPythonSystem::GetAnonymousCountryMode() const
{
	return m_Config.bAnonymousCountryMode;
}

void CPythonSystem::SetShowCountryFlag(bool isEnable)
{
	m_Config.bShowCountryFlag = isEnable;
}

bool CPythonSystem::IsShowCountryFlag() const
{
	return m_Config.bShowCountryFlag;
}

void CPythonSystem::SetShowEmpireFlag(bool isEnable)
{
	m_Config.bShowEmpireFlag = isEnable;
}

bool CPythonSystem::IsShowEmpireFlag() const
{
	return m_Config.bShowEmpireFlag;
}

void CPythonSystem::AddChatFilterCountry(const std::string& country)
{
	m_setFilterCountry.emplace(country);
	CPythonChat::Instance().ArrangeAllShowingChat();
}

void CPythonSystem::RemoveChatFilterCountry(const std::string& country)
{
	m_setFilterCountry.erase(country);
	CPythonChat::Instance().ArrangeAllShowingChat();
}

bool CPythonSystem::IsChatFilterCountry(const std::string& country) const
{
	return std::find(m_setFilterCountry.begin(), m_setFilterCountry.end(), country) != m_setFilterCountry.end();
}

void CPythonSystem::AddChatFilterEmpire(BYTE bEmpire)
{
	SET_BIT(m_dwFilterEmpireFlag, (1 << bEmpire));
	CPythonChat::Instance().ArrangeAllShowingChat();
}

void CPythonSystem::RemoveChatFilterEmpire(BYTE bEmpire)
{
	REMOVE_BIT(m_dwFilterEmpireFlag, (1 << bEmpire));
	CPythonChat::Instance().ArrangeAllShowingChat();
}

bool CPythonSystem::IsChatFilterEmpire(BYTE bEmpire) const
{
	return IS_SET(m_dwFilterEmpireFlag, (1 << bEmpire));
}

void CPythonSystem::LoadChatFilterSettings()
{
	FILE* f = fopen("chat_filter.bin", "rb");
	if (!f)
		return;

	if (!feof(f))
		fread(&m_dwFilterEmpireFlag, sizeof(DWORD), 1, f);

	while (!feof(f))
	{
		size_t len;
		fread(&len, sizeof(size_t), 1, f);

		if (feof(f))
			break;

		char* buffer = new char[len + 1];
		fread(buffer, sizeof(char), len, f);
		buffer[len] = '\0';
		m_setFilterCountry.insert(buffer);
		delete[] buffer;
	}

	fclose(f);
}

void CPythonSystem::SaveChatFilterSettings() const
{
	FILE* f = fopen("chat_filter.bin", "wb");
	if (!f)
		return;

	fwrite(&m_dwFilterEmpireFlag, sizeof(DWORD), 1, f);

	for (const std::string& str : m_setFilterCountry)
	{
		const size_t len = str.size();
		fwrite(&len, sizeof(size_t), 1, f);
		fwrite(str.c_str(), sizeof(char), len, f);
	}

	fclose(f);
}
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
void CPythonSystem::SetStoneScale(float fScale)
{
	m_Config.m_fStoneScale = fScale;
}

float CPythonSystem::GetStoneScale()
{
	return m_Config.m_fStoneScale;
}
#endif

#ifdef ENABLE_MAP_OBJECT_OPTIMIZATION
void CPythonSystem::SetObjectDistance(float fDist)
{
	if (fDist > 25600.0)
		fDist = 25600.0;

	m_Config.fobjectDist = fDist;
}
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
