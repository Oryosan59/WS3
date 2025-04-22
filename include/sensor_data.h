#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <string>
#include <vector> // For ADC data

#define SENSOR_BUFFER_SIZE 512

// 関数のプロトタイプ宣言
// Reads all relevant sensors and formats them into a string
// Returns true if successful, false otherwise. Output string is in buffer.
bool read_and_format_sensor_data(char* buffer, size_t buffer_size);

#endif // SENSOR_DATA_H
