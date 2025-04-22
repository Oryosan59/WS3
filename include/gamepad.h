#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <stdint.h>
#include <string>
#include <vector> // For potential future use or alternative parsing

// 受信データの構造体
struct GamepadData
{
    int leftThumbX = 0;
    int leftThumbY = 0;
    int rightThumbX = 0;
    int rightThumbY = 0;
    int LT = 0;
    int RT = 0;
    uint16_t buttons = 0;
};

// ボタンの定義
enum GamepadButton : uint32_t // Explicit underlying type
{
    None = 0x0000,
    DPadUp = 0x0001,
    DPadDown = 0x0002,
    DPadLeft = 0x0004,
    DPadRight = 0x0008,
    // Note: Values seem unusual, verify with sending application
    Start = 0x0016, // Typically 0x0010
    Back = 0x0032,  // Typically 0x0020
    LeftShoulder = 0x0256, // Typically 0x0100
    RightShoulder = 0x0512,// Typically 0x0200
    A = 0x8192,     // Typically 0x1000
    B = 0x4096,     // Typically 0x2000
    X = 0x16384,    // Typically 0x4000
    Y = 0x32768     // Typically 0x8000
};

// 関数のプロトタイプ宣言
GamepadData parseGamepadData(const std::string &data);

#endif // GAMEPAD_H
