#pragma once
#ifndef _SKINNEDSHADERCLASS_H_
#define _SKINNEDSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "MyDebugger.h"

using namespace DirectX;
using namespace std;

class SkinnedShaderClass
{
public:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX boneMatrices[10];
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};
	SkinnedShaderClass();
	~SkinnedShaderClass();
	//1. compile shaders.
	//2. create resource description for shaders. 
	// like the vertex input structure, constant buffer description, texture RV, sampler info.
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	//1. update shader parameter, 
	//2. set everything about this shader to context.
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};



#endif // !_SKINNEDSHADERCLASS_H_
