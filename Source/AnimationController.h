#pragma once
#ifndef  _ANIMATIONCONTROLLER_H
#define _ANIMATIONCONTROLLER_H

#include "Skeleton.h"
#include <iostream>
#include <chrono>
#include "LineRendererClass.h"

class AnimationController
{
public:
	AnimationController(SkeletonClass*, LineRendererClass*);
	~AnimationController();
	void Play();
	void Tick();
	void Stop();

private:
	int64_t GetCurrentTimeMillis();
	void TickSkeleton(int frame);
private:

	int m_totalFrame = 0;
	const int m_fps = 24;
	int64_t m_startTime = 0;
	int64_t m_lastTime = 0;
	int m_frameIndex = 0;
	bool m_isPlaying;

	SkeletonClass* m_skeleton;
	LineRendererClass* m_lineRenderer;
};


#endif // ! _ANIMATIONCONTROLLER_H

