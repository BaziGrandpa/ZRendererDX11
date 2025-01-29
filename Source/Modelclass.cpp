#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
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
	//VertexType* vertices;
	//unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//// Create the vertex array.
	//vertices = new VertexType[m_vertexCount];
	//if (!vertices)
	//{
	//	return false;
	//}

	//// Create the index array.
	//indices = new unsigned long[m_indexCount];
	//if (!indices)
	//{
	//	return false;
	//}

	//// Load the vertex array and index array with data.
	//for (i = 0; i < m_vertexCount; i++)
	//{
	//	vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
	//	vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
	//	vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

	//	indices[i] = i;
	//}

	m_vertexCount = m_fbxvertices.size();
	m_indexCount = m_fbxindices.size();

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	//vertexData.pSysMem = vertices;
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
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	//indexData.pSysMem = indices;
	indexData.pSysMem = m_fbxindices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	//delete[] vertices;
	//vertices = 0;

	//delete[] indices;
	//indices = 0;

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
			float scale = 0.1f;
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
				vertex.texture = XMFLOAT2(static_cast<float>(uv[0]), static_cast<float>(1-uv[1]));
			}
			else
				vertex.texture = XMFLOAT2(0, 0);

			m_fbxvertices.push_back(vertex);
			m_fbxindices.push_back(static_cast<uint32_t>(m_fbxvertices.size() - 1));
		}
	}
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

