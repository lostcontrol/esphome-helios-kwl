#pragma once

#include <numeric>

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

namespace esphome {
namespace helios_kwl_component {

class HeliosKwlFan;

class HeliosKwlComponent : public uart::UARTDevice, public PollingComponent {
 private:
  static constexpr uint8_t ADDRESS = 0x2F;
  static constexpr uint8_t MAINBOARD = 0x11;

 public:
  using Datagram = std::array<uint8_t, 6>;

  void setup() override;
  void update() override;
  void dump_config() override;

  void control_fan(bool on, optional<uint8_t> speed);
  void set_state_flag(uint8_t bit, bool state);

  void control_bypass_operating_temperature(int8_t temperature);
  void control_dc_supply_air_fan_control_setpoint(uint8_t setpoint);
  void control_dc_exhaust_fan_control_setpoint(uint8_t setpoint);

  void control_max_fan_speed(uint8_t speed);
  void control_basic_fan_speed(uint8_t speed);
  void control_service_reminder_interval(uint8_t interval);
  void reset_maintenance_reminder();

  void set_fan(HeliosKwlFan* fan) { m_fan = fan; }

  void set_temperature_outside_sensor(sensor::Sensor* sensor) { m_temperature_outside = sensor; }
  void set_temperature_exhaust_sensor(sensor::Sensor* sensor) { m_temperature_exhaust = sensor; }
  void set_temperature_inside_sensor(sensor::Sensor* sensor) { m_temperature_inside = sensor; }
  void set_temperature_incoming_sensor(sensor::Sensor* sensor) { m_temperature_incoming = sensor; }
  void set_fan_speed_sensor(sensor::Sensor* sensor) { m_fan_speed = sensor; }
  void set_service_reminder_monthly_counter_sensor(sensor::Sensor* sensor) {
    m_service_reminder_monthly_counter = sensor;
  }

  void set_power_state_sensor(binary_sensor::BinarySensor* sensor) { m_power_state = sensor; }
  void set_bypass_state_sensor(binary_sensor::BinarySensor* sensor) { m_bypass_state = sensor; }
  void set_heating_indicator_sensor(binary_sensor::BinarySensor* sensor) { m_heating_indicator = sensor; }
  void set_fault_indicator_sensor(binary_sensor::BinarySensor* sensor) { m_fault_indicator = sensor; }
  void set_service_reminder_sensor(binary_sensor::BinarySensor* sensor) { m_service_reminder = sensor; }

  void set_winter_mode_switch(switch_::Switch* switch_) { m_winter_mode_switch = switch_; }

  void set_bypass_operating_temperature_number(number::Number* number) { m_bypass_operating_temperature = number; }
  void set_dc_supply_air_fan_control_setpoint_number(number::Number* number) {
    m_dc_supply_air_fan_control_setpoint = number;
  }
  void set_dc_exhaust_fan_control_setpoint_number(number::Number* number) {
    m_dc_exhaust_fan_control_setpoint = number;
  }

  void set_max_fan_speed_number(number::Number* number) { m_max_fan_speed = number; }
  void set_basic_fan_speed_number(number::Number* number) { m_basic_fan_speed = number; }
  void set_service_reminder_interval_number(number::Number* number) { m_service_reminder_interval = number; }

 private:
  void poll_temperature_outside();
  void poll_temperature_exhaust();
  void poll_temperature_inside();
  void poll_temperature_incoming();
  void poll_states();
  void poll_io_port();

  void poll_bypass_operating_temperature();
  void poll_dc_supply_air_fan_control_setpoint();
  void poll_dc_exhaust_fan_control_setpoint();

  void poll_max_fan_speed();
  void poll_basic_fan_speed();
  void poll_service_reminder_interval();
  void poll_service_reminder_monthly_counter();

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

  static float ntc_to_celsius(uint8_t ntc);
  static uint8_t celsius_to_ntc(int8_t temperature);

 private:
  sensor::Sensor* m_temperature_outside{nullptr};
  sensor::Sensor* m_temperature_exhaust{nullptr};
  sensor::Sensor* m_temperature_inside{nullptr};
  sensor::Sensor* m_temperature_incoming{nullptr};
  sensor::Sensor* m_fan_speed{nullptr};

  binary_sensor::BinarySensor* m_power_state{nullptr};
  binary_sensor::BinarySensor* m_bypass_state{nullptr};
  binary_sensor::BinarySensor* m_heating_indicator{nullptr};
  binary_sensor::BinarySensor* m_fault_indicator{nullptr};
  binary_sensor::BinarySensor* m_service_reminder{nullptr};

  switch_::Switch* m_winter_mode_switch{nullptr};

  number::Number* m_bypass_operating_temperature{nullptr};
  number::Number* m_dc_supply_air_fan_control_setpoint{nullptr};
  number::Number* m_dc_exhaust_fan_control_setpoint{nullptr};

  number::Number* m_max_fan_speed{nullptr};
  number::Number* m_basic_fan_speed{nullptr};
  number::Number* m_service_reminder_interval{nullptr};
  sensor::Sensor* m_service_reminder_monthly_counter{nullptr};

  HeliosKwlFan* m_fan{nullptr};

  using PollerFunction = std::function<void()>;
  std::vector<PollerFunction> m_pollers{};
  std::vector<PollerFunction>::const_iterator m_current_poller{};
};

}  // namespace helios_kwl_component
}  // namespace esphome
