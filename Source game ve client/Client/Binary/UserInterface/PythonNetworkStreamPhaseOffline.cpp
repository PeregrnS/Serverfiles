#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonApplication.h"
#include "Packet.h"

void CPythonNetworkStream::OffLinePhase()
{
	TPacketHeader header;

	if (!CheckPacket(&header))
		return;

#if defined(_DEBUG) && defined(ENABLE_PRINT_RECV_PACKET_DEBUG)
	Tracenf("RECV HEADER : %u , phase %s ", header, m_strPhase.c_str());
#endif

	switch (header)
	{
		case HEADER_GC_PHASE:
			if (RecvPhasePacket())
				return;
			break;
	}

	RecvErrorPacket(header);
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
