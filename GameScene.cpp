#include "GameScene.h"
#include "Math.h"
#include <numbers>
#include <cmath>

using namespace KamataEngine;
// デストラクト
GameScene::~GameScene() {
	delete modelBlock_;
	delete player_;
	delete debugCamera_;
	delete modelSkydome_;
	delete skydome_;
	delete modelPlayer_;
	delete mapChipField_;
	
	delete modelDeathParticles_;
	delete fade_;

	//ゴール
	delete goal_;
	delete modelGoal_;
	//矢印
	delete sprite_;
	delete aimArrowSprite_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

}

void GameScene::Initialize() {

	modelBlock_ = Model::CreateFromOBJ("block", true);

	modelPlayer_ = Model::CreateFromOBJ("player", true);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);

	debugCamera_ = new DebugCamera(1280, 720);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	player_ = new Player();

	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	
	player_->SetMapChipField(mapChipField_);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	worldTransform_.Initialize();
	camera_.Initialize();

	GenerateBlocks();

	// deathParticles_ = new DeathParticles;
	// deathParticles_->Initialize(modelDeathParticles_, &camera_, playerPosition);

	phase_ = Phase::kFadeIn;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	//追加した部分
	////////////////////////////////////////
	 
	// ゴールにはブロックモデルを使用（後で変更する
	modelGoal_ = Model::CreateFromOBJ("cube", true);

	// ゴールをステージの奥の位置に配置
	Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(80, 18);
	goal_ = new Goal();
	goal_->Initialize(modelGoal_, &camera_, goalPosition);

	// ファイルからテクスチャを読み込む
	textureHandle_ = TextureManager::Load("sirusi.png");
	sprite_ = Sprite::Create(textureHandle_, {playerPosition.x, playerPosition.y});
	
		// 照準調整用の矢印スプライトを作成
	// 照準方向を示す矢印スプライトを作成（同じテクスチャを使用）
	aimArrowSprite_ = Sprite::Create(textureHandle_, {-100, -100}); // 初期位置はオフスクリーン
	
	// 矢印のサイズを設定
	aimArrowSprite_->SetSize({kArrowSize, kArrowSize});

	// 矢印の表示状態を初期化
	shouldShowArrows_ = false;
	arrowAnimationTimer_ = 0.0f;

	///////////////////////////////////////
}

void GameScene::Update() {

	fade_->Update();

	player_->Update();
	debugCamera_->Update();
	cameraController_->Update();

	if (goal_) {
		goal_->Update();
	}



	if (deathParticles_) {
		deathParticles_->Update();
	}



	
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		camera_.TransferMatrix();
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->TransferMatrix();
		}
	}

	CheckAllCollisions();
	ChangePhase();


	//追加した部分
	///////////////////////////////////////
	
	// 照準調整中に矢印の位置を更新
	Player::State currentState = player_ ? player_->GetState() : Player::State::Moving;
	shouldShowArrows_ = player_ && (currentState == Player::State::Idle || currentState == Player::State::Charging);
	if (shouldShowArrows_) {
		// プレイヤーの照準角度を取得
		float aimAngle = player_->GetAimAngle();

		// 照準方向に矢印を配置（画面中央から指定距離）
		float arrowX = kScreenCenterX + std::cos(aimAngle) * kArrowDistance;
		float arrowY = kScreenCenterY - std::sin(aimAngle) * kArrowDistance; // Y軸は下向きが正なので反転
		aimArrowSprite_->SetPosition({arrowX, arrowY});

		// 矢印を照準方向に回転（上向きが基準なので90度調整）
		aimArrowSprite_->SetRotation(-aimAngle + std::numbers::pi_v<float> / 2.0f);
		// 矢印のパルスアニメーション
		arrowAnimationTimer_ += kArrowAnimationSpeed;

		// タイマーを2πの範囲内に保つ（浮動小数点の精度を維持）
		arrowAnimationTimer_ = std::fmod(arrowAnimationTimer_, 2.0f * std::numbers::pi_v<float>);

		float alpha = kArrowMinAlpha + (kArrowMaxAlpha - kArrowMinAlpha) * (std::sin(arrowAnimationTimer_) * 0.5f + 0.5f);

		// 矢印にアルファ値を適用
		aimArrowSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
	} else {
		// 非表示時はタイマーをリセット
		arrowAnimationTimer_ = 0.0f;

	}

	///////////////////////////////////////
	

}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	player_->Draw();

	skydome_->Draw();


	///////////////////////////////////////
	// ゴールの描画
		if (goal_) {
		goal_->Draw();
	}
    ///////////////////////////////////////
	

	if (deathParticles_) {
		deathParticles_->Draw();
	}


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	
	Model::PostDraw();
	

	///////////////////////////////////////
	
	Sprite::PreDraw(dxCommon->GetCommandList());
	sprite_->Draw();
	// 照準調整中に矢印を描画
	if (shouldShowArrows_) {
		aimArrowSprite_->Draw();
	}
	Sprite::PostDraw();

	///////////////////////////////////////
	

	fade_->Draw();
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {

				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

//追加した部分
//////////////////////////////////////////////////////////////////////
void GameScene::CheckAllCollisions() {
#pragma region
	AABB aabb1;

	aabb1 = player_->GetAABB();

	//ゴールとの当たり判定をチェック
	if (goal_ && phase_ == Phase::kPlay) {
		Vector3 playerPos = player_->GetWorldPosition();
		Vector3 goalPos = goal_->GetWorldPosition();

	// 最適化のために距離の二乗を計算する
		float dx = playerPos.x - goalPos.x;
		float dy = playerPos.y - goalPos.y;
		float dz = playerPos.z - goalPos.z;
		float distanceSquared = dx * dx + dy * dy + dz * dz;
		float radiusSquared = Goal::kRadius * Goal::kRadius;

		// プレイヤーがゴールに到達したか確認する
		if (distanceSquared < radiusSquared) {
			phase_ = Phase::kGoalReached;
		}
	}


#pragma endregion
}


void GameScene::ChangePhase() {

	switch (phase_) {
	case Phase::kPlay:

		if (player_->IsDead()) {

			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kGoalReached:
	
		//ゴール到達後にタイトルへ戻る
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kDeath:
		if (deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////
