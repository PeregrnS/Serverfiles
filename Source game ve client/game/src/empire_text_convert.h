#ifndef __INC_EMPIRE_CHAT_CONVERT
#define __INC_EMPIRE_CHAT_CONVERT

bool LoadEmpireTextConvertTable(DWORD dwEmpireID, const char* c_szFileName);
void ConvertEmpireText(DWORD dwEmpireID, char* szText, size_t len, int iPct);

#endif
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
