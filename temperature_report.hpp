#pragma once

#include "downlink_protocol.h"

#include <uORB/Subscription.hpp>
#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/sensor_hygrometer.h>
#include <uORB/topics/vehicle_air_data.h>

class TemperatureReport{
public:
	TemperatureReport() = default;
	~TemperatureReport() = default;

	DOWNLINK_TEMPERATURE_REPORT get_data();

private:
	DOWNLINK_TEMPERATURE_REPORT msg;
	uORB::Subscription sensor_baro_sub{ORB_ID(sensor_baro)};
	uORB::Subscription sensor_hygrometer_sub{ORB_ID(sensor_hygrometer)};
	uORB::Subscription vehicle_air_data_sub{ORB_ID(vehicle_air_data)};
};
