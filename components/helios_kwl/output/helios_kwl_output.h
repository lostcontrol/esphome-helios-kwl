#pragma once

#include "../helios_kwl.h"
#include "esphome/components/output/float_output.h"

namespace esphome {
namespace helios_kwl_component {

class HeliosKwlSpeedOutput : public output::FloatOutput {
 public:
  void set_parent(HeliosKwlComponent* parent) { m_parent = parent; };

  void write_state(float state) override { m_parent->set_fan_speed(state); }

 private:
  HeliosKwlComponent* m_parent;
};

}  // namespace helios_kwl_component
}  // namespace esphome
