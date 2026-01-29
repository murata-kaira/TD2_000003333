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
	KamataEngine::Sprite* arrowLeftSprite_ = nullptr;
	KamataEngine::Sprite* arrowRightSprite_ = nullptr;
	KamataEngine::Sprite* arrowUpSprite_ = nullptr;
	KamataEngine::Sprite* arrowDownSprite_ = nullptr;

	// 矢印表示用の定数
	static inline const float kScreenCenterX = 640.0f;  // 画面中央X (1280/2)
	static inline const float kScreenCenterY = 360.0f;  // 画面中央Y (720/2)
	static inline const float kArrowDistance = 80.0f;   // 画面中央からの矢印距離
	static inline const float kArrowSize = 32.0f;       // 矢印のサイズ

	// 矢印アニメーション用の定数
	static inline const float kArrowAnimationSpeed = 0.05f;  // アニメーション速度
	static inline const float kArrowMinAlpha = 0.5f;         // 最小不透明度
	static inline const float kArrowMaxAlpha = 1.0f;         // 最大不透明度

	// 矢印の表示状態
	bool shouldShowArrows_ = false;
	float arrowAnimationTimer_ = 0.0f;  // アニメーションタイマー

	


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
