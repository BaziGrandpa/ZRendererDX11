#pragma once

#ifndef _LINERENDERER_H_
#define _LINERENDERER_H_

#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <fstream>
#include <vector>
#include <queue>

#include "Colorshaderclass.h"

using namespace std;
using namespace DirectX;

class LineRendererClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct MyLine
	{
		XMFLOAT3 start;
		XMFLOAT3 end;
		XMFLOAT4 color;
	};

public:
	LineRendererClass();
	~LineRendererClass();

	bool Initialize(ID3D11Device*, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, ID3D11Device*, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix);
	void AddLine(XMFLOAT3, XMFLOAT3, XMFLOAT4);
	void ClearLines();

private:
	vector<MyLine> m_Lines;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	ColorShaderClass* m_ColorShader;

};

#endif
