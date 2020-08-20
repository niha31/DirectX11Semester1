#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pConstantBuffer = nullptr;

	// Light direction from surface (XYZ)
	_lightDirection = XMFLOAT3(1.0f, 0.0f, -1.0f);
	// Diffuse material properties (RGBA)
	_diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
	// Diffuse light colour (RGBA)
	_diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//ambient light
	_ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_ambientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//specular light
	_specularMtrl = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	_specularLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_specularPower = 8.0f;

	_cameraPositions[0] = XMVectorSet(-0.01f, 15.0f, 0.0f, 1.0f);
	_cameraPositions[1] = XMVectorSet(5.0f, 5.0f, 0.0f, 1.0f);
	_cameraPositions[2] = XMVectorSet(-1.0f, 1.5f, 0.0f, 1.0f);
	_cameraPositions[3] = XMVectorSet(5.0f, 5.0f, 5.0f, 1.0f);

	_cameraLookAtPositions[0] = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	_cameraLookAtPositions[1] = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	_cameraLookAtPositions[2] = XMVectorSet(5.0f, 2.0f, 0.0f, 0.0f);
	_cameraLookAtPositions[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


	_whichCamera = 1;
	_camera = new Camera(_cameraPositions[0], _cameraLookAtPositions[0], _WindowWidth, _WindowHeight, 0.01f, 100.0f);	
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	InitObject();

	HRESULT hr = CreateDDSTextureFromFile(_pd3dDevice, L"diffuseTexture.dds", nullptr, &_pTextureRV);

	if (FAILED(hr)) {
		return hr;
	}

	hr = CreateDDSTextureFromFile(_pd3dDevice, L"ModelsAndTextures/OceanTxture.dds", nullptr, &_pTextureOcean);

	if (FAILED(hr)) {
		return hr;
	}

	hr = CreateDDSTextureFromFile(_pd3dDevice, L"ModelsAndTextures/boat.dds", nullptr, &_pTextureBoat);

	if (FAILED(hr)) {
		return hr;
	}

	hr = CreateDDSTextureFromFile(_pd3dDevice, L"ModelsAndTextures/wall.dds", nullptr, &_pTextureWall);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

	CreateSphere(10, 10);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);
	//for skybox
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "SKYMAP_VS", "vs_4_0", &SKYMAP_VS_Buffer);
	hr = CompileShaderFromFile(L"DX11 Framework.fx","SKYMAP_PS", "ps_4_0", &SKYMAP_PS_Buffer);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

	//for skybox
	hr = _pd3dDevice->CreateVertexShader(SKYMAP_VS_Buffer->GetBufferPointer(), SKYMAP_VS_Buffer->GetBufferSize(), nullptr, &SKYMAP_VS);
	if (FAILED(hr))
		return hr;
	hr = _pd3dDevice->CreatePixelShader(SKYMAP_PS_Buffer->GetBufferPointer(), SKYMAP_PS_Buffer->GetBufferSize(), nullptr, &SKYMAP_PS);
	if (FAILED(hr))
		return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};



	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}


HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{

    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 1280, 720};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

void Application::InitObject()
{
	_boat = new Object(_pd3dDevice, "ModelsAndTextures/pirate_boat.obj", Vector3(0.0f, 0.0f, 0.0f));
	_waterPlane = new Object(_pd3dDevice, "ModelsAndTextures/Water.obj", Vector3(0.0f, 0.0f, 0.0f));
	_boundingWalls = new Object(_pd3dDevice, "ModelsAndTextures/walls.obj", Vector3(0.0f, 0.0f, 0.0f));
	_mine = new Object(_pd3dDevice, "ModelsAndTextures/mine.obj", Vector3(5.0f, 0.0f, 5.0f));
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	


    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	hr = _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);

	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);


    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	if (FAILED(hr))
		return hr;

	//create rasterizer to do wireframe rendering
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);
	if (FAILED(hr))
		return hr;

	D3D11_RASTERIZER_DESC solidRenderDesc;
	ZeroMemory(&solidRenderDesc, sizeof(D3D11_RASTERIZER_DESC));
	solidRenderDesc.FillMode = D3D11_FILL_SOLID;
	solidRenderDesc.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&solidRenderDesc, &_solidRender);
	if (FAILED(hr))
		return hr;

	solidRenderDesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&solidRenderDesc, &_RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &_DSLessEqual);


	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;



	//skybox

	//Load the texture
	//ID3D11Texture2D* SMTexture = 0;
	//_pd3dDevice->CreateTexture2D();
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"ModelsAndTextures/SkyBox.dds", nullptr, &_SMTexture);
	
	if (FAILED(hr))
		return hr;

	//Create the textures description
	//D3D11_TEXTURE2D_DESC SMTextureDesc;

	//Tell D3D We have a cube texture, which is an array of 2D textures
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	_SMTexture->GetDesc(&SMViewDesc);

	/*SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;*/

	//Create the Resource view
	ID3D11Resource* res;
	_SMTexture->GetResource(&res);
	hr = _pd3dDevice->CreateShaderResourceView(res, &SMViewDesc, &_smrv);






	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	if (FAILED(hr))
		return hr;

	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_solidRender) _solidRender->Release();

	if (_sphereIndexBuffer)_sphereIndexBuffer->Release();
	if (_sphereVertBuffer)_sphereVertBuffer->Release();

	if (SKYMAP_VS)SKYMAP_VS->Release();
	if (SKYMAP_PS)SKYMAP_PS->Release();
	if (SKYMAP_VS_Buffer)SKYMAP_VS_Buffer->Release();
	if (SKYMAP_PS_Buffer)SKYMAP_PS_Buffer->Release();

	if (_smrv)_smrv->Release();

	if (_DSLessEqual)_DSLessEqual->Release();
	if (_RSCullNone)_RSCullNone->Release();
}

void Application::CreateSphere(int LatLines, int LongLines)
{
	NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	NumSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<SimpleVertex> vertices(NumSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].Pos.x = 0.0f;
	vertices[0].Pos.y = 0.0f;
	vertices[0].Pos.z = 1.0f;

	for (DWORD i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / (LongLines));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i * LongLines + j + 1].Pos.x = XMVectorGetX(currVertPos);
			vertices[i * LongLines + j + 1].Pos.y = XMVectorGetY(currVertPos);
			vertices[i * LongLines + j + 1].Pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[NumSphereVertices - 1].Pos.x = 0.0f;
	vertices[NumSphereVertices - 1].Pos.y = 0.0f;
	vertices[NumSphereVertices - 1].Pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	HRESULT hr = _pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &_sphereVertBuffer);


	std::vector<DWORD> indices(NumSphereFaces * 3);

	int k = 0;
	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < LatLines - 3; ++i)
	{
		for (DWORD j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;

			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * LongLines) + LongLines;
		indices[k + 1] = (i * LongLines) + 1;
		indices[k + 2] = ((i + 1) * LongLines) + LongLines;

		indices[k + 3] = ((i + 1) * LongLines) + LongLines;
		indices[k + 4] = (i * LongLines) + 1;
		indices[k + 5] = ((i + 1) * LongLines) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	_pd3dDevice->CreateBuffer(&indexBufferDesc, &iinitData, &_sphereIndexBuffer);
}


void Application::Update()
{
    // Update our time
    _t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        _t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

		if (dwTimeStart == 0)
		{
			dwTimeStart = dwTimeCur;
		}

        _t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

	//changing render state
	if (GetAsyncKeyState(VK_RETURN))
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}

	if (GetAsyncKeyState(VK_SHIFT))
	{
		_pImmediateContext->RSSetState(_solidRender);
	}

	//changing camera
	if (GetAsyncKeyState(0x30))
	{
		_camera->SetPosition(_cameraPositions[0]);
		_camera->SetLookAt(_cameraLookAtPositions[0]);
		_whichCamera = 1;
	}

	if (GetAsyncKeyState(0x31))
	{
		_camera->SetPosition(_cameraPositions[1]);
		_camera->SetLookAt(_cameraLookAtPositions[1]);
		_whichCamera = 2;
	}

	if (GetAsyncKeyState(0x32))
	{
		_camera->SetPosition(_cameraPositions[2]);
		_camera->SetLookAt(_cameraLookAtPositions[2]);
		_whichCamera = 3;
	}

	if (GetAsyncKeyState(0x33))
	{
		_camera->SetPosition(_cameraPositions[3]);
		_camera->SetLookAt(_cameraLookAtPositions[3]);
		_whichCamera = 4;
	}


	////move boat up
	//if (GetAsyncKeyState(0x57))
	//{
	//	_boat->UpdatePosition(0.05f, 0.0f, 0.0f);
	//}
	////move boat down
	//else if (GetAsyncKeyState(0x53))
	//{
	//	_boat->UpdatePosition(-0.05f, 0.0f, 0.0f);
	//}
	////move boat right
	//else if (GetAsyncKeyState(0x44))
	//{
	//	_boat->UpdatePosition(0.0f, 0.0f, -0.05f);
	//}
	////move boat left
	//else if (GetAsyncKeyState(0x41))
	//{
	//	_boat->UpdatePosition(0.0f, 0.0f, 0.05f);
	//}
	////rotate boat anticlockwise
	//else if (GetAsyncKeyState(0x51))
	//{
	//	rotation -= 5;
	//}
	////rotate boat clockwise
	//else if (GetAsyncKeyState(0x45))
	//{
	//	rotation += 5;
	//}
	////Reset Rotation
	//else if (GetAsyncKeyState(0x52))
	//{
	//	rotation = 0;
	//}

	_boat->GetParticleModel()->Update(_t);


    //
    // Animate the cubes
    //
	Vector3 object = _boat->GetParticleModel()->GetPosition();
	XMStoreFloat4x4(_boat->ReturnModelMatrixPointer(), XMMatrixRotationY(XMConvertToRadians(rotation)) * XMMatrixTranslation(object.x, object.y, object.z));

	object = _waterPlane->GetParticleModel()->GetPosition();
	XMStoreFloat4x4(_waterPlane->ReturnModelMatrixPointer(), XMMatrixTranslation(object.x, object.y, object.z));
	
	object = _mine->GetParticleModel()->GetPosition();
	//XMStoreFloat4x4(_mine->ReturnModelMatrixPointer(), XMMatrixTranslation(object.x, object.y, object.z) * XMMatrixRotationY(_t));
	_mine->GetQuaternion()->addScaledVector(Vector3(0.0f, 0.2f, 0.2f), 0.01f);
	XMStoreFloat4x4(_mine->ReturnModelMatrixPointer(), XMMatrixRotationQuaternion(*_mine->ReturnVectorQuaternion()) * XMMatrixTranslation(object.x, object.y, object.z) * XMMatrixRotationY(_t/10));


	//update Camera
	if (_whichCamera == 2)
	{
		Vector3 object;
		object = _boat->GetParticleModel()->GetPosition();
		_camera->SetPosition(XMVectorSet(object.x - 5.0f, object.y + 5, object.z, 1.0f));
		_camera->SetLookAt(XMVectorSet(object.x, object.y, object.z, 0.0f));
		
	}
	if (_whichCamera == 3)
	{
		Vector3 object;
		object = _boat->GetParticleModel()->GetPosition();
		_camera->SetPosition(XMVectorSet(object.x - 1.0f, object.y + 2, object.z, 1.0f));
		_camera->SetLookAt(XMVectorSet(object.x + 5.0f, object.y, object.z, 0.0f));
	}
	if (_whichCamera == 4)
	{
		if (GetAsyncKeyState(VK_UP))
		{
			_camera->MoveFowards();
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			_camera->MoveBackwards();
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			_camera->MoveLeft();
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			_camera->MoveRight();
		}
		Vector3 object;
		object = _boat->GetParticleModel()->GetPosition();
		_camera->SetLookAt(XMVectorSet(object.x, _camera->ReturnLookAt().y, object.z, 0.0f));
	}
	

	//Reset sphereWorld
	sphereWorld = XMMatrixIdentity();

	//Define sphereWorld's world space matrix
	_scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	//Make sure the sphere is always centered around camera
	_translation = XMMatrixTranslation(_camera->ReturnPosition().x, _camera->ReturnPosition().y, _camera->ReturnPosition().z);

	//Set sphereWorld's world space using the transformations
	sphereWorld = _scale * _translation;

	_camera->Update();
	Draw();
}
void Application::Draw()
{
	
    //
    // Clear the back buffer
    //
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

    //
    // Update variables
    //

	ConstantBuffer cb;
	cb.mView = XMMatrixTranspose(_camera->ReturnView());
	cb.mProjection = XMMatrixTranspose(_camera->ReturnProjection());
	cb.DiffuseMtrl = _diffuseMaterial;
	cb.DiffuseLight = _diffuseLight;
	cb.LightVecW = _lightDirection;
	cb.AmbientMtrl = _ambientMaterial;
	cb.AmbientLight = _ambientLight;
	cb.SpecularMtrl = _specularMtrl;
	cb.SpecularLight = _specularLight;
	cb.SpecularPower = _specularPower;
	cb.EyePosW = _camera->ReturnPosition();
	cb.gameTime = _t;
	cb.wavePattern = 0;

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);

	//skybox
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	
	//Set the WVP matrix and send it to the constant buffer in effect file
	XMMATRIX WVP = sphereWorld * _camera->ReturnView() * _camera->ReturnProjection();
	skyBoxcb.WVP = XMMatrixTranspose(WVP);
	skyBoxcb.World = XMMatrixTranspose(sphereWorld);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, NULL, &skyBoxcb, 0, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);

	//Send our skymap resource view to pixel shader
	_pImmediateContext->PSSetShaderResources(0, 1, &_smrv);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	//Set the new VS and PS shaders
	_pImmediateContext->VSSetShader(SKYMAP_VS, 0, 0);
	_pImmediateContext->PSSetShader(SKYMAP_PS, 0, 0);

	//Set the spheres index buffer
	_pImmediateContext->IASetIndexBuffer(_sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the spheres vertex buffer
	_pImmediateContext->IASetVertexBuffers(0, 1, &_sphereVertBuffer, &stride, &offset);

	//Set the new depth/stencil and RS states
	_pImmediateContext->OMSetDepthStencilState(_DSLessEqual, 0);
	_pImmediateContext->RSSetState(_RSCullNone);
	_pImmediateContext->DrawIndexed(NumSphereFaces * 3, 0, 0);

	//Set the default VS shader and depth/stencil state
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->OMSetDepthStencilState(NULL, 0);


	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureBoat);
	///////////////////BOAT DRAW////////////////////
	_pImmediateContext->IASetVertexBuffers(0, 1, &_boat->ReturnMeshDataPointer()->VertexBuffer, &_boat->ReturnMeshDataPointer()->VBStride, &_boat->ReturnMeshDataPointer()->VBOffset);
	_pImmediateContext->IASetIndexBuffer(_boat->ReturnMeshDataPointer()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	///////////////////BOAT DRAW////////////////////

	XMMATRIX world = XMLoadFloat4x4(&_boat->ReturnModelMatrix());
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(_boat->ReturnMeshDataPointer()->IndexCount, 0, 0);


	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureOcean);
	///////////////////WATER DRAW////////////////////
	_pImmediateContext->IASetVertexBuffers(0, 1, &_waterPlane->ReturnMeshDataPointer()->VertexBuffer, &_waterPlane->ReturnMeshDataPointer()->VBStride, &_waterPlane->ReturnMeshDataPointer()->VBOffset);
	_pImmediateContext->IASetIndexBuffer(_waterPlane->ReturnMeshDataPointer()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	///////////////////WATER DRAW////////////////////

	world = XMLoadFloat4x4(&_waterPlane->ReturnModelMatrix());
	cb.mWorld = XMMatrixTranspose(world);

	cb.wavePattern = 1;

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(_waterPlane->ReturnMeshDataPointer()->IndexCount, 0, 0);

	cb.wavePattern = 0;

	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureWall);
	///////////////////WALL DRAW////////////////////
	_pImmediateContext->IASetVertexBuffers(0, 1, &_boundingWalls->ReturnMeshDataPointer()->VertexBuffer, &_boundingWalls->ReturnMeshDataPointer()->VBStride, &_boundingWalls->ReturnMeshDataPointer()->VBOffset);
	_pImmediateContext->IASetIndexBuffer(_boundingWalls->ReturnMeshDataPointer()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	///////////////////WALL DRAW////////////////////

	world = XMLoadFloat4x4(&_boundingWalls->ReturnModelMatrix());
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(_boundingWalls->ReturnMeshDataPointer()->IndexCount, 0, 0);

	///////////////////MINE DRAW////////////////////
	_pImmediateContext->IASetVertexBuffers(0, 1, &_mine->ReturnMeshDataPointer()->VertexBuffer, &_mine->ReturnMeshDataPointer()->VBStride, &_mine->ReturnMeshDataPointer()->VBOffset);
	_pImmediateContext->IASetIndexBuffer(_mine->ReturnMeshDataPointer()->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	///////////////////MINE DRAW////////////////////

	world = XMLoadFloat4x4(&_mine->ReturnModelMatrix());
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(_mine->ReturnMeshDataPointer()->IndexCount, 0, 0);

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}