#include "SkinnedMeshModelClass.h"
#include "MyDebugger.h"

SkinnedMeshClass::SkinnedMeshClass()
{
}

SkinnedMeshClass::~SkinnedMeshClass()
{
}

bool SkinnedMeshClass::Initialize(const char* modelPath, const char* textureFilename)
{
	FbxManager* sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(sdkManager, "");
	if (!importer->Initialize("../ZRendererDX11/Resources/Models/BoxMan.fbx", -1, sdkManager->GetIOSettings())) {
		printf("FBX Import Error: %s\n", importer->GetStatus().GetErrorString());
		return false;
	}

	m_scene = FbxScene::Create(sdkManager, "Scene");
	importer->Import(m_scene);
	importer->Destroy();

	LoadFBXBones(m_scene->GetRootNode());
	return true;
}

bool SkinnedMeshClass::LoadFBXBones(FbxNode* node)
{
	// Get the root node
	FbxNode* rootNode = m_scene->GetRootNode();
	if (!rootNode) {
		LOG_WARNING("No root node found in the scene!");
		return false;
	}

	// Look for bind pose
	FbxPose* bindPose = nullptr;
	int poseCount = m_scene->GetPoseCount();
	for (int i = 0; i < poseCount; i++) {
		FbxPose* pose = m_scene->GetPose(i);
		if (pose->IsBindPose()) {
			bindPose = pose;
			break;
		}
	}
	if (!bindPose) {
		LOG_WARNING("No bind pose found in the scene!");
	}

	// Process all child nodes from the root
	int boneIndex = 0;
	for (int i = 0; i < rootNode->GetChildCount(); i++) {
		ProcessBone(rootNode->GetChild(i), bindPose, nullptr, 0, &boneIndex);
	}


	return true;
}

void SkinnedMeshClass::ProcessBone(FbxNode* node, FbxPose* bindPose, Bone* current, int depth, int* index)
{
	if (!node)
	{
		return;
	}



	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

	//LOG_DEBUG("node: " + string(node->GetName()));

	int type = nodeAttribute->GetAttributeType();

	// Todo deal with root bone isn't eSkeleton
	if (!nodeAttribute || nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		return;
	}


	int nodeIndex = bindPose->Find(node);
	if (nodeIndex == -1) {
		// don't construct bone
		LOG_WARNING("Node not found in bind pose: " + string(node->GetName()));

	}
	else
	{
		const FbxMatrix bindPoseMatrix = bindPose->GetMatrix(nodeIndex);
		XMMATRIX bindPoseMatrixXM = FbxMatrixToXMMATRIX(bindPoseMatrix);


		if (depth == 0) {
			*index = 0;
			Bone* bone = new Bone();
			m_boneRoot = bone;
			bone->bindPoseMatrix = bindPoseMatrixXM;
			bone->name = node->GetName();
			bone->index = *index;
			current = bone;
		}
		else {
			*index = *index + 1;

			current->bindPoseMatrix = bindPoseMatrixXM;
			current->name = node->GetName();
			current->index = *index;
		}

	}

	LOG_DEBUG("node: " + string(node->GetName()) + "depth:" + to_string(depth) + " index:" + to_string(*index));



	for (int i = 0; i < node->GetChildCount(); i++) {

		Bone* child = new Bone();
		child->parent = current;
		current->children.push_back(child);
		ProcessBone(node->GetChild(i), bindPose, child, depth + 1, index);



	}


}

XMMATRIX SkinnedMeshClass::FbxMatrixToXMMATRIX(const FbxMatrix& fbxMatrix) {
	// Create a float array to store the converted values
	float matrix[4][4];

	// Convert double-precision FbxMatrix to float-precision
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			matrix[row][col] = static_cast<float>(fbxMatrix.Get(row, col));
		}
	}

	// Return as XMMATRIX
	return XMMATRIX(
		matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
		matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
		matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
		matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]
	);
}

void SkinnedMeshClass::ConstructBoneLines(LineRendererClass* lineRenderer) {
	// traverse the bone hierarchy BFS

	Bone* current = m_boneRoot;
	if (!current)
	{
		LOG_WARNING("No bone root found!");
		return;
	}
	//clear all
	lineRenderer->ClearLines();
	XMVECTOR originalPos = XMVectorSet(0, 0, 0, 1);


	queue<Bone*> boneQueue;
	boneQueue.push(current);

	float scale = 1.0f;
	while (!boneQueue.empty()) {
		Bone* current = boneQueue.front();
		boneQueue.pop();
		//LOG_DEBUG("bone name: " + current->name + " index: " + to_string(current->index));
		//construct lines 

		XMVECTOR startPos = XMVector4Transform(originalPos, current->bindPoseMatrix);

		for (int i = 0; i < current->children.size(); i++) {
			boneQueue.push(current->children[i]);

			XMVECTOR endPos = XMVector4Transform(originalPos, current->children[i]->bindPoseMatrix);

			lineRenderer->AddLine(XMFLOAT3(XMVectorGetX(startPos) * scale, XMVectorGetY(startPos) * scale, XMVectorGetZ(startPos) * scale),
				XMFLOAT3(XMVectorGetX(endPos) * scale, XMVectorGetY(endPos) * scale, XMVectorGetZ(endPos) * scale),
				XMFLOAT4(1.0, 0, 0, 1.0), true);
		}
	}

	//

}