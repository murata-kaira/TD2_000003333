#include "TitleScene.h"
#include "MyMath.h"
#include <numbers>

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete model_;
	delete modelPlayer_;

	delete fade_;
}

void TitleScene::Initialize() {

	model_ = Model::CreateFromOBJ("titleFont");
	modelPlayer_ = Model::CreateFromOBJ("player");

	camera_.Initialize();

	worldTransform_.Initialize();
	worldTransform_.scale_ = {2, 2, 2};
	worldTransform_.translation_ = {0, 8, 0};

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.scale_ = {10, 10, 10};
	worldTransformPlayer_.translation_ = {0, -8, 0};
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float>;

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {

	switch (phase_) {
	case Phase::kMain:

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {

			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeIn:

		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kFadeOut:

		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
	}

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	rotate += 0.1f;
	worldTransformPlayer_.rotation_.y = sin(rotate) + std::numbers::pi_v<float>;

	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();
}

void TitleScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, camera_);
	modelPlayer_->Draw(worldTransformPlayer_, camera_);

	Model::PostDraw();

	// Display game title and instructions
	DebugText::GetInstance()->SetPos(250, 150);
	DebugText::GetInstance()->Printf("100 Acre Wood Golf");
	
	DebugText::GetInstance()->SetPos(300, 200);
	DebugText::GetInstance()->Printf("How to Play:");
	
	DebugText::GetInstance()->SetPos(250, 230);
	DebugText::GetInstance()->Printf("LEFT/RIGHT: Aim direction");
	
	DebugText::GetInstance()->SetPos(250, 260);
	DebugText::GetInstance()->Printf("SPACE: Charge and shoot");
	
	DebugText::GetInstance()->SetPos(250, 290);
	DebugText::GetInstance()->Printf("Goal: Reach the end!");
	
	DebugText::GetInstance()->SetPos(280, 350);
	DebugText::GetInstance()->Printf("Press SPACE to start");

	fade_->Draw();
}
