#include <Arduino.h>
#include "BattMon.h"

#include "config.h"

namespace BattMon
{

    static uint32_t adcBuffer[VBAT_FILTER_BUFF_SIZE]{0};
    static uint8_t dataIndex = 0;
    static bool bufferFilled = false;

    const char *TAG = "BattMon";
    /**
     * @brief Read the battery voltage and store it in the buffer.
     * @param TimerHandle_t Timer handle (not used).
     *
     * @note This function is called periodically by a FreeRTOS timer.
     */
    void readVoltage(TimerHandle_t)
    {
        adcBuffer[dataIndex++] = analogReadMilliVolts(BAT_VSENSE_PIN);
        if (dataIndex >= VBAT_FILTER_BUFF_SIZE)
        {
            dataIndex = 0;
            bufferFilled = true;
        }
    }

    /**
     * @brief Initialize the battery monitor.
     *
     */
    void begin()
    {
        analogReadResolution(ADC_RES);     // Set ADC resolution
        pinMode(CHARGE_STATUS_PIN, INPUT); // battery charging status
        auto readBattTimer = xTimerCreate("ReadBatteryTimer", pdMS_TO_TICKS(VBAT_SAMPLE_PERIOD), pdTRUE, nullptr, readVoltage);
        xTimerStart(readBattTimer, 0);
    }

    /**
     * @brief Check if the battery is charging.
     *
     * @return true if charging, false otherwise.
     */
    bool isCharging()
    {
        return !digitalRead(CHARGE_STATUS_PIN);
    }

    /**
     * @brief Get the filtered battery level as a percentage.
     *
     * @return Battery level (0-100).
     */
    uint8_t getBatteryLevel()
    {
        uint32_t sum = 0;

        // Average the samples
        if (bufferFilled)
        {
            for (int i = 0; i < VBAT_FILTER_BUFF_SIZE; ++i)
            {
                sum += adcBuffer[i];
            }
            sum /= VBAT_FILTER_BUFF_SIZE;
        }
        else
        {
            if (!dataIndex)
                return 0; // Avoid division by zero

            for (int i = 0; i < dataIndex; ++i)
            {
                sum += adcBuffer[i];
            }
            sum /= dataIndex;
        }

        auto voltage = sum * VBAT_MULTIPLIER / N_BATT_SERIES; // Convert to voltage per cell in mV

        // ESP_LOGD(TAG, "Battery voltage: %f mV", voltage);

        // Map the voltage to a percentage
        if (voltage <= VBAT_EMPTY)
            return 0;
        if (voltage >= VBAT_FULL)
            return 100;
        return static_cast<uint8_t>((voltage - VBAT_EMPTY) * 100 / (VBAT_FULL - VBAT_EMPTY));
    }
} // namespace BattMon
