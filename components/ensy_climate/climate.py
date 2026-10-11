"""Ensy LMC311 climate: one HA thermostat wrapping the AHU's Modbus entities.

Holds no Modbus logic itself — reads state from, and writes through, the
switch/select/number entities declared in ensy-ahu.yaml.
"""

import esphome.codegen as cg
from esphome.components import climate, number, select, sensor, switch, text_sensor
import esphome.config_validation as cv

ensy_ns = cg.esphome_ns.namespace("ensy_climate")
EnsyClimate = ensy_ns.class_("EnsyClimate", climate.Climate, cg.Component)

REFS = {
    "current_temperature": sensor.Sensor,
    "current_humidity": sensor.Sensor,
    "heater_capacity": sensor.Sensor,
    "power": switch.Switch,
    "mode_select": select.Select,
    "fan_step": select.Select,
    "boost": select.Select,
    "setpoint": number.Number,
    "boost_minutes": number.Number,
    "boost_length": number.Number,
    "hood_length": number.Number,
    "state_text": text_sensor.TextSensor,
}

CONFIG_SCHEMA = (
    climate.climate_schema(EnsyClimate)
    .extend({cv.Required(k): cv.use_id(t) for k, t in REFS.items()})
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await climate.new_climate(config)
    await cg.register_component(var, config)
    for key in REFS:
        cg.add(getattr(var, f"set_{key}")(await cg.get_variable(config[key])))
