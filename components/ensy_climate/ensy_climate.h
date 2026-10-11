#pragma once
// Ensy LMC311 climate — see climate.py. Mode/fan/preset map:
//   OFF = power (1001) off; HEAT/COOL/AUTO = power on + mode select (1002)
//   fan OFF/LOW/MEDIUM/HIGH = fan step (1003) 0..3
//   preset BOOST / "Cooker hood" = party mode (551) with countdown (550) from
//   the Boost length / Cooker hood length numbers; NONE = stop party mode.
// The AHU has one boost: "Cooker hood" is just a boost we remember starting
// with the hood length, until the AHU reports the boost stopped.

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/number/number.h"
#include "esphome/components/select/select.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome::ensy_climate {

static const char *const PRESET_COOKER_HOOD = "Cooker hood";

class EnsyClimate : public climate::Climate, public Component {
 public:
  void set_current_temperature(sensor::Sensor *s) { temp_ = s; }
  void set_current_humidity(sensor::Sensor *s) { rh_ = s; }
  void set_heater_capacity(sensor::Sensor *s) { heater_ = s; }
  void set_power(switch_::Switch *s) { power_ = s; }
  void set_mode_select(select::Select *s) { mode_ = s; }
  void set_fan_step(select::Select *s) { fan_ = s; }
  void set_boost(select::Select *s) { boost_ = s; }
  void set_setpoint(number::Number *n) { setpoint_ = n; }
  void set_boost_minutes(number::Number *n) { boost_min_ = n; }
  void set_boost_length(number::Number *n) { boost_len_ = n; }
  void set_hood_length(number::Number *n) { hood_len_ = n; }
  void set_state_text(text_sensor::TextSensor *t) { state_ = t; }

  float get_setup_priority() const override { return setup_priority::LATE; }

  void setup() override {
    this->set_supported_custom_presets({PRESET_COOKER_HOOD});
    auto changed = [this](auto &&...) { this->defer("sync", [this]() { this->sync_(); }); };
    temp_->add_on_state_callback(changed);
    rh_->add_on_state_callback(changed);
    heater_->add_on_state_callback(changed);
    power_->add_on_state_callback(changed);
    mode_->add_on_state_callback(changed);
    fan_->add_on_state_callback(changed);
    boost_->add_on_state_callback(changed);
    setpoint_->add_on_state_callback(changed);
    state_->add_on_state_callback(changed);
  }

  // Also used by the Boost / Cooker hood template switches. Writes only show up
  // in the registers on the next poll, so a just-made request wins until the
  // AHU confirms it (or REQUEST_TIMEOUT passes).
  bool boost_running() const { return this->pending_() ? req_ != REQ_STOP : this->boost_reported_(); }
  bool hood_running() const { return hood_ && this->boost_running(); }
  void start_boost(bool hood) {
    this->request_(hood ? REQ_HOOD : REQ_BOOST);
    hood_ = hood;
    float minutes = (hood ? hood_len_ : boost_len_)->state;
    boost_min_->make_call().set_value(minutes).perform();   // queued before the start command
    boost_->make_call().set_option("Start").perform();
    this->sync_();
  }
  void stop_boost() {
    this->request_(REQ_STOP);
    hood_ = false;
    boost_->make_call().set_option("Stop").perform();
    this->sync_();
  }

 protected:
  climate::ClimateTraits traits() override {
    climate::ClimateTraits t;
    t.add_feature_flags(climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE | climate::CLIMATE_SUPPORTS_CURRENT_HUMIDITY |
                        climate::CLIMATE_SUPPORTS_ACTION);
    t.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_COOL,
                           climate::CLIMATE_MODE_AUTO});
    t.set_supported_fan_modes({climate::CLIMATE_FAN_OFF, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                               climate::CLIMATE_FAN_HIGH});
    t.set_supported_presets({climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_BOOST});
    return t;
  }

  void control(const climate::ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      auto m = *call.get_mode();
      if (m == climate::CLIMATE_MODE_OFF) {
        power_->turn_off();
      } else {
        const char *opt = m == climate::CLIMATE_MODE_HEAT ? "Heat" : m == climate::CLIMATE_MODE_COOL ? "Cool" : "Auto";
        if (mode_->current_option() != opt)
          mode_->make_call().set_option(opt).perform();
        if (!power_->state)
          power_->turn_on();
      }
    }
    if (call.get_target_temperature().has_value())
      setpoint_->make_call().set_value(*call.get_target_temperature()).perform();
    if (call.get_fan_mode().has_value()) {
      static const char *const steps[] = {"Off", "Step 1", "Step 2", "Step 3"};
      int i = 0;
      switch (*call.get_fan_mode()) {
        case climate::CLIMATE_FAN_LOW: i = 1; break;
        case climate::CLIMATE_FAN_MEDIUM: i = 2; break;
        case climate::CLIMATE_FAN_HIGH: i = 3; break;
        default: i = 0;
      }
      fan_->make_call().set_option(steps[i]).perform();
    }
    if (call.has_custom_preset()) {
      if (call.get_custom_preset() == PRESET_COOKER_HOOD)
        this->start_boost(true);
    } else if (call.get_preset().has_value()) {
      if (*call.get_preset() == climate::CLIMATE_PRESET_BOOST)
        this->start_boost(false);
      else if (this->boost_running())
        this->stop_boost();
    }
    this->sync_();
  }

  // Rebuild the climate state from the underlying entities and publish.
  void sync_() {
    if (temp_->has_state()) this->current_temperature = temp_->state;
    if (rh_->has_state()) this->current_humidity = rh_->state;
    if (setpoint_->has_state()) this->target_temperature = setpoint_->state;

    auto mi = mode_->active_index();
    bool on = power_->state && mi.has_value() && *mi != 0;
    this->mode = !on ? climate::CLIMATE_MODE_OFF
                 : *mi == 1 ? climate::CLIMATE_MODE_HEAT
                 : *mi == 2 ? climate::CLIMATE_MODE_COOL
                            : climate::CLIMATE_MODE_AUTO;

    if (auto fi = fan_->active_index(); fi.has_value()) {
      static const climate::ClimateFanMode fans[] = {climate::CLIMATE_FAN_OFF, climate::CLIMATE_FAN_LOW,
                                                     climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH};
      this->set_fan_mode_(fans[*fi < 4 ? *fi : 0]);
    }

    if (this->pending_() && (req_ != REQ_STOP) == this->boost_reported_())
      req_at_ = 0;  // AHU confirmed the request
    if (!this->boost_running()) hood_ = false;
    if (hood_)
      this->set_custom_preset_(PRESET_COOKER_HOOD);
    else
      this->set_preset_(this->boost_running() ? climate::CLIMATE_PRESET_BOOST : climate::CLIMATE_PRESET_NONE);

    const std::string &s = state_->get_state();
    bool heating = heater_->has_state() && heater_->state > 0;
    if (!on) this->action = climate::CLIMATE_ACTION_OFF;
    else if (s == "Heating") this->action = heating ? climate::CLIMATE_ACTION_HEATING : climate::CLIMATE_ACTION_FAN;
    else if (s == "Cooling") this->action = climate::CLIMATE_ACTION_COOLING;
    else if (s == "Ventilation") this->action = climate::CLIMATE_ACTION_FAN;
    else if (s == "Defrost" || s == "Frost secure") this->action = climate::CLIMATE_ACTION_DEFROSTING;
    else this->action = climate::CLIMATE_ACTION_IDLE;

    this->publish_state();
  }

  sensor::Sensor *temp_{nullptr}, *rh_{nullptr}, *heater_{nullptr};
  switch_::Switch *power_{nullptr};
  select::Select *mode_{nullptr}, *fan_{nullptr}, *boost_{nullptr};
  number::Number *setpoint_{nullptr}, *boost_min_{nullptr}, *boost_len_{nullptr}, *hood_len_{nullptr};
  text_sensor::TextSensor *state_{nullptr};
  bool hood_{false};

  enum Request : uint8_t { REQ_STOP, REQ_BOOST, REQ_HOOD };
  static constexpr uint32_t REQUEST_TIMEOUT = 60000;  // > 30s poll + write queue
  Request req_{REQ_STOP};
  uint32_t req_at_{0};
  void request_(Request r) { req_ = r; req_at_ = millis() | 1; }
  bool pending_() const { return req_at_ != 0 && millis() - req_at_ < REQUEST_TIMEOUT; }
  bool boost_reported_() const { return boost_->has_state() && boost_->current_option() == "Start"; }
};

}  // namespace esphome::ensy_climate
