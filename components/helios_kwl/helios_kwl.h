#pragma once

#include <numeric>

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

namespace esphome {
namespace helios_kwl_component {

class HeliosKwlComponent : public uart::UARTDevice, public PollingComponent {
 private:
  static constexpr uint8_t ADDRESS = 0x2F;
  static constexpr uint8_t MAINBOARD = 0x11;
  static const int TEMPERATURE[];

 public:
  using Datagram = std::array<uint8_t, 6>;

  void setup() override;
  void update() override;
  void dump_config() override;

  void set_fan_speed(float speed);
  void set_state_flag(uint8_t bit, bool state);

  void set_fan_speed_sensor(sensor::Sensor* sensor) { m_fan_speed = sensor; }
  void set_temperature_outside_sensor(sensor::Sensor* sensor) { m_temperature_outside = sensor; }
  void set_temperature_exhaust_sensor(sensor::Sensor* sensor) { m_temperature_exhaust = sensor; }
  void set_temperature_inside_sensor(sensor::Sensor* sensor) { m_temperature_inside = sensor; }
  void set_temperature_incoming_sensor(sensor::Sensor* sensor) { m_temperature_incoming = sensor; }

  void set_power_state_sensor(binary_sensor::BinarySensor* sensor) { m_power_state = sensor; }
  void set_bypass_state_sensor(binary_sensor::BinarySensor* sensor) { m_bypass_state = sensor; }
  void set_heating_indicator_sensor(binary_sensor::BinarySensor* sensor) { m_heating_indicator = sensor; }
  void set_fault_indicator_sensor(binary_sensor::BinarySensor* sensor) { m_fault_indicator = sensor; }
  void set_service_reminder_sensor(binary_sensor::BinarySensor* sensor) { m_service_reminder = sensor; }

  void set_winter_mode_switch(switch_::Switch* switch_) { m_winter_mode_switch = switch_; }

 private:
  void poll_temperature_outside();
  void poll_temperature_exhaust();
  void poll_temperature_inside();
  void poll_temperature_incoming();
  void poll_fan_speed();
  void poll_states();

  optional<uint8_t> poll_register(uint8_t address);

  bool set_value(uint8_t address, uint8_t value);

  void flush_read_buffer();

  template <typename Iterator>
  static bool check_crc(const Iterator begin, const Iterator end) {
    const auto crc = checksum(begin, std::prev(end));
    return *std::prev(end) == crc;
  }

  template <typename Iterator>
  static uint8_t checksum(const Iterator begin, const Iterator end) {
    return std::accumulate(begin, end, 0);
  }

  static uint8_t count_ones(uint8_t byte);

 private:
  sensor::Sensor* m_fan_speed{nullptr};
  sensor::Sensor* m_temperature_outside{nullptr};
  sensor::Sensor* m_temperature_exhaust{nullptr};
  sensor::Sensor* m_temperature_inside{nullptr};
  sensor::Sensor* m_temperature_incoming{nullptr};

  binary_sensor::BinarySensor* m_power_state{nullptr};
  binary_sensor::BinarySensor* m_bypass_state{nullptr};
  binary_sensor::BinarySensor* m_heating_indicator{nullptr};
  binary_sensor::BinarySensor* m_fault_indicator{nullptr};
  binary_sensor::BinarySensor* m_service_reminder{nullptr};

  switch_::Switch* m_winter_mode_switch{nullptr};

  using PollerFunction = std::function<void()>;
  std::vector<PollerFunction> m_pollers{};
  std::vector<PollerFunction>::const_iterator m_current_poller{};
};

}  // namespace helios_kwl_component
}  // namespace esphome