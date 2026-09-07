#ifndef __INC_CLIENTMANAGER_H__
#define __INC_CLIENTMANAGER_H__

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include "../../common/stl.h"
#include "../../common/building.h"

#include "Peer.h"
#include "DBManager.h"
#include "LoginData.h"

#include <chrono>

#ifdef __PREMIUM_PRIVATE_SHOP__
#include "PrivateShop.h"
class CPrivateShop;
class CPrivateShopCache;
class CPrivateShopItemCache;
class CPrivateShopSaleCache;
#endif

#define ENABLE_PROTO_FROM_DB

class CPlayerTableCache;
class CItemCache;
class CItemPriceListTableCache;
#ifdef ENABLE_SKILL_COLOR_SYSTEM
class CSKillColorCache;
#endif

class CPacketInfo
{
    public:
	void Add(int header);
	void Reset();

	std::map<int, int> m_map_info;
};

size_t CreatePlayerSaveQuery(char * pszQuery, size_t querySize, TPlayerTable * pkTab);

class CClientManager : public CNetBase, public singleton<CClientManager>
{
    public:
	typedef std::list<CPeer *>			TPeerList;
	typedef boost::unordered_map<DWORD, CPlayerTableCache *> TPlayerTableCacheMap;
	typedef boost::unordered_map<DWORD, CItemCache *> TItemCacheMap;
	typedef boost::unordered_set<CItemCache *, boost::hash<CItemCache*> > TItemCacheSet;
	typedef boost::unordered_map<DWORD, TItemCacheSet *> TItemCacheSetPtrMap;
	typedef boost::unordered_map<DWORD, CItemPriceListTableCache*> TItemPriceListCacheMap;
	typedef boost::unordered_map<short, BYTE> TChannelStatusMap;

#ifdef __PREMIUM_PRIVATE_SHOP__
	typedef boost::unordered_map<DWORD, std::unique_ptr<CPrivateShopCache> > TPrivateShopCacheMap;
	typedef boost::unordered_map<DWORD, std::unique_ptr<CPrivateShop> > TPrivateShopMap;

	typedef boost::unordered_map<DWORD, std::unique_ptr<CPrivateShopItemCache> > TPrivateShopItemCacheMap;
	typedef boost::unordered_set<CPrivateShopItemCache*, boost::hash<CPrivateShopItemCache*> > TPrivateShopItemCacheSet;
	typedef boost::unordered_map<DWORD, std::unique_ptr<TPrivateShopItemCacheSet> > TPrivateShopItemCacheSetPtrMap;

	typedef boost::unordered_map<DWORD, std::unique_ptr<CPrivateShopSaleCache> > TPrivateShopSaleCacheMap;
	typedef boost::unordered_set<CPrivateShopSaleCache*, boost::hash<CPrivateShopSaleCache*> > TPrivateShopSaleCacheSet;
	typedef boost::unordered_map<DWORD, std::unique_ptr<TPrivateShopSaleCacheSet> > TPrivateShopSaleCacheSetPtrMap;

	typedef std::list<TItemPrice>								TMarketItemPriceList;
	typedef boost::unordered_map<DWORD, TMarketItemPriceList>	TMarketItemPriceListMap;
	typedef boost::unordered_map<DWORD, TItemPrice>				TMarketItemPriceMap;

	typedef std::list<CPrivateShop*>							TPrivateShopPtrList;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	typedef boost::unordered_map<DWORD, CSKillColorCache *> TSkillColorCacheMap;
#endif

	// MYSHOP_PRICE_LIST

	typedef std::pair< DWORD, DWORD >		TItemPricelistReqInfo;
	// END_OF_MYSHOP_PRICE_LIST

	class ClientHandleInfo
	{
	    public:
		DWORD	dwHandle;
		DWORD	account_id;
		DWORD	player_id;
		BYTE	account_index;
		char	login[LOGIN_MAX_LEN + 1];
		char	safebox_password[SAFEBOX_PASSWORD_MAX_LEN + 1];
		char	ip[MAX_HOST_LENGTH + 1];

		TAccountTable * pAccountTable;
		TSafeboxTable * pSafebox;

		ClientHandleInfo(DWORD argHandle, DWORD dwPID = 0)
		{
		    dwHandle = argHandle;
		    pSafebox = NULL;
		    pAccountTable = NULL;
		    player_id = dwPID;
		};

		ClientHandleInfo(DWORD argHandle, DWORD dwPID, DWORD accountId)
		{
		    dwHandle = argHandle;
		    pSafebox = NULL;
		    pAccountTable = NULL;
		    player_id = dwPID;
			account_id = accountId;
		};

		~ClientHandleInfo()
		{
		    if (pSafebox)
			{
				delete pSafebox;
				pSafebox = NULL;
			}
		}
	};

	public:
	CClientManager();
	~CClientManager();

	bool	Initialize();
	time_t	GetCurrentTime();

	void	MainLoop();
	void	Quit();

	void	GetPeerP2PHostNames(std::string& peerHostNames);
	void	SetTablePostfix(const char* c_pszTablePostfix);
	void	SetPlayerIDStart(int iIDStart);
	int		GetPlayerIDStart() { return m_iPlayerIDStart; }

	int		GetPlayerDeleteLevelLimit() { return m_iPlayerDeleteLevelLimit; }

	void	SetChinaEventServer(bool flag) { m_bChinaEventServer = flag; }
	bool	IsChinaEventServer() { return m_bChinaEventServer; }

	DWORD	GetUserCount();

	void	SendAllGuildSkillRechargePacket();
	void	SendTime();

	CPlayerTableCache *	GetPlayerCache(DWORD id);
	void			PutPlayerCache(TPlayerTable * pNew);

	void			CreateItemCacheSet(DWORD dwID);
	TItemCacheSet *		GetItemCacheSet(DWORD dwID);
	void			FlushItemCacheSet(DWORD dwID);

	CItemCache *		GetItemCache(DWORD id);
	void			PutItemCache(TPlayerItem * pNew, bool bSkipQuery = false);
	bool			DeleteItemCache(DWORD id);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	CSKillColorCache*	GetSkillColorCache(DWORD id);
	void				PutSkillColorCache(const TSkillColor* pNew);
	void				UpdateSkillColorCache();
#endif

	void			UpdatePlayerCache();
	void			UpdateItemCache();

	// MYSHOP_PRICE_LIST

	CItemPriceListTableCache*	GetItemPriceListCache(DWORD dwID);

	void			PutItemPriceListCache(const TItemPriceListTable* pItemPriceList);

	void			UpdateItemPriceListCache(void);
	// END_OF_MYSHOP_PRICE_LIST

	void			SendGuildSkillUsable(DWORD guild_id, DWORD dwSkillVnum, bool bUsable);

	void			SetCacheFlushCountLimit(int iLimit);

	template <class Func>
	Func		for_each_peer(Func f);

	CPeer *		GetAnyPeer();

	void			ForwardPacket(BYTE header, const void* data, int size, BYTE bChannel = 0, CPeer * except = NULL);

	void			SendNotice(const char * c_pszFormat, ...);

	// @fixme203 directly GetCommand instead of strcpy
	char*			GetCommand(char* str, char* command);
	void			ItemAward(CPeer * peer, char* login);

	void RecvGuildStoragePacket(CPeer* peer, const char* data);
	bool RecvGuildStorageRequestItemRemove(CPeer* peer, const char* data);
	bool RecvGuildStorageRequestItemMove(CPeer* peer, const char* data);
	bool RecvGuildStorageRequestGoldAdd(CPeer* peer, const char* data);
	bool RecvGuildStorageRequestGoldRemove(CPeer* peer, const char* data);
	bool RecvGuildStorageRequestStorage(CPeer* peer, const char* data);
	bool RecvGuildStorageAddItemAddLog(CPeer* peer, const char* data);
	void QueryResultGuildStorage(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void QueryResultGuildStorageAddItem(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void QueryResultGuildStorageGoldUpdate(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void QueryResultGuildStorageGoldLoad(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void QueryResultGuildStorageLogLoad(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void AddGuildStorageLogEntry(CPeer* peer, uint32_t guildId, const char* name, uint32_t vnum, uint16_t slot, uint16_t slot_new, long long gold = 0);
	std::unordered_map<uint32_t, std::shared_ptr<DBguildStorage>> m_guildStorages;

    protected:
	void	Destroy();

    private:
	bool		InitializeTables();
	bool		InitializeShopTable();
#if defined(ENABLE_RENEWAL_SHOPEX)
	bool		InitializeShopEXTable();
#endif
	bool		InitializeMobTable();
	bool		InitializeItemTable();
	bool		InitializeQuestItemTable();
	bool		InitializeSkillTable();
	bool		InitializeRefineTable();
	bool		InitializeBanwordTable();
	bool		InitializeItemAttrTable();
	bool		InitializeItemRareTable();
	bool		InitializeLandTable();
	bool		InitializeObjectProto();
	bool		InitializeObjectTable();
	bool		InitializeMonarch();
#ifdef ENABLE_EVENT_MANAGER
	bool		InitializeEventTable();
#endif

#ifdef __DAILY_QUESTS__
	bool		InitializeDailyQuestTable();
	bool		InitializeDailyQuestRewardTable();
#endif

#ifdef ENABLE_GEM_SYSTEM
	bool		InitializeGemShopTable();
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	bool		InitializeBiologMissions();
	bool		InitializeBiologRewards();
	bool		InitializeBiologMonsters();
#endif

	bool		MirrorMobTableIntoDB();
	bool		MirrorItemTableIntoDB();

	void		AddPeer(socket_t fd);
	void		RemovePeer(CPeer * pPeer);
	CPeer *		GetPeer(IDENT ident);

	int		AnalyzeQueryResult(SQLMsg * msg);
	int		AnalyzeErrorMsg(CPeer * peer, SQLMsg * msg);

	int		Process();

        void            ProcessPackets(CPeer * peer);

	CLoginData *	GetLoginData(DWORD dwKey);
	CLoginData *	GetLoginDataByLogin(const char * c_pszLogin);
	CLoginData *	GetLoginDataByAID(DWORD dwAID);

	void		InsertLoginData(CLoginData * pkLD);
	void		DeleteLoginData(CLoginData * pkLD);

	bool		InsertLogonAccount(const char * c_pszLogin, DWORD dwHandle, const char * c_pszIP);
	bool		DeleteLogonAccount(const char * c_pszLogin, DWORD dwHandle);
	bool		FindLogonAccount(const char * c_pszLogin);

	void		GuildCreate(CPeer * peer, DWORD dwGuildID);
	void		GuildSkillUpdate(CPeer * peer, TPacketGuildSkillUpdate* p);
	void		GuildExpUpdate(CPeer * peer, TPacketGuildExpUpdate* p);
	void		GuildAddMember(CPeer * peer, TPacketGDGuildAddMember* p);
	void		GuildChangeGrade(CPeer * peer, TPacketGuild* p);
	void		GuildRemoveMember(CPeer * peer, TPacketGuild* p);
	void		GuildChangeMemberData(CPeer * peer, TPacketGuildChangeMemberData* p);
	void		GuildDisband(CPeer * peer, TPacketGuild * p);
	void		GuildWar(CPeer * peer, TPacketGuildWar * p);
	void		GuildWarScore(CPeer * peer, TPacketGuildWarScore * p);
	void		GuildChangeLadderPoint(TPacketGuildLadderPoint* p);
	void		GuildUseSkill(TPacketGuildUseSkill* p);
	void		GuildDepositMoney(TPacketGDGuildMoney* p);
	void		GuildWithdrawMoney(CPeer* peer, TPacketGDGuildMoney* p);
	void		GuildWithdrawMoneyGiveReply(TPacketGDGuildMoneyWithdrawGiveReply* p);
	void		GuildWarBet(TPacketGDGuildWarBet * p);
	void		GuildChangeMaster(TPacketChangeGuildMaster* p);

	void		SetGuildWarEndTime(DWORD guild_id1, DWORD guild_id2, time_t tEndTime);

	void		QUERY_BOOT(CPeer * peer, TPacketGDBoot * p);

	void		QUERY_LOGIN(CPeer * peer, DWORD dwHandle, SLoginPacket* data);
	void		QUERY_LOGOUT(CPeer * peer, DWORD dwHandle, const char *);

	void		RESULT_LOGIN(CPeer * peer, SQLMsg *msg);

	void		QUERY_PLAYER_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket*);
	void		RESULT_COMPOSITE_PLAYER(CPeer * peer, SQLMsg * pMsg, DWORD dwQID);
	void		RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo);
	void		RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
	void		RESULT_QUEST_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
	// @fixme402 (RESULT_AFFECT_LOAD +dwRealPID)
	void		RESULT_AFFECT_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwRealPID);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	void		QUERY_SKILL_COLOR_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket * packet);
	void		RESULT_SKILL_COLOR_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle);
#endif

	// PLAYER_INDEX_CREATE_BUG_FIX
	void		RESULT_PLAYER_INDEX_CREATE(CPeer *pkPeer, SQLMsg *msg);
	// END_PLAYER_INDEX_CREATE_BUG_FIX

	// MYSHOP_PRICE_LIST

	void		RESULT_PRICELIST_LOAD(CPeer* peer, SQLMsg* pMsg);

	void		RESULT_PRICELIST_LOAD_FOR_UPDATE(SQLMsg* pMsg);
	// END_OF_MYSHOP_PRICE_LIST

	void		QUERY_PLAYER_SAVE(CPeer * peer, DWORD dwHandle, TPlayerTable*);

	void		__QUERY_PLAYER_CREATE(CPeer * peer, DWORD dwHandle, TPlayerCreatePacket *);
	void		__QUERY_PLAYER_DELETE(CPeer * peer, DWORD dwHandle, TPlayerDeletePacket *);
	void		__RESULT_PLAYER_DELETE(CPeer * peer, SQLMsg* msg);

	void		QUERY_PLAYER_COUNT(CPeer * pkPeer, TPlayerCountPacket *);
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	void		QUERY_SKILL_COLOR_SAVE(const char * c_pData);
#endif

	void		QUERY_ITEM_SAVE(CPeer * pkPeer, const char * c_pData);
	void		QUERY_ITEM_DESTROY(CPeer * pkPeer, const char * c_pData);
	void		QUERY_ITEM_FLUSH(CPeer * pkPeer, const char * c_pData);

	void		QUERY_QUEST_SAVE(CPeer * pkPeer, TQuestTable *, DWORD dwLen);
	void		QUERY_ADD_AFFECT(CPeer * pkPeer, TPacketGDAddAffect * p);
	void		QUERY_REMOVE_AFFECT(CPeer * pkPeer, TPacketGDRemoveAffect * p);

	void		QUERY_SAFEBOX_LOAD(CPeer * pkPeer, DWORD dwHandle, TSafeboxLoadPacket *, bool bMall);
	void		QUERY_SAFEBOX_SAVE(CPeer * pkPeer, TSafeboxTable * pTable);
	void		QUERY_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangeSizePacket * p);
	void		QUERY_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangePasswordPacket * p);

	void		RESULT_SAFEBOX_LOAD(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(CPeer * pkPeer, SQLMsg * msg);

	void		QUERY_EMPIRE_SELECT(CPeer * pkPeer, DWORD dwHandle, TEmpireSelectPacket * p);
	void		QUERY_SETUP(CPeer * pkPeer, DWORD dwHandle, const char * c_pData);

	void		SendPartyOnSetup(CPeer * peer);

	void		QUERY_HIGHSCORE_REGISTER(CPeer * peer, TPacketGDHighscore* data);
	void		RESULT_HIGHSCORE_REGISTER(CPeer * pkPeer, SQLMsg * msg);

	void		QUERY_FLUSH_CACHE(CPeer * pkPeer, const char * c_pData);

	void		QUERY_PARTY_CREATE(CPeer * peer, TPacketPartyCreate* p);
	void		QUERY_PARTY_DELETE(CPeer * peer, TPacketPartyDelete* p);
	void		QUERY_PARTY_ADD(CPeer * peer, TPacketPartyAdd* p);
	void		QUERY_PARTY_REMOVE(CPeer * peer, TPacketPartyRemove* p);
	void		QUERY_PARTY_STATE_CHANGE(CPeer * peer, TPacketPartyStateChange* p);
	void		QUERY_PARTY_SET_MEMBER_LEVEL(CPeer * peer, TPacketPartySetMemberLevel* p);

	void		QUERY_RELOAD_PROTO();

	void		QUERY_CHANGE_NAME(CPeer * peer, DWORD dwHandle, TPacketGDChangeName * p);
	void		GetPlayerFromRes(TPlayerTable * player_table, MYSQL_RES* res);

	void		QUERY_LOGIN_KEY(CPeer * pkPeer, TPacketGDLoginKey * p);

	void		QUERY_ACCOUNT_SET_CASH(CPeer *peer, DWORD dwHandle, TPacketGDSetCash *data);
	void		QUERY_ACCOUNT_SET_COINS(CPeer *peer, DWORD dwHandle, TPacketGDSetCoins *data);

	void		AddGuildPriv(TPacketGiveGuildPriv* p);
	void		AddEmpirePriv(TPacketGiveEmpirePriv* p);
	void		AddCharacterPriv(TPacketGiveCharacterPriv* p);

	void		MoneyLog(TPacketMoneyLog* p);

	void		QUERY_AUTH_LOGIN(CPeer * pkPeer, DWORD dwHandle, TPacketGDAuthLogin * p);

	void		QUERY_LOGIN_BY_KEY(CPeer * pkPeer, DWORD dwHandle, TPacketGDLoginByKey * p);
	void		RESULT_LOGIN_BY_KEY(CPeer * peer, SQLMsg * msg);

	void		ChargeCash(const TRequestChargeCash * p);

	void		LoadEventFlag();
	void		SetEventFlag(TPacketSetEventFlag* p);
	void		SendEventFlagsOnSetup(CPeer* peer);

	void		BillingExpire(TPacketBillingExpire * p);
	void		BillingCheck(const char * data);

	void		SendAllLoginToBilling();
	void		SendLoginToBilling(CLoginData * pkLD, bool bLogin);

	void		MarriageAdd(TPacketMarriageAdd * p);
	void		MarriageUpdate(TPacketMarriageUpdate * p);
	void		MarriageRemove(TPacketMarriageRemove * p);

	void		WeddingRequest(TPacketWeddingRequest * p);
	void		WeddingReady(TPacketWeddingReady * p);
	void		WeddingEnd(TPacketWeddingEnd * p);

#if defined(ENABLE_OFFLINE_MESSAGE)
	void		RequestReadOfflineMessages(CPeer* pkPeer, DWORD dwHandle, TPacketGDReadOfflineMessage* p);
	void		SendOfflineMessage(TPacketGDSendOfflineMessage* p);
	void		OfflineMessageGarbage();
#endif

	// MYSHOP_PRICE_LIST
	void		MyshopPricelistUpdate(const TItemPriceListTable* pPacket); // @fixme403 (TPacketMyshopPricelistHeader to TItemPriceListTable)
	void		MyshopPricelistRequest(CPeer* peer, DWORD dwHandle, DWORD dwPlayerID);
	// END_OF_MYSHOP_PRICE_LIST

	// Building
	void		CreateObject(TPacketGDCreateObject * p);
	void		DeleteObject(DWORD dwID);
	void		UpdateLand(DWORD * pdw);

	// BLOCK_CHAT
	void		BlockChat(TPacketBlockChat * p);
	// END_OF_BLOCK_CHAT

#ifdef ENABLE_BATTLE_PASS
	void RESULT_BATTLE_PASS_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwRealPID);
	void QUERY_SAVE_BATTLE_PASS(CPeer * peer, DWORD dwHandle, TPlayerBattlePassMission* battlePass);
	void QUERY_REGISTER_RANKING(CPeer * peer, DWORD dwHandle, TBattlePassRegisterRanking* pRanking);

	bool LoadBattlePassRanking();
	void RequestLoadBattlePassRanking(CPeer * peer, DWORD dwHandle, const char* data);
#endif

    private:
	int					m_looping;
	socket_t				m_fdAccept;
	TPeerList				m_peerList;

	CPeer *					m_pkAuthPeer;

	// LoginKey, LoginData pair
	typedef boost::unordered_map<DWORD, CLoginData *> TLoginDataByLoginKey;
	TLoginDataByLoginKey			m_map_pkLoginData;

	// Login LoginData pair
	typedef boost::unordered_map<std::string, CLoginData *> TLoginDataByLogin;
	TLoginDataByLogin			m_map_pkLoginDataByLogin;

	// AccountID LoginData pair
	typedef boost::unordered_map<DWORD, CLoginData *> TLoginDataByAID;
	TLoginDataByAID				m_map_pkLoginDataByAID;

	typedef boost::unordered_map<std::string, CLoginData *> TLogonAccountMap;
	TLogonAccountMap			m_map_kLogonAccount;

	int					m_iPlayerIDStart;
	int					m_iPlayerDeleteLevelLimit;
	int					m_iPlayerDeleteLevelLimitLower;
	bool					m_bChinaEventServer;

	std::vector<TMobTable>			m_vec_mobTable;
	std::vector<TItemTable>			m_vec_itemTable;
	std::map<DWORD, TItemTable *>		m_map_itemTableByVnum;

	int					m_iShopTableSize;
	TShopTable *				m_pShopTable;

#if defined(ENABLE_RENEWAL_SHOPEX)
	int					m_iShopEXTableSize;
	TShopTable *				m_pShopEXTable;
#endif

	int					m_iRefineTableSize;
	TRefineTable*				m_pRefineTable;

#ifdef __DAILY_QUESTS__
	int32_t							m_iDailyQuestTableSize;
	TDailyQuestTable*				m_pDailyQuestTable;
	int32_t							m_iDailyQuestRewardTableSize;
	TDailyQuestRewardTable*			m_pDailyQuestRewardTable;
#endif

	std::vector<TSkillTable>		m_vec_skillTable;
	std::vector<TBanwordTable>		m_vec_banwordTable;
	std::vector<TItemAttrTable>		m_vec_itemAttrTable;
	std::vector<TItemAttrTable>		m_vec_itemRareTable;

	std::vector<building::TLand>		m_vec_kLandTable;
	std::vector<building::TObjectProto>	m_vec_kObjectProto;
	std::map<DWORD, building::TObject *>	m_map_pkObjectTable;
#ifdef ENABLE_EVENT_MANAGER
	std::vector<TEventTable> m_vec_eventTable;
#endif
#ifdef ENABLE_GEM_SYSTEM
	int m_iGemShopTableSize;
	TGemShopTable *m_pGemShopTable;
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	std::vector<TBiologMissionsProto> m_vec_BiologMissions;
	std::vector<TBiologRewardsProto> m_vec_BiologRewards;
	std::vector<TBiologMonstersProto> m_vec_BiologMonsters;
#endif

	bool					m_bShutdowned;

	TPlayerTableCacheMap			m_map_playerCache;

	TItemCacheMap				m_map_itemCache;
	TItemCacheSetPtrMap			m_map_pkItemCacheSetPtr;

	// MYSHOP_PRICE_LIST

	TItemPriceListCacheMap m_mapItemPriceListCache;
	// END_OF_MYSHOP_PRICE_LIST
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	TSkillColorCacheMap m_map_SkillColorCache;
#endif

	TChannelStatusMap m_mChannelStatus;

#ifdef ENABLE_BATTLE_PASS
	std::vector<TBattlePassRanking*> m_vec_battlePassRanking;
#endif

	struct TPartyInfo
	{
	    BYTE bRole;
	    int bLevel;

		TPartyInfo() :bRole(0), bLevel(0)
		{
		}
	};

	typedef std::map<DWORD, TPartyInfo>	TPartyMember;
	typedef std::map<DWORD, TPartyMember>	TPartyMap;
	typedef std::map<BYTE, TPartyMap>	TPartyChannelMap;
	TPartyChannelMap m_map_pkChannelParty;

	typedef std::map<std::string, long>	TEventFlagMap;
	TEventFlagMap m_map_lEventFlag;

#if defined(ENABLE_OFFLINE_MESSAGE)
	struct SOfflineMessage
	{
		std::string From;
		std::string Message;
		std::chrono::system_clock::time_point t;
		SOfflineMessage(const char* szFrom, const char* szMessage)
			: From(szFrom), Message(szMessage), t(std::chrono::system_clock::now()) {}
	};
	std::unordered_map<std::string, std::vector<std::shared_ptr<SOfflineMessage>>> m_OfflineMessage;
#endif

	BYTE					m_bLastHeader;
	int					m_iCacheFlushCount;
	int					m_iCacheFlushCountLimit;

    private :
	TItemIDRangeTable m_itemRange;

    public :
	bool InitializeNowItemID();
	DWORD GetItemID();
	DWORD GainItemID();
	TItemIDRangeTable GetItemRange() { return m_itemRange; }

	//BOOT_LOCALIZATION
    public:

	bool InitializeLocalization();

    private:
	std::vector<tLocale> m_vec_Locale;
	//END_BOOT_LOCALIZATION
	//ADMIN_MANAGER

	bool __GetAdminInfo(const char *szIP, std::vector<tAdminInfo> & rAdminVec);
	bool __GetHostInfo(std::vector<std::string> & rIPVec);
	bool __GetAdminConfig(DWORD adwAdminConfig[GM_DISABLE]);
	//END_ADMIN_MANAGER

	//RELOAD_ADMIN
	void ReloadAdmin(CPeer * peer, TPacketReloadAdmin * p);
	//END_RELOAD_ADMIN
	void BreakMarriage(CPeer * peer, const char * data);

	struct TLogoutPlayer
	{
	    DWORD	pid;
	    time_t	time;

	    bool operator < (const TLogoutPlayer & r)
	    {
		return (pid < r.pid);
	    }
	};

	typedef boost::unordered_map<DWORD, TLogoutPlayer*> TLogoutPlayerMap;
	TLogoutPlayerMap m_map_logout;

	void InsertLogoutPlayer(DWORD pid);
	void DeleteLogoutPlayer(DWORD pid);
	void UpdateLogoutPlayer();
	void UpdateItemCacheSet(DWORD pid);

	void FlushPlayerCacheSet(DWORD pid);

	//MONARCH
	void Election(CPeer * peer, DWORD dwHandle, const char * p);
	void Candidacy(CPeer * peer, DWORD dwHandle, const char * p);
	void AddMonarchMoney(CPeer * peer, DWORD dwHandle, const char * p);
	void TakeMonarchMoney(CPeer * peer, DWORD dwHandle, const char * p);
	void ComeToVote(CPeer * peer, DWORD dwHandle, const char * p);
	void RMCandidacy(CPeer * peer, DWORD dwHandle, const char * p);
	void SetMonarch(CPeer * peer, DWORD dwHandle, const char * p);
	void RMMonarch(CPeer * peer, DWORD dwHandle, const char * p);

	void DecMonarchMoney(CPeer * peer, DWORD dwHandle, const char * p);
	//END_MONARCH

	void ChangeMonarchLord(CPeer* peer, DWORD dwHandle, TPacketChangeMonarchLord* info);

	void SendSpareItemIDRange(CPeer* peer);

	void UpdateHorseName(TPacketUpdateHorseName* data, CPeer* peer);
	void AckHorseName(DWORD dwPID, CPeer* peer);
	void DeleteLoginKey(TPacketDC *data);
	void ResetLastPlayerID(const TPacketNeedLoginLogInfo* data);
	//delete gift notify icon
	void DeleteAwardId(TPacketDeleteAwardID* data);
	void UpdateChannelStatus(TChannelStatus* pData);
	void RequestChannelStatus(CPeer* peer, DWORD dwHandle);

#ifdef ENABLE_EVENT_MANAGER
	void UpdateEventStatus(DWORD dwID);
	void EventNotification(TPacketSetEventFlag *p);
#endif

#ifdef ENABLE_RANK_PLAYER
	void QUERY_ADD_RANKING(CPeer * peer, TPacketGDAddRanking * p, DWORD dwLen);
#endif
#ifdef __RANKING_SYSTEM__
	void QUERY_ADD_RANKING(CPeer * peer, TPacketGDAddRanking * p, DWORD dwLen);
#endif

#ifdef ENABLE_PROTO_FROM_DB
	public:
	bool		InitializeMobTableFromDB();
	bool		InitializeItemTableFromDB();
	protected:
	bool		bIsProtoReadFromDB;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
public:
	void				RESULT_PRIVATE_SHOP_LOAD(CPeer* pPeer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID);
	void				RESULT_PRIVATE_SHOP_ITEM_LOAD(CPeer* pPeer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID);
	void				RESULT_PRIVATE_SHOP_SALE_LOAD(CPeer* pPeer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID);

	CPeer* GetPrivateShopPeer(BYTE bChannel, WORD wListenPort);
	TItemTable* GetItemTable(DWORD dwVnum);

	// Private Shop Cache
	CPrivateShopCache* GetPrivateShopCache(DWORD dwPID);
	void				PutPrivateShopCache(TPrivateShop* pCache);
	bool				DeletePrivateShopCache(DWORD dwPID);
	void				UpdatePrivateShopCache();
	void				FlushPrivateShopCache(DWORD dwPID);

	// Item Cache
	void						CreatePrivateShopItemCacheSet(DWORD dwPID);
	TPrivateShopItemCacheSet* GetPrivateShopItemCacheSet(DWORD dwPID);
	void						FlushPrivateShopItemCacheSet(DWORD dwPID);
	bool						DeletePrivateShopItemCacheSet(DWORD dwPID);

	CPrivateShopItemCache* GetPrivateShopItemCache(DWORD dwID);
	void						PutPrivateShopItemCache(TPlayerPrivateShopItem* pNew, bool bSkipQuery = false);
	bool						DeletePrivateShopItemCache(DWORD dwID);

	void						UpdatePrivateShopItemCache();
	void						UpdatePrivateShopItemCacheSet(DWORD dwPID);

	// Sale History Cache
	void						CreatePrivateShopSaleCacheSet(DWORD dwPID);
	TPrivateShopSaleCacheSet* GetPrivateShopSaleCacheSet(DWORD dwPID);
	void						FlushPrivateShopSaleCacheSet(DWORD dwPID);
	bool						DeletePrivateShopSaleCacheSet(DWORD dwPID);

	CPrivateShopSaleCache* GetPrivateShopSaleCache(DWORD dwID);
	void						PutPrivateShopSaleCache(TPrivateShopSale* pNew, bool bSkipQuery = false);
	bool						DeletePrivateShopSaleCache(DWORD dwID);

	void						UpdatePrivateShopSaleCache();
	void						UpdatePrivateShopSaleCacheSet(DWORD dwPID);

	// Sales
	void						AddMarketItemPrice(TPrivateShopSale& rSale);
	TItemPrice* GetMarketItemPrice(DWORD dwVnum);
	void						UpdateMarketItemPrice();


	// Database Entity
	LPPRIVATE_SHOP		CreatePrivateShop(DWORD dwPID);
	bool				DeletePrivateShop(DWORD dwPID);
	LPPRIVATE_SHOP		GetPrivateShop(DWORD dwPID);

	// SQL Data Processing
	bool				InitializePrivateShopMarketItemPrice();

	// Packet Processing
	void				ProcessPrivateShopPacket(CPeer* pPeer, DWORD dwHandle, const char* c_szData);

	LPPRIVATE_SHOP		PrivateShopSpawn(DWORD dwShopID);
	LPPRIVATE_SHOP		PrivateShopCreate(TPrivateShop* pTable, const std::vector<TPlayerPrivateShopItem>& c_vec_shopItem);
	void				PrivateShopBuild(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopClose(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopDelete(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopDespawn(CPeer* pPeer, DWORD dwHandle, const char* c_szData);

	void				PrivateShopWithdrawRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopModifyRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopBuyRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemPriceChangeRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemMoveRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemCheckinRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemCheckoutRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopTitleChangeRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopWarpRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopSlotUnlockRequest(CPeer* pPeer, DWORD dwHandle, const char* c_szData);

	void				PrivateShopItemCheckinUpdate(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopItemCheckoutUpdate(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopWithdraw(const char* c_szData);
	void				PrivateShopBuy(CPeer* pPeer, DWORD dwHandle, const char* c_szData);
	void				PrivateShopFailedBuy(const char* c_szData);
	void				PrivateShopItemTransfer(TPlayerItem* pTItem);
	void				PrivateShopItemDelete(const char* c_szData);
	void				PrivateShopItemExpire(const char* c_szData);
	void				PrivateShopPremiumTimeUpdate(const char* c_szData);

	void				PrivateShopStartPremiumEvent(DWORD dwPID);
	void				PrivateShopEndPremiumEvent(DWORD dwPID);
	void				UpdatePrivateShopPremiumEvent();
	bool				IsPrivateShopPremiumEvent(DWORD dwPID);

	void				PrivateShopDestroy(LPPRIVATE_SHOP pPrivateShop);
	void				PrivateShopGameDespawn(LPPRIVATE_SHOP pPrivateShop);
	void				PrivateShopGameSpawn(LPPRIVATE_SHOP pPrivateShop);

	bool				PrivateShopFetchData(DWORD dwShopID, TPrivateShop& rTable, std::vector<TPlayerPrivateShopItem>& c_vec_shopItem);

	void				PrivateShopPeerSpawn(CPeer* pPeer);
private:
	TPrivateShopCacheMap					m_map_privateShopCache;
	TPrivateShopMap							m_map_privateShop;

	TPrivateShopItemCacheMap				m_map_privateShopItemCache;
	TPrivateShopItemCacheSetPtrMap			m_map_pPrivateShopItemCacheSetPtr;

	TPrivateShopSaleCacheMap				m_map_privateShopSaleCache;
	TPrivateShopSaleCacheSetPtrMap			m_map_pPrivateShopSaleCacheSetPtr;

	TMarketItemPriceMap						m_map_marketItemPrice;
	TMarketItemPriceListMap					m_map_marketItemPriceList;

	TPrivateShopPtrList						m_list_privateShopPremium;
	std::vector<TItemTable*>				m_vec_itemVnumRange;
#endif

#ifdef ENABLE_ITEMSHOP
protected:
	bool InitializeItemshopCategoryTable();
	bool InitializeItemshopItemTable();
	bool InitializeItemshopSpecialOffers();
	bool InitializePromotionCodes();
	bool InitializeRedeemedPromotionCodes();
	void ItemshopCheckBuy(CPeer* pkPeer, DWORD dwHandle, TItemshopCheckBuy* p);
	void RemoveSingleItemshopItem(long long llItemIndex);
	void AddSingleItemshopItem(TItemshopItemTable* item, long long llItemIndex);
	void RedeemPromotionCode(CPeer* pkPeer, DWORD dwHandle, TPromotionRedeem* p);
	void UpdatePromotionTables();
	void UpdateItemshopTable();
private:
	void QUERY_RELOAD_ITEMSHOP();
	int m_iItemshopTableCategorySize;
	TItemshopCategoryTable* m_pItemshopTableCategories;
	int m_iItemshopTableItemSize;
	TItemshopItemTable* m_pItemshopTableItems;
	std::vector<unsigned long> m_vec_usedSeeds;
	std::unordered_map <std::string, std::pair<long long, TItemshopItemTable>> m_ItemshopItems;
	std::unordered_map <std::string, std::pair<DWORD, long long>> m_map_ItemshopLimitCountFlush;
	std::vector<TSpecialOfferItems>	m_vec_itemshopSpecialOfferItems;
	std::unordered_map<std::string, std::pair<long long, std::vector<TPromotionItemTable>>> m_map_promotionCodeInfo;
	std::unordered_map<DWORD, std::vector<std::string>> m_vec_redeemedPromotionCodes;
	std::unordered_map<DWORD, std::string> m_map_redeemedPromotionFlush;
#endif
public:
	void EraseItemCache(DWORD id);
	const TItemTable*	GetItemTable(DWORD dwVnum) const;
#ifdef ENABLE_EVENT_CALENDAR
public:
	bool InitializeEventManager(bool updateFromGameMaster = false);
	void RecvEventManagerPacket(const char* data);
	void UpdateEventManager();
	void SendEventData(CPeer* pkPeer = NULL, bool updateFromGameMaster = false);
protected:
	std::map<uint8_t, std::vector<TEventManagerData>> m_EventManager;
#endif
};

template<class Func>
Func CClientManager::for_each_peer(Func f)
{
    TPeerList::iterator it;
    for (it = m_peerList.begin(); it!=m_peerList.end();++it)
    {
	f(*it);
    }
    return f;
}
#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
