#include "gamepad.h"
#include <sstream>
#include <iostream>
#include <stdexcept> // For std::invalid_argument, std::out_of_range

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}


GamepadData parseGamepadData(const std::string &data)
{
    GamepadData gamepad; // Default initialized (members are 0)
    std::stringstream ss(data);
    std::string token;
    int values[7] = {0}; // Array for parsed integer values
    int index = 0;
    const int EXPECTED_VALUES = 7;

    while (std::getline(ss, token, ',') && index < EXPECTED_VALUES)
    {
        try
        {
            std::string trimmed_token = trim(token);
            if (!trimmed_token.empty()) {
                 values[index++] = std::stoi(trimmed_token);
            } else {
                // Handle empty token - treat as 0
                values[index++] = 0;
                std::cerr << "警告: 空のトークンを検出。0として扱います。" << std::endl;
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "stoiエラー: 無効なデータ形式 (" << token << ") - " << e.what() << std::endl;
            // Return default gamepad data on critical parsing error
            return GamepadData{}; // Return default initialized struct
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "stoiエラー: 数値が範囲外 (" << token << ") - " << e.what() << std::endl;
             // Return default gamepad data on critical parsing error
            return GamepadData{}; // Return default initialized struct
        }
    }

    // Check if we received the expected number of values
    if (index < EXPECTED_VALUES) {
         std::cerr << "警告: 受信データが不足しています。項目数: " << index << " (期待値: " << EXPECTED_VALUES << ")" << std::endl;
         // Depending on requirements, you might return default data here as well
    }

    // Assign parsed values to the struct members
    gamepad.leftThumbX = values[0];
    gamepad.leftThumbY = values[1];
    gamepad.rightThumbX = values[2];
    gamepad.rightThumbY = values[3];
    gamepad.LT = values[4];
    gamepad.RT = values[5];
    // Cast the button value safely
    gamepad.buttons = static_cast<uint16_t>(values[6]);

    return gamepad;
}
