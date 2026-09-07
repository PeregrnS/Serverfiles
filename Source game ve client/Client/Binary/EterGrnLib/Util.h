#pragma once

// bool GrannyMeshGetTextureAnimation(granny_mesh* pgrnMesh, float* puVelocity, float* pvVelocity);
// bool GrannyMeshIsTextureAnimation(granny_mesh* pgrnMesh);
bool GrannyMeshIsDeform(granny_mesh* pgrnMesh);

class CGraphicImage;

struct SMaterialData
{
	CGraphicImage * pImage;
	float fSpecularPower;
	BOOL isSpecularEnable;
	BYTE bSphereMapIndex;
};
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
