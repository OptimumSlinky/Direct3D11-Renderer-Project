// DEV_Project-Portfolio4.cpp 
// Started: January 2020
// Trevor Cook

#pragma region Project Set Up
#include "Resource.h"
#include "targetver.h"
#include "Window.h"
#include "Device.h"
#include "3DContent.h"
#include "Controls.h"

// Forward declarations 
void CleanupDevice();
void Render();
#pragma endregion

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

#pragma region Deployment & Clean Up
void Render()
{
	UpdateTime();

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
	XMMATRIX translateDoggo = XMMatrixTranslation(3.5f, -1.0f, 0.0);
	g_Doggo = downscaleDoggo * translateDoggo;

	//// Place mage
	//XMMATRIX magePosition = XMMatrixTranslation(-3.5f, 0.0f, 0.0f);
	//g_Mage = g_Mage * magePosition;

	// Clear the back buffer 
	gpImmediateContext->ClearRenderTargetView(gpRenderTargetView.Get(), Colors::Black);

	// Clear the depth buffer to max depth (1.0f)
	gpImmediateContext->ClearDepthStencilView(gpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	UpdateCamera();

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

	//// Render instanced cubes
	//gpImmediateContext->UpdateSubresource(cubeShaderController.VS_ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	//cubeShaderMaterials.Bind(gpImmediateContext.Get());
	//cubeShaderController.Bind(gpImmediateContext.Get());
	//cubeBufferController.Bind(gpImmediateContext.Get());
	//gpImmediateContext->DrawIndexedInstanced(36, 3, 0, 0, 0);

	//// Update for orbit cube
	//ConstantBuffer cb2;
	//cb2.mWorld[0] = g_OrbitCrate;
	//cb2.mView = g_View;
	//cb2.mProjection = g_Projection;

	//cb2.vLightPosition[0] = vLightPositions[0];
	//cb2.vLightPosition[1] = vLightPositions[1];
	//cb2.vLightDirection[0] = vLightDirections[0];
	//cb2.vLightDirection[1] = vLightDirections[1];
	//cb2.vLightDirection[2] = vLightDirections[2];
	//cb2.vLightColor[0] = vLightColors[0];
	//cb2.vLightColor[1] = vLightColors[1];
	//cb2.vLightColor[2] = vLightColors[2];
	//cb2.vOutputColor = g_vOutputColor;
	//// cb2.CameraPosition = *(XMFLOAT4*)&cameraPosition;
	//cb2.CameraPosition = cameraPosition;

	//// Render orbit cube
	//gpImmediateContext->UpdateSubresource(cubeShaderController.VS_ConstantBuffer.Get(), 0, nullptr, &cb2, 0, 0);
	//cubeShaderMaterials.Bind(gpImmediateContext.Get());
	//cubeShaderController.Bind(gpImmediateContext.Get());
	//cubeBufferController.Bind(gpImmediateContext.Get());
	//gpImmediateContext->DrawIndexed(36, 0, 0);

	// Render doggo
	cb.mWorld[0] = g_Doggo;
	gpImmediateContext->UpdateSubresource(doggoShader.VS_ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	doggoMaterials.Bind(gpImmediateContext.Get());
	doggoShader.Bind(gpImmediateContext.Get());
	doggoBuffer.Bind(gpImmediateContext.Get());
	gpImmediateContext->DrawIndexed(11412, 0, 0);

	// Render mage
	cb.mWorld[0] = XMMatrixIdentity();
	gpImmediateContext->UpdateSubresource(mageShaders.VS_ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	mageMaterials.Bind(gpImmediateContext.Get());
	mageShaders.Bind(gpImmediateContext.Get());
	mageBuffers.Bind(gpImmediateContext.Get());
	gpImmediateContext->DrawIndexed(mageMesh.indexList.size(),0,0);

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