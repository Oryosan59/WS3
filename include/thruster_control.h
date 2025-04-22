#ifndef THRUSTER_CONTROL_H
#define THRUSTER_CONTROL_H

#include "gamepad.h" // Needs GamepadData definition

// --- 定数定義 ---
#define PWM_MIN 1100
#define PWM_NEUTRAL 1500 // Neutral/Stop PWM value (often 1500 for ESCs)
#define PWM_NORMAL_MAX 1500 // 通常の最大PWM値 (ニュートラルと同じ？要確認)
#define PWM_BOOST_MAX 1900  // ブースト時の最大PWM値
#define PWM_FREQUENCY 50.0f
#define PWM_PERIOD_US (1000000.0f / PWM_FREQUENCY) // 20000.0f for 50Hz

#define JOYSTICK_DEADZONE 6500
#define NUM_THRUSTERS 5 // 4 horizontal + 1 vertical/forward

// --- 関数のプロトタイプ宣言 ---
bool thruster_init();
void thruster_disable();
void thruster_update(const GamepadData& data); // Combines horizontal and vertical control

// Helper function (can be kept internal to .cpp if not needed elsewhere)
// float map_value(float x, float in_min, float in_max, float out_min, float out_max);

#endif // THRUSTER_CONTROL_H
