#ifndef __INC_MESSENGER_MANAGER_H
#define __INC_MESSENGER_MANAGER_H

#include "db.h"

#if defined(__MESSENGER_GM__)
typedef std::string keyG;
typedef std::set<std::string> KeyGSet;
typedef std::map<std::string, KeyGSet> KeyGRelation;
#endif

class MessengerManager : public singleton<MessengerManager>
{
	public:
		typedef std::string keyT;
		typedef const std::string & keyA;

		MessengerManager();
		virtual ~MessengerManager();

	public:
		void	P2PLogin(keyA account);
		void	P2PLogout(keyA account);

		void	Login(keyA account);
		void	Logout(keyA account);

		void	RequestToAdd(LPCHARACTER ch, LPCHARACTER target);
#ifdef CROSS_CHANNEL_FRIEND_REQUEST
		void	RegisterRequestToAdd(const char* szAccount, const char* szTarget);
		void	P2PRequestToAdd_Stage1(LPCHARACTER ch, const char* targetName);
		void	P2PRequestToAdd_Stage2(const char* characterName, LPCHARACTER target);
#endif
		bool	AuthToAdd(keyA account, keyA companion, bool bDeny); // @fixme130 void -> bool

		void	__AddToList(keyA account, keyA companion);
		void	AddToList(keyA account, keyA companion);

		void	__RemoveFromList(keyA account, keyA companion);
		void	RemoveFromList(keyA account, keyA companion);

		void	RemoveAllList(keyA account);

		void	Initialize();

	private:
		void	SendList(keyA account);
		void	SendLogin(keyA account, keyA companion);
		void	SendLogout(keyA account, keyA companion);

		void	LoadList(SQLMsg * pmsg);

#if defined(__MESSENGER_GM__)
		void SendGMList(const std::string& account);
		void SendGMLogin(const std::string& account, const std::string& companion);
		void SendGMLogout(const std::string& account, const std::string& companion);

		void LoadGMList(SQLMsg* pmsg);
#endif

		void	Destroy();

		std::set<keyT>			m_set_loginAccount;
		std::map<keyT, std::set<keyT> >	m_Relation;
		std::map<keyT, std::set<keyT> >	m_InverseRelation;
		std::set<DWORD>			m_set_requestToAdd;

#if defined(__MESSENGER_GM__)
		KeyGRelation m_GMRelation;
		KeyGRelation m_InverseGMRelation;
#endif

};

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
