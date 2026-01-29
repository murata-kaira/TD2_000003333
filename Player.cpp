#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include <algorithm>
#include <numbers>
#include <cmath>

using namespace KamataEngine;
using namespace MathUtility;

Player::~Player() {
	delete aimArrowShaft_;
	delete aimArrowHead1_;
	delete aimArrowHead2_;
}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position) {

	assert(model);

	camera_ = camera;

	model_ = model;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// Initialize aim arrow sprites (shaft + arrowhead)
	aimArrowTextureHandle_ = TextureManager::Load("white1x1.png");
	
	// Arrow shaft - horizontal line
	aimArrowShaft_ = Sprite::Create(aimArrowTextureHandle_, {640, 360});
	aimArrowShaft_->SetSize(Vector2(80, 6)); // 80 pixels long, 6 pixels wide
	aimArrowShaft_->SetAnchorPoint(Vector2(0.0f, 0.5f)); // Anchor at left-center
	aimArrowShaft_->SetColor(Vector4(1, 0.2f, 0.2f, 0.9f)); // Red color
	
	// Arrowhead part 1 (upper diagonal)
	aimArrowHead1_ = Sprite::Create(aimArrowTextureHandle_, {640, 360});
	aimArrowHead1_->SetSize(Vector2(20, 4)); // 20 pixels long, 4 pixels wide
	aimArrowHead1_->SetAnchorPoint(Vector2(0.0f, 0.5f));
	aimArrowHead1_->SetColor(Vector4(1, 0.2f, 0.2f, 0.9f));
	
	// Arrowhead part 2 (lower diagonal)
	aimArrowHead2_ = Sprite::Create(aimArrowTextureHandle_, {640, 360});
	aimArrowHead2_->SetSize(Vector2(20, 4)); // 20 pixels long, 4 pixels wide
	aimArrowHead2_->SetAnchorPoint(Vector2(0.0f, 0.5f));
	aimArrowHead2_->SetColor(Vector4(1, 0.2f, 0.2f, 0.9f));
}

void Player::Update() {

	InputGolf(); // Use golf input instead of platformer input

	CollisionMapInfo collisionMapInfo;

	collisionMapInfo.move = velocity_;

	CheckMapCollision(collisionMapInfo);

	CheckMapMove(collisionMapInfo);

	CheckMapWall(collisionMapInfo);

	CheckMapLanding(collisionMapInfo);

	CheckMapLanding(collisionMapInfo);

	AnimateTurn();

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

	// Update aim arrow sprite position and rotation when aiming
	if (aimArrowShaft_ && (state_ == State::Idle || state_ == State::Charging)) {
		// Get player world position
		Vector3 playerWorldPos = GetWorldPosition();
		
		// Convert world position to screen position using view-projection matrix
		Vector3 playerPosNDC;
		playerPosNDC.x = playerWorldPos.x * camera_->matView.m[0][0] + playerWorldPos.y * camera_->matView.m[1][0] + playerWorldPos.z * camera_->matView.m[2][0] + camera_->matView.m[3][0];
		playerPosNDC.y = playerWorldPos.x * camera_->matView.m[0][1] + playerWorldPos.y * camera_->matView.m[1][1] + playerWorldPos.z * camera_->matView.m[2][1] + camera_->matView.m[3][1];
		playerPosNDC.z = playerWorldPos.x * camera_->matView.m[0][2] + playerWorldPos.y * camera_->matView.m[1][2] + playerWorldPos.z * camera_->matView.m[2][2] + camera_->matView.m[3][2];
		
		Vector3 playerPosProj;
		playerPosProj.x = playerPosNDC.x * camera_->matProjection.m[0][0] + playerPosNDC.y * camera_->matProjection.m[1][0] + playerPosNDC.z * camera_->matProjection.m[2][0] + camera_->matProjection.m[3][0];
		playerPosProj.y = playerPosNDC.x * camera_->matProjection.m[0][1] + playerPosNDC.y * camera_->matProjection.m[1][1] + playerPosNDC.z * camera_->matProjection.m[2][1] + camera_->matProjection.m[3][1];
		float w = playerPosNDC.x * camera_->matProjection.m[0][3] + playerPosNDC.y * camera_->matProjection.m[1][3] + playerPosNDC.z * camera_->matProjection.m[2][3] + camera_->matProjection.m[3][3];
		
		// Perspective divide
		if (w != 0) {
			playerPosProj.x /= w;
			playerPosProj.y /= w;
		}
		
		// Convert from NDC (-1 to 1) to screen coordinates
		float screenX = (playerPosProj.x + 1.0f) * 0.5f * 1280.0f; // Assuming 1280 width
		float screenY = (1.0f - playerPosProj.y) * 0.5f * 720.0f;  // Assuming 720 height
		
		// Set arrow shaft position and rotation
		aimArrowShaft_->SetPosition(Vector2(screenX, screenY));
		aimArrowShaft_->SetRotation(aimAngle_);
		
		// Calculate arrowhead positions (at the end of the shaft)
		float shaftLength = 80.0f;
		float arrowTipX = screenX + std::cos(aimAngle_) * shaftLength;
		float arrowTipY = screenY + std::sin(aimAngle_) * shaftLength;
		
		// Upper arrowhead diagonal (angled at +135 degrees from shaft direction)
		aimArrowHead1_->SetPosition(Vector2(arrowTipX, arrowTipY));
		aimArrowHead1_->SetRotation(aimAngle_ + 2.356f); // +135 degrees in radians
		
		// Lower arrowhead diagonal (angled at -135 degrees from shaft direction)
		aimArrowHead2_->SetPosition(Vector2(arrowTipX, arrowTipY));
		aimArrowHead2_->SetRotation(aimAngle_ - 2.356f); // -135 degrees in radians
	}
}

void Player::Draw() {

	if (isDead_ == false) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Player::DrawArrow() {
	// Draw aim arrow when aiming (Idle or Charging state)
	if (aimArrowShaft_ && (state_ == State::Idle || state_ == State::Charging)) {
		DirectXCommon* dxCommon = DirectXCommon::GetInstance();
		Sprite::PreDraw(dxCommon->GetCommandList());
		
		// Draw arrow components
		aimArrowShaft_->Draw();
		aimArrowHead1_->Draw();
		aimArrowHead2_->Draw();
		
		Sprite::PostDraw();
	}
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	// velocity_ += Vector3(0,1,0);

	isDead_ = true;
}

Vector3 Player::GetWorldPosition() {

	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::InputMove() {

	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}

			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_ += acceleration;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ += Vector3(0, kJumpAcceleration, 0);
		}

	} else {
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CheckMapCollision(CollisionMapInfo& info) {

	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, kHeight / 2.0f, 0));
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			info.ceiling = true;
		}
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	if (info.move.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			info.landing = true;
		}
	}
}
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		info.hitWall = true;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		info.hitWall = true;
	}
}

void Player::CheckMapMove(const CollisionMapInfo& info) { worldTransform_.translation_ += info.move; }

void Player::CheckMapCeiling(const CollisionMapInfo& info) {

	if (info.ceiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::CheckMapWall(const CollisionMapInfo& info) {

	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::CheckMapLanding(const CollisionMapInfo& info) {

	if (onGround_) {

		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			MapChipType mapChipType;

			bool hit = false;

			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}

	} else {
		if (info.landing) {

			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

void Player::AnimateTurn() {

	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotionYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		float destinationRotationY = destinationRotionYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::InputGolf() {
	// Apply gravity
	if (!onGround_) {
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
	switch (state_) {
	case State::Idle:
		// Ball is stopped, can aim and charge
		velocity_.x = 0;
		velocity_.y = 0;

		// Rotate aim with LEFT/RIGHT keys
		if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			aimAngle_ -= kAimRotateSpeed;
		}
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			aimAngle_ += kAimRotateSpeed;
		}
		// UP/DOWN keys for vertical aiming
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			aimAngle_ += kAimRotateSpeed;
		}
		if (Input::GetInstance()->PushKey(DIK_DOWN)) {
			aimAngle_ -= kAimRotateSpeed;
		}
		
		// Clamp angle to prevent unlimited rotation
		aimAngle_ = std::clamp(aimAngle_, kMinAimAngle, kMaxAimAngle);

		// Start charging with SPACE
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			state_ = State::Charging;
			chargePower_ = 0.0f;
		}
		break;
	case State::Charging:
		// Charge power while holding SPACE
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			chargePower_ += kChargeSpeed;
			chargePower_ = std::min(chargePower_, 1.0f);

			// Continue rotating aim while charging
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				aimAngle_ -= kAimRotateSpeed;
			}
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				aimAngle_ += kAimRotateSpeed;
			}
			// UP/DOWN keys for vertical aiming while charging
			if (Input::GetInstance()->PushKey(DIK_UP)) {
				aimAngle_ += kAimRotateSpeed;
			}
			if (Input::GetInstance()->PushKey(DIK_DOWN)) {
				aimAngle_ -= kAimRotateSpeed;
			}
			
			// Clamp angle to prevent unlimited rotation
			aimAngle_ = std::clamp(aimAngle_, kMinAimAngle, kMaxAimAngle);
		} else {
			// SPACE was released, shoot the ball
			if (chargePower_ > 0.0f) {
				float launchSpeed = chargePower_ * kMaxChargePower;
				velocity_.x = std::cos(aimAngle_) * launchSpeed;
				velocity_.y = std::sin(aimAngle_) * launchSpeed;
				state_ = State::Moving;
				shotCount_++;
			}
			chargePower_ = 0.0f;
		}
		break;
	case State::Moving:
		// Ball is moving, apply friction
		if (onGround_) {
			velocity_.x *= kFriction;
			velocity_.y *= kFriction;

			// Stop if velocity is very small
			float speed = std::sqrt(velocity_.x * velocity_.x + velocity_.y * velocity_.y);
			if (speed < kMinVelocity) {
				velocity_.x = 0;
				velocity_.y = 0;
				state_ = State::Idle;
			}
		}
		break;
	}
}

