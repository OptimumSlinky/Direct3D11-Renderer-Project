#pragma once
#include "Definitions.h"

HRESULT Init3DContent()
{
	HRESULT hr = S_OK;

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