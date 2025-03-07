import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import DEVICE_CLASS_HEAT, DEVICE_CLASS_POWER, DEVICE_CLASS_PROBLEM

from .. import CONF_HELIOS_KWL_ID, HELIOS_KWL_COMPONENT_SCHEMA

TYPES = {
    "power_state": binary_sensor.binary_sensor_schema(device_class=DEVICE_CLASS_POWER),
    "bypass_state": binary_sensor.binary_sensor_schema(),
    "heating_indicator": binary_sensor.binary_sensor_schema(device_class=DEVICE_CLASS_HEAT),
    "fault_indicator": binary_sensor.binary_sensor_schema(device_class=DEVICE_CLASS_PROBLEM),
    "service_reminder": binary_sensor.binary_sensor_schema(),
}

CONFIG_SCHEMA = HELIOS_KWL_COMPONENT_SCHEMA.extend({cv.Optional(type): schema for type, schema in TYPES.items()})


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HELIOS_KWL_ID])

    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(parent, f"set_{type}_sensor")(sens))
