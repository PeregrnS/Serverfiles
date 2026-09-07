#ifndef __ITEM_ADDON_H
#define __ITEM_ADDON_H

class CItemAddonManager : public singleton<CItemAddonManager>
{
	public:
		CItemAddonManager();
		virtual ~CItemAddonManager();

		void ApplyAddonTo(int iAddonType, LPITEM pItem);
};

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
