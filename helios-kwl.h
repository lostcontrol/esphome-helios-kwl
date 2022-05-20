#pragma once

#include "esphome.h"

class HeliosKwlComponent : public PollingComponent, public UARTDevice {
 private:
  static constexpr uint8_t ADDRESS = 0x22;
  static constexpr uint8_t MAINBOARD = 0x11;

  static constexpr int TEMPERATURE[] = {
      -74, -70, -66, -62, -59, -56, -54, -52, -50, -48, -47, -46, -44, -43, -42, -41, -40, -39, -38,
      -37, -36, -35, -34, -33, -33, -32, -31, -30, -30, -29, -28, -28, -27, -27, -26, -25, -25, -24,
      -24, -23, -23, -22, -22, -21, -21, -20, -20, -19, -19, -19, -18, -18, -17, -17, -16, -16, -16,
      -15, -15, -14, -14, -14, -13, -13, -12, -12, -12, -11, -11, -11, -10, -10, -9,  -9,  -9,  -8,
      -8,  -8,  -7,  -7,  -7,  -6,  -6,  -6,  -5,  -5,  -5,  -4,  -4,  -4,  -3,  -3,  -3,  -2,  -2,
      -2,  -1,  -1,  -1,  -1,  0,   0,   0,   1,   1,   1,   2,   2,   2,   3,   3,   3,   4,   4,
      4,   5,   5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,
      10,  11,  11,  11,  12,  12,  12,  13,  13,  13,  14,  14,  14,  15,  15,  15,  16,  16,  16,
      17,  17,  18,  18,  18,  19,  19,  19,  20,  20,  21,  21,  21,  22,  22,  22,  23,  23,  24,
      24,  24,  25,  25,  26,  26,  27,  27,  27,  28,  28,  29,  29,  30,  30,  31,  31,  32,  32,
      33,  33,  34,  34,  35,  35,  36,  36,  37,  37,  38,  38,  39,  40,  40,  41,  41,  42,  43,
      43,  44,  45,  45,  46,  47,  48,  48,  49,  50,  51,  52,  53,  53,  54,  55,  56,  57,  59,
      60,  61,  62,  63,  65,  66,  68,  69,  71,  73,  75,  77,  79,  81,  82,  86,  90,  93,  97,
      100, 100, 100, 100, 100, 100, 100, 100, 100};

 public:
  using Datagram = std::array<uint8_t, 6>;

  HeliosKwlComponent(UARTComponent* parent, Sensor* fan_speed, Sensor* sensor_temperature_outside,
                     Sensor* sensor_temperature_exhaust, Sensor* sensor_temperature_inside,
                     Sensor* sensor_temperature_incoming, BinarySensor* power_state,
                     BinarySensor* bypass_state, BinarySensor* heating_indicator,
                     BinarySensor* fault_indicator, BinarySensor* service_reminder)
      : PollingComponent(10 * 1000),
        UARTDevice(parent),
        m_fan_speed{fan_speed},
        m_sensor_temperature_outside{sensor_temperature_outside},
        m_sensor_temperature_exhaust{sensor_temperature_exhaust},
        m_sensor_temperature_inside{sensor_temperature_inside},
        m_sensor_temperature_incoming{sensor_temperature_incoming},
        m_power_state{power_state},
        m_bypass_state{bypass_state},
        m_heating_indicator{heating_indicator},
        m_fault_indicator{fault_indicator},
        m_service_reminder{service_reminder} {}

  void setup() override { ESP_LOGI("custom", "setup()"); }

  void update() override {
    if (const auto value = poll_register(0x32)) {
      m_sensor_temperature_outside->publish_state(TEMPERATURE[*value]);
    }
    if (const auto value = poll_register(0x33)) {
      m_sensor_temperature_exhaust->publish_state(TEMPERATURE[*value]);
    }
    if (const auto value = poll_register(0x34)) {
      m_sensor_temperature_inside->publish_state(TEMPERATURE[*value]);
    }
    if (const auto value = poll_register(0x35)) {
      m_sensor_temperature_incoming->publish_state(TEMPERATURE[*value]);
    }
    if (const auto value = poll_register(0x29)) {
      m_fan_speed->publish_state(count_ones(*value));
    }
    if (const auto value = poll_register(0xA3)) {
      m_power_state->publish_state(*value & (0x01 << 0));
      m_bypass_state->publish_state(*value & (0x01 << 3));
      m_heating_indicator->publish_state(*value & (0x01 << 5));
      m_fault_indicator->publish_state(*value & (0x01 << 6));
      m_service_reminder->publish_state(*value & (0x01 << 7));
    }
  }

  void set_fan_speed(float speed) {
    if (speed == 0.f) {
      set_state_flag(0, false);
    } else {
      assert(speed >= 0.f && speed <= 8.f);
      const uint8_t speed_byte = 0xFF >> (8 - static_cast<int>(speed * 8));
      if (set_value(0x29, speed_byte)) {
        ESP_LOGD("custom", "Wrote speed: %02x", speed_byte);
        set_state_flag(0, true);
      } else {
        ESP_LOGE("custom", "Failed to set fan speed");
      }
    }
  }

  void set_state_flag(uint8_t bit, bool state) {
    if (auto value = poll_register(0xA3)) {
      if (state == ((*value >> bit) & 0x01)) {
        ESP_LOGD("custom", "State flag already set");
      } else {
        if (state) {
          *value |= 0x01 << bit;
        } else {
          *value &= ~(0x01 << bit);
        }
        if (set_value(0xA3, *value)) {
          ESP_LOGD("custom", "Wrote state flag to: %x", *value);
        } else {
          ESP_LOGE("custom", "Failed to set state flag");
        }
      }
    } else {
      ESP_LOGE("custom", "Unable to poll register 0xA3");
    }
  }

 private:
  optional<uint8_t> poll_register(uint8_t address) {
    // Flush read buffer
    flush_read_buffer();

    // Write
    Datagram temp = {0x01, ADDRESS, MAINBOARD, 0x00, address};
    temp[5] = checksum(temp.cbegin(), temp.cend());
    write_array(temp);
    flush();

    // Read
    if (const auto response = read_array<6>()) {
      const auto& array = *response;
      if (check_crc(array.cbegin(), array.cend())) {
        if (array[1] == MAINBOARD && array[2] == ADDRESS && array[3] == address) {
          return array[4];
        } else {
          const auto hex = format_hex_pretty(array.data(), array.size());
          ESP_LOGE("custom", "Wrong response from mainboard: %s", hex.c_str());
        }
      } else {
        const auto hex = format_hex_pretty(array.data(), array.size());
        ESP_LOGE("custom", "Bad checksum for response: %s", hex.c_str());
      }
    }
    return {};
  }

  bool set_value(uint8_t address, uint8_t value) {
    Datagram temp = {0x01, ADDRESS, MAINBOARD, address, value};
    temp[5] = checksum(temp.cbegin(), temp.cend());

    // To the mainboard
    int retry = 3;
    do {
      // Flush read buffer
      flush_read_buffer();
      // Write
      write_array(temp);
      flush();
    } while (read() != temp[5] && retry-- > 0);

    return retry >= 0;
  }

  void flush_read_buffer() {
    // Flush read buffer and wait for the bus to be quiet for 10 ms
    uint32_t last_time = millis();
    while (millis() - last_time < 10) {
      while (available()) {
        read();
        last_time = millis();
      }
      yield();
    }
  }

  template <typename Iterator>
  static bool check_crc(const Iterator begin, const Iterator end) {
    const auto crc = checksum(begin, std::prev(end));
    return *std::prev(end) == crc;
  }

  template <typename Iterator>
  static uint8_t checksum(const Iterator begin, const Iterator end) {
    return std::accumulate(begin, end, 0);
  }

  static uint8_t count_ones(uint8_t byte) {
    static const uint8_t NIBBLE_LOOKUP[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
    return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
  }

 private:
  Sensor* m_fan_speed{nullptr};
  Sensor* m_sensor_temperature_outside{nullptr};
  Sensor* m_sensor_temperature_exhaust{nullptr};
  Sensor* m_sensor_temperature_inside{nullptr};
  Sensor* m_sensor_temperature_incoming{nullptr};

  BinarySensor* m_power_state{nullptr};
  BinarySensor* m_bypass_state{nullptr};
  BinarySensor* m_heating_indicator{nullptr};
  BinarySensor* m_fault_indicator{nullptr};
  BinarySensor* m_service_reminder{nullptr};
};

constexpr int HeliosKwlComponent::TEMPERATURE[256];

class HeliosKwlSpeedOutput : public Component, public FloatOutput {
 public:
  HeliosKwlSpeedOutput(HeliosKwlComponent* component) : m_component{component} {}

  void setup() override {}

  void write_state(float state) override { m_component->set_fan_speed(state); }

 private:
  HeliosKwlComponent* m_component;
};

class HeliosKwlWinterModeOutput : public Component, public BinaryOutput {
 public:
  HeliosKwlWinterModeOutput(HeliosKwlComponent* component) : m_component{component} {}

  void setup() override {}

  void write_state(bool state) override { m_component->set_state_flag(3, state); }

 private:
  HeliosKwlComponent* m_component;
};
