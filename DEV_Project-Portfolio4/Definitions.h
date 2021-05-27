#pragma once
#include "framework.h"
#define GATEWARE_ENABLE_CORE
#define GATEWARE_ENABLE_INPUT
#include "Gateware.h"

using namespace DirectX;
using namespace std;
using Microsoft::WRL::ComPtr;

// Global variables
LPCWSTR g_WindowClassName = L"EngineDev";      // The title bar text
LPCWSTR g_WindowName = L"RenderingWindow";   // the main window class name
LONG g_WindowWidth = 1280;
LONG g_WindowHeight = 720;
const BOOL g_EnableVSync = TRUE;
const UINT boxCount = 3;
GW::INPUT::GInput MouseLook;
static float t = 0.0f;
float moveScale = 0.0015f;
XMFLOAT3 gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);

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
ComPtr<ID3D11RasterizerState> gpDefaultRasterState = nullptr;

// Matrices 
XMMATRIX				g_Camera;
XMMATRIX                g_World[boxCount];
XMMATRIX                g_OrbitCrate;
XMMATRIX                g_View;
XMMATRIX                g_Projection;
XMFLOAT4				g_vOutputColor(0.7f, 0.7f, 0.7f, 1.0f);

// 3D Cube
ShaderMaterials cubeShaderMaterials;
ShaderController cubeShaderController;
BufferController<SimpleVertex> cubeBufferController;

// Grid
ShaderMaterials gridShaderMaterials;
ShaderController gridShaderController;
BufferController<GridVertex> gridBufferController;

class Particle
{
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 prev_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	float lifespan;

	Particle() = default;
	Particle(XMFLOAT3 pos, XMFLOAT3 prev_pos, XMFLOAT4 vel, XMFLOAT4 col, float life);
	~Particle() = default;
	Particle& operator=(const Particle&) = default;
};
