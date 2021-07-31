#pragma once

#include "Device.h"

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
	skyboxBuffer.CreateBuffers(gpD3D_Device.Get(), skybox.indexList, skybox.vertexList);

	// Create skybox constant buffer
	skyboxController.CreateVSConstantBuffer(gpD3D_Device.Get(), sizeof(ConstantBuffer));
	skyboxController.PS_ConstantBuffer = skyboxController.VS_ConstantBuffer;

	// Load skybox texture 
	skyboxMaterials.CreateTextureFromFile(gpD3D_Device.Get(), "./SkyboxOcean.dds");

	// Create skybox sampler state
	skyboxMaterials.CreateDefaultSampler(gpD3D_Device.Get());
		
	// Create grid
	GenerateGrid();

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
		temp.Pos.x = DogKnight_data[i].pos[0];
		temp.Pos.y = DogKnight_data[i].pos[1];
		temp.Pos.z = DogKnight_data[i].pos[2];

		temp.Tex.x = DogKnight_data[i].uvw[0];
		temp.Tex.y = DogKnight_data[i].uvw[1];

		temp.Normal.x = DogKnight_data[i].nrm[0];
		temp.Normal.y = DogKnight_data[i].nrm[1];
		temp.Normal.z = DogKnight_data[i].nrm[2];

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

	// DEV5 Mage Model
	// Define mage input layout
	D3D11_INPUT_ELEMENT_DESC mageLayout[] =
	{

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	// mage vertex shader and input layout
	hr = mageShaders.CreateVSandILFromFile(gpD3D_Device.Get(), "MAGE_VS.cso", mageLayout, ARRAYSIZE(mageLayout));

	// mage pixel shader
	hr = mageShaders.CreatePSFromFile(gpD3D_Device.Get(), "MAIN_PS.cso");

	// FBX loading
	// Initialize FBX
	InitFBX();

	// Mage file name
	std::string mageTexFile;

	// Load FBX
	LoadFBX(".//MageAssets//Run.fbx", mageMesh, mageTexFile);

	// mage vertex and index buffers
	hr = mageBuffers.CreateBuffers(gpD3D_Device.Get(), mageMesh.indexList, mageMesh.vertexList);

	// mage constant buffer
	mageShaders.CreateVSConstantBuffer(gpD3D_Device.Get(), sizeof(ConstantBuffer));
	mageShaders.PS_ConstantBuffer = mageShaders.VS_ConstantBuffer;

	// mage textures
	mageDiffuseMaterial.CreateTextureFromFile(gpD3D_Device.Get(), ".//MageAssets//MageTexture.dds");
	mageSpecularMaterial.CreateTextureFromFile(gpD3D_Device.Get(), ".//MageAssets//MageSpecular.dds");
	mageEmissiveMaterial.CreateTextureFromFile(gpD3D_Device.Get(), ".//MageAssets//MageEmissive.dds");

	// mage sampler state
	mageDiffuseMaterial.CreateDefaultSampler(gpD3D_Device.Get());
	mageSpecularMaterial.CreateDefaultSampler(gpD3D_Device.Get());
	mageEmissiveMaterial.CreateDefaultSampler(gpD3D_Device.Get());

	// Create skeleton layout
	D3D11_INPUT_ELEMENT_DESC skeletonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = skeletonShaderController.CreateVSandILFromFile(gpD3D_Device.Get(), "GRID_VS.cso", skeletonLayout, ARRAYSIZE(skeletonLayout));
	hr = skeletonShaderController.CreatePSFromFile(gpD3D_Device.Get(), "GRID_PS.cso");

	// Create skeleton constant and vertex buffers
	GenerateBonesFromTransforms(TransformJointArray);
	skeletonShaderController.CreateVSConstantBuffer(gpD3D_Device.Get(), sizeof(GridConstantBuffer));
	skeletonBufferController.CreateVertexBuffer(gpD3D_Device.Get(), skeletonBones);
	skeletonBufferController.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

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
	gpD3D_Device.Get()->CreateRasterizerState(&skyboxRasterState, gpSkyboxRasterState.GetAddressOf());

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