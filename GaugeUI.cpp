#include "GaugeUI.h"
#include <algorithm>

using namespace KamataEngine;

GaugeUI::~GaugeUI() {
	delete spriteBackground_;
	delete spriteForeground_;
}

void GaugeUI::Initialize() {
	// Use white texture for both background and foreground
	uint32_t textureHandle = TextureManager::Load("white1x1.png");
	textureHandleBackground_ = textureHandle;
	textureHandleForeground_ = textureHandle;

	// Create background sprite (dark gray)
	spriteBackground_ = Sprite::Create(textureHandleBackground_, {kGaugeX, kGaugeY});
	spriteBackground_->SetSize(Vector2(kGaugeWidth, kGaugeHeight));
	spriteBackground_->SetColor(Vector4(0.3f, 0.3f, 0.3f, 0.8f));

	// Create foreground sprite (green/yellow/red based on power)
	spriteForeground_ = Sprite::Create(textureHandleForeground_, {kGaugeX, kGaugeY});
	spriteForeground_->SetSize(Vector2(0, kGaugeHeight));
	spriteForeground_->SetColor(Vector4(0.0f, 1.0f, 0.0f, 0.9f)); // Green
}

void GaugeUI::Update(float chargePower, bool isCharging) {
	currentPower_ = std::clamp(chargePower, 0.0f, 1.0f);
	isVisible_ = isCharging;

	// Update foreground size based on charge power
	float fillWidth = kGaugeWidth * currentPower_;
	spriteForeground_->SetSize(Vector2(fillWidth, kGaugeHeight));

	// Change color based on power level
	if (currentPower_ < 0.5f) {
		// Green for low power
		spriteForeground_->SetColor(Vector4(0.0f, 1.0f, 0.0f, 0.9f));
	} else if (currentPower_ < 0.8f) {
		// Yellow for medium power
		spriteForeground_->SetColor(Vector4(1.0f, 1.0f, 0.0f, 0.9f));
	} else {
		// Red for high power
		spriteForeground_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 0.9f));
	}
}

void GaugeUI::Draw() {
	if (!isVisible_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	spriteBackground_->Draw();
	spriteForeground_->Draw();
	Sprite::PostDraw();
}
