#pragma once

#include "../helios_kwl.h"
#include "esphome/components/fan/fan.h"
#include "esphome/core/component.h"

namespace esphome {
namespace helios_kwl_component {

class HeliosKwlComponent;

class HeliosKwlFan : public Component, public fan::Fan {
 public:
  void set_parent(HeliosKwlComponent *parent) { parent_ = parent; }

  fan::FanTraits get_traits() override {
    return fan::FanTraits(false, true, false, 8);
  }

  void control(const fan::FanCall &call) override;

 protected:
  HeliosKwlComponent *parent_{nullptr};
};

}  // namespace helios_kwl_component
}  // namespace esphome
