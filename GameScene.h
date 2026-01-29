#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>
#include"Fade.h"
#include "Goal.h"



class GameScene {


	KamataEngine::Model* modelBlock_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Model* modelPlayer_ = nullptr;

	
	KamataEngine::Model* modelDeathParticles_ = nullptr;

	KamataEngine::Model* modelGoal_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Camera camera_;

	Player* player_ = nullptr;

	bool isDebugCameraActive_ = false;


	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	
	Skydome* skydome_ = nullptr;

	MapChipField* mapChipField_;

	CameraController* cameraController_;

	DeathParticles* deathParticles_ = nullptr;

	Goal* goal_ = nullptr;

	enum class Phase {
		kPlay,
		kDeath,
		kFadeIn,
		kFadeOut,
		kGoalReached
	};

	Phase phase_;

	bool finished_ = false;

	Fade* fade_ = nullptr;


	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//スプライト
	KamataEngine::Sprite* sprite_ = nullptr;

	// 照準調整用の矢印スプライト
	uint32_t arrowUpTextureHandle_ = 0;
	uint32_t arrowDownTextureHandle_ = 0;
	uint32_t arrowLeftTextureHandle_ = 0;
	uint32_t arrowRightTextureHandle_ = 0;
	KamataEngine::Sprite* arrowUpSprite_ = nullptr;
	KamataEngine::Sprite* arrowDownSprite_ = nullptr;
	KamataEngine::Sprite* arrowLeftSprite_ = nullptr;
	KamataEngine::Sprite* arrowRightSprite_ = nullptr;

	


public:
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	bool IsFnished() const { return finished_; }
};
