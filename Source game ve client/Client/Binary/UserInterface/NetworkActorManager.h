#pragma once

#include "InstanceBase.h"
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	#include "Packet.h"
#endif

struct SNetworkActorData
{
	std::string m_stName;
#if defined(__BL_MULTI_LANGUAGE_PREMIUM__)
	std::string m_stCountryName;
#endif

	CAffectFlagContainer	m_kAffectFlags;

	BYTE	m_bType;
	DWORD	m_dwVID;
	DWORD	m_dwStateFlags;
	DWORD	m_dwEmpireID;
	DWORD	m_dwRace;
	DWORD	m_dwMovSpd;
	DWORD	m_dwAtkSpd;
	FLOAT	m_fRot;
	LONG	m_lCurX;
	LONG	m_lCurY;
	LONG	m_lSrcX;
	LONG	m_lSrcY;
	LONG	m_lDstX;
	LONG	m_lDstY;

	DWORD	m_dwServerSrcTime;
	DWORD	m_dwClientSrcTime;
	DWORD	m_dwDuration;

	DWORD	m_dwArmor;
	DWORD	m_dwWeapon;
	DWORD	m_dwHair;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	DWORD	m_dwAcce;
#endif
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	DWORD	m_dwShining[CHR_SHINING_NUM];
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	m_dwArrow;
#endif

	DWORD	m_dwOwnerVID;

	short	m_sAlignment;
	BYTE	m_byPKMode;
	DWORD	m_dwMountVnum;

	DWORD	m_dwGuildID;
	DWORD	m_dwLevel;
	BYTE	byTitleIndex;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD	m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_SKILL_COSTUME_SYSTEM
	BYTE	m_bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT];
#endif

	SNetworkActorData();

	void SetDstPosition(DWORD dwServerTime, LONG lDstX, LONG lDstY, DWORD dwDuration);
	void SetPosition(LONG lPosX, LONG lPosY);
	void UpdatePosition();

	// NETWORK_ACTOR_DATA_COPY
	SNetworkActorData(const SNetworkActorData& src);
	void operator=(const SNetworkActorData& src);
	void __copy__(const SNetworkActorData& src);
	// END_OF_NETWORK_ACTOR_DATA_COPY
};

struct SNetworkMoveActorData
{
	DWORD	m_dwVID;
	DWORD	m_dwTime;
	LONG	m_lPosX;
	LONG	m_lPosY;
	float	m_fRot;
	DWORD	m_dwFunc;
	DWORD	m_dwArg;
	DWORD	m_dwDuration;

	SNetworkMoveActorData()
	{
		m_dwVID=0;
		m_dwTime=0;
		m_fRot=0.0f;
		m_lPosX=0;
		m_lPosY=0;
		m_dwFunc=0;
		m_dwArg=0;
		m_dwDuration=0;
	}
};

struct SNetworkUpdateActorData
{
	DWORD m_dwVID;
	DWORD m_dwGuildID;
	DWORD m_dwArmor;
	DWORD m_dwWeapon;
	DWORD m_dwHair;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	DWORD	m_dwAcce;
#endif
#ifdef ENABLE_SHINING_ITEM_SYSTEM
	DWORD m_dwShining[CHR_SHINING_NUM];
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD m_dwArrow;
#endif
	DWORD m_dwMovSpd;
	DWORD m_dwAtkSpd;
	short m_sAlignment;
	DWORD m_dwLevel;
	BYTE byTitleIndex;
	BYTE m_byPKMode;
	DWORD m_dwMountVnum;
	DWORD m_dwStateFlags;
	CAffectFlagContainer m_kAffectFlags;
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_SKILL_COSTUME_SYSTEM
	BYTE m_bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT];
#endif

	SNetworkUpdateActorData()
	{
		m_dwGuildID=0;
		m_dwVID=0;
		m_dwArmor=0;
		m_dwWeapon=0;
		m_dwHair=0;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		m_dwAcce = 0;
#endif
#ifdef ENABLE_SHINING_ITEM_SYSTEM
		for (int i = 0; i < CHR_SHINING_NUM; i++)
		{
			m_dwShining[i] = 0;
		}
#endif
#ifdef ENABLE_QUIVER_SYSTEM
		m_dwArrow=0;
#endif
		m_dwMovSpd=0;
		m_dwAtkSpd=0;
		m_sAlignment=0;
		m_dwLevel = 0;
		byTitleIndex = 0;
		m_byPKMode=0;
		m_dwMountVnum=0;
		m_dwStateFlags=0;
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		memset(m_dwSkillColor, 0, sizeof(m_dwSkillColor));
#endif
#ifdef ENABLE_SKILL_COSTUME_SYSTEM
		memset(m_bSkillMotion, 0, sizeof(m_bSkillMotion));
#endif
		m_kAffectFlags.Clear();
	}
};

class CPythonCharacterManager;

class CNetworkActorManager : public CReferenceObject
{
	public:
		CNetworkActorManager();
		virtual ~CNetworkActorManager();

		void Destroy();

		void SetMainActorVID(DWORD dwVID);

		void RemoveActor(DWORD dwVID);
		void AppendActor(const SNetworkActorData& c_rkNetActorData);
		void UpdateActor(const SNetworkUpdateActorData& c_rkNetUpdateActorData);
		void MoveActor(const SNetworkMoveActorData& c_rkNetMoveActorData);

		void SyncActor(DWORD dwVID, LONG lPosX, LONG lPosY);
		void SetActorOwner(DWORD dwOwnerVID, DWORD dwVictimVID);

		void Update();

	protected:
		void __OLD_Update();

		void __UpdateMainActor();

		bool __IsVisiblePos(LONG lPosX, LONG lPosY, const SNetworkActorData& c_rkNetActorData);
		bool __IsVisibleActor(const SNetworkActorData& c_rkNetActorData);
		bool __IsMainActorVID(DWORD dwVID);

		void __RemoveAllGroundItems();
		void __RemoveAllActors();
		void __RemoveDynamicActors();
		void __RemoveCharacterManagerActor(SNetworkActorData& rkNetActorData);

		SNetworkActorData* __FindActorData(DWORD dwVID);

		CInstanceBase* __AppendCharacterManagerActor(SNetworkActorData& rkNetActorData);
		CInstanceBase* __FindActor(SNetworkActorData& rkNetActorData);
		CInstanceBase* __FindActor(SNetworkActorData& rkNetActorData, LONG lDstX, LONG lDstY);

		CPythonCharacterManager& __GetCharacterManager();

	protected:
		DWORD m_dwMainVID;

		LONG m_lMainPosX;
		LONG m_lMainPosY;

		std::map<DWORD, SNetworkActorData> m_kNetActorDict;
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
