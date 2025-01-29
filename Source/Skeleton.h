#pragma once
#ifndef _SKELETON_H_
#define _SKELETON_H_

#include <fbxsdk.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <directxmath.h>
#include "LineRendererClass.h"

class SkeletonClass
{
public:
	struct Bone
	{
		std::string name;
		FbxNode* node;
		Bone* parent;
		FbxAMatrix bindPose;
		std::vector<Bone*> children;
		DirectX::XMFLOAT4 localPos;
		//when update animation, each frame differs, but maybe don't need this, let's see
		FbxMatrix currenFrameM;
		//final animated matrix, M_obj2world * M_bone2obj * M_bindpose
		FbxMatrix animatedVertex2Object;
	};

public:
	SkeletonClass();
	~SkeletonClass();

	void InitSkeletonRoot(FbxNode* pNode);
	void InitAnimationData(FbxScene* scene);
	void ExtractBindpose(Bone*);
	Bone* ExtractSkeleton(FbxNode* pNode);
	void ExtractAnimation(FbxScene* lScene, Bone* bone, std::map<std::string, std::vector<FbxAMatrix>>& animationData);
	void ContructVisualBone(LineRendererClass*);
	void TickAnimation(int frame);
	void TickBones(Bone*, int);
	const int GetAnimationFrame() { return m_animationFrame; };

private:
	void UpdateParents(Bone* bone);
	XMFLOAT4 MultiplyFbxMatrixWithXMFLOAT4(const FbxMatrix& fbxMatrix, const XMFLOAT4& vector);

private:
	Bone* m_root;
	std::map<std::string, std::vector<FbxAMatrix>> m_animationData;
	int m_animationFrame;
};



#endif // !_SKELETON

