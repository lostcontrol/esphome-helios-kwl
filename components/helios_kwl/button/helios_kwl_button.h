#pragma once

#include "esphome/components/button/button.h"
#include "../helios_kwl.h"

namespace esphome {
namespace helios_kwl_component {

class HeliosKwlButtonBase : public button::Button, public Component {
 public:
  void set_parent(HeliosKwlComponent *parent) { m_parent = parent; }

 protected:
  HeliosKwlComponent *m_parent{nullptr};
};

class HeliosKwlAckMaintenanceButton : public HeliosKwlButtonBase {
 protected:
  void press_action() override {
    if (m_parent != nullptr) {
      m_parent->acknowledge_maintenance();
    }
  }
};

}  // namespace helios_kwl_component
}  // namespace esphome
