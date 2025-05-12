#include "gstPipeline.h"
#include <iostream>
#include <vector>

static GstElement *pipeline1 = nullptr;
static GstElement *pipeline2 = nullptr;
static GMainLoop *main_loop1 = nullptr;
static GMainLoop *main_loop2 = nullptr;
static std::thread loop_thread1;
static std::thread loop_thread2;

static void run_main_loop(GMainLoop* loop) {
    g_main_loop_run(loop);
}

static bool create_pipeline(const std::string& device, int port, GstElement** pipeline, GMainLoop** loop) {
    std::string pipeline_str = "v4l2src device=" + device + " ! "
                               "image/jpeg,width=1280,height=720,framerate=30/1 ! jpegdec ! "
                               "videoconvert ! x264enc tune=zerolatency bitrate=5000 speed-preset=superfast ! "
                               "rtph264pay config-interval=1 pt=96 ! "
                               "udpsink host=192.168.4.10 port=" + std::to_string(port);

    GError* error = nullptr;
    *pipeline = gst_parse_launch(pipeline_str.c_str(), &error);
    if (!*pipeline) {
        std::cerr << "GStreamerパイプライン作成失敗: " << error->message << std::endl;
        g_error_free(error);
        return false;
    }

    *loop = g_main_loop_new(nullptr, FALSE);
    gst_element_set_state(*pipeline, GST_STATE_PLAYING);

    return true;
}

bool start_gstreamer_pipelines() {
    gst_init(nullptr, nullptr);

    if (!create_pipeline("/dev/video0", 5000, &pipeline1, &main_loop1)) return false;
    if (!create_pipeline("/dev/video2", 5001, &pipeline2, &main_loop2)) return false;

    loop_thread1 = std::thread(run_main_loop, main_loop1);
    loop_thread2 = std::thread(run_main_loop, main_loop2);

    std::cout << "GStreamerパイプラインを非同期で起動しました。" << std::endl;
    return true;
}

void stop_gstreamer_pipelines() {
    std::cout << "GStreamerパイプラインを停止します..." << std::endl;

    if (pipeline1) {
        gst_element_set_state(pipeline1, GST_STATE_NULL);
        gst_object_unref(pipeline1);
        pipeline1 = nullptr;
    }
    if (main_loop1) {
        g_main_loop_quit(main_loop1);
        if (loop_thread1.joinable()) loop_thread1.join();
        g_main_loop_unref(main_loop1);
        main_loop1 = nullptr;
    }

    if (pipeline2) {
        gst_element_set_state(pipeline2, GST_STATE_NULL);
        gst_object_unref(pipeline2);
        pipeline2 = nullptr;
    }
    if (main_loop2) {
        g_main_loop_quit(main_loop2);
        if (loop_thread2.joinable()) loop_thread2.join();
        g_main_loop_unref(main_loop2);
        main_loop2 = nullptr;
    }

    std::cout << "GStreamerパイプラインを停止しました。" << std::endl;
}
