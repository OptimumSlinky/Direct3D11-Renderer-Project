#pragma once

#include "framework.h"
#include <wrl\client.h>

using namespace DirectX;
using namespace std;
using Microsoft::WRL::ComPtr;

#ifndef __OBJ_VERT__
typedef struct _OBJ_VERT_
{
	float pos[3]; // Left-handed +Z forward coordinate w not provided, assumed to be 1.
	float uvw[3]; // D3D/Vulkan style top left 0,0 coordinate.
	float nrm[3]; // Provided direct from obj file, may or may not be normalized.
}OBJ_VERT;
#define __OBJ_VERT__
#endif

// Shader compile function
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

// 3DCC binary BLOB loader
vector<uint8_t> load_binary_blob(const char* path)
{
	vector<uint8_t> blob;

	fstream file{ path, ios_base::in | ios_base::binary };

	if (file.is_open())
	{
		file.seekg(0, ios_base::end);
		blob.resize(file.tellg());
		file.seekg(0, ios_base::beg);

		file.read((char*)blob.data(), blob.size());

		file.close();
	}

	return move(blob);
}

struct ShaderMaterials
{
	ComPtr<ID3D11ShaderResourceView> ResourceView = nullptr;
	ComPtr<ID3D11SamplerState> SamplerState = nullptr;

	HRESULT CreateDefaultSampler(ID3D11Device* gpu_device)
	{
		HRESULT hr = S_OK;
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = gpu_device->CreateSamplerState(&sampDesc, SamplerState.ReleaseAndGetAddressOf());
		return hr;

	}

	HRESULT CreateTextureFromFile(ID3D11Device* gpu_device, string filename)
	{
		HRESULT hr = S_OK;
		// String magic from 3DCC to convert std::string to format compatible with DDS texture loader
		wstring widestr_file = wstring(filename.begin(), filename.end());
		const wchar_t* widecstr_file = widestr_file.c_str();

		// Create the texture
		hr = CreateDDSTextureFromFile(gpu_device, widecstr_file, nullptr, ResourceView.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}
	}

	void Bind(ID3D11DeviceContext* currentContext)
	{
		// Sets texture
		if (ResourceView.Get())
		{
			currentContext->PSSetShaderResources(0, 1, ResourceView.GetAddressOf());
		}

		// Sets sampler
		if (SamplerState.Get())
		{
			currentContext->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
		}
	}
};

struct ShaderController
{
	ComPtr<ID3D11InputLayout> InputLayout = nullptr;
	ComPtr<ID3D11VertexShader> VertexShader = nullptr;
	ComPtr<ID3D11PixelShader> PixelShader = nullptr;
	ComPtr<ID3D11Buffer> VS_ConstantBuffer = nullptr;
	ComPtr<ID3D11Buffer> PS_ConstantBuffer = nullptr;

	// Create vertex shader
	HRESULT CreateVSandILFromFile(ID3D11Device* gpu_device, const char* filename, D3D11_INPUT_ELEMENT_DESC layout[], UINT numberElements)
	{
		HRESULT hr = S_OK;

		// Load shader file
		auto vs_blob = load_binary_blob(filename);

		// Create vertex shader
		hr = gpu_device->CreateVertexShader(vs_blob.data(), vs_blob.size(), nullptr, VertexShader.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}

		// Create input layout
		hr = gpu_device->CreateInputLayout(layout, numberElements, vs_blob.data(), vs_blob.size(), InputLayout.ReleaseAndGetAddressOf());
		return hr;

	}

	HRESULT CreatePSFromFile(ID3D11Device* gpu_device, const char* filename)
	{
		HRESULT hr = S_OK;

		// Load shader file
		auto vs_blob = load_binary_blob(filename);

		// Create the pixel shader
		hr = gpu_device->CreatePixelShader(vs_blob.data(), vs_blob.size(), nullptr, PixelShader.ReleaseAndGetAddressOf());
		return hr;
	}

	// Create constant buffer
	HRESULT CreateConstantBuffer(ID3D11Device* gpu_device, UINT size, ComPtr<ID3D11Buffer>& ConstantBuffer)
	{
		HRESULT hr = S_OK;
		// Create the constant buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = size;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		hr = gpu_device->CreateBuffer(&bd, nullptr, &ConstantBuffer);
		return hr;
	}

	HRESULT CreateVSConstantBuffer(ID3D11Device* gpu_device, UINT size)
	{
		return CreateConstantBuffer(gpu_device, size, VS_ConstantBuffer);
	}

	HRESULT CreatePSConstantBuffer(ID3D11Device* gpu_device, UINT size)
	{
		return CreateConstantBuffer(gpu_device, size, PS_ConstantBuffer);
	}

	void Bind(ID3D11DeviceContext* currentContext)
	{
		if (VS_ConstantBuffer)
			currentContext->VSSetConstantBuffers(0, 1, VS_ConstantBuffer.GetAddressOf());
		if (PS_ConstantBuffer)
			currentContext->PSSetConstantBuffers(0, 1, PS_ConstantBuffer.GetAddressOf());
		if (InputLayout)
			currentContext->IASetInputLayout(InputLayout.Get());
		if (VertexShader)
			currentContext->VSSetShader(VertexShader.Get(), nullptr, 0);
		if (PixelShader)
			currentContext->PSSetShader(PixelShader.Get(), nullptr, 0);
	}
};

template <typename T> struct BufferController
{
	ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	int vertexCount = 0;
	ComPtr<ID3D11Buffer> IndexBuffer = nullptr;
	int indexCount = 0;

	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT CreateBuffers(ID3D11Device* gpu_device, vector<int>& indices, vector<T>& vertices)
	{
		HRESULT hr = S_OK;
		hr = CreateIndexBuffer(gpu_device, indices);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = CreateVertexBuffer(gpu_device, vertices);
		return hr;
	}

	HRESULT CreateBuffersArray(ID3D11Device* gpu_device, const int indices[], const OBJ_VERT vertices[])
	{
		HRESULT hr = S_OK;
		hr = CreateIndexBufferArray(gpu_device, indices);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = CreateVertexBufferArray(gpu_device, vertices);
		return hr;
	}

	HRESULT CreateIndexBuffer(ID3D11Device* gpu_device, vector<int>& indices)
	{
		indexCount = (int)indices.size();
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * indexCount;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = indices.data();
		hr = gpu_device->CreateBuffer(&bd, &InitData,
			IndexBuffer.GetAddressOf());
		return hr;
	}

	HRESULT CreateIndexBufferArray(ID3D11Device* gpu_device, const int indices[])
	{
		indexCount = sizeof(indices) / sizeof(indices[0]);
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * indexCount;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = indices;
		hr = gpu_device->CreateBuffer(&bd, &InitData,
			IndexBuffer.GetAddressOf());
		return hr;
	}

	HRESULT CreateVertexBuffer(ID3D11Device* gpu_device, vector<T>& vertices)
	{
		vertexCount = (int)vertices.size();
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(T) * vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = vertices.data();
		hr = gpu_device->CreateBuffer(&bd, &InitData,
			VertexBuffer.ReleaseAndGetAddressOf());
		return hr;
	}

	HRESULT CreateVertexBufferArray(ID3D11Device* gpu_device, const OBJ_VERT vertices[])
	{
		vertexCount = sizeof(vertices) / sizeof(vertices[0]);
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(T) * vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = vertices;
		hr = gpu_device->CreateBuffer(&bd, &InitData,
			VertexBuffer.ReleaseAndGetAddressOf());
		return hr;
	}

	void Bind(ID3D11DeviceContext* currentContext)
	{
		// Set vertex buffer
		UINT stride = sizeof(T);
		UINT offset = 0;
		if (VertexBuffer)
			currentContext->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride,
				&offset);

		// Set index buffer
		if (IndexBuffer)
			currentContext->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Set primitive topology
		currentContext->IASetPrimitiveTopology(PrimitiveTopology);
	}

	void Draw(ID3D11DeviceContext* currentContext)
	{
		if (IndexBuffer)
			currentContext->DrawIndexed(indexCount, 0, 0);

		else if (VertexBuffer)
			currentContext->Draw(vertexCount, 0);
	}

	void DrawIndexed(ID3D11DeviceContext* currentContext)
	{
		if (IndexBuffer && VertexBuffer)
			currentContext->DrawIndexed(indexCount, 0, 0);
	}

	void BindAndDraw(ID3D11DeviceContext* currentContext)
	{
		Bind(currentContext);
		Draw(currentContext);
	}
};