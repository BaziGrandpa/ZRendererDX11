#include "Skeleton.h"
#include "MyDebugger.h"

SkeletonClass::SkeletonClass()
{
}

SkeletonClass::~SkeletonClass()
{
}

void SkeletonClass::InitSkeletonRoot(FbxNode* pNode) {
	m_root = ExtractSkeleton(pNode);
	if (m_root) {
		LOG_DEBUG(string("Found Root: ") + m_root->name);
		//no need to update, because now we have GetNodeGlobalTransform, to transform in to obj space.
		UpdateParents(m_root);
	}

}
void SkeletonClass::UpdateParents(Bone* parent) {
	for (int i = 0; i < parent->children.size(); i++)
	{
		Bone* child = parent->children[i];
		child->parent = parent;

		UpdateParents(child);

	}


}
XMFLOAT4 SkeletonClass::MultiplyFbxMatrixWithXMFLOAT4(const FbxMatrix& fbxMatrix, const XMFLOAT4& vector) {
	// Convert FbxMatrix to XMMATRIX
	XMMATRIX xmMatrix = XMMATRIX(
		static_cast<float>(fbxMatrix[0][0]), static_cast<float>(fbxMatrix[0][1]), static_cast<float>(fbxMatrix[0][2]), static_cast<float>(fbxMatrix[0][3]),
		static_cast<float>(fbxMatrix[1][0]), static_cast<float>(fbxMatrix[1][1]), static_cast<float>(fbxMatrix[1][2]), static_cast<float>(fbxMatrix[1][3]),
		static_cast<float>(fbxMatrix[2][0]), static_cast<float>(fbxMatrix[2][1]), static_cast<float>(fbxMatrix[2][2]), static_cast<float>(fbxMatrix[2][3]),
		static_cast<float>(fbxMatrix[3][0]), static_cast<float>(fbxMatrix[3][1]), static_cast<float>(fbxMatrix[3][2]), static_cast<float>(fbxMatrix[3][3])
	);

	// Convert XMFLOAT4 to XMVECTOR
	XMVECTOR xmVector = XMVectorSet(vector.x, vector.y, vector.z, vector.w);

	// Perform Matrix-Vector multiplication
	XMVECTOR resultVector = XMVector4Transform(xmVector, xmMatrix);

	// Store result back into XMFLOAT4
	XMFLOAT4 result;
	XMStoreFloat4(&result, resultVector);

	return result;
}

//construct, parent to child bone position
//for start point, the position should be parent.M_b2o*current.local 
//for end point, must be current.M_b2o*child.local
void SkeletonClass::ContructVisualBone(LineRendererClass* lines) {
	if (!m_root)
		return;

	queue<Bone*> boneQueue;
	boneQueue.push(m_root);

	while (!boneQueue.empty())
	{
		Bone* current = boneQueue.front();
		boneQueue.pop();
		for (size_t i = 0; i < current->children.size(); i++)
		{
			XMFLOAT4 startPos = current->localPos;

			//m_root don't need to transform,
			//because there no root translation animation
			if (current != m_root) {
				Bone* parent = current->parent;
				FbxMatrix fbxMatrix = parent->animatedVertex2Object;
				startPos = MultiplyFbxMatrixWithXMFLOAT4(fbxMatrix, startPos);


			}

			//
			Bone* child = current->children[i];
			XMFLOAT4 endPos = child->localPos;
			FbxMatrix bone2object = current->animatedVertex2Object;
			endPos = MultiplyFbxMatrixWithXMFLOAT4(bone2object, endPos);


			lines->AddLine(XMFLOAT3(startPos.x, startPos.y, startPos.z), XMFLOAT3(endPos.x, endPos.y, endPos.z), XMFLOAT4(1, 1, 1, 1));
			boneQueue.push(child);
		}
	}
}

SkeletonClass::Bone* SkeletonClass::ExtractSkeleton(FbxNode* pNode) {
	if (pNode->GetNodeAttribute() && pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
		LOG_DEBUG(string("Found skeleton joint: ") + pNode->GetName());
		Bone* bone = new Bone();
		bone->name = pNode->GetName();
		bone->node = pNode;
		FbxVector4 translation = pNode->LclTranslation.Get();
		string output = to_string(translation[0]) + " " + to_string(translation[1]) + " " + to_string(translation[2]);
		LOG_DEBUG(string("translation:" + output));

		//first put in the local data, 
		bone->localPos = DirectX::XMFLOAT4(
			static_cast<float>(translation[0]),
			static_cast<float>(translation[1]),
			static_cast<float>(translation[2]),
			static_cast<float>(translation[3])
		);

		for (int i = 0; i < pNode->GetChildCount(); i++) {
			Bone* child = ExtractSkeleton(pNode->GetChild(i));
			if (child) {
				bone->children.push_back(child);
			}
		}



		return bone;
	}

	for (int i = 0; i < pNode->GetChildCount(); i++) {
		Bone* bone = ExtractSkeleton(pNode->GetChild(i));
		if (bone) return bone;
	}

	return nullptr;

}

void SkeletonClass::InitAnimationData(FbxScene* scene) {
	if (!m_root) {
		LOG_WARNING("no bone root found! please check.");
		return;
	}
	ExtractAnimation(scene, m_root, m_animationData);
	//Because there is no explicit poses stored inside fbx.
	//regard the first frame of anim as BindPose 

	ExtractBindpose(m_root);
}


void SkeletonClass::ExtractAnimation(FbxScene* lScene, Bone* bone, std::map<std::string, std::vector<FbxAMatrix>>& animationData) {
	fbxsdk::FbxAnimStack* lAnimStack = lScene->GetSrcObject<fbxsdk::FbxAnimStack>(0);
	if (!lAnimStack) return;

	FbxAnimLayer* lAnimLayer = lAnimStack->GetSrcObject<FbxAnimLayer>(0);
	if (!lAnimLayer) return;

	FbxTime lTime;
	fbxsdk::FbxAnimEvaluator* lAnimEvaluator = lScene->GetAnimationEvaluator();

	int totalFrames = static_cast<int>(lAnimStack->GetLocalTimeSpan().GetDuration().GetFrameCount(FbxTime::eFrames24));
	m_animationFrame = totalFrames;
	for (int frame = 0; frame < totalFrames; frame++) { // Example: 100 frames
		lTime.SetFrame(frame, FbxTime::eFrames24);
		//GetNodeGlobalTransform, the global here means global in DCC, but back in engine, is still object space.
		FbxAMatrix globalTransform = lAnimEvaluator->GetNodeGlobalTransform(bone->node, lTime);
		animationData[bone->name].push_back(globalTransform);
	}

	for (Bone* child : bone->children) {
		ExtractAnimation(lScene, child, animationData);
	}
}

void SkeletonClass::TickAnimation(int frame) {
	TickBones(m_root, frame);
}

void SkeletonClass::TickBones(Bone* bone, int frame) {
	// update
	const string boneName = bone->name;
	FbxMatrix animatedBoneToObj = m_animationData.at(boneName)[frame];

	bone->animatedVertex2Object = animatedBoneToObj;


	for (size_t i = 0; i < bone->children.size(); i++)
	{
		TickBones(bone->children[i], frame);
	}
}

void SkeletonClass::ExtractBindpose(Bone* bone) {

	string boneName = bone->name;
	auto firstFrame = m_animationData.at(boneName)[0];
	bone->bindPose = firstFrame;
	for (Bone* child : bone->children) {
		ExtractBindpose(child);
	}

}



