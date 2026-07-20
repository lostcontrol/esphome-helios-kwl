#pragma once

#include "../helios_kwl.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace helios_kwl_component {

class HeliosKwlButtonBase : public button::Button, public Component {
 public:
  void set_parent(HeliosKwlComponent *parent) { m_parent = parent; }

 protected:
  HeliosKwlComponent *m_parent{nullptr};
};

class HeliosKwlResetMaintenanceReminderButton : public HeliosKwlButtonBase {
 protected:
  void press_action() override {
    if (m_parent != nullptr) {
      m_parent->reset_maintenance_reminder();
    }
  }
};

}  // namespace helios_kwl_component
}  // namespace esphome
