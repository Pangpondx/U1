#pragma once

#include "downlink_protocol.h"

#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/rpm.h>
#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/vehicle_air_data.h>

class EngineStatus{
public:
	EngineStatus() = default;
	~EngineStatus() = default;

	DOWNLINK_ENGINE_STATUS get_data();

private:
	DOWNLINK_ENGINE_STATUS msg;
	uORB::Subscription battery_status_sub{ORB_ID(battery_status)};
	uORB::Subscription rpm_sub{ORB_ID(rpm)};
	uORB::Subscription sensor_baro_sub{ORB_ID(sensor_baro)};
	uORB::Subscription vehicle_air_data_s{ORB_ID(vehicle_air_data)};
};
