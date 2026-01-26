#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include"Enemy.h"

class MapChipField;
class Enemy;
class Player {

public:

	//ゴルフゲームの状態
	enum class State {
		Idle,     //ボールが停止中、角度選択待ち 
		Charging, //チャージ可能
		Moving    //ボールが移動中
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	AABB GetAABB();

	void OnCollision(const Enemy* enemy);

	 KamataEngine::Vector3 GetWorldPosition();


	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	bool IsDead() const { return isDead_; }

	// ゴルフゲーム用
	State GetState() const { return state_; }
	float GetChargePower() const { return chargePower_; }
	float GetAimAngle() const { return aimAngle_; }
	int GetShotCount() const { return shotCount_; }



private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	MapChipField* mapChipField_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	
	

	static inline const float kAcceleration = 1.0;

	static inline const float kAttenuation = 0.5;

	static inline const float kLimitRunSpeed = 0.5;

	static inline const float kTimeTurn = 0.3f;

	static inline const float kGravityAcceleration = 0.1f;

	static inline const float kLimitFallSpeed = 1.0;

	static inline const float kJumpAcceleration = 1.0f;

	static inline const float kWidth = 0.8f;

	static inline const float kHeight = 0.8f;

	bool onGround_ = true;


	// ゴルフゲームの状態
	State state_ = State::Idle;// 現在の状態
	float chargePower_ = 0.0f; //現在のチャージパワー 
	float aimAngle_ = 0.0f;    //照準方向 
	int shotCount_ = 0;        //ショット数

	//ゴルフゲーム用定数
	static inline const float kMaxChargePower = 2.0f;  //最大発射速度
	static inline const float kChargeSpeed = 0.02f;    //チャージ速度
	static inline const float kFriction = 0.98f;       //摩擦係数 
	static inline const float kMinVelocity = 0.01f;    //停止判定の速度閾値
	static inline const float kAimRotateSpeed = 0.05f; //照準回転速度








	enum class LRDirection {
		kRight,
		kLeft,
	};

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		KamataEngine::Vector3 move;
	};

	bool isDead_ = false;

	void InputMove();

	
	void InputGolf();

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);

	void CheckMapCollisionDown(CollisionMapInfo& info);

	void CheckMapCollisionRight(CollisionMapInfo& info);

	void CheckMapCollisionLeft(CollisionMapInfo& info);



	void CheckMapMove(const CollisionMapInfo& info);

	void CheckMapCeiling(const CollisionMapInfo& info);

	void CheckMapWall(const CollisionMapInfo& info);


	void CheckMapLanding(const CollisionMapInfo& info);


	void AnimateTurn();

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner
	};

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	static inline const float kBlank = 0.1f;

	static inline const float kAttenuationLanding = 0.5f;

	static inline const float kGroundSearchHeight = 0.1f;

	static inline const float kAttenuationWall = 0.5f;
};
