#include "gstPipeline.h"
#include <cstdlib>    // system() 関数を使用するため
#include <iostream>   // エラー出力のため (std::cerr)
#include <string>     // コマンド文字列を扱うため
#include <sys/wait.h> // WIFEXITED, WEXITSTATUS を使用するため
#include <errno.h>    // errno を使用するため

// GStreamerパイプラインを起動する関数
bool start_gstreamer_pipelines()
{
    // カメラ1のパイプラインコマンド
    const std::string cmd1 = "gst-launch-1.0 v4l2src device=/dev/video0 ! "
                             "image/jpeg,width=1280,height=720,framerate=30/1 ! jpegdec ! " // jpegdec の後にスペースを追加
                             "videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! "
                             "rtph264pay config-interval=1 pt=96 ! udpsink host=192.168.0.16 port=5000 > /dev/null 2>&1 &";

    // カメラ2のパイプラインコマンド
    const std::string cmd2 = "gst-launch-1.0 v4l2src device=/dev/video2 ! "
                             "image/jpeg,width=1280,height=720,framerate=30/1 ! jpegdec ! " // jpegdec の後にスペースを追加
                             "videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! "
                             "rtph264pay config-interval=1 pt=96 ! udpsink host=192.168.0.16 port=5001 > /dev/null 2>&1 &";

    std::cout << "GStreamerパイプラインを起動します..." << std::endl;

    // コマンド1を実行
    std::cout << "実行中: " << cmd1 << std::endl;
    int ret1 = system(cmd1.c_str());

    // コマンド2を実行
    std::cout << "実行中: " << cmd2 << std::endl;
    int ret2 = system(cmd2.c_str());

    // system() の戻り値は複雑ですが、ここでは単純に 0 以外をエラーとして扱います
    // (バックグラウンド実行の場合、起動自体の成否判定は難しい場合があります)
    if (ret1 != 0 || ret2 != 0)
    {
        std::cerr << "警告: GStreamerパイプラインの起動に問題があった可能性があります。" << std::endl;
        // 起動自体は試みているので、ここでは false を返さないでおきます。
        // より厳密なエラーチェックが必要な場合は、プロセス管理の方法を検討してください。
    }
    else
    {
        std::cout << "GStreamerパイプラインがバックグラウンドで起動されました。" << std::endl;
    }

    return true; // 起動試行は成功したとみなす
}

// GStreamerパイプラインを停止する関数
void stop_gstreamer_pipelines()
{
    std::cout << "GStreamerパイプラインを停止します..." << std::endl;

    // 停止対象のパイプラインを識別するためのコマンドパターン
    // start_gstreamer_pipelines で使用したコマンドの一部にマッチするように指定
    const char *kill_cmd_patterns[] = {
        "pkill -f \"gst-launch-1.0 v4l2src device=/dev/video0.*udpsink host=192.168.0.16 port=5000\"",
        "pkill -f \"gst-launch-1.0 v4l2src device=/dev/video2.*udpsink host=192.168.0.16 port=5001\""};
    const char *pipeline_names[] = {"パイプライン1 (video0)", "パイプライン2 (video2)"};

    for (int i = 0; i < 2; ++i)
    {
        std::cout << "停止コマンド実行中: " << kill_cmd_patterns[i] << std::endl;
        int status = system(kill_cmd_patterns[i]);

        if (status == -1)
        {
            // system() 自体の呼び出し失敗
            std::cerr << "警告: " << pipeline_names[i] << " の停止コマンド実行に失敗しました (system error, errno: " << errno << ")." << std::endl;
        }
        else
        {
            if (WIFEXITED(status))
            {
                int exit_code = WEXITSTATUS(status);
                if (exit_code == 0)
                {
                    std::cout << pipeline_names[i] << " の停止コマンドが正常に実行されました (プロセスが見つかりシグナル送信)." << std::endl;
                }
                else if (exit_code == 1) // pkill: no processes matched
                {
                    std::cout << "情報: " << pipeline_names[i] << " は見つからなかったか、既に停止していました (pkill exit code 1)." << std::endl;
                }
                else
                {
                    std::cerr << "警告: " << pipeline_names[i] << " の停止コマンドが終了コード " << exit_code << " で失敗しました (例: pkill自体がない場合など)。" << std::endl;
                }
            }
            else
            {
                std::cerr << "警告: " << pipeline_names[i] << " の停止コマンドが異常終了しました。" << std::endl;
            }
        }
    }
    // pkillがプロセスを終了させるのに少し時間がかかる場合があるため、必要に応じて短い待機時間を追加
    // usleep(200000); // 0.2秒待機 (オプション)
}