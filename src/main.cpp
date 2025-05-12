// --- インクルード ---
#include "network.h"          // ネットワーク通信関連 (UDP送受信)
#include "gamepad.h"          // ゲームパッドデータ構造体とパース関数
#include "thruster_control.h" // スラスター制御関連
#include "sensor_data.h"      // センサーデータ読み取り・フォーマット関連
#include "gstPipeline.h"      // GStreamerパイプライン起動用

#include <iostream> // 標準入出力 (std::cout, std::cerr)
#include <unistd.h> // POSIX API (usleep)
#include <string.h> // 文字列操作 (strlen)

// --- メイン関数 ---
int main()
{
    printf("Navigator C++ Control Application\n");

    // --- 初期化 ---
    printf("Initiating navigator module.\n");
    init(); // Navigator ハードウェアライブラリの初期化 (bindings.h 経由)

    // ネットワークコンテキストの初期化
    NetworkContext net_ctx;
    if (!network_init(&net_ctx, DEFAULT_RECV_PORT, DEFAULT_SEND_PORT))
    {
        std::cerr << "ネットワーク初期化失敗。終了します。" << std::endl;
        return -1;
    }

    // スラスター制御の初期化
    if (!thruster_init())
    {
        std::cerr << "スラスター初期化失敗。終了します。" << std::endl;
        network_close(&net_ctx); // ネットワークリソースを解放
        return -1;
    }

    // GStreamerパイプラインの起動
    if (!start_gstreamer_pipelines())
    {
        std::cerr << "GStreamerパイプラインの起動に失敗しました。処理を続行します..." << std::endl;
        // パイプライン起動失敗は致命的ではないかもしれないので、ここでは続行
    }

    // --- メインループ ---
    GamepadData latest_gamepad_data;                 // 最後に受信した有効なゲームパッドデータを保持
    char recv_buffer[NET_BUFFER_SIZE];               // UDP受信バッファ
    AxisData current_gyro_data = {0.0f, 0.0f, 0.0f}; // 最新のジャイロデータを保持
    char sensor_buffer[SENSOR_BUFFER_SIZE];          // センサーデータ送信用文字列バッファ
    unsigned int loop_counter = 0;                   // センサーデータ送信間隔制御用カウンター
    const unsigned int SENSOR_SEND_INTERVAL = 500;   // センサーデータを送信するループ間隔 (例: 5000Hzループ / 500 = 10Hz送信)
    bool running = true;                             // メインループの実行フラグ

    std::cout << "メインループ開始。Startボタンで終了。" << std::endl;

    // running フラグが true の間、ループを継続
    while (running)
    {
        // 1. ゲームパッドデータ受信
        ssize_t recv_len = network_receive(&net_ctx, recv_buffer, sizeof(recv_buffer));

        if (recv_len > 0)
        {
            // データ受信成功、パースして最新状態を更新
            std::string received_str(recv_buffer);
            latest_gamepad_data = parseGamepadData(received_str); // 受信文字列をパース
            // std::cout << "受信: " << received_str << std::endl; // Debug
        }
        else if (recv_len < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            // 受信エラー (データなし以外)
            std::cerr << "致命的な受信エラー。ループを継続します..." << std::endl;
            // エラーが頻発する場合、停止またはリセットするロジックの追加を検討
        }
        // recv_len == 0 または EAGAIN/EWOULDBLOCK の場合:
        // データがないことを示す。この場合、前回受信した latest_gamepad_data をそのまま使用する。

        // 2. スラスター制御更新 (毎ループ実行)
        // 最新のジャイロデータを読み取り、スラスター制御に使用
        current_gyro_data = read_gyro();
        // 常に最新(または前回受信)のゲームパッド状態でスラスターを更新
        thruster_update(latest_gamepad_data, current_gyro_data);

        // 3. センサーデータ処理 (読み取り、フォーマット、送信) - 一定間隔で実行
        if (loop_counter >= SENSOR_SEND_INTERVAL)
        {
            loop_counter = 0; // カウンターリセット

            if (read_and_format_sensor_data(sensor_buffer, sizeof(sensor_buffer)))
            {
                std::cout << "[SENSOR LOG] " << sensor_buffer << std::endl; // ログは送信時のみ表示

                // 4. センサーデータ送信 (送信先が分かっている場合のみ)
                if (net_ctx.client_addr_known)
                {
                    network_send(&net_ctx, sensor_buffer, strlen(sensor_buffer)); // フォーマットされたセンサーデータを送信
                }
            }
            else
            {
                std::cerr << "センサーデータの読み取り/フォーマットに失敗。" << std::endl;
            }
        }
        else
        {
            loop_counter++; // カウンターインクリメント
        }

        // 5. 終了条件チェック
        // ゲームパッドの Start ボタンが押されたらループを終了
        if (latest_gamepad_data.buttons & GamepadButton::Start)
        {
            std::cout << "Startボタン検出。終了します。" << std::endl;
            running = false;
        }

        // 6. ループ待機 (CPU負荷軽減とループ頻度調整)
        usleep(10000); // 0.2ミリ秒待機 (約5000Hzのループ周波数)
    }

    // --- クリーンアップ ---
    thruster_disable();      // スラスターへのPWM出力を停止
    network_close(&net_ctx); // ネットワークソケットをクローズ

    std::cout << "プログラム終了。" << std::endl;
    return 0;
}
