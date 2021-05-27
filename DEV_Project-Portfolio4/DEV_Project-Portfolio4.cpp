// Started: January 2020
// Trevor Cook

#pragma region Project Set Up
#include "3DContent.h"

// Forward declarations 
void CleanupDevice();
void Render();
void Update();
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
			Update();
			Render();
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}

#pragma region Deployment & Clean Up
void Render()
{
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

	// Clear the back buffer 
	gpImmediateContext->ClearRenderTargetView(gpRenderTargetView.Get(), Colors::Black);

	// Clear the depth buffer to max depth (1.0f)
	gpImmediateContext->ClearDepthStencilView(gpDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// SKYBOX: Determine camera's position in world space
	XMVECTOR cameraPosition = g_Camera.r[3];

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
	cb2.CameraPosition = cameraPosition;

	// Render orbit cube
	gpImmediateContext->UpdateSubresource(cubeShaderController.VS_ConstantBuffer.Get(), 0, nullptr, &cb2, 0, 0);
	cubeShaderMaterials.Bind(gpImmediateContext.Get());
	cubeShaderController.Bind(gpImmediateContext.Get());
	cubeBufferController.Bind(gpImmediateContext.Get());
	gpImmediateContext->DrawIndexed(36, 0, 0);

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
}
void Update()
{
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