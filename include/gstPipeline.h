#ifndef GST_PIPELINE_H
#define GST_PIPELINE_H

/**
 * @brief 指定されたGStreamerパイプラインをバックグラウンドで起動します。
 * @return 成功した場合は true、失敗した場合は false。
 */
bool start_gstreamer_pipelines();

// GStreamerパイプラインを停止する関数
void stop_gstreamer_pipelines();

#endif // GST_PIPELINE_H