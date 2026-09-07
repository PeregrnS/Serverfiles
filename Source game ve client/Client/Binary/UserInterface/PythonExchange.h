#pragma once

#include "Packet.h"

class CPythonExchange : public CSingleton<CPythonExchange>
{
	public:
		enum
		{
#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
			EXCHANGE_ITEM_MAX_NUM = 24,
#else
			EXCHANGE_ITEM_MAX_NUM = 12,
#endif
		};

		typedef struct trade
		{
			char					name[CHARACTER_NAME_MAX_LEN + 1];
#if defined(ENABLE_LEVEL_IN_TRADE) || defined(ENABLE_EXCHANGE_WINDOW_RENEWAL)
			DWORD					level;
#endif
#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
			DWORD					race;
#endif

			DWORD					item_vnum[EXCHANGE_ITEM_MAX_NUM];
#ifdef ENABLE_EXTENDED_ITEM_COUNT
			short					item_count[EXCHANGE_ITEM_MAX_NUM];
#else
			BYTE					item_count[EXCHANGE_ITEM_MAX_NUM];
#endif
			DWORD					item_metin[EXCHANGE_ITEM_MAX_NUM][ITEM_SOCKET_SLOT_MAX_NUM];
			TPlayerItemAttribute	item_attr[EXCHANGE_ITEM_MAX_NUM][ITEM_ATTRIBUTE_SLOT_MAX_NUM];

			BYTE					accept;
#ifdef ENABLE_GOLD_MAX_EXTENDED
			unsigned long long		elk;
#else
			DWORD					elk;
#endif
		} TExchangeData;

	public:
		CPythonExchange();
		virtual ~CPythonExchange();

		void			Clear();

		void			Start();
		void			End();
		bool			isTrading();

		// Interface

		void			SetSelfName(const char *name);
		void			SetTargetName(const char *name);

		char			*GetNameFromSelf();
		char			*GetNameFromTarget();

#if defined(ENABLE_LEVEL_IN_TRADE) || defined(ENABLE_EXCHANGE_WINDOW_RENEWAL)
		void			SetSelfLevel(DWORD level);
		void			SetTargetLevel(DWORD level);

		DWORD			GetLevelFromSelf();
		DWORD			GetLevelFromTarget();
#endif

#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
		void			SetSelfRace(DWORD race);
		void			SetTargetRace(DWORD race);

		DWORD			GetRaceFromSelf();
		DWORD			GetRaceFromTarget();
#endif

#ifdef ENABLE_GOLD_MAX_EXTENDED
		void 			SetElkToTarget(unsigned long long elk);
		void 			SetElkToSelf(unsigned long long elk);

		unsigned long long	GetElkFromTarget();
		unsigned long long	GetElkFromSelf();
#else
		void			SetElkToTarget(DWORD elk);
		void			SetElkToSelf(DWORD elk);

		DWORD			GetElkFromTarget();
		DWORD			GetElkFromSelf();
#endif

#ifdef ENABLE_EXTENDED_ITEM_COUNT
		void			SetItemToTarget(DWORD pos, DWORD vnum, short count);
		void			SetItemToSelf(DWORD pos, DWORD vnum, short count);
#else
		void			SetItemToTarget(DWORD pos, DWORD vnum, BYTE count);
		void			SetItemToSelf(DWORD pos, DWORD vnum, BYTE count);
#endif

		void			SetItemMetinSocketToTarget(int pos, int imetinpos, DWORD vnum);
		void			SetItemMetinSocketToSelf(int pos, int imetinpos, DWORD vnum);

		void			SetItemAttributeToTarget(int pos, int iattrpos, BYTE byType, short sValue);
		void			SetItemAttributeToSelf(int pos, int iattrpos, BYTE byType, short sValue);

		void			DelItemOfTarget(BYTE pos);
		void			DelItemOfSelf(BYTE pos);

		DWORD			GetItemVnumFromTarget(BYTE pos);
		DWORD			GetItemVnumFromSelf(BYTE pos);

#ifdef ENABLE_EXTENDED_ITEM_COUNT
		short			GetItemCountFromTarget(BYTE pos);
		short			GetItemCountFromSelf(BYTE pos);
#else
		BYTE			GetItemCountFromTarget(BYTE pos);
		BYTE			GetItemCountFromSelf(BYTE pos);
#endif

		DWORD			GetItemMetinSocketFromTarget(BYTE pos, int iMetinSocketPos);
		DWORD			GetItemMetinSocketFromSelf(BYTE pos, int iMetinSocketPos);

		void			GetItemAttributeFromTarget(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue);
		void			GetItemAttributeFromSelf(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue);

		void			SetAcceptToTarget(BYTE Accept);
		void			SetAcceptToSelf(BYTE Accept);

		bool			GetAcceptFromTarget();
		bool			GetAcceptFromSelf();

		bool			GetElkMode();
		void			SetElkMode(bool value);

	protected:
		bool				m_isTrading;

		bool				m_elk_mode;
		TExchangeData		m_self;
		TExchangeData		m_victim;
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
