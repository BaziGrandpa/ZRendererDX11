#include "AnimationController.h"


AnimationController::AnimationController(SkeletonClass* skeleton, LineRendererClass* lineRenderer)
{
	m_skeleton = skeleton;
	m_lineRenderer = lineRenderer;
	m_isPlaying = false;
	m_startTime = 0;
	m_lastTime = 0;
	m_frameIndex = 0;
	m_totalFrame = skeleton->GetAnimationFrame();
}

AnimationController::~AnimationController()
{
}

int64_t AnimationController::GetCurrentTimeMillis() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
	).count();
}

void AnimationController::Play() {
	m_isPlaying = true;

	m_startTime = GetCurrentTimeMillis();
}

void AnimationController::Tick() {
	if (!m_isPlaying)
		return;
	int64_t currentTime = GetCurrentTimeMillis();
	int64_t deltaTime = currentTime - m_lastTime;

	// Time per frame at 24 FPS (41.67ms per frame)
	const int64_t frameDuration = 1000 / m_fps; // 41.67ms

	if (deltaTime >= frameDuration) {
		m_frameIndex++;  // Advance the animation frame
		m_frameIndex %= m_totalFrame;
		m_lastTime = currentTime; // Reset last time to prevent frame skips
		TickSkeleton(m_frameIndex);

	}
}

void AnimationController::TickSkeleton(int frame) {
	m_skeleton->TickAnimation(frame);
	m_lineRenderer->ClearLines();
	// must called after Tick, to get the updated information.
	m_skeleton->ContructVisualBone(m_lineRenderer);
}


void AnimationController::Stop() {
	m_isPlaying = false;

}

