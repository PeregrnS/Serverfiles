#ifndef __INC_METIN_II_GAME_EXCHANGE_H__
#define __INC_METIN_II_GAME_EXCHANGE_H__

class CGrid;

enum EExchangeValues
{
#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
	EXCHANGE_ITEM_MAX_NUM = 24,
#else
	EXCHANGE_ITEM_MAX_NUM = 12,
#endif
	EXCHANGE_MAX_DISTANCE	= 1000
};

class CExchange
{
	public:
		CExchange(LPCHARACTER pOwner);
		~CExchange();

		bool		Accept(bool bIsAccept = true);
		void		Cancel();

#ifdef ENABLE_GOLD_MAX_EXTENDED
		bool AddGold(unsigned long long lGold);
#else
		bool AddGold(long lGold);
#endif
#ifdef ENABLE_CHECKINOUT_UPDATE
		int			GetEmptyExchange(BYTE size);
#endif
		bool		AddItem(TItemPos item_pos, BYTE display_pos
#ifdef ENABLE_CHECKINOUT_UPDATE
				 ,
				 bool bAutoItemPos
#endif
		);
		bool		RemoveItem(BYTE pos);

		LPCHARACTER	GetOwner()	{ return m_pOwner;	}
		CExchange *	GetCompany()	{ return m_pCompany;	}

		bool		GetAcceptStatus() { return m_bAccept; }

		void		SetCompany(CExchange * pExchange)	{ m_pCompany = pExchange; }

	private:
#ifdef ENABLE_TRADE_LOG
		bool		Done(DWORD tradeID);
#else
		bool		Done();
#endif
		bool		Check(int * piItemCount);
		bool		CheckSpace();

	private:
		CExchange *	m_pCompany;

		LPCHARACTER	m_pOwner;

		TItemPos		m_aItemPos[EXCHANGE_ITEM_MAX_NUM];
		LPITEM		m_apItems[EXCHANGE_ITEM_MAX_NUM];
		BYTE		m_abItemDisplayPos[EXCHANGE_ITEM_MAX_NUM];

		bool 		m_bAccept;
#ifdef ENABLE_GOLD_MAX_EXTENDED
		unsigned long long m_lGold;
#else
		long m_lGold;
#endif

		CGrid *		m_pGrid;
};

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
