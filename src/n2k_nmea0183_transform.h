#ifndef SH_WG_FIRMWARE_N2K_NMEA0183_TRANSFORM_H_
#define SH_WG_FIRMWARE_N2K_NMEA0183_TRANSFORM_H_

#include <NMEA0183.h>
#include <NMEA2000.h>

#include "ReactESP.h"

#include "elapsedMillis.h"
#include "sensesp/transforms/transform.h"

using namespace sensesp;

class N2KTo0183Transform : public Transform<tN2kMsg, String> {
 public:
  N2KTo0183Transform(String config_path = "") : Transform(config_path) {
    // invalidate old data
    ReactESP::app->onRepeat(10, [this]() {
      this->invalidate_old_data();
    });
    // send RMC periodically
    ReactESP::app->onRepeat(kRMCPeriod_, [this]() {
      this->send_rmc();
    });
  }
  virtual void set_input(tN2kMsg new_value, uint8_t input_channel = 0) override;

 protected:
  static const unsigned long kRMCPeriod_ = 1000;  // ms
  static const unsigned int kMaxNMEA0183MessageSize_ = 164;

  // containers for last known values
  double latitude_;
  double longitude_;
  double altitude_;
  double variation_;
  double heading_;
  double cog_;
  double sog_;
  double wind_speed_;
  double wind_angle_;

  uint16_t days_since_1970_;
  double seconds_since_midnight_;

  // time of last known values
  elapsedMillis last_heading_elapsed_;
  elapsedMillis last_cogsog_elapsed_;
  elapsedMillis last_position_elapsed_;
  elapsedMillis last_wind_elapsed_;

  tNMEA0183* nmea0183_;

  // N2K message handlers

  void handle_heading(const tN2kMsg& msg);  // 127250
  void handle_variation(const tN2kMsg& msg);  // 127258
  void handle_boat_speed(const tN2kMsg& msg);  // 128259
  void handle_depth(const tN2kMsg& msg);  // 128267
  void handle_position(const tN2kMsg& msg);  // 129025
  void handle_cogsog(const tN2kMsg& msg);  // 129026
  void handle_gnss(const tN2kMsg& msg);  // 129029
  void handle_wind(const tN2kMsg& msg);  // 130306

  void invalidate_old_data();
  void send_rmc();

  void emit_0183_string(const tNMEA0183Msg& msg);
};

#endif  // SH_WG_FIRMWARE_N2K_NMEA0183_TRANSFORM_H_