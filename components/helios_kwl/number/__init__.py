import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_MAX_VALUE, CONF_MIN_VALUE, CONF_STEP, ENTITY_CATEGORY_CONFIG, UNIT_CELSIUS, UNIT_PERCENT

from .. import CONF_HELIOS_KWL_ID, HELIOS_KWL_COMPONENT_SCHEMA, helios_kwl_component_ns

HeliosKwlNumber = helios_kwl_component_ns.class_("HeliosKwlNumber", number.Number)


def number_schema(min_value, max_value, step, unit, icon):
    return number.number_schema(
        HeliosKwlNumber, entity_category=ENTITY_CATEGORY_CONFIG, unit_of_measurement=unit, icon=icon
    ).extend(
        {
            cv.Optional(CONF_MIN_VALUE, default=min_value): cv.int_,
            cv.Optional(CONF_MAX_VALUE, default=max_value): cv.int_,
            cv.Optional(CONF_STEP, default=step): cv.positive_int,
        }
    )


TYPES = {
    "bypass_operating_temperature": number_schema(0, 20, 1, UNIT_CELSIUS, "mdi:thermometer-chevron-up"),
    "dc_supply_air_fan_control_setpoint": number_schema(65, 100, 1, UNIT_PERCENT, "mdi:fan-speed-1"),
    "dc_exhaust_fan_control_setpoint": number_schema(65, 100, 1, UNIT_PERCENT, "mdi:fan-speed-2"),
    "max_fan_speed": number_schema(1, 8, 1, "", "mdi:fan-plus"),
    "basic_fan_speed": number_schema(1, 8, 1, "", "mdi:fan-minus"),
    "service_reminder_interval": number_schema(1, 15, 1, "months", "mdi:wrench-clock"),
}

CONFIG_SCHEMA = HELIOS_KWL_COMPONENT_SCHEMA.extend({cv.Optional(type): schema for type, schema in TYPES.items()})

_MAP = [
    (
        "bypass_operating_temperature",
        "set_bypass_operating_temperature_number",
        "set_int8_setter",
        "control_bypass_operating_temperature",
    ),
    (
        "dc_supply_air_fan_control_setpoint",
        "set_dc_supply_air_fan_control_setpoint_number",
        "set_uint8_setter",
        "control_dc_supply_air_fan_control_setpoint",
    ),
    (
        "dc_exhaust_fan_control_setpoint",
        "set_dc_exhaust_fan_control_setpoint_number",
        "set_uint8_setter",
        "control_dc_exhaust_fan_control_setpoint",
    ),
    ("max_fan_speed", "set_max_fan_speed_number", "set_uint8_setter", "control_max_fan_speed"),
    ("basic_fan_speed", "set_basic_fan_speed_number", "set_uint8_setter", "control_basic_fan_speed"),
    (
        "service_reminder_interval",
        "set_service_reminder_interval_number",
        "set_uint8_setter",
        "control_service_reminder_interval",
    ),
]


async def to_code(config):
    parent = await cg.get_variable(config[CONF_HELIOS_KWL_ID])
    for ck, ps, nsm, cm in _MAP:
        if ck in config:
            conf = config[ck]
            num = await number.new_number(
                conf, min_value=conf[CONF_MIN_VALUE], max_value=conf[CONF_MAX_VALUE], step=conf[CONF_STEP]
            )
            cg.add(getattr(parent, ps)(num))
            cg.add(num.set_parent(parent))
            cg.add(getattr(num, nsm)(cg.RawExpression(f"&helios_kwl_component::HeliosKwlComponent::{cm}")))
