#include "helios_kwl_fan.h"

#include "esphome/core/log.h"

namespace esphome {
namespace helios_kwl_component {

static const char* TAG = "helios_kwl.fan";

void HeliosKwlFan::control(const fan::FanCall& call) {
  if (m_parent == nullptr) return;

  optional<uint8_t> speed;
  if (call.get_speed().has_value()) {
    speed = static_cast<uint8_t>(*call.get_speed());
  }

  bool on = call.get_state().value_or(this->state);

  m_parent->control_fan(on, speed);

  if (call.get_state().has_value()) {
    this->state = *call.get_state();
  }
  if (speed.has_value()) {
    this->speed = *speed;
  }
  this->publish_state();
}

}  // namespace helios_kwl_component
}  // namespace esphome
