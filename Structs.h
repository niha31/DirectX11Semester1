#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct ConstantBuffer
{
	//PACK_SEAM
	XMMATRIX mWorld;

	//PACK_SEAM
	XMMATRIX mView;

	//PACK_SEAM
	XMMATRIX mProjection;

	//PACK_SEAM
	XMFLOAT4 DiffuseMtrl;

	//PACK_SEAM
	XMFLOAT4 DiffuseLight;

	//PACK_SEAM
	XMFLOAT3 LightVecW;
	float padding1;

	//PACK_SEAM
	XMFLOAT4 AmbientMtrl;

	//PACK_SEAM
	XMFLOAT4 AmbientLight;

	//PACK_SEAM
	XMFLOAT4 SpecularMtrl;

	//PACK_SEAM
	XMFLOAT4 SpecularLight;

	//PACK_SEAM
	XMFLOAT3 EyePosW; // Camera position in world space
	float SpecularPower;

	//PACK_SEAM
	float gameTime;
	int wavePattern;
	XMFLOAT2 padding2;
};

struct cbSkybox
{
	XMMATRIX  WVP;
	XMMATRIX World;
};