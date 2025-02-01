#pragma once
#ifndef _SKELETON_H_
#define _SKELETON_H_

#include <fbxsdk.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
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
	void ExtractBindpose(FbxScene* scene);
	void ExtractBindposeByAnimation(Bone*);
	Bone* ExtractSkeleton(FbxNode* pNode);
	void ExtractAnimation(FbxScene* lScene, Bone* bone, std::map<std::string, std::vector<FbxAMatrix>>& animationData);
	void ContructVisualBone(LineRendererClass*);
	void TickAnimation(int frame);
	void TickBones(Bone*, int);
	vector<XMMATRIX> GetAnimatedMatrixesByFrame(int);
	const int GetAnimationFrame() { return m_animationFrame; };


private:
	void UpdateParents(Bone* bone);
	XMFLOAT4 MultiplyFbxMatrixWithXMFLOAT4(const FbxMatrix& fbxMatrix, const XMFLOAT4& vector);
	void UpdateFinalyMatrixes();
	XMMATRIX ConvertFbxAMatrixToXMMATRIX(const FbxAMatrix& fbxMatrix);

private:
	//tree structure
	Bone* m_root;
	//map structure
	unordered_map<string, Bone*> m_boneLookUp;

	std::map<std::string, std::vector<FbxAMatrix>> m_animationData;
	int m_animationFrame;

	std::map<std::string, std::vector<FbxAMatrix>> m_animatedBoneMulInvBindpose;

	// This structure is bone in order and the index in vector is exactly the same as its index in cbuuffer.
	vector<Bone*> m_orderedBone;
};



#endif // !_SKELETON

