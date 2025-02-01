#pragma once
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <fbxsdk.h>
#include <iostream>
#include <vector>
#include <queue>
using namespace DirectX;
using namespace std;
using namespace fbxsdk;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "MyDebugger.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct SkinnedVertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMUINT4  boneIndices; // Max 4 bones per vertex
		XMFLOAT4 boneWeights;    // Normalized weights
	};

	// Structure to store bone weight data per vertex
	struct VertexWeight {
		int boneIndex;
		float weight;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};


public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	bool InitializeFBX(char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void RenderSkinnedMesh(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	bool LoadModel(char*);

	void ReleaseModel();

	bool LoadFBXModel();
	bool ExtractFBXModel();
	void TraverseNode(FbxNode* node);
	void ProcessMesh(FbxMesh* mesh);
	void ProcessSkinnedMesh(FbxMesh* mesh);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	ID3D11Buffer* m_skinnedVertexBuffer, * m_skinnedIndexBuffer;

	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;

	ModelType* m_model;

	FbxScene* m_scene;

	std::vector<VertexType> m_fbxvertices;
	std::vector<ULONG> m_fbxindices;

	std::vector<SkinnedVertexType> m_fbxSkinnedVertices;
	std::vector<ULONG> m_fbxSkinnedIndices;

	std::vector<std::vector<VertexWeight>> GetVertexWeights(FbxMesh* mesh);
};

#endif