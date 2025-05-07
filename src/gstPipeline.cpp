#include "gstPipeline.h"
#include <cstdlib>  // system() 関数を使用するため
#include <iostream> // エラー出力のため (std::cerr)
#include <string>   // コマンド文字列を扱うため

// GStreamerパイプラインを起動する関数
bool start_gstreamer_pipelines()
{
    // カメラ1のパイプラインコマンド
    const std::string cmd1 = "gst-launch-1.0 v4l2src device=/dev/video0 ! "
                             "image/jpeg,width=1280,height=720,framerate=30/1 ! jpegdec ! "
                             "videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=ultrafast ! "
                             "mpegtsmux ! udpsink host=192.168.6.10 port=5000 > /dev/null 2>&1 &";

    // カメラ2のパイプラインコマンド
    const std::string cmd2 = "gst-launch-1.0 v4l2src device=/dev/video2 ! "
                             "image/jpeg,width=1280,height=720,framerate=30/1 ! jpegdec ! "
                             "videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=ultrafast ! "
                             "mpegtsmux ! udpsink host=192.168.6.10 port=5002 > /dev/null 2>&1 &";

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