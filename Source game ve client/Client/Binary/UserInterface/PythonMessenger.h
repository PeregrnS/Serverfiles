#pragma once

class CPythonMessenger : public CSingleton<CPythonMessenger>
{
	public:
		typedef std::set<std::string> TFriendNameMap;
		typedef std::map<std::string, BYTE> TGuildMemberStateMap;
#if defined(ENABLE_MESSENGER_GM)
		typedef std::set<std::string> TGMNameMap;
#endif

		enum EMessengerGroupIndex
		{
			MESSENGER_GRUOP_INDEX_FRIEND,
			MESSENGER_GRUOP_INDEX_GUILD,
#if defined(ENABLE_MESSENGER_GM)
			MESSENGER_GROUP_INDEX_GM,
#endif
		};

	public:
		CPythonMessenger();
		virtual ~CPythonMessenger();

		void Destroy();

		// Friend
		void RemoveFriend(const char * c_szKey);
		void OnFriendLogin(const char * c_szKey);
		void OnFriendLogout(const char * c_szKey);
		BOOL IsFriendByKey(const char * c_szKey);
		BOOL IsFriendByName(const char * c_szName);

#if defined(ENABLE_MESSENGER_GM)
		// GM
		void OnGMLogin(const char* c_szName);
		void OnGMLogout(const char* c_szName);
#endif

		// Guild
		void AppendGuildMember(const char * c_szName);
		void RemoveGuildMember(const char * c_szName);
		void RemoveAllGuildMember();
		void LoginGuildMember(const char * c_szName);
		void LogoutGuildMember(const char * c_szName);
		void RefreshGuildMember();

		void SetMessengerHandler(PyObject* poHandler);

	protected:
		TFriendNameMap m_FriendNameMap;
		TGuildMemberStateMap m_GuildMemberStateMap;
#if defined(ENABLE_MESSENGER_GM)
		TGMNameMap m_GMNameMap;
#endif

	private:
		PyObject * m_poMessengerHandler;
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
