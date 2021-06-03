#pragma once
#include "framework.h"
#define GATEWARE_ENABLE_CORE
#define GATEWARE_ENABLE_INPUT
#include "Gateware.h"

using namespace DirectX;
using namespace std;
using Microsoft::WRL::ComPtr;

// Global variables
const BOOL g_EnableVSync = TRUE;
const UINT boxCount = 3;
GW::INPUT::GInput MouseLook;

LPCWSTR g_WindowClassName = L"EngineDev";      // The title bar text
LPCWSTR g_WindowName = L"RenderingWindow";   // the main window class name
LONG g_WindowWidth = 1280;
LONG g_WindowHeight = 720;

XMVECTOR gravity = { 0.0f, -9.8f, 0.0f };
XMVECTOR fall = { 0.0f, -1.0f, 0.0f };
float particleLaunchSpeed = 1.0f;
float moveScale = 0.0015f;
static float deltaTime = 0.0f;

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

// 3D Crate
ShaderMaterials crateShaderMaterials;
ShaderController crateShaderController;
BufferController<SimpleVertex> crateBufferController;

// Grid
ShaderMaterials gridShaderMaterials;
ShaderController gridShaderController;
BufferController<GridVertex> gridBufferController;

class Particle
{
public:
	XMVECTOR position = { 0.0f, 0.0f, 0.0f };
	XMVECTOR prev_position = { 0.0f, 0.0f, 0.0f };
	XMVECTOR velocity = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4 color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	float lifespan = 3.0f;

	Particle() = default;
	Particle(XMVECTOR pos, XMVECTOR prev_pos, XMVECTOR vel, XMFLOAT4 col, float life) : position(pos), prev_position(prev_pos), velocity(vel), color(col), lifespan(life) {};
	~Particle() = default;
	Particle& operator=(const Particle&) = default;
};

class Emitter
{
private:
	XMVECTOR em_position = { 0.0f, 0.0f, 0.0f };
	sorted_pool_t<Particle, 256> SortedPool;

public:
	Emitter() = default;
	Emitter(XMVECTOR pos) : em_position(pos) {};
	~Emitter() = default;

};