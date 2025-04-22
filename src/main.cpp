#include "network.h"
#include "gamepad.h"
#include "thruster_control.h"
#include "sensor_data.h"
#include "bindings.h" // For init()

#include <iostream>
#include <unistd.h> // For usleep
#include <string.h> // For strlen

int main() {
    printf("Navigator C++ Control Application\n");

    // --- 初期化 ---
    printf("Initiating navigator module.\n");
    init(); // Initialize navigator hardware library

    NetworkContext net_ctx;
    if (!network_init(&net_ctx, DEFAULT_RECV_PORT, DEFAULT_SEND_PORT)) {
        std::cerr << "ネットワーク初期化失敗。終了します。" << std::endl;
        return -1;
    }

    if (!thruster_init()) {
        std::cerr << "スラスター初期化失敗。終了します。" << std::endl;
        network_close(&net_ctx);
        return -1;
    }

    // --- メインループ ---
    GamepadData latest_gamepad_data; // Keep the last known state
    char recv_buffer[NET_BUFFER_SIZE];
    char sensor_buffer[SENSOR_BUFFER_SIZE];
    bool running = true;

    std::cout << "メインループ開始。Startボタンで終了。" << std::endl;

    while (running) {
        // 1. ゲームパッドデータ受信
        ssize_t recv_len = network_receive(&net_ctx, recv_buffer, sizeof(recv_buffer));

        if (recv_len > 0) {
            // データ受信成功、パースして最新状態を更新
            std::string received_str(recv_buffer);
            latest_gamepad_data = parseGamepadData(received_str);
            // std::cout << "受信: " << received_str << std::endl; // Debug
        } else if (recv_len < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            // 受信エラー (データなし以外)
            std::cerr << "致命的な受信エラー。ループを継続します..." << std::endl;
            // Consider adding logic to stop or reset if errors persist
        }
        // recv_len == 0 or EAGAIN/EWOULDBLOCK: データなし。latest_gamepad_data をそのまま使う。

        // 2. スラスター制御更新
        // 常に最新(または前回受信)のゲームパッド状態でスラスターを更新
        thruster_update(latest_gamepad_data);

        // 3. センサーデータ読み取り＆フォーマット
        if (read_and_format_sensor_data(sensor_buffer, sizeof(sensor_buffer))) {
            // 4. センサーデータ送信 (送信先が分かっている場合のみ)
            if (net_ctx.client_addr_known) {
                network_send(&net_ctx, sensor_buffer, strlen(sensor_buffer));
            }
        } else {
            std::cerr << "センサーデータの読み取り/フォーマットに失敗。" << std::endl;
        }


        // 5. 終了条件チェック
        if (latest_gamepad_data.buttons & GamepadButton::Start) {
            std::cout << "Startボタン検出。終了します。" << std::endl;
            running = false;
        }

        // 6. ループ待機 (CPU負荷軽減とループ頻度調整)
        usleep(10000); // 10ms待機 (~100Hz)
    }

    // --- クリーンアップ ---
    thruster_disable();
    network_close(&net_ctx);

    std::cout << "プログラム終了。" << std::endl;
    return 0;
}
