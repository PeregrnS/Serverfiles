#include "stdafx.h"
#include "constants.h"
#include "gm.h"
#include "locale_service.h"
#include "config.h"
#include "utils.h"

//ADMIN_MANAGER
std::set<std::string> g_set_Host;
std::map<std::string, tGM> g_map_GM;
DWORD g_adwAdminConfig[GM_DISABLE];

void gm_new_clear()
{
	g_set_Host.clear();
	g_map_GM.clear();
}

void initAdminConfig(DWORD* adwAdminConfig)
{
	memcpy(&g_adwAdminConfig[0], adwAdminConfig, sizeof(g_adwAdminConfig));
}

void gm_new_insert( const tAdminInfo &rAdminInfo )
{
	sys_log( 0, "InsertGMList(account:%s, player:%s, contact_ip:%s, server_ip:%s, auth:%d)",
			rAdminInfo.m_szAccount,
			rAdminInfo.m_szName,
			rAdminInfo.m_szContactIP,
			rAdminInfo.m_szServerIP,
			rAdminInfo.m_Authority);

	tGM t;

	if ( strlen( rAdminInfo.m_szContactIP ) == 0 )
	{
		t.pset_Host = &g_set_Host;
		sys_log(0, "GM Use ContactIP" );
	}
	else
	{
		t.pset_Host = NULL;
		sys_log(0, "GM Use Default Host List" );
	}

	memcpy ( &t.Info, &rAdminInfo, sizeof ( rAdminInfo ) );

	g_map_GM[rAdminInfo.m_szName] = t;
}

void gm_new_host_inert( const char * host )
{
	g_set_Host.emplace(host);
	sys_log( 0, "InsertGMHost(ip:%s)", host );
}

BYTE gm_new_get_level( const char * name, const char * host, const char* account)
{
	if ( test_server ) return GM_IMPLEMENTOR;

	std::map<std::string, tGM >::iterator it = g_map_GM.find(name);

	if (g_map_GM.end() == it)
		return GM_PLAYER;

	// GERMAN_GM_NOT_CHECK_HOST

#ifdef ENABLE_NEWSTUFF
	if (!g_bGMHostCheck)
#else
	if (true)
#endif
	{
	    if (account)
	    {
		if ( strcmp ( it->second.Info.m_szAccount, account  ) != 0 )
		{
		    sys_log(0, "GM_NEW_GET_LEVEL : BAD ACCOUNT [ACCOUNT:%s/%s", it->second.Info.m_szAccount, account);
		    return GM_PLAYER;
		}
	    }
	    sys_log(0, "GM_NEW_GET_LEVEL : FIND ACCOUNT");
	    return it->second.Info.m_Authority;
	}
	// END_OF_GERMAN_GM_NOT_CHECK_HOST
	else
	{
	    if ( host )
	    {
		if ( it->second.pset_Host )
		{
		    if ( it->second.pset_Host->end() == it->second.pset_Host->find( host ) )
		    {
			sys_log(0, "GM_NEW_GET_LEVEL : BAD HOST IN HOST_LIST");
			return GM_PLAYER;
		    }
		}
		else
		{
		    if ( strcmp ( it->second.Info.m_szContactIP, host  ) != 0 )
		    {
			sys_log(0, "GM_NEW_GET_LEVEL : BAD HOST IN GMLIST");
			return GM_PLAYER;
		    }
		}
	    }
	    sys_log(0, "GM_NEW_GET_LEVEL : FIND HOST");

	    return it->second.Info.m_Authority;
	}
	return GM_PLAYER;
}

//END_ADMIN_MANAGER
BYTE gm_get_level(const char * name, const char * host, const char* account)
{
	return gm_new_get_level( name, host, account );
}

bool check_allow(BYTE bGMLevel, DWORD dwCheckFlag)
{
	return IS_SET(g_adwAdminConfig[bGMLevel], dwCheckFlag);
}

bool check_account_allow(const std::string& stAccountName, DWORD dwCheckFlag)
{
	std::map<std::string, tGM >::iterator it = g_map_GM.begin();
	bool bHasGM = false;
	bool bCheck = false;

	while (it != g_map_GM.end() && !bCheck)
	{
		if (!stAccountName.compare(it->second.Info.m_szAccount))
		{
			bHasGM = true;
			BYTE bGMLevel = it->second.Info.m_Authority;
			bCheck = check_allow(bGMLevel, dwCheckFlag);
		}

		++it;
	}

	return !bHasGM || bCheck;
}

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
