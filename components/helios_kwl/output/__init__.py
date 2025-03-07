import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import (
    CONF_ID,
)

from .. import CONF_HELIOS_KWL_ID, HELIOS_KWL_COMPONENT_SCHEMA, helios_kwl_component_ns

DEPENDENCIES = ["uart"]


HeliosKwlSpeedOutput = helios_kwl_component_ns.class_("HeliosKwlSpeedOutput", output.FloatOutput)

TYPES = {
    "fan_speed": output.FLOAT_OUTPUT_SCHEMA.extend({
        cv.Required(CONF_ID): cv.declare_id(HeliosKwlSpeedOutput),
    }),
}

CONFIG_SCHEMA = HELIOS_KWL_COMPONENT_SCHEMA.extend({cv.Optional(type): schema for type, schema in TYPES.items()})


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HELIOS_KWL_ID])
    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            var = cg.new_Pvariable(conf[CONF_ID])
            await output.register_output(var, conf)
            # cg.add(getattr(parent, f"set_{type}_switch")(var))
            cg.add(var.set_parent(parent))
