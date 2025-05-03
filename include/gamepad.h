#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <stdint.h> // 固定幅整数型 (uint16_t など) を使用するため
#include <string>   // std::string を使用するため
#include <vector>   // 将来的な使用や代替のパース方法のために含める (現在は未使用)

// ゲームパッドからの受信データを格納する構造体
struct GamepadData
{
    int leftThumbX = 0;   // 左スティック X軸 (-32768 ~ 32767)
    int leftThumbY = 0;   // 左スティック Y軸 (-32768 ~ 32767)
    int rightThumbX = 0;  // 右スティック X軸 (-32768 ~ 32767)
    int rightThumbY = 0;  // 右スティック Y軸 (-32768 ~ 32767)
    int LT = 0;           // 左トリガー (0 ~ 1023?)
    int RT = 0;           // 右トリガー (0 ~ 1023?)
    uint16_t buttons = 0; // ボタンの状態 (ビットフラグ)
};

// ゲームパッドのボタンを表すビットフラグの定義
enum GamepadButton : uint32_t // 基底型を明示的に指定
{
    None = 0x0000,      // ボタンが押されていない状態
    DPadUp = 0x0001,    // 十字キー 上
    DPadDown = 0x0002,  // 十字キー 下
    DPadLeft = 0x0004,  // 十字キー 左
    DPadRight = 0x0008, // 十字キー 右
    // 注意: 以下の値は一般的ではないようです。送信側アプリケーションと一致しているか確認してください。
    Start = 0x0016,         // Start ボタン (通常は 0x0010)
    Back = 0x0032,          // Back ボタン (通常は 0x0020)
    LeftShoulder = 0x0256,  // Lボタン (LB) (通常は 0x0100)
    RightShoulder = 0x0512, // Rボタン (RB) (通常は 0x0200)
    A = 0x8192,             // A ボタン (通常は 0x1000)
    B = 0x4096,             // B ボタン (通常は 0x2000)
    X = 0x16384,            // X ボタン (通常は 0x4000)
    Y = 0x32768             // Y ボタン (通常は 0x8000)
};

// 関数のプロトタイプ宣言
// 受信した文字列データを GamepadData 構造体にパースする関数
GamepadData parseGamepadData(const std::string &data);

#endif // GAMEPAD_H
