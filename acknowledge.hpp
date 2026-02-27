#pragma once

#include "downlink_protocol.h"

// #include <uORB/Subscription.hpp>
// #include <uORB/topics/message_header.h>
// #include <uORB/topics/vehicle_command_ack.h>

class Acknowledge{
public:
	Acknowledge() = default;
	~Acknowledge() = default;

	DOWNLINK_ACKNOWLEDGE get_data();

private:
	DOWNLINK_ACKNOWLEDGE msg;
	// uORB::Subscription message_header_sub{ORB_ID(message_header)};
	// uORB::Subscription vehicle_command_ack_sub{ORB_ID(vehicle_command_ack)};
};
