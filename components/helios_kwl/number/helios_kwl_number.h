#pragma once

#include "esphome/components/number/number.h"
#include "../helios_kwl.h"

namespace esphome {
namespace helios_kwl_component {

using NumberFloatSetter = void (HeliosKwlComponent::*)(float);
using NumberUint8Setter = void (HeliosKwlComponent::*)(uint8_t);

class HeliosKwlNumber : public number::Number {
 public:
  void set_parent(HeliosKwlComponent *parent) { parent_ = parent; }

  void set_float_setter(NumberFloatSetter setter) { float_setter_ = setter; }
  void set_uint8_setter(NumberUint8Setter setter) { uint8_setter_ = setter; }

 protected:
  void control(float value) override {
    if (parent_ == nullptr) return;

    if (float_setter_ != nullptr) {
      (parent_->*float_setter_)(value);
    } else if (uint8_setter_ != nullptr) {
      (parent_->*uint8_setter_)(static_cast<uint8_t>(value));
    }

    publish_state(value);
  }

 private:
  HeliosKwlComponent *parent_{nullptr};
  NumberFloatSetter   float_setter_{nullptr};
  NumberUint8Setter   uint8_setter_{nullptr};
};

}  // namespace helios_kwl_component
}  // namespace esphome
