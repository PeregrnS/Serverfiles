#pragma once

#include "GrpVertexBuffer.h"

class CDynamicVertexBuffer : public CGraphicVertexBuffer
{
	public:
		CDynamicVertexBuffer();
		virtual ~CDynamicVertexBuffer();

		bool Create(int vtxCount, int fvf);

	protected:
		int m_vtxCount;
		int m_fvf;
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
