#pragma once

#include "../helios_kwl.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace helios_kwl_component {

class HeliosKwlWinterMode : public Component, public switch_::Switch {
 public:
  void set_parent(HeliosKwlComponent* parent) { m_parent = parent; };

  void write_state(bool state) override { m_parent->set_state_flag(3, state); }

 private:
  HeliosKwlComponent* m_parent;
};

}  // namespace helios_kwl_component
}  // namespace esphome
