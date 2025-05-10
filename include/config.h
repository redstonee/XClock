#pragma once

// Battery Monitor
constexpr auto CHARGE_STATUS_PIN = 35;
constexpr auto BAT_VSENSE_PIN = 39;

constexpr auto VBAT_FULL = 4200;        // in mV
constexpr auto VBAT_EMPTY = 3300;       // in mV
constexpr auto N_BATT_SERIES = 2;       // number of batteries in series
constexpr auto VBAT_MULTIPLIER = 4.35f; // for ADC reading
constexpr auto VBAT_FILTER_BUFF_SIZE = 200;
constexpr auto VBAT_SAMPLE_PERIOD = 20; // in ms
constexpr auto ADC_RES = 12;            // ADC resolution

// Microphone
constexpr auto MIC_SIG_PIN = 34;
constexpr auto MIC_EN_PIN = 16;
constexpr auto NOISE_THRESH = 500.0f;

// RGB Matrix
constexpr auto LED_EN_PIN = 27;
constexpr auto LED_DATA_PIN = 17;

// RTC
constexpr auto RTC_SDA_PIN = 23;
constexpr auto RTC_SCL_PIN = 22;
constexpr auto RTC_ADDR = 0x32;

// Buttons
constexpr auto BUTTON_LEFT_PIN = 32;
constexpr auto BUTTON_OK_PIN = 33;
constexpr auto BUTTON_RIGHT_PIN = 25;

// Audio
constexpr auto AUDIO_DATA_PIN = 19;
constexpr auto AUDIO_BCLK_PIN = 18;
constexpr auto AUDIO_LR_PIN = 5;
constexpr auto AUDIO_EN_PIN = 21;

// Light Sensor
constexpr auto LIGHT_SENSOR_PIN = 36; // ADC1_CH0
constexpr auto LIGHT_FILTER_BUFF_SIZE = 200;

constexpr auto SLEEP_TIME = 5e6; // in microseconds
