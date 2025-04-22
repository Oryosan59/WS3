#include "sensor_data.h"
#include "bindings.h" // For read_* functions
#include <stdio.h>    // For snprintf
#include <iostream>   // For cerr

bool read_and_format_sensor_data(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return false;
    }

    // --- センサーデータの取得 ---
    float temperature = read_temp();
    float pressure = read_pressure();
    bool leak = read_leak();
    float adc[4];
    read_adc_all(adc, 4); // Assuming read_adc_all is efficient
    AxisData accel = read_accel();
    AxisData gyro = read_gyro();
    AxisData mag = read_mag();

    // --- 文字列へのフォーマット ---
    int written = snprintf(buffer, buffer_size,
             "TEMP:%.6f,PRESSURE:%.6f,LEAK:%d,ADC:%.6f,%.6f,%.6f,%.6f,"
             "ACC:%.6f,%.6f,%.6f,GYRO:%.6f,%.6f,%.6f,MAG:%.6f,%.6f,%.6f",
             temperature, pressure, leak ? 1 : 0,
             adc[0], adc[1], adc[2], adc[3],
             accel.x, accel.y, accel.z,
             gyro.x, gyro.y, gyro.z,
             mag.x, mag.y, mag.z);

    // --- エラーチェック ---
    if (written < 0) {
        // snprintf error
        std::cerr << "エラー: センサーデータ文字列のフォーマットに失敗しました (snprintf)。" << std::endl;
        buffer[0] = '\0'; // Ensure buffer is empty on error
        return false;
    } else if ((size_t)written >= buffer_size) {
        // Buffer too small, data truncated
        std::cerr << "警告: センサーデータ文字列がバッファサイズを超えました。切り捨てられました。" << std::endl;
        // Data is truncated but might still be usable depending on receiver.
        // Consider returning false or logging more severely if truncation is critical.
        return true; // Or false, depending on requirements
    }

    return true; // Success
}
