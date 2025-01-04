#pragma once
#ifndef _SKINNEDMESHMODEL_H_
#define _SKINNEDMESHMODEL_H_

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <fbxsdk.h>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "textureclass.h"

using namespace DirectX;
using namespace std;
using namespace fbxsdk;


class SkinnedMeshClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMINT4 boneIndices;
		XMFLOAT4 boneWeights;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	SkinnedMeshClass();
	~SkinnedMeshClass();
	// 
	// 1.load fbx bones,
	//	1.1 load bone hierarchy,and give each bone a unique index
	//	1.2 get bind pose matrix for each bone
	//  1.3 get bone offset matrix for each bone
	// 
	// 2.load fbx mesh,
	//	2.1 for each vertex, get the bone indices and bone weights
	// 
	// 3.load fbx animations, store animation curve for each bone
	//	3.1 should have a key velue pair for each bone, key is the bone index, value is the animation curve
	//	3.2 
	void Initialize(char* modelPath, char* textureFilename);

	// 1.push the mesh data to the vertex buffer and index buffer
	// 2.push the animated bone data to constant buffer 
	void Render(ID3D11DeviceContext* deviceContext);

	// 1. update the bone matrices for each bone, namely animated bone data
	void UpdateAnimation(float deltaTime);

	void Shutdown();

private:
	void LoadFBXBones(FbxNode* node);
	void LoadFBXMesh(char* modelPath);

	void LoadFBXAnimations(FbxScene* scene);

	void LoadTexture(char* textureFilename);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	TextureClass* m_Texture;

	ModelType* m_model;

	FbxScene* m_scene;
	std::vector<VertexType> m_fbxvertices;
	std::vector<ULONG> m_fbxindices;

	// for bone data
	struct Bone
	{
		int parentIndex;
		string name;
		XMFLOAT4X4 bindPoseMatrix;
		XMFLOAT3 localT;
		XMFLOAT3 localR;
		XMFLOAT3 localS;
	};

	unordered_map<int, Bone> m_bones;

	// updated every tick, push to constant buffer, 
	// the ID would be the same as the bone index
	// also the same as VertexType::boneIndices
	vector<XMFLOAT4X4> m_boneMatrices;

	// also construct a bone tree.


};



#endif