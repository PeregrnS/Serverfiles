#include "StdAfx.h"
#include "NetDevice.h"

CNetworkDevice::CNetworkDevice()
{
	Initialize();
}

CNetworkDevice::~CNetworkDevice()
{
	Destroy();
}

void CNetworkDevice::Initialize()
{
	m_isWSA=false;
}

void CNetworkDevice::Destroy()
{
	if (m_isWSA)
	{
		WSACleanup();
		m_isWSA=false;
	}
}

bool CNetworkDevice::Create()
{
	Destroy();

	Initialize();

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData)!=0)
		return false;

	m_isWSA=true;

	return true;
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
