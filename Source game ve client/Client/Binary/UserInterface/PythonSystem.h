#pragma once

class CPythonSystem : public CSingleton<CPythonSystem>
{
	public:
		enum EWindow
		{
			WINDOW_STATUS,
			WINDOW_INVENTORY,
			WINDOW_ABILITY,
			WINDOW_SOCIETY,
			WINDOW_JOURNAL,
			WINDOW_COMMAND,

			WINDOW_QUICK,
			WINDOW_GAUGE,
			WINDOW_MINIMAP,
			WINDOW_CHAT,

			WINDOW_MAX_NUM,
		};

		enum
		{
			FREQUENCY_MAX_NUM  = 30,
			RESOLUTION_MAX_NUM = 100
		};

		typedef struct SResolution
		{
			DWORD	width;
			DWORD	height;
			DWORD	bpp;		// bits per pixel (high-color = 16bpp, true-color = 32bpp)

			DWORD	frequency[20];
			BYTE	frequency_count;
		} TResolution;

		typedef struct SWindowStatus
		{
			int		isVisible;
			int		isMinimized;

			int		ixPosition;
			int		iyPosition;
			int		iHeight;
		} TWindowStatus;

		typedef struct SConfig
		{
			DWORD			width;
			DWORD			height;
			DWORD			bpp;
			DWORD			frequency;

			bool			is_software_cursor;
			bool			is_object_culling;
			int				iDistance;
			int				iShadowLevel;
#ifdef ENABLE_FOV_OPTION
			FLOAT            iFOVLevel;
#endif

			FLOAT			music_volume;
			BYTE			voice_volume;

			int				gamma;

			int				isSaveID;
			char			SaveID[20];

			bool			bWindowed;
			bool			bDecompressDDS;
			bool			bNoSoundCard;
			bool			bUseDefaultIME;
			BYTE			bSoftwareTiling;
			bool			bViewChat;
			// bool			bAlwaysShowName;
			int				bAlwaysShowName;
			bool			bShowDamage;
			bool			bShowRainBow;
			bool			bShowSalesText;
			bool			bShowTitleText;
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
			bool			bShowMobAIFlag;
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
			bool			bShowMobLevel;
#endif
#ifdef ENABLE_RENDER_TARGET_PREVIEW
			bool			bPreviewModel;
			bool			bIsRenderRotation;
#endif
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
			float			fPrivateShopViewDistance;
#endif
#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
			bool			bAnonymousCountryMode{};
			bool			bShowCountryFlag{};
			bool			bShowEmpireFlag{};
#endif
#ifdef ENABLE_NEW_GAMEOPTION
			bool			questLetter;
			bool			affectIcons;
#endif
#ifdef ENABLE_STONE_SCALE_OPTION
			float			m_fStoneScale;
#endif
#ifdef ENABLE_MAP_OBJECT_OPTIMIZATION
			float			fobjectDist;
#endif
		} TConfig;

#if defined(ENABLE_PICK_FILTER)
		class CPickUpFilter final
		{
		public:
			CPickUpFilter();
			~CPickUpFilter();

			void	SetFilter(size_t sIndex, bool b);
			void	SetSize(size_t sIndex, bool b);
			void	SetRefine(BYTE min, BYTE max);
			void	SetLevel(long min, long max);
			void	SetModeAll(bool b);

			bool	CanPickItem(DWORD dwIID);

			bool	GetFilter(size_t sIndex) const;
			bool	GetSize(size_t sIndex) const;
			bool	IsModeAll() const;

			std::pair<BYTE, BYTE> GetRefine();
			std::pair<long, long> GetLevel();

		private:
			bool	CheckRefine(const CItemData* pItem) const;
			bool	CheckLevel(const CItemData* pItem) const;
			bool	CheckSize(const CItemData* pItem) const;
			bool	CheckType(const CItemData* pItem) const;

			static constexpr const char* cPickUpFilterFileName = "pickupfilter.dat";

		public:
			enum EPICKFILTER
			{
				/*WEAPON-SUB*/
				SUB_WEAPON_SWORD,
				SUB_WEAPON_DAGGER,
				SUB_WEAPON_BOW,
				SUB_WEAPON_TWO_HANDED,
				SUB_WEAPON_BELL,
				SUB_WEAPON_FAN,
				SUB_WEAPON_ARROW,
				//SUB_WEAPON_MOUNT_SPEAR,
				/*WEAPON-SUB*/

				/*ARMOR-SUB*/
				SUB_ARMOR_BODY,
				SUB_ARMOR_HEAD,
				SUB_ARMOR_SHIELD,
				SUB_ARMOR_WRIST,
				SUB_ARMOR_FOOTS,
				SUB_ARMOR_NECK,
				SUB_ARMOR_EAR,
				/*ARMOR-SUB*/

				/*OTHER*/
				TYPE_METIN,
				TYPE_YANG,
				TYPE_SKILLBOOK,
				TYPE_GIFTBOX,
				TYPE_BELT,
				TYPE_POLY,
				TYPE_RING,
				SUB_POTION,
				TYPE_MATERIAL,
				/*OTHER*/

				EPICKFILTER_MAX
			};

			enum ESIZE
			{
				SMALL,
				MID,
				BIG,

				ESIZE_MAX
			};
			
		private:
			bool bPickFilter[EPICKFILTER::EPICKFILTER_MAX];
			bool bPickSize[ESIZE::ESIZE_MAX];

			bool bModeAll;
			
			BYTE m_bRefineMin;
			BYTE m_bRefineMax;
			
			long m_lLevelMin;
			long m_lLevelMax;
		} TPickUpFilter;
#endif

	public:
		CPythonSystem();
		virtual ~CPythonSystem();

		void Clear();
		void SetInterfaceHandler(PyObject * poHandler);
		void DestroyInterfaceHandler();

		// Config
		void							SetDefaultConfig();
		bool							LoadConfig();
		bool							SaveConfig();
		void							ApplyConfig();
		void							SetConfig(TConfig * set_config);
		TConfig *						GetConfig();
		void							ChangeSystem();

		// Interface
		bool							LoadInterfaceStatus();
		void							SaveInterfaceStatus();
		bool							isInterfaceConfig();
		const TWindowStatus &			GetWindowStatusReference(int iIndex);

		DWORD							GetWidth();
		DWORD							GetHeight();
		DWORD							GetBPP();
		DWORD							GetFrequency();
		bool							IsSoftwareCursor();
		bool							IsWindowed();
		bool							IsViewChat();
		// bool							IsAlwaysShowName();
		int								IsAlwaysShowName();
		bool							IsShowDamage();
		bool							IsShowRainBow();
		bool							IsShowSalesText();
		bool							IsShowTitleText();
		bool							IsUseDefaultIME();
		bool							IsNoSoundCard();
		bool							IsAutoTiling();
		bool							IsSoftwareTiling();
		void							SetSoftwareTiling(bool isEnable);
		void							SetViewChatFlag(int iFlag);
		void							SetAlwaysShowNameFlag(int iFlag);
		void							SetShowDamageFlag(int iFlag);
		void							SetShowRainBowFlag(int iFlag);
		void							SetShowSalesTextFlag(int iFlag);
		void							SetShowTitleTextFlag(int iFlag);

#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
		bool							IsShowMobAIFlag();
		void							SetShowMobAIFlagFlag(int iFlag);
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
		bool							IsShowMobLevel();
		void							SetShowMobLevelFlag(int iFlag);
#endif

#ifdef ENABLE_RENDER_TARGET_PREVIEW
		bool							IsPreviewModel();
		void							SetPreviewModel(int iFlag);

		bool							IsRenderRotation();
		void							SetRenderRotation(int iFlag);
#endif

		// Window
		void							SaveWindowStatus(int iIndex, int iVisible, int iMinimized, int ix, int iy, int iHeight);

		// SaveID
		int								IsSaveID();
		const char *					GetSaveID();
		void							SetSaveID(int iValue, const char * c_szSaveID);

		/// Display
		void							GetDisplaySettings();

		int								GetResolutionCount();
		int								GetFrequencyCount(int index);
		bool							GetResolution(int index, OUT DWORD *width, OUT DWORD *height, OUT DWORD *bpp);
		bool							GetFrequency(int index, int freq_index, OUT DWORD *frequncy);
		int								GetResolutionIndex(DWORD width, DWORD height, DWORD bpp);
		int								GetFrequencyIndex(int res_index, DWORD frequency);
		bool							isViewCulling();

		// Sound
		float							GetMusicVolume();
		int								GetSoundVolume();
		void							SetMusicVolume(float fVolume);
		void							SetSoundVolumef(float fVolume);

		int								GetDistance();
		int								GetShadowLevel();
		void							SetShadowLevel(unsigned int level);

#ifdef ENABLE_FOV_OPTION
		float                            GetFOVLevel();
		void                            SetFOVLevel(float fFOV);
#endif

#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
		void							SetAnonymousCountryMode(bool isEnable);
		bool							GetAnonymousCountryMode() const;

		void							SetShowCountryFlag(bool isEnable);
		bool							IsShowCountryFlag() const;

		void							SetShowEmpireFlag(bool isEnable);
		bool							IsShowEmpireFlag() const;

		void							AddChatFilterCountry(const std::string& country);
		void							RemoveChatFilterCountry(const std::string& country);
		bool							IsChatFilterCountry(const std::string& country) const;

		void							AddChatFilterEmpire(BYTE bEmpire);
		void							RemoveChatFilterEmpire(BYTE bEmpire);
		bool							IsChatFilterEmpire(BYTE bEmpire) const;

		void							LoadChatFilterSettings();
		void							SaveChatFilterSettings() const;
#endif

#ifdef ENABLE_NEW_GAMEOPTION
		void							SetQuestLetter(bool flag) { m_Config.questLetter = flag; }
		bool							GetQuestLetter() { return m_Config.questLetter; }

		void							SetAffectIcons(bool flag) { m_Config.affectIcons = flag; }
		bool							GetAffectIcons() { return m_Config.affectIcons; }
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
		void							SetStoneScale(float fScale);
		float							GetStoneScale();
#endif

#ifdef ENABLE_MAP_OBJECT_OPTIMIZATION
		float							GetObjectDistance() { return m_Config.fobjectDist; }
		void							SetObjectDistance(float fDist);
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
	public:
		float							GetPrivateShopViewDistance() { return m_Config.fPrivateShopViewDistance; }
		void							SetPrivateShopViewDistance(float fDistance) { m_Config.fPrivateShopViewDistance = fDistance; }
#endif

	protected:
		TResolution						m_ResolutionList[RESOLUTION_MAX_NUM];
		int								m_ResolutionCount;

		TConfig							m_Config;
		TConfig							m_OldConfig;

		bool							m_isInterfaceConfig;
		PyObject *						m_poInterfaceHandler;
		TWindowStatus					m_WindowStatus[WINDOW_MAX_NUM];

#if defined(__BL_MULTI_LANGUAGE_ULTIMATE__)
		std::set<std::string>			m_setFilterCountry;
		DWORD							m_dwFilterEmpireFlag;
#endif

#ifdef ENABLE_HWID_BAN
	public:
		uint32_t						getVolumeHash();
		const char*						getCpuInfos();
		const char*						getMachineName();
		const char*						getBiosDate();
		const char*						getMainboardName();
		const char*						getGPUName();
		const char*						GetHWID();
		const char*						generateHash(const std::string& bytes);
#endif
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
