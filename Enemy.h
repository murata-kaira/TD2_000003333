#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "Player.h"

class MapChipField;
class Player;
class Enemy {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	AABB GetAABB();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void OnCollision(const Player* player);

	 KamataEngine::Vector3 GetWorldPosition();
	
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	MapChipField* mapChipField_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	float walkTimer_ = 0.0f;

	static inline const float kWalkSpeed = 0.05f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};
