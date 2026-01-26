#pragma once
#include <KamataEngine.h>

class Player;

class CameraController {

public:
	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	void SetMovableArea(Rect area) { movableArea_ = area; }

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera camera_;

	Player* target_ = nullptr;

	KamataEngine::Vector3 target0ffset_ = {0, 0, -15.0f};

	KamataEngine::Vector3 targetPosition_;

	static inline const float kInterpolationRate = 0.1f;

	static inline const float kVelocityBias = 10;

	Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};

	Rect movableArea_ = {0, 100, 0, 100};
};
