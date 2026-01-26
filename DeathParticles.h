#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include <array>
#include <numbers>

class DeathParticles {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

bool IsFinished() const { return finished_; }

	
private:
	
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_;



	static inline const uint32_t kNumParticles = 8;

	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	static inline const float kDuration = 1;

	static inline const float kSpeed = 0.1f;

	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / kNumParticles;

	bool finished_ = false;
	float counter_ = 0.0f;



};