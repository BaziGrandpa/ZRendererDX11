#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_skinnedVertexBuffer = 0;
	m_skinnedIndexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{
	bool result;

	result = InitializeFBX(modelFilename);
	if (!result)
	{
		return false;
	}

	// Load the FBX model.
	result = ExtractFBXModel();
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::InitializeFBX(char* modelFilename)
{
	FbxManager* sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(sdkManager, "");
	if (!importer->Initialize(modelFilename, -1, sdkManager->GetIOSettings())) {
		printf("FBX Import Error: %s\n", importer->GetStatus().GetErrorString());
		return false;
	}

	m_scene = FbxScene::Create(sdkManager, "Scene");



	importer->Import(m_scene);
	importer->Destroy();

	//fbxsdk::FbxAxisSystem directXSystem(fbxsdk::FbxAxisSystem::Motionbuilder);
	//directXSystem.ConvertScene(m_scene);
	return true;
}

void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

void ModelClass::RenderSkinnedMesh(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(SkinnedVertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_skinnedVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_skinnedIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{

	HRESULT result;

	m_vertexCount = m_fbxvertices.size();
	m_indexCount = m_fbxindices.size();
	int skinnedVCount = m_fbxSkinnedVertices.size();
	int skinnedICount = m_fbxSkinnedIndices.size();

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_fbxvertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = m_fbxindices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//3. Set up the description of the skinned vertex buffer.
	D3D11_BUFFER_DESC skinnedVertexBufferDesc;
	skinnedVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	skinnedVertexBufferDesc.ByteWidth = sizeof(SkinnedVertexType) * m_vertexCount;
	skinnedVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	skinnedVertexBufferDesc.CPUAccessFlags = 0;
	skinnedVertexBufferDesc.MiscFlags = 0;
	skinnedVertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA skinnedVertexData;
	skinnedVertexData.pSysMem = m_fbxSkinnedVertices.data();
	skinnedVertexData.SysMemPitch = 0;
	skinnedVertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&skinnedVertexBufferDesc, &skinnedVertexData, &m_skinnedVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//4. Set up the description of the skinned index buffer.
	D3D11_BUFFER_DESC skinnedIndexBufferDesc;
	skinnedIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	skinnedIndexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
	skinnedIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	skinnedIndexBufferDesc.CPUAccessFlags = 0;
	skinnedIndexBufferDesc.MiscFlags = 0;
	skinnedIndexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA skinnedIndexData;
	skinnedIndexData.pSysMem = m_fbxSkinnedIndices.data();
	skinnedIndexData.SysMemPitch = 0;
	skinnedIndexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&skinnedIndexBufferDesc, &skinnedIndexData, &m_skinnedIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}



	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if (m_skinnedIndexBuffer) {
		m_skinnedIndexBuffer->Release();
		m_skinnedIndexBuffer = 0;
	}

	if (m_skinnedVertexBuffer) {
		m_skinnedVertexBuffer->Release();
		m_skinnedVertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// Create and initialize the texture object.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

void ModelClass::TraverseNode(FbxNode* node) {
	if (FbxMesh* mesh = node->GetMesh()) {
		ProcessMesh(mesh);
		ProcessSkinnedMesh(mesh);
		LOG_DEBUG(mesh->GetName());
	}

	for (int i = 0; i < node->GetChildCount(); i++) {
		TraverseNode(node->GetChild(i));
	}
}

// Extract mesh data
void ModelClass::ProcessMesh(FbxMesh* mesh) {
	int vertexCount = mesh->GetControlPointsCount();

	//actual vertexes position,osnormal,uv data
	FbxVector4* controlPoints = mesh->GetControlPoints();

	FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
	FbxGeometryElementUV* uvElement = mesh->GetElementUV();


	int polygonCount = mesh->GetPolygonCount();


	for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
		// in each triangle.
		int polygonSize = mesh->GetPolygonSize(polygonIndex);
		if (polygonSize != 3)
			LOG_WARNING("Polygon size is not 3");
		for (int vertexIndex = 0; vertexIndex < polygonSize; vertexIndex++) {

			int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
			FbxVector4 position = controlPoints[controlPointIndex];

			VertexType vertex;
			float scale = 1.0f;
			vertex.position = XMFLOAT3(static_cast<float>(position[0]) * scale,
				static_cast<float>(position[1]) * scale,
				static_cast<float>(position[2]) * scale);

			if (normalElement) {
				FbxVector4 normal;
				mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal);
				vertex.normal = XMFLOAT3(static_cast<float>(normal[0]), static_cast<float>(normal[1]), static_cast<float>(normal[2]));
			}
			else
				vertex.normal = XMFLOAT3(0, 0, 0);

			if (uvElement) {
				FbxVector2 uv;
				bool unmapped;
				mesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvElement->GetName(), uv, unmapped);
				vertex.texture = XMFLOAT2(static_cast<float>(uv[0]), static_cast<float>(1 - uv[1]));
			}
			else
				vertex.texture = XMFLOAT2(0, 0);

			m_fbxvertices.push_back(vertex);
			m_fbxindices.push_back(static_cast<uint32_t>(m_fbxvertices.size() - 1));
		}
	}
}

// Extract skinned mesh data
void ModelClass::ProcessSkinnedMesh(FbxMesh* mesh) {
	int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount <= 0)
	{

		LOG_DEBUG("No skin found for " + string(mesh->GetName()));
		return;
	}

	LOG_DEBUG("skin count" + std::to_string(skinCount));
	vector<vector<ModelClass::VertexWeight>> vertexIndex2Weights = GetVertexWeights(mesh);


	FbxVector4* controlPoints = mesh->GetControlPoints();
	FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();
	FbxGeometryElementUV* uvElement = mesh->GetElementUV();
	int polygonCount = mesh->GetPolygonCount();

	for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
		int polygonSize = mesh->GetPolygonSize(polygonIndex);
		if (polygonSize != 3)
			LOG_WARNING("Polygon size is not 3");
		for (int vertexIndex = 0; vertexIndex < polygonSize; vertexIndex++) {

			int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);

			FbxVector4 position = controlPoints[controlPointIndex];

			// better explicitly declare
			SkinnedVertexType vertex{};

			vertex.position = XMFLOAT3(static_cast<float>(position[0]),
				static_cast<float>(position[1]),
				static_cast<float>(position[2]));

			//handness conversion!!!!
			vertex.position.z *= -1;

			if (normalElement) {
				FbxVector4 normal;
				mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal);
				vertex.normal = XMFLOAT3(static_cast<float>(normal[0]), static_cast<float>(normal[1]), static_cast<float>(normal[2]));
			}
			else
				vertex.normal = XMFLOAT3(0, 0, 0);

			if (uvElement) {
				FbxVector2 uv;
				bool unmapped;
				mesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvElement->GetName(), uv, unmapped);
				vertex.texture = XMFLOAT2(static_cast<float>(uv[0]), static_cast<float>(1 - uv[1]));
			}
			else
				vertex.texture = XMFLOAT2(0, 0);


			//so now 
			auto bonesAndWeights = vertexIndex2Weights[controlPointIndex];
			for (size_t i = 0; i < bonesAndWeights.size(); i++)
			{
				int boneIndex = bonesAndWeights[i].boneIndex;
				float boneWeight = bonesAndWeights[i].weight;
				// bonesAndWeights.size() maximum 4

				switch (i)
				{
				case 0:
					vertex.boneWeights.x = boneWeight;
					vertex.boneIndices.x = boneIndex;
					break;
				case 1:
					vertex.boneWeights.y = boneWeight;
					vertex.boneIndices.y = boneIndex;
					break;
				case 2:
					vertex.boneWeights.z = boneWeight;
					vertex.boneIndices.z = boneIndex;
					break;
				case 3:
					vertex.boneWeights.w = boneWeight;
					vertex.boneIndices.w = boneIndex;
					break;
				default:
					break;
				}

			}

			m_fbxSkinnedVertices.push_back(vertex);
			m_fbxSkinnedIndices.push_back(static_cast<uint32_t>(m_fbxSkinnedVertices.size() - 1));
		}
	}
}

vector<vector<ModelClass::VertexWeight>> ModelClass::GetVertexWeights(FbxMesh* mesh) {

	//the index of outer vector would be vertex index.
	// 0 {boneindex1,weight1},{boneindex2,weight2}...
	// 1 {boneindex1,weight1},{boneindex2,weight2}...
	// ...
	vector<vector<ModelClass::VertexWeight>> result(mesh->GetControlPointsCount());

	int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);

	for (size_t i = 0; i < skinCount; i++)
	{
		// get weights information from skin.
		fbxsdk::FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin);

		int clusterCount = skin->GetClusterCount();
		//LOG_DEBUG("cluster count" + to_string(clusterCount));
		for (size_t j = 0; j < clusterCount; j++)
		{
			fbxsdk::FbxCluster* cluster = skin->GetCluster(j);
			FbxNode* boneNode = cluster->GetLink(); // The joint that influences vertices
			string boneName = boneNode->GetName();
			//Warning!!!!!!
			//this is a temp solution because it require bone's name convention!!!!
			int boneIndex = boneName.back() - '0';
			//let index start from 0
			boneIndex -= 1;
			LOG_DEBUG("bone id:" + to_string(boneIndex));
			if (boneIndex > 10)
				LOG_WARNING("Bone index convert error!!");

			//an array of indeces
			int* ctlPointsIndices = cluster->GetControlPointIndices();
			int indexCount = cluster->GetControlPointIndicesCount();
			double* ctlPointWeights = cluster->GetControlPointWeights();

			for (size_t k = 0; k < indexCount; k++)
			{
				int vertexIndex = ctlPointsIndices[k];
				float vertexWeightOfBone = (float)ctlPointWeights[k];
				result[vertexIndex].push_back({ boneIndex,vertexWeightOfBone });
			}

		}
	}

	for (size_t vertexIndex = 0; vertexIndex < result.size(); vertexIndex++)
	{
		float totalWeight = 0;
		for (size_t i = 0; i < result[vertexIndex].size(); i++)
		{
			auto verteexWeight = result[vertexIndex][i];
			totalWeight += verteexWeight.weight;
		}
		float delta = 1 - totalWeight;
		if (delta >= 0.05) {
			LOG_DEBUG("weight need:" + to_string(delta));
		}
	}

	return result;
}



bool ModelClass::LoadFBXModel()
{
	FbxNode* root = m_scene->GetRootNode();
	if (!root) {
		return false;
	}


	// Process mesh
	FbxMesh* mesh = nullptr;

	std::queue<FbxNode*> nodeQueue;
	nodeQueue.push(root);

	// try find a mesh 
	while (!nodeQueue.empty()) {
		FbxNode* current = nodeQueue.front();
		nodeQueue.pop();

		// find mesh
		mesh = current->GetMesh();

		if (mesh) {
			break;
		}

		// add all child
		int childCount = current->GetChildCount();
		for (int i = 0; i < childCount; i++) {
			nodeQueue.push(current->GetChild(i));
		}

	}

	//can't find mesh 
	if (!mesh)
		return false;

	const char* meshName = mesh->GetName();

	LOG_DEBUG(string("Mesh name: ") + meshName);

	if (mesh) {
		ProcessMesh(mesh);
	}

	//for (int i = 0; i < node->GetChildCount(); i++) {
	//	ProcessNode(node->GetChild(i), vertices, indices);
	//}



	return true;
}

// Load all the fbx vertex data in.
bool ModelClass::ExtractFBXModel() {
	FbxNode* root = m_scene->GetRootNode();
	if (!root) {
		return false;
	}

	TraverseNode(root);
	return true;
}

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}

