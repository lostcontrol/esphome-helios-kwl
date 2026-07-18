import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import ENTITY_CATEGORY_CONFIG

from .. import CONF_HELIOS_KWL_ID, HELIOS_KWL_COMPONENT_SCHEMA, helios_kwl_component_ns

HeliosKwlButtonBase = helios_kwl_component_ns.class_("HeliosKwlButtonBase", button.Button, cg.Component)
HeliosKwlAckMaintenanceButton = helios_kwl_component_ns.class_(
    "HeliosKwlAckMaintenanceButton", HeliosKwlButtonBase
)

TYPES = {
    "reset_maintenance_reminder": button.button_schema(
        HeliosKwlAckMaintenanceButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:air-filter",
    ),
}

CONFIG_SCHEMA = HELIOS_KWL_COMPONENT_SCHEMA.extend({cv.Optional(type): schema for type, schema in TYPES.items()})


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HELIOS_KWL_ID])
    for type, _ in TYPES.items():
        if type in config:
            btn = await button.new_button(config[type])
            await cg.register_component(btn, config[type])
            cg.add(btn.set_parent(parent))
