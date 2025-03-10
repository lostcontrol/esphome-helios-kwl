import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

helios_kwl_component_ns = cg.esphome_ns.namespace("helios_kwl_component")
HeliosKwlComponent = helios_kwl_component_ns.class_("HeliosKwlComponent", cg.PollingComponent, uart.UARTDevice)

CONF_HELIOS_KWL_ID = "helios_kwl_id"


HELIOS_KWL_COMPONENT_SCHEMA = cv.Schema({cv.Required(CONF_HELIOS_KWL_ID): cv.use_id(HeliosKwlComponent)})

CONFIG_SCHEMA = (
    cv.Schema({cv.GenerateID(): cv.declare_id(HeliosKwlComponent)})
    .extend(cv.polling_component_schema("2s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
