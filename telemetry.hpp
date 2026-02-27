#pragma once

#include "downlink_protocol.h"

#include <uORB/Subscription.hpp>
#include <uORB/topics/airspeed.h>
#include <uORB/topics/message_header.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/vehicle_air_data.h>
#include <uORB/topics/vehicle_angular_velocity.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/wind.h>
#include <matrix/matrix/math.hpp>

class Telemetry{
public:
	Telemetry() = default;
	~Telemetry() = default;

	DOWNLINK_TELEMETRY get_data();

private:
	DOWNLINK_TELEMETRY msg;
	uORB::Subscription airspeed_sub{ORB_ID(airspeed)};
	uORB::Subscription message_header_sub{ORB_ID(message_header)};
	uORB::Subscription sensor_sub{ORB_ID(sensor_combined)};
	uORB::Subscription air_data_sub{ORB_ID(vehicle_air_data)};
	uORB::Subscription angular_velocity_sub{ORB_ID(vehicle_angular_velocity)};
	uORB::Subscription attitude_sub{ORB_ID(vehicle_attitude)};
	uORB::Subscription global_pos_sub{ORB_ID(vehicle_global_position)};
	uORB::Subscription local_pos_sub{ORB_ID(vehicle_local_position)};
	uORB::Subscription status_sub{ORB_ID(vehicle_status)};
	uORB::Subscription wind_sub{ORB_ID(wind)};
};
