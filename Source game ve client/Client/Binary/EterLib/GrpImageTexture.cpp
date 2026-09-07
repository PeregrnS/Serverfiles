#include "StdAfx.h"
#include "../eterBase/MappedFile.h"
#include "../eterPack/EterPackManager.h"
#include "GrpImageTexture.h"

bool CGraphicImageTexture::Lock(int* pRetPitch, void** ppRetPixels, int level)
{
	D3DLOCKED_RECT lockedRect;
	if (FAILED(m_lpd3dTexture->LockRect(level, &lockedRect, NULL, 0)))
		return false;

	*pRetPitch = lockedRect.Pitch;
	*ppRetPixels = (void*)lockedRect.pBits;
	return true;
}

void CGraphicImageTexture::Unlock(int level)
{
	assert(m_lpd3dTexture != NULL);
	m_lpd3dTexture->UnlockRect(level);
}

void CGraphicImageTexture::Initialize()
{
	CGraphicTexture::Initialize();

	m_stFileName = "";

	m_d3dFmt=D3DFMT_UNKNOWN;
	m_dwFilter=0;
}

void CGraphicImageTexture::Destroy()
{
	CGraphicTexture::Destroy();

	Initialize();
}

bool CGraphicImageTexture::CreateDeviceObjects()
{
	assert(ms_lpd3dDevice != NULL);
	assert(m_lpd3dTexture == NULL);

	if (m_stFileName.empty())
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_width, m_height, 1, D3DUSAGE_DYNAMIC, m_d3dFmt, D3DPOOL_DEFAULT, &m_lpd3dTexture, NULL)))
			return false;
	}
	else
	{
		CMappedFile	mappedFile;
		LPCVOID		c_pvMap;

		if (!CEterPackManager::Instance().Get(mappedFile, m_stFileName.c_str(), &c_pvMap))
			return false;

		//@fixme002
		if (!CreateFromMemoryFile(mappedFile.Size(), c_pvMap, m_d3dFmt, m_dwFilter))
		{
			TraceError("CGraphicImageTexture::CreateDeviceObjects: CreateFromMemoryFile: texture not found(%s)", m_stFileName.c_str());
			return false;
		}
		return true;
		// return CreateFromMemoryFile(mappedFile.Size(), c_pvMap, m_d3dFmt, m_dwFilter);
	}

	m_bEmpty = false;
	return true;
}

bool CGraphicImageTexture::Create(UINT width, UINT height, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != NULL);
	Destroy();

	m_width = width;
	m_height = height;
	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;

	return CreateDeviceObjects();
}

void CGraphicImageTexture::CreateFromTexturePointer(const CGraphicTexture * c_pSrcTexture)
{
	if (m_lpd3dTexture)
		m_lpd3dTexture->Release();

	m_width = c_pSrcTexture->GetWidth();
	m_height = c_pSrcTexture->GetHeight();
	m_lpd3dTexture = c_pSrcTexture->GetD3DTexture();

	if (m_lpd3dTexture)
		m_lpd3dTexture->AddRef();

	m_bEmpty = false;
}

bool CGraphicImageTexture::ConvertTexture(IDirect3DTexture9* pkTexSrc, IDirect3DTexture9* pkTexDst, int mipMapCount)
{
	HRESULT hr;
	bool failed = false;
	IDirect3DSurface9* ppsSrc = NULL;
	IDirect3DSurface9* ppsDst = NULL;

	for (int i = 0; i < mipMapCount; ++i) {
		hr = pkTexSrc->GetSurfaceLevel(i, &ppsSrc);
		if (FAILED(hr)) {
			TraceError("CreateDDSTexture: ms_bSupportDXT = false GetSurfaceLevel(ppsSrc) failed (file: %s, mipLevel: %d) Error: ", m_stFileName.c_str(), i);
			failed = true;
			break;
		}

		hr = pkTexDst->GetSurfaceLevel(i, &ppsDst);
		if (FAILED(hr)) {
			TraceError("CreateDDSTexture: ms_bSupportDXT = false GetSurfaceLevel(ppsDst) failed (file: %s, mipLevel: %d) Error: ", m_stFileName.c_str(), i );
			failed = true;
			break;
		}

		hr = D3DXLoadSurfaceFromSurface(ppsDst, NULL, NULL, ppsSrc, NULL, NULL, D3DX_FILTER_NONE, 0);
		if (FAILED(hr)) {
			TraceError("CreateDDSTexture: ms_bSupportDXT = false D3DXLoadSurfaceFromSurface failed (file: %s, mipLevel: %d) Error: ", m_stFileName.c_str(), i);
			failed = true;
			break;
		}

		ppsDst->Release();
		ppsSrc->Release();
	}

	if (failed) {
		if (ppsSrc)
			ppsSrc->Release();

		if (ppsDst)
			ppsDst->Release();

		return false;
	}

	return true;
}


bool CGraphicImageTexture::CreateDDSTexture(CDXTCImage& image, const BYTE* /*c_pbBuf*/)
{
	int mipmapCount = image.m_dwMipMapCount == 0 ? 1 : image.m_dwMipMapCount;

	LPDIRECT3DTEXTURE9 lpd3dTexture = NULL;
	HRESULT hr;
	const D3DFORMAT dxtFmt = (D3DFORMAT)image.m_xddPixelFormat.dwFourCC;

	if (ms_bSupportDXT)
	{
		// D3D9Ex: no D3DPOOL_MANAGED — stage compressed mips in SYSTEMMEM, then UpdateTexture to DEFAULT.
		hr = D3DXCreateTexture(ms_lpd3dDevice, image.m_nWidth, image.m_nHeight, mipmapCount, 0, dxtFmt, D3DPOOL_SYSTEMMEM, &lpd3dTexture);
		if (FAILED(hr))
		{
			TraceError("CreateDDSTexture: Cannot create SYSTEMMEM staging texture (file: %s, ms_bSupportDXT: 1) Error", m_stFileName.c_str());
			return false;
		}

		for (DWORD i = 0; i < mipmapCount; ++i)
		{
			D3DLOCKED_RECT lockedRect;
			hr = lpd3dTexture->LockRect(i, &lockedRect, NULL, 0);
			if (FAILED(hr))
			{
				TraceError("CreateDDSTexture: Cannot lock staging texture (file: %s, mip: %u) Error:", m_stFileName.c_str(), i);
				lpd3dTexture->Release();
				return false;
			}
			image.Copy(i, (BYTE*)lockedRect.pBits, lockedRect.Pitch);
			lpd3dTexture->UnlockRect(i);
		}

		hr = D3DXCreateTexture(ms_lpd3dDevice, image.m_nWidth, image.m_nHeight, mipmapCount, 0, dxtFmt, D3DPOOL_DEFAULT, &m_lpd3dTexture);
		if (FAILED(hr))
		{
			TraceError("CreateDDSTexture: Cannot create DEFAULT DXT texture (file: %s) Error", m_stFileName.c_str());
			lpd3dTexture->Release();
			return false;
		}

		hr = ms_lpd3dDevice->UpdateTexture(lpd3dTexture, m_lpd3dTexture);
		lpd3dTexture->Release();
		lpd3dTexture = NULL;
		if (FAILED(hr))
		{
			TraceError("CreateDDSTexture: UpdateTexture failed (file: %s) Error", m_stFileName.c_str());
			if (m_lpd3dTexture)
			{
				m_lpd3dTexture->Release();
				m_lpd3dTexture = NULL;
			}
			return false;
		}
	}
	else
	{
		hr = D3DXCreateTexture(ms_lpd3dDevice, image.m_nWidth, image.m_nHeight, mipmapCount, 0, dxtFmt, D3DPOOL_SCRATCH, &lpd3dTexture);
		if (FAILED(hr))
		{
			TraceError("CreateDDSTexture: Cannot create SCRATCH texture (file: %s, ms_bSupportDXT: 0) Error", m_stFileName.c_str());
			return false;
		}

		for (DWORD i = 0; i < mipmapCount; ++i)
		{
			D3DLOCKED_RECT lockedRect;
			hr = lpd3dTexture->LockRect(i, &lockedRect, NULL, 0);
			if (FAILED(hr))
			{
				TraceError("CreateDDSTexture: Cannot lock texture (file: %s, mipmapCount: %d) Error:", m_stFileName.c_str(), mipmapCount);
				lpd3dTexture->Release();
				return false;
			}
			image.Copy(i, (BYTE*)lockedRect.pBits, lockedRect.Pitch);
			lpd3dTexture->UnlockRect(i);
		}

		D3DFORMAT compatFormat;
		if (image.m_CompFormat == PF_DXT1)
			compatFormat = D3DFMT_A1R5G5B5;
		else if (IsLowTextureMemory())
			compatFormat = D3DFMT_A4R4G4B4;
		else
			compatFormat = D3DFMT_A8R8G8B8;

		const UINT imgWidth = image.m_nWidth;
		const UINT imgHeight = image.m_nHeight;

		hr = D3DXCreateTexture(ms_lpd3dDevice, imgWidth, imgHeight, mipmapCount, D3DUSAGE_DYNAMIC, compatFormat, D3DPOOL_DEFAULT, &m_lpd3dTexture);
		if (FAILED(hr))
		{
			TraceError("CreateDDSTexture: ms_bSupportDXT = false Cannot create texture (file: %s, compatFormat: %d) Error: ", m_stFileName.c_str(), compatFormat);
			lpd3dTexture->Release();
			return false;
		}

		IDirect3DTexture9* pkTexSrc = lpd3dTexture;
		IDirect3DTexture9* pkTexDst = m_lpd3dTexture;
		if (!ConvertTexture(pkTexSrc, pkTexDst, mipmapCount))
		{
			lpd3dTexture->Release();
			return false;
		}
		lpd3dTexture->Release();
	}

	m_width = image.m_nWidth;
	m_height = image.m_nHeight;
	m_bEmpty = false;

	return true;
}

bool CGraphicImageTexture::CreateFromMemoryFile(UINT bufSize, const void* c_pvBuf, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != NULL);
	assert(m_lpd3dTexture == NULL);

	static CDXTCImage image;

	if (image.LoadHeaderFromMemory2((const BYTE*)c_pvBuf, bufSize))	// DDS Texture found
		return (CreateDDSTexture(image, (const BYTE*)c_pvBuf));

	/************************************************************************/
	/* We got a non .DDS DXT file                                           */
	/************************************************************************/

	UINT width, height;
	height = width = D3DX_DEFAULT;

	// This is only valid because we don not load an DXT1 - DXT5 compressed texture if partial Support is given the call would fail 
	// for said formats.
	int pow2TextureSupportLevel = GetNonPow2TextureSupportLevel();
	if (pow2TextureSupportLevel == NONPOW2_SUPPORT_LEVEL::FULL_SUPPORT || pow2TextureSupportLevel == NONPOW2_SUPPORT_LEVEL::PARTIAL_SUPPORT) {
		width = D3DX_DEFAULT_NONPOW2;
		height = D3DX_DEFAULT_NONPOW2;
	}

	HRESULT hr;
	D3DXIMAGE_INFO imageInfo;

	hr = D3DXCreateTextureFromFileInMemoryEx(ms_lpd3dDevice, c_pvBuf, bufSize, width, height, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, d3dFmt, D3DPOOL_DEFAULT, dwFilter, D3DX_DEFAULT, 0xffff00ff, &imageInfo, NULL, &m_lpd3dTexture);

	if (FAILED(hr)) {
		TraceError("CreateFromMemoryFile: Non DDS file cannot create texture (file: %s) Error:", m_stFileName.c_str());
		return false;
	}

	m_width = imageInfo.Width;
	m_height = imageInfo.Height;

	// If someone knows why we need to switch formats here i would happily appreciate an explanation.
	D3DFORMAT format = imageInfo.Format;
	switch (imageInfo.Format) {
	case D3DFMT_A8R8G8B8:
		format = D3DFMT_A4R4G4B4;
		break;

	case D3DFMT_X8R8G8B8:
	case D3DFMT_R8G8B8:
		format = D3DFMT_A1R5G5B5;
		break;
	}

	if (IsLowTextureMemory() && (format != imageInfo.Format))
	{
		IDirect3DTexture9* pkTexSrc = m_lpd3dTexture;
		IDirect3DTexture9* pkTexDst;

		hr = D3DXCreateTexture(ms_lpd3dDevice, imageInfo.Width, imageInfo.Height, imageInfo.MipLevels, D3DUSAGE_DYNAMIC, format, D3DPOOL_DEFAULT, &pkTexDst);

		if (FAILED(hr)) {
			TraceError("CreateFromMemoryFile: Non DDS file IsLowTextureMemory cannot create texture (file: %s) Error:", m_stFileName.c_str());
			return  false;
		}

		m_lpd3dTexture = pkTexDst;

		if (!ConvertTexture(pkTexSrc, pkTexDst, imageInfo.MipLevels)) {
			pkTexSrc->Release();
			return false;
		}

		pkTexSrc->Release();
	}

	m_bEmpty = false;
	return true;
}
void CGraphicImageTexture::SetFileName(const char * c_szFileName)
{
	m_stFileName=c_szFileName;
}

bool CGraphicImageTexture::CreateFromDiskFile(const char * c_szFileName, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	Destroy();

	SetFileName(c_szFileName);

	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;
	return CreateDeviceObjects();
}

CGraphicImageTexture::CGraphicImageTexture()
{
	Initialize();
}

CGraphicImageTexture::~CGraphicImageTexture()
{
	Destroy();
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
