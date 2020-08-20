#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXMath.h>
#include <vector>
#include "resource.h"
#include"DDSTextureLoader.h"
#include "Structs.h"
#include "OBJLoader.h"
#include "Camera.h"
#include "Object.h"
#include "Vector3.h"

using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pConstantBuffer;

	//lighting
	UINT					_bufferStride;
	UINT					_bufferOffset;
	float					_t;
	XMFLOAT3				_lightDirection;
	XMFLOAT4                _diffuseMaterial;
	XMFLOAT4				_diffuseLight;
	XMFLOAT4				_ambientMaterial;
	XMFLOAT4				_ambientLight;
	XMFLOAT4				_specularMtrl;
	XMFLOAT4				_specularLight;
	float					_specularPower;

	//camera
	Camera*					_camera;
	XMVECTOR                _cameraPositions[4];
	XMVECTOR                _cameraLookAtPositions[4];
	int						_whichCamera;

	//abjects
	Object					*_boat, *_waterPlane, *_boundingWalls, *_mine;

	//skybox
	ID3D11Buffer* _sphereIndexBuffer;
	ID3D11Buffer* _sphereVertBuffer;

	ID3D11VertexShader* SKYMAP_VS;
	ID3D11PixelShader* SKYMAP_PS;
	ID3D10Blob* SKYMAP_VS_Buffer;
	ID3D10Blob* SKYMAP_PS_Buffer;

	ID3D11ShaderResourceView* _smrv;
	ID3D11BlendState* Transparency;


	ID3D11DepthStencilState* _DSLessEqual;
	ID3D11RasterizerState* _RSCullNone;
	cbSkybox skyBoxcb;
	XMMATRIX _scale;
	XMMATRIX _translation;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	void InitObject();


	UINT _WindowHeight;
	UINT _WindowWidth;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidRender;
	
	//textures
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11ShaderResourceView* _pTextureOcean = nullptr;
	ID3D11ShaderResourceView* _pTextureBoat = nullptr;
	ID3D11ShaderResourceView* _pTextureWall = nullptr;

	ID3D11ShaderResourceView* _SMTexture = nullptr;

	ID3D11SamplerState* _pSamplerLinear = nullptr;

	float rotation = 0;

	//skybox
	int NumSphereVertices;
	int NumSphereFaces;
	XMMATRIX Rotationx;
	XMMATRIX Rotationy;
	XMMATRIX Rotationz;

	XMMATRIX sphereWorld;


public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
	void CreateSphere(int LatLines, int LongLines);
};

