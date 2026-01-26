#pragma once

#include "KamataEngine.h"

class GaugeUI {
public:
	void Initialize();

	void Update(float chargePower, bool isCharging);

	void Draw();

private:
	// Background gauge sprites
	uint32_t textureHandleBackground_ = 0;
	KamataEngine::Sprite* spriteBackground_ = nullptr;

	// Foreground gauge (fills up)
	uint32_t textureHandleForeground_ = 0;
	KamataEngine::Sprite* spriteForeground_ = nullptr;

	// Current charge power (0.0 to 1.0)
	float currentPower_ = 0.0f;

	// Whether the gauge should be visible
	bool isVisible_ = false;

	// Gauge dimensions and position
	static inline const float kGaugeWidth = 400.0f;
	static inline const float kGaugeHeight = 30.0f;
	static inline const float kGaugeX = 440.0f; // Centered horizontally: (1280 - 400) / 2
	static inline const float kGaugeY = 650.0f; // Near bottom
};
