import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan
from esphome.const import CONF_ID

from .. import CONF_HELIOS_KWL_ID, HeliosKwlComponent, helios_kwl_component_ns

HeliosKwlFan = helios_kwl_component_ns.class_("HeliosKwlFan", fan.Fan, cg.Component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_ID): cv.declare_id(HeliosKwlFan),
            cv.GenerateID(CONF_HELIOS_KWL_ID): cv.use_id(HeliosKwlComponent),
        }
    )
    .extend(fan.fan_schema(HeliosKwlFan))
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HELIOS_KWL_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await fan.register_fan(var, config)
    cg.add(var.set_parent(parent))
    cg.add(parent.set_fan(var))
