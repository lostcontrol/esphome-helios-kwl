import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch

from .. import CONF_HELIOS_KWL_ID, HELIOS_KWL_COMPONENT_SCHEMA, helios_kwl_component_ns

HeliosKwlWinterMode = helios_kwl_component_ns.class_("HeliosKwlWinterMode", switch.Switch, cg.Component)

TYPES = {
    "winter_mode": switch.switch_schema(HeliosKwlWinterMode, block_inverted=True),
}

CONFIG_SCHEMA = HELIOS_KWL_COMPONENT_SCHEMA.extend({cv.Optional(type): schema for type, schema in TYPES.items()})


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HELIOS_KWL_ID])
    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            var = await switch.new_switch(conf)
            await cg.register_component(var, conf)
            cg.add(var.set_parent(parent))
