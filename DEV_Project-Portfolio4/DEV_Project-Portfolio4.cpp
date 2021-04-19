// DEV_Project-Portfolio4.cpp 
// Started: January 2020
// Trevor Cook

#pragma region Project Set Up
#include "Resource.h"
#include "RenderTools.h"
#include "MeshTools.h"
#include "Grid.h"
#include "DogKnight.h"
#define GATEWARE_ENABLE_CORE
#define GATEWARE_ENABLE_INPUT
#include "Gateware.h"

using namespace DirectX;
using namespace std;
using Microsoft::WRL::ComPtr;

// Global variables
LPCWSTR g_WindowClassName = L"Project&Portfolio4";      // The title bar text
LPCWSTR g_WindowName = L"RenderingWindow";   // the main window class name
LONG g_WindowWidth = 1280;
LONG g_WindowHeight = 720;
const BOOL g_EnableVSync = TRUE;
const UINT boxCount = 3;
GW::INPUT::GInput MouseLook;

struct ConstantBuffer
{
	XMMATRIX mWorld[boxCount];
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 vLightPosition[3];
	XMFLOAT4 vLightDirection[3];
	XMFLOAT4 vLightColor[3];
	XMFLOAT4 vOutputColor;
	XMVECTOR CameraPosition;
};

struct GridConstantBuffer
{
	XMMATRIX gridWorld;
	XMMATRIX gridView;
	XMMATRIX gridProjection;
};

// Direct3D global variables
HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

// Direct3D device and swap chain
ComPtr<ID3D11Device> gpD3D_Device = nullptr;
ComPtr<ID3D11Device1> gpD3D_Device1 = nullptr;
ComPtr<ID3D11DeviceContext> gpImmediateContext = nullptr;
ComPtr<ID3D11DeviceContext1> gpImmediateContext1 = nullptr;
ComPtr<IDXGISwapChain> gpSwapChain = nullptr;
ComPtr<IDXGISwapChain1> gpSwapChain1 = nullptr;
ComPtr<ID3D11RenderTargetView> gpRenderTargetView = nullptr;
ComPtr<ID3D11Texture2D> gpDepthStencil = nullptr;
ComPtr<ID3D11DepthStencilView> gpDepthStencilView = nullptr;
ComPtr<ID3D11ShaderResourceView> gpTextureRV = nullptr;
ComPtr<ID3D11SamplerState> gpSamplerLinear = nullptr;
ComPtr<ID3D11RasterizerState> gpSkyboxRasterState = nullptr;
ComPtr<ID3D11RasterizerState> gpDefaultRasterState = nullptr;

// Matrices 
XMMATRIX				g_Camera;
XMMATRIX				g_Skybox;
XMMATRIX                g_World[boxCount];
XMMATRIX                g_OrbitCrate;
XMMATRIX                g_Doggo;
XMMATRIX                g_View;
XMMATRIX                g_Projection;
XMFLOAT4				g_vOutputColor(0.7f, 0.7f, 0.7f, 1.0f);

// Skybox
ShaderMaterials skyboxMaterials;
ShaderController skyboxController;
BufferController<SimpleVertex> skyboxBuffer;

// 3D Cube
ShaderMaterials cubeShaderMaterials;
ShaderController cubeShaderController;
BufferController<SimpleVertex> cubeBufferController;

// Grid
ShaderMaterials gridShaderMaterials;
ShaderController gridShaderController;
BufferController<GridVertex> gridBufferController;

// Dog Knight
BufferController<SimpleVertex> doggoBuffer;
ShaderController doggoShader;
ShaderMaterials doggoMaterials;

// Forward declarations 
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
HRESULT Init3DContent();
void CleanupDevice();
void Render();
#pragma endregion

#pragma region Win 32 API 
// Entry point for the application.
int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEVPROJECTPORTFOLIO4));

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };
	Init3DContent();
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}

// Register class and create window
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEVPROJECTPORTFOLIO4));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DEVPROJECTPORTFOLIO4);
	wcex.lpszClassName = g_WindowClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, g_WindowWidth, g_WindowHeight }; // Set window size
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(g_WindowClassName, g_WindowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);

	if (!g_hWnd)
	{
		return E_FAIL;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return S_OK;
}

//  Processes messages for the main window.
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			
			case IDM_EXIT:
			DestroyWindow(hWnd);
				break;
			
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	break;
	
	case WM_PAINT:
		{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
		}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


// Message handler for about box
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
#pragma endregion

#pragma region Direct3D Device Initialization
// Create Direct3D device and swap chain
HRESULT InitDevice()
{
	HRESULT hr = S_OK;
	GW::GReturn gr;
	gr = MouseLook.Create(GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE{ g_hWnd, nullptr });

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

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
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, gpD3D_Device.GetAddressOf(), &g_featureLevel, gpImmediateContext.GetAddressOf());

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, gpD3D_Device.GetAddressOf(), &g_featureLevel, gpImmediateContext.GetAddressOf());
		}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = gpD3D_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
		return hr;

	// Create swap chain
	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2)
	{
		// DirectX 11.1 or later
		hr = gpD3D_Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(gpD3D_Device1.GetAddressOf()));
		if (SUCCEEDED(hr))
		{
			(void)gpImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(gpImmediateContext1.GetAddressOf()));
		}

		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(gpD3D_Device.Get(), g_hWnd, &sd, nullptr, nullptr, gpSwapChain1.GetAddressOf());
		if (SUCCEEDED(hr))
		{
			hr = gpSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(gpSwapChain.GetAddressOf()));
		}

		dxgiFactory2->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = g_hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = dxgiFactory->CreateSwapChain(gpD3D_Device.Get(), &sd, gpSwapChain.GetAddressOf());
	}

	dxgiFactory->Release();

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = gpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = gpD3D_Device->CreateRenderTargetView(pBackBuffer, nullptr, gpRenderTargetView.GetAddressOf());
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = gpD3D_Device->CreateTexture2D(&descDepth, nullptr, gpDepthStencil.GetAddressOf());
	if (FAILED(hr))
	{
		return hr;
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSView = {};
	descDSView.Format = descDepth.Format;
	descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSView.Texture2D.MipSlice = 0;
	hr = gpD3D_Device->CreateDepthStencilView(gpDepthStencil.Get(), &descDSView, gpDepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		return hr;
	}

	gpImmediateContext->OMSetRenderTargets(1, gpRenderTargetView.GetAddressOf(), gpDepthStencilView.Get());

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gpImmediateContext->RSSetViewports(1, &vp);

	// Initialize world matrix
	// Add for loop to cycle through array
	for (UINT i = 0; i < boxCount; i++)
	{
		g_World[i] = XMMatrixIdentity();
	}

	g_OrbitCrate = XMMatrixIdentity();

	// Initialize view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, 7.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	// Stage 1: Convert Camera to World Space (for camera control)
	g_Camera = XMMatrixInverse(nullptr, g_View);

	// Initialize projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

	return S_OK;
};
#pragma endregion

#pragma region 3D Content Initialization
HRESULT Init3DContent()
{
	HRESULT hr = S_OK;

	// Define skybox layout
	D3D11_INPUT_ELEMENT_DESC skyboxLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create skybox vertex shader and input layout from file
	hr = skyboxController.CreateVSandILFromFile(gpD3D_Device.Get(), "SKYBOX_VS.cso", skyboxLayout, ARRAYSIZE(skyboxLayout));

	// Create skybox pixel shader from file
	hr = skyboxController.CreatePSFromFile(gpD3D_Device.Get(), "SKYBOX_PS.cso");

	// Create 3D cube for skybox
	SimpleMesh<SimpleVertex> skybox = CreateCube();

	// Create skybox vertex buffers
	skyboxBuffer.CreateBuffers(gpD3D_Device.Get(), skybox.indicesList, skybox.vertexList);

	// Create skybox constant buffer
	skyboxController.CreateVSConstantBuffer(gpD3D_Device.Get(), sizeof(ConstantBuffer));
	skyboxController.PS_ConstantBuffer = skyboxController.VS_ConstantBuffer;

	// Load skybox texture 
	skyboxMaterials.CreateTextureFromFile(gpD3D_Device.Get(), "./SkyboxOcean.dds");

	// Create skybox sampler state
	skyboxMaterials.CreateDefaultSampler(gpD3D_Device.Get());

	// TODO: define input layout
	D3D11_INPUT_ELEMENT_DESC cubeLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// Create vertex shader and input layout from file
	hr = cubeShaderController.CreateVSandILFromFile(gpD3D_Device.Get(), "MAIN_VS.cso", cubeLayout, ARRAYSIZE(cubeLayout));

	// Create pixel shader from file
	hr = cubeShaderController.CreatePSFromFile(gpD3D_Device.Get(), "MAIN_PS.cso");

	// Create 3D cube
	SimpleMesh<SimpleVertex> crate = CreateCube();

	// Create vertex buffers
	cubeBufferController.CreateBuffers(gpD3D_Device.Get(), crate.indicesList, crate.vertexList);

	// Create constant buffer
	cubeShaderController.CreateVSConstantBuffer(gpD3D_Device.Get(), sizeof(ConstantBuffer));
	cubeShaderController.PS_ConstantBuffer = cubeShaderController.VS_ConstantBuffer;

	// Load texture 
	cubeShaderMaterials.CreateTextureFromFile(gpD3D_Device.Get(), "./crate.dds");

	// Create sampler state
	cubeShaderMaterials.CreateDefaultSampler(gpD3D_Device.Get());

	// Create grid
	DrawGrid();

	// Create grid constant and vertex buffers
	gridShaderController.CreateVSConstantBuffer(gpD3D_Device.Get(), sizeof(GridConstantBuffer));
	gridBufferController.CreateVertexBuffer(gpD3D_Device.Get(), gridlines);
	gridBufferController.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	// Create grid layout
	D3D11_INPUT_ELEMENT_DESC gridLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = gridShaderController.CreateVSandILFromFile(gpD3D_Device.Get(), "GRID_VS.cso", gridLayout, ARRAYSIZE(gridLayout));
	hr = gridShaderController.CreatePSFromFile(gpD3D_Device.Get(), "GRID_PS.cso");

	// For Loop to convert OBJ -> Vector?
	// create new vectors for doggo
	vector <SimpleVertex> doggoVerts;
	vector <int> doggoIndices;
	
	for (size_t i = 0; i < 7848; i++)
	{
		// create temp simplevertex
		SimpleVertex temp;

		// store the data from objvert to simplevertex
		temp.position.x = DogKnight_data[i].pos[0];
		temp.position.y = DogKnight_data[i].pos[1];
		temp.position.z = DogKnight_data[i].pos[2];

		temp.texture.x= DogKnight_data[i].uvw[0];
		temp.texture.y = DogKnight_data[i].uvw[1];

		temp.normal.x = DogKnight_data[i].nrm[0];
		temp.normal.y = DogKnight_data[i].nrm[1];
		temp.normal.z = DogKnight_data[i].nrm[2];

		//push into vector
		doggoVerts.push_back(temp);
	}

	for (size_t i = 0; i < 11412; i++)
	{
		int temp = DogKnight_indicies[i];
		doggoIndices.push_back(temp);
	}

	 // Create doggo input layout
	D3D11_INPUT_ELEMENT_DESC doggoLayout[] =
	{
		
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};
	hr = doggoShader.CreateVSandILFromFile(gpD3D_Device.Get(), "DOGGO_VS.cso", doggoLayout, ARRAYSIZE(doggoLayout));

	// Create doggo pixel shader
	hr = doggoShader.CreatePSFromFile(gpD3D_Device.Get(), "MAIN_PS.cso");

	// Create doggo vertex + index buffer
	hr = doggoBuffer.CreateBuffers(gpD3D_Device.Get(), doggoIndices, doggoVerts); 

	// Constant buffer
	doggoShader.CreateVSConstantBuffer(gpD3D_Device.Get(), sizeof(ConstantBuffer));
	doggoShader.PS_ConstantBuffer = doggoShader.VS_ConstantBuffer;

	// Load doggo textures
	doggoMaterials.CreateTextureFromFile(gpD3D_Device.Get(), "./DK_StandardAlbedo.dds");

	// Sampler state
	doggoMaterials.CreateDefaultSampler(gpD3D_Device.Get());

	// Initialize skybox raster state variant
	D3D11_RASTERIZER_DESC skyboxRasterState;
	skyboxRasterState.FrontCounterClockwise = true; // Changed from default for skybox
	skyboxRasterState.DepthBias = 0;
	skyboxRasterState.SlopeScaledDepthBias = 0.0f;
	skyboxRasterState.DepthBiasClamp = 0.0f;
	skyboxRasterState.DepthClipEnable = true;
	skyboxRasterState.ScissorEnable = false;
	skyboxRasterState.MultisampleEnable = false;
	skyboxRasterState.AntialiasedLineEnable = false;
	skyboxRasterState.FillMode = D3D11_FILL_SOLID;
	skyboxRasterState.CullMode = D3D11_CULL_BACK;
	
	// Create skybox raster state
	gpD3D_Device.Get()->CreateRasterizerState(&skyboxRasterState,gpSkyboxRasterState.GetAddressOf());

	// Initialize default raster state variant
	D3D11_RASTERIZER_DESC defaultRasterState;
	defaultRasterState.FrontCounterClockwise = false; // Changed from skybox state
	defaultRasterState.DepthBias = 0;
	defaultRasterState.SlopeScaledDepthBias = 0.0f;
	defaultRasterState.DepthBiasClamp = 0.0f;
	defaultRasterState.DepthClipEnable = true;
	defaultRasterState.ScissorEnable = false;
	defaultRasterState.MultisampleEnable = false;
	defaultRasterState.AntialiasedLineEnable = false;
	defaultRasterState.FillMode = D3D11_FILL_SOLID;
	defaultRasterState.CullMode = D3D11_CULL_BACK;

	// Create default raster state
	gpD3D_Device.Get()->CreateRasterizerState(&defaultRasterState, gpDefaultRasterState.GetAddressOf());

	return S_OK;
};
#pragma endregion

#pragma region Deployment & Clean Up
void Render()
{
	// Update time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static ULONGLONG timeStart = 0;
		ULONGLONG timeCurrent = GetTickCount64();
		if (timeStart == 0)
			timeStart = timeCurrent;
		t = (timeCurrent - timeStart) / 1000.0f;
	}

	// Setup lighting parameters
	XMFLOAT4 vLightPositions[3] =
	{
		XMFLOAT4(-4.0f, 1.5f, 0.0f, 1.0f), // point light position
		XMFLOAT4(0.0f, -3.0f, 6.0f, 1.0f), // spotlight
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	};

	XMFLOAT4 vLightDirections[3] =
	{
		XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),
		XMFLOAT4(0.0f, 5.0f, 0.0f, 1.0f), // spotlight
		XMFLOAT4(4.0f, 0.0f, 0.0f, 1.0f), // directional light
	};

	XMFLOAT4 vLightColors[3] =
	{
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), // r -> point light
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), // g -> spotlight
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), // b -> directional light
	};

	// Spin first cube around the origin
	g_World[0] = XMMatrixRotationY(t);

	// Orbit second cube around the origin
	XMMATRIX spin = XMMatrixRotationZ(-t);
	XMMATRIX orbit = XMMatrixRotationY(-t * 1.0f);
	XMMATRIX translate = XMMatrixTranslation(-3.0f, 0, 0);
	XMMATRIX downscale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	g_OrbitCrate = downscale * spin * translate * orbit;

	// Place and downsize doggo 
	XMMATRIX downscaleDoggo = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	XMMATRIX translateDoggo = XMMatrixTranslation(3.5, -1.0f, 5.0);
	g_Doggo = downscaleDoggo * translateDoggo;

	// Clear the back buffer 
	gpImmediateContext->ClearRenderTargetView(gpRenderTargetView.Get(), Colors::Black);

	// Clear the depth buffer to max depth (1.0f)
	gpImmediateContext->ClearDepthStencilView(gpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Stage 2: Get user input and apply motion
	float moveScale = 0.0015f;

	if (GetAsyncKeyState('W'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, 0, moveScale);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('S'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, 0, -moveScale);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('A'))
	{
		XMMATRIX temp = XMMatrixTranslation(-moveScale, 0, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('D'))
	{
		XMMATRIX temp = XMMatrixTranslation(moveScale, 0, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('Q'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationZ(-t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('E'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationZ(t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('R'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationX(-t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('F'))
	{
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX temp = XMMatrixRotationX(t * 0.00075f); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, temp); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	if (GetAsyncKeyState('T'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, moveScale, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	if (GetAsyncKeyState('G'))
	{
		XMMATRIX temp = XMMatrixTranslation(0, -moveScale, 0);
		g_Camera = XMMatrixMultiply(temp, g_Camera);
	}

	// Mouse Look Implementation
	// Floats for storing movement deltas
	float deltaX;
	float deltaY;
	float mouseScale = 0.0015f; 

	//Gateware black magic
	GW::GReturn result;
	result = MouseLook.GetMouseDelta(deltaY, deltaX);

	if (G_PASS(result) && result != GW::GReturn::REDUNDANT)
	{
		XMMATRIX tempX = XMMatrixRotationX(deltaX * mouseScale);
		g_Camera = XMMatrixMultiply(tempX, g_Camera); // X rotation complete; go do global Y

		// Do global Y here
		// To solve weird rotation angles (for global rotation)
		XMVECTOR position = g_Camera.r[3]; // Save matrix position
		g_Camera.r[3] = XMVectorSet(0, 0, 0, 1); // Place matrix at origin
		XMMATRIX tempY = XMMatrixRotationY(deltaY * mouseScale); // Rotate
		g_Camera = XMMatrixMultiply(g_Camera, tempY); // Multiply matrices in reverse order
		g_Camera.r[3] = position; // Return to original position
	}

	// Stage 3: Convert updated camera back to View Space
	g_View = XMMatrixInverse(nullptr, g_Camera);

	// SKYBOX: Determine camera's position in world space
	XMVECTOR cameraPosition = g_Camera.r[3];

	// SKYBOX: Move skybox cube to camera position
	g_Skybox = XMMatrixTranslationFromVector(cameraPosition);

	// Create raster state for skybox
	gpImmediateContext->RSSetState(gpSkyboxRasterState.Get());

	// Draw skybox
	ConstantBuffer skyCB;
	skyCB.mWorld[0] = g_Skybox;
	skyCB.mView = g_View;
	skyCB.mProjection = g_Projection;
	skyCB.vLightPosition[0] = vLightPositions[0];
	skyCB.vLightPosition[1] = vLightPositions[1];
	skyCB.vLightDirection[0] = vLightDirections[0];
	skyCB.vLightDirection[1] = vLightDirections[1];
	skyCB.vLightColor[0] = vLightColors[0];
	skyCB.vLightColor[1] = vLightColors[1];
	skyCB.vLightColor[2] = vLightColors[2];
	skyCB.vOutputColor = g_vOutputColor;

	gpImmediateContext->UpdateSubresource(skyboxController.VS_ConstantBuffer.Get(), 0, nullptr, &skyCB, 0, 0);
	skyboxMaterials.Bind(gpImmediateContext.Get());
	skyboxController.Bind(gpImmediateContext.Get());
	skyboxBuffer.Bind(gpImmediateContext.Get());
	gpImmediateContext->DrawIndexedInstanced(36, 3, 0, 0, 0);
	
	// Reset raster state after drawing skybox
	// gpImmediateContext->RSSetState(nullptr); // disables skybox raster setting WITHOUT deleting it; returns rasterizer to the default state!!
	gpImmediateContext->RSSetState(gpDefaultRasterState.Get()); // the above didn't work; created second raster state and switched to that instead.

	// Clear depth buffer before drawing the cubes
	gpImmediateContext->ClearDepthStencilView(gpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Create constant buffer
	ConstantBuffer cb;
	cb.mWorld[0] = g_World[0];
	cb.mWorld[1] = g_World[1];
	cb.mWorld[2] = g_World[2];
	cb.mView = g_View;
	cb.mProjection = g_Projection;
	cb.vLightPosition[0] = vLightPositions[0];
	cb.vLightPosition[1] = vLightPositions[1];
	cb.vLightDirection[0] = vLightDirections[0];
	cb.vLightDirection[1] = vLightDirections[1];
	cb.vLightDirection[2] = vLightDirections[2];
	cb.vLightColor[0] = vLightColors[0];
	cb.vLightColor[1] = vLightColors[1];
	cb.vLightColor[2] = vLightColors[2];
	cb.vOutputColor = g_vOutputColor;
	cb.CameraPosition = cameraPosition;

	// Position and rotate instanced cubes
	XMMATRIX instanceSpin = XMMatrixRotationY(t);
	XMMATRIX instancePOS1 = XMMatrixTranslation(4.0f, 2.0f, -1.0f);
	XMMATRIX instancePOS2 = XMMatrixTranslation(-3.0f, 3.5f, 4.0f);
	cb.mWorld[1] = instanceSpin * instancePOS1;
	cb.mWorld[2] = instanceSpin * instancePOS2;

	// Render instanced cubes
	gpImmediateContext->UpdateSubresource(cubeShaderController.VS_ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	cubeShaderMaterials.Bind(gpImmediateContext.Get());
	cubeShaderController.Bind(gpImmediateContext.Get());
	cubeBufferController.Bind(gpImmediateContext.Get());
	gpImmediateContext->DrawIndexedInstanced(36, 3, 0, 0, 0);

	// Update for orbit cube
	ConstantBuffer cb2;
	cb2.mWorld[0] = g_OrbitCrate;
	cb2.mView = g_View;
	cb2.mProjection = g_Projection;

	cb2.vLightPosition[0] = vLightPositions[0];
	cb2.vLightPosition[1] = vLightPositions[1];
	cb2.vLightDirection[0] = vLightDirections[0];
	cb2.vLightDirection[1] = vLightDirections[1];
	cb2.vLightDirection[2] = vLightDirections[2];
	cb2.vLightColor[0] = vLightColors[0];
	cb2.vLightColor[1] = vLightColors[1];
	cb2.vLightColor[2] = vLightColors[2];
	cb2.vOutputColor = g_vOutputColor;
	// cb2.CameraPosition = *(XMFLOAT4*)&cameraPosition;
	cb2.CameraPosition = cameraPosition;

	// Render orbit cube
	gpImmediateContext->UpdateSubresource(cubeShaderController.VS_ConstantBuffer.Get(), 0, nullptr, &cb2, 0, 0);
	cubeShaderMaterials.Bind(gpImmediateContext.Get());
	cubeShaderController.Bind(gpImmediateContext.Get());
	cubeBufferController.Bind(gpImmediateContext.Get());
	gpImmediateContext->DrawIndexed(36, 0, 0);

	// Render doggo
	cb.mWorld[0] = g_Doggo;
	gpImmediateContext->UpdateSubresource(doggoShader.VS_ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	doggoMaterials.Bind(gpImmediateContext.Get());
	doggoShader.Bind(gpImmediateContext.Get());
	doggoBuffer.Bind(gpImmediateContext.Get());
	gpImmediateContext->DrawIndexed(11412, 0, 0);

	// Render gridlines
	GridConstantBuffer gridCB;
	gridCB.gridWorld = XMMatrixIdentity();
	gridCB.gridView = g_View;
	gridCB.gridProjection = g_Projection;
	gpImmediateContext->UpdateSubresource(gridShaderController.VS_ConstantBuffer.Get(), 0, nullptr, &gridCB, 0, 0);
	gridShaderController.Bind(gpImmediateContext.Get());
	gridBufferController.BindAndDraw(gpImmediateContext.Get());

	// Present back buffer information to the front buffer (user viewpoint)
	gpSwapChain->Present(0, 0);
};

void CleanupDevice()
{
	if (gpImmediateContext) gpImmediateContext->ClearState();
	if (gpDepthStencil) gpDepthStencil->Release();
	if (gpDepthStencilView) gpDepthStencilView->Release();
	if (gpTextureRV) gpTextureRV->Release();
	if (gpSamplerLinear) gpSamplerLinear->Release();
	if (gpRenderTargetView) gpRenderTargetView->Release();
	if (gpSwapChain1) gpSwapChain1->Release();
	if (gpSwapChain) gpSwapChain->Release();
	if (gpImmediateContext1) gpImmediateContext1->Release();
	if (gpImmediateContext) gpImmediateContext->Release();
	if (gpD3D_Device1) gpD3D_Device1->Release();
	if (gpD3D_Device) gpD3D_Device->Release();
};
#pragma endregion