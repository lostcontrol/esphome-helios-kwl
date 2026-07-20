#pragma once

#include "../helios_kwl.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace helios_kwl_component {

using NumberInt8Setter = void (HeliosKwlComponent::*)(int8_t);
using NumberUint8Setter = void (HeliosKwlComponent::*)(uint8_t);

class HeliosKwlNumber : public number::Number {
 public:
  void set_parent(HeliosKwlComponent* parent) { m_parent = parent; }

  void set_int8_setter(NumberInt8Setter setter) { m_int8_setter = setter; }
  void set_uint8_setter(NumberUint8Setter setter) { m_uint8_setter = setter; }

 protected:
  void control(float value) override {
    if (m_parent == nullptr) return;

    // Protect against Not-a-Number (NaN)
    if (std::isnan(value)) return;

    if (m_int8_setter != nullptr) {
      const float clamped = std::clamp(value, -128.0f, 127.0f);
      (m_parent->*m_int8_setter)(static_cast<int8_t>(clamped));
    }

    if (m_uint8_setter != nullptr) {
      const float clamped = std::clamp(value, 0.0f, 255.0f);
      (m_parent->*m_uint8_setter)(static_cast<uint8_t>(clamped));
    }

    publish_state(value);
  }

 private:
  HeliosKwlComponent* m_parent{nullptr};
  NumberInt8Setter m_int8_setter{nullptr};
  NumberUint8Setter m_uint8_setter{nullptr};
};

}  // namespace helios_kwl_component
}  // namespace esphome
