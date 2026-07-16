#include "helios_kwl.h"
#include "fan/helios_kwl_fan.h"

#include "esphome/core/log.h"

namespace esphome {
namespace helios_kwl_component {

static const char* TAG = "helios_kwl_component.component";

const int HeliosKwlComponent::TEMPERATURE[] = {
    -74, -70, -66, -62, -59, -56, -54, -52, -50, -48, -47, -46, -44, -43, -42, -41, -40, -39, -38, -37, -36, -35,
    -34, -33, -33, -32, -31, -30, -30, -29, -28, -28, -27, -27, -26, -25, -25, -24, -24, -23, -23, -22, -22, -21,
    -21, -20, -20, -19, -19, -19, -18, -18, -17, -17, -16, -16, -16, -15, -15, -14, -14, -14, -13, -13, -12, -12,
    -12, -11, -11, -11, -10, -10, -9,  -9,  -9,  -8,  -8,  -8,  -7,  -7,  -7,  -6,  -6,  -6,  -5,  -5,  -5,  -4,
    -4,  -4,  -3,  -3,  -3,  -2,  -2,  -2,  -1,  -1,  -1,  -1,  0,   0,   0,   1,   1,   1,   2,   2,   2,   3,
    3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,
    10,  10,  11,  11,  11,  12,  12,  12,  13,  13,  13,  14,  14,  14,  15,  15,  15,  16,  16,  16,  17,  17,
    18,  18,  18,  19,  19,  19,  20,  20,  21,  21,  21,  22,  22,  22,  23,  23,  24,  24,  24,  25,  25,  26,
    26,  27,  27,  27,  28,  28,  29,  29,  30,  30,  31,  31,  32,  32,  33,  33,  34,  34,  35,  35,  36,  36,
    37,  37,  38,  38,  39,  40,  40,  41,  41,  42,  43,  43,  44,  45,  45,  46,  47,  48,  48,  49,  50,  51,
    52,  53,  53,  54,  55,  56,  57,  59,  60,  61,  62,  63,  65,  66,  68,  69,  71,  73,  75,  77,  79,  81,
    82,  86,  90,  93,  97,  100, 100, 100, 100, 100, 100, 100, 100, 100};

void HeliosKwlComponent::setup() {
  ESP_LOGI(TAG, "setup()");

  if (m_temperature_outside != nullptr) {
    m_pollers.push_back([this]() { poll_temperature_outside(); });
  }

  if (m_temperature_exhaust != nullptr) {
    m_pollers.push_back([this]() { poll_temperature_exhaust(); });
  }

  if (m_temperature_inside != nullptr) {
    m_pollers.push_back([this]() { poll_temperature_inside(); });
  }

  if (m_temperature_incoming != nullptr) {
    m_pollers.push_back([this]() { poll_temperature_incoming(); });
  }

  const std::vector<const EntityBase*> states{m_power_state,       m_bypass_state,    m_winter_mode_switch,
                                              m_heating_indicator, m_fault_indicator, m_service_reminder};
  if (std::any_of(states.cbegin(), states.cend(), [](const EntityBase* pointer) { return pointer != nullptr; })) {
    m_pollers.push_back([&]() { poll_states(); });
  }

  m_current_poller = m_pollers.cbegin();
}

void HeliosKwlComponent::update() {
  if (m_current_poller != m_pollers.cend()) {
    (*m_current_poller)();
    // Move to the next polling function
    std::advance(m_current_poller, 1);
  } else {
    // Start again from the beginning
    m_current_poller = m_pollers.cbegin();
  }
}

void HeliosKwlComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Helios KWL:");
  LOG_SENSOR("  ", "Temperature outside", m_temperature_outside);
  LOG_SENSOR("  ", "Temperature exhaust", m_temperature_exhaust);
  LOG_SENSOR("  ", "Temperature inside", m_temperature_inside);
  LOG_SENSOR("  ", "Temperature incoming", m_temperature_incoming);
  LOG_BINARY_SENSOR("  ", "Power state", m_power_state);
  LOG_BINARY_SENSOR("  ", "Bypass state", m_bypass_state);
  LOG_BINARY_SENSOR("  ", "Fault indicator", m_fault_indicator);
  LOG_BINARY_SENSOR("  ", "Service reminder", m_service_reminder);
}

void HeliosKwlComponent::control_fan(bool on, optional<uint8_t> speed) {
  if (!on) {
    set_state_flag(0, false);
  } else {
    if (speed.has_value()) {
      uint8_t s = *speed;
      if (s >= 1 && s <= 8) {
        const uint8_t speed_byte = 0xFF >> (8 - s);
        if (set_value(0x29, speed_byte)) {
          ESP_LOGD(TAG, "Wrote speed: %02x", speed_byte);
          set_state_flag(0, true);
        } else {
          ESP_LOGE(TAG, "Failed to set fan speed");
        }
      }
    } else {
      set_state_flag(0, true);
    }
  }
}

void HeliosKwlComponent::set_state_flag(uint8_t bit, bool state) {
  if (auto value = poll_register(0xA3)) {
    if (state == ((*value >> bit) & 0x01)) {
      ESP_LOGD(TAG, "State flag already set");
    } else {
      if (state) {
        *value |= 0x01 << bit;
      } else {
        *value &= ~(0x01 << bit);
      }
      if (set_value(0xA3, *value)) {
        ESP_LOGD(TAG, "Wrote state flag to: %x", *value);
      } else {
        ESP_LOGE(TAG, "Failed to set state flag");
      }
    }
  } else {
    ESP_LOGE(TAG, "Unable to poll register 0xA3");
  }
}

void HeliosKwlComponent::poll_temperature_outside() {
  if (const auto value = poll_register(0x32)) {
    m_temperature_outside->publish_state(TEMPERATURE[*value]);
  }
}

void HeliosKwlComponent::poll_temperature_exhaust() {
  if (const auto value = poll_register(0x33)) {
    m_temperature_exhaust->publish_state(TEMPERATURE[*value]);
  }
}

void HeliosKwlComponent::poll_temperature_inside() {
  if (const auto value = poll_register(0x34)) {
    m_temperature_inside->publish_state(TEMPERATURE[*value]);
  }
}

void HeliosKwlComponent::poll_temperature_incoming() {
  if (const auto value = poll_register(0x35)) {
    m_temperature_incoming->publish_state(TEMPERATURE[*value]);
  }
}

void HeliosKwlComponent::poll_states() {
  if (const auto value = poll_register(0xA3)) {
    bool pwr = *value & (0x01 << 0);
    if (m_power_state != nullptr) {
      m_power_state->publish_state(pwr);
    }

    // Also poll fan speed to sync the fan component speed level
    if (m_fan != nullptr) {
      bool fan_changed = false;
      if (m_fan->state != pwr) {
        m_fan->state = pwr;
        fan_changed = true;
      }

      if (const auto speed_val = poll_register(0x29)) {
        uint8_t spd = count_ones(*speed_val);
        if (spd > 0 && m_fan->speed != spd) {
          m_fan->speed = spd;
          fan_changed = true;
        }
      }

      if (fan_changed) {
        m_fan->publish_state();
      }
    }
    const bool bypass_state = *value & (0x01 << 3);
    if (m_bypass_state != nullptr) {
      m_bypass_state->publish_state(bypass_state);
    }
    if (m_winter_mode_switch != nullptr) {
      m_winter_mode_switch->publish_state(bypass_state);
    }
    if (m_heating_indicator != nullptr) {
      m_heating_indicator->publish_state(*value & (0x01 << 5));
    }
    if (m_fault_indicator != nullptr) {
      m_fault_indicator->publish_state(*value & (0x01 << 6));
    }
    if (m_service_reminder != nullptr) {
      m_service_reminder->publish_state(*value & (0x01 << 7));
    }
  }
}

optional<uint8_t> HeliosKwlComponent::poll_register(uint8_t address) {
  // Flush read buffer
  flush_read_buffer();

  // Write
  Datagram temp = {0x01, ADDRESS, MAINBOARD, 0x00, address};
  temp[5] = checksum(temp.cbegin(), temp.cend());
  write_array(temp);
  flush();

  // Read
  uint32_t start_time = millis();
  while (millis() - start_time < 50) {
    if (available()) {
      uint8_t byte;
      if (read_byte(&byte) && byte == 0x01) {
        std::array<uint8_t, 6> array;
        array[0] = byte;
        uint8_t i = 1;
        uint32_t wait_start = millis();
        while (i < 6 && millis() - wait_start < 20) {
          if (available()) {
            read_byte(&array[i]);
            i++;
            wait_start = millis();
          } else {
            yield();
          }
        }

        if (i == 6) {
          if (check_crc(array.cbegin(), array.cend())) {
            if (array[1] == MAINBOARD && array[2] == ADDRESS && array[3] == address) {
              return array[4];
            } else {
              const auto hex = format_hex_pretty(array.data(), array.size());
              ESP_LOGD(TAG, "Wrong response from mainboard: %s", hex.c_str());
            }
          } else {
            const auto hex = format_hex_pretty(array.data(), array.size());
            ESP_LOGW(TAG, "Bad checksum for response: %s", hex.c_str());
          }
        }
      }
    }
    yield();
  }
  return {};
}

bool HeliosKwlComponent::set_value(uint8_t address, uint8_t value) {
  Datagram temp = {0x01, ADDRESS, MAINBOARD, address, value};
  temp[5] = checksum(temp.cbegin(), temp.cend());

  // To the mainboard
  int retry = 3;
  bool success = false;
  do {
    // Flush read buffer
    flush_read_buffer();
    // Write
    write_array(temp);
    flush();

    // Wait for echo byte
    uint32_t wait_start = millis();
    while (millis() - wait_start < 50) {
      if (available()) {
        int byte = read();
        if (byte == temp[5]) {
          success = true;
          break;
        }
      } else {
        yield();
      }
    }
  } while (!success && retry-- > 0);

  return success;
}

void HeliosKwlComponent::flush_read_buffer() {
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

uint8_t HeliosKwlComponent::count_ones(uint8_t byte) {
  static const uint8_t NIBBLE_LOOKUP[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
  return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
}

}  // namespace helios_kwl_component
}  // namespace esphome