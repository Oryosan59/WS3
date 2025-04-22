#include "thruster_control.h"
#include "bindings.h" // For set_pwm_* functions
#include <cmath>      // For std::abs
#include <algorithm>  // For std::max, std::min
#include <stdio.h>    // For printf

// --- ヘルパー関数 ---

// 線形補正関数
static float map_value(float x, float in_min, float in_max, float out_min, float out_max)
{
    if (in_max == in_min) {
        // Avoid division by zero
        return out_min;
    }
    // Clamp input value to the specified range before mapping
    x = std::max(in_min, std::min(x, in_max));
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// PWM値を設定するヘルパー (範囲チェックとデューティサイクル計算を含む)
static void set_thruster_pwm(int channel, int pulse_width_us) {
    // Clamp PWM value to ensure it's within valid operational range
    // Note: Using PWM_BOOST_MAX as the upper limit for clamping
    int clamped_pwm = std::max(PWM_MIN, std::min(pulse_width_us, PWM_BOOST_MAX));

    // Calculate duty cycle
    float duty_cycle = static_cast<float>(clamped_pwm) / PWM_PERIOD_US;

    // Set the PWM duty cycle for the specified channel
    set_pwm_channel_duty_cycle(channel, duty_cycle);

    // Debug print (optional)
    // printf("Ch%d: Set PWM = %d (Clamped: %d), Duty = %.4f\n", channel, pulse_width_us, clamped_pwm, duty_cycle);
}


// --- モジュール関数 ---

bool thruster_init() {
    printf("Enabling PWM\n");
    set_pwm_enable(true);
    printf("Setting PWM frequency to %.1f Hz\n", PWM_FREQUENCY);
    set_pwm_freq_hz(PWM_FREQUENCY);
    // Initialize all thrusters to neutral/min?
    for (int i = 0; i < NUM_THRUSTERS; ++i) {
         set_thruster_pwm(i, PWM_MIN); // Or PWM_NEUTRAL if applicable
    }
    printf("Thrusters initialized to PWM %d.\n", PWM_MIN);
    return true; // Assuming init functions don't return status easily
}

void thruster_disable() {
    printf("Disabling PWM\n");
    // Optionally set all thrusters to neutral/min before disabling
    for (int i = 0; i < NUM_THRUSTERS; ++i) {
         set_thruster_pwm(i, PWM_MIN); // Or PWM_NEUTRAL
    }
    set_pwm_enable(false);
}

// 水平スラスター制御ロジック (updateThrustersFromSticksの内容を移植・調整)
static void update_horizontal_thrusters(const GamepadData& data, int pwm_out[4]) {
    // Initialize output PWM array to neutral/min
    for(int i=0; i<4; ++i) pwm_out[i] = PWM_MIN;

    bool lx_active = std::abs(data.leftThumbX) > JOYSTICK_DEADZONE;
    bool rx_active = std::abs(data.rightThumbX) > JOYSTICK_DEADZONE;

    int pwm_lx[4] = {PWM_MIN, PWM_MIN, PWM_MIN, PWM_MIN};
    int pwm_rx[4] = {PWM_MIN, PWM_MIN, PWM_MIN, PWM_MIN};

    // Lx (Rotation) contribution (maps to PWM_MIN - PWM_NORMAL_MAX)
    if (data.leftThumbX < -JOYSTICK_DEADZONE) { // Turn Left
        int val = static_cast<int>(map_value(data.leftThumbX, -32768, -JOYSTICK_DEADZONE, PWM_NORMAL_MAX, PWM_MIN));
        pwm_lx[1] = val; // Ch 1 (Front Right)
        pwm_lx[2] = val; // Ch 2 (Rear Left)
    } else if (data.leftThumbX > JOYSTICK_DEADZONE) { // Turn Right
        int val = static_cast<int>(map_value(data.leftThumbX, JOYSTICK_DEADZONE, 32767, PWM_MIN, PWM_NORMAL_MAX));
        pwm_lx[0] = val; // Ch 0 (Front Left)
        pwm_lx[3] = val; // Ch 3 (Rear Right)
    }

    // Rx (Strafe) contribution (maps to PWM_MIN - PWM_NORMAL_MAX)
    if (data.rightThumbX < -JOYSTICK_DEADZONE) { // Strafe Left
        int val = static_cast<int>(map_value(data.rightThumbX, -32768, -JOYSTICK_DEADZONE, PWM_NORMAL_MAX, PWM_MIN));
        pwm_rx[1] = val; // Ch 1 (Front Right) - Assuming X config where FR/RL push left
        pwm_rx[3] = val; // Ch 3 (Rear Right)  - Assuming X config where FR/RL push left
    } else if (data.rightThumbX > JOYSTICK_DEADZONE) { // Strafe Right
        int val = static_cast<int>(map_value(data.rightThumbX, JOYSTICK_DEADZONE, 32767, PWM_MIN, PWM_NORMAL_MAX));
        pwm_rx[0] = val; // Ch 0 (Front Left) - Assuming X config where FL/RR push right
        pwm_rx[2] = val; // Ch 2 (Rear Left)  - Assuming X config where FL/RR push right
    }

    // Combine contributions and apply boost if both sticks are active
    if (lx_active && rx_active) {
        const int boost_range = PWM_BOOST_MAX - PWM_NORMAL_MAX;
        int abs_lx = std::abs(data.leftThumbX);
        int abs_rx = std::abs(data.rightThumbX);
        int weaker_input_abs = std::min(abs_lx, abs_rx);
        int boost_add = static_cast<int>(map_value(weaker_input_abs, JOYSTICK_DEADZONE, 32768, 0, boost_range));

        // Determine which channel gets boosted based on stick directions
        if (data.leftThumbX < 0 && data.rightThumbX < 0) { // Turn L + Strafe L -> Boost Ch 1 (FR)
            pwm_out[0] = std::max(pwm_lx[0], pwm_rx[0]);
            pwm_out[1] = std::max(pwm_lx[1], pwm_rx[1]) + boost_add;
            pwm_out[2] = std::max(pwm_lx[2], pwm_rx[2]);
            pwm_out[3] = std::max(pwm_lx[3], pwm_rx[3]);
        } else if (data.leftThumbX < 0 && data.rightThumbX > 0) { // Turn L + Strafe R -> Boost Ch 2 (RL)
            pwm_out[0] = std::max(pwm_lx[0], pwm_rx[0]);
            pwm_out[1] = std::max(pwm_lx[1], pwm_rx[1]);
            pwm_out[2] = std::max(pwm_lx[2], pwm_rx[2]) + boost_add;
            pwm_out[3] = std::max(pwm_lx[3], pwm_rx[3]);
        } else if (data.leftThumbX > 0 && data.rightThumbX < 0) { // Turn R + Strafe L -> Boost Ch 3 (RR)
            pwm_out[0] = std::max(pwm_lx[0], pwm_rx[0]);
            pwm_out[1] = std::max(pwm_lx[1], pwm_rx[1]);
            pwm_out[2] = std::max(pwm_lx[2], pwm_rx[2]);
            pwm_out[3] = std::max(pwm_lx[3], pwm_rx[3]) + boost_add;
        } else { // Turn R + Strafe R -> Boost Ch 0 (FL)
            pwm_out[0] = std::max(pwm_lx[0], pwm_rx[0]) + boost_add;
            pwm_out[1] = std::max(pwm_lx[1], pwm_rx[1]);
            pwm_out[2] = std::max(pwm_lx[2], pwm_rx[2]);
            pwm_out[3] = std::max(pwm_lx[3], pwm_rx[3]);
        }
    } else {
        // Only one or no stick active: Simple combination (max)
        for (int i = 0; i < 4; ++i) {
            pwm_out[i] = std::max(pwm_lx[i], pwm_rx[i]);
        }
    }

    // Final clamping is done in set_thruster_pwm
}

// 前後進スラスター制御ロジック (controlForwardThrusterの内容を移植・調整)
// Note: Original code only handled forward (value > DEADZONE). Assuming bi-directional control needed.
static int calculate_forward_reverse_pwm(int value) {
    int pulse_width;
    // Using PWM_BOOST_MAX as the limit for forward/reverse thrust
    const int current_max_pwm = PWM_BOOST_MAX;
    const int current_min_pwm = PWM_MIN; // Assuming reverse uses the same range mirrored

    if (std::abs(value) <= JOYSTICK_DEADZONE) {
        pulse_width = PWM_NEUTRAL; // Use neutral for deadzone if ESC requires it, else PWM_MIN
    } else if (value > JOYSTICK_DEADZONE) {
        // Forward thrust (maps DEADZONE to 32767 -> PWM_NEUTRAL to PWM_BOOST_MAX)
        pulse_width = static_cast<int>(map_value(value, JOYSTICK_DEADZONE, 32767, PWM_NEUTRAL, current_max_pwm));
    } else { // value < -JOYSTICK_DEADZONE
        // Reverse thrust (maps -32768 to -DEADZONE -> PWM_MIN to PWM_NEUTRAL)
        pulse_width = static_cast<int>(map_value(value, -32768, -JOYSTICK_DEADZONE, current_min_pwm, PWM_NEUTRAL));
    }
    return pulse_width;
    // Clamping will be done by set_thruster_pwm
}


// メインの更新関数
void thruster_update(const GamepadData& data) {
    int horizontal_pwm[4];
    update_horizontal_thrusters(data, horizontal_pwm);

    // Assuming channel 4 is for forward/reverse based on original main()
    int forward_pwm = calculate_forward_reverse_pwm(data.rightThumbY);

    // --- PWM信号をスラスターに送信 ---
    printf("--- Thruster PWM ---\n");
    for (int i = 0; i < 4; ++i) {
        set_thruster_pwm(i, horizontal_pwm[i]);
        printf("Ch%d: Hori PWM = %d\n", i, horizontal_pwm[i]); // Debug
    }
    set_thruster_pwm(4, forward_pwm);
    printf("Ch4: FwdRev PWM = %d\n", forward_pwm); // Debug
    printf("--------------------\n");
}
