#pragma once
#include "KamataEngine.h"

class Goal {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	KamataEngine::Vector3 GetWorldPosition() const;

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// Collision detection radius for reaching the goal
	static inline const float kRadius = 1.5f;

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	float rotationSpeed_ = 0.02f; // Rotation speed for visual effect
};