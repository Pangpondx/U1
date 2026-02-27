#pragma once

#include "downlink_protocol.h"

#include <uORB/Subscription.hpp>
#include <uORB/topics/actuator_outputs.h>

class ControlSurface{
public:
	ControlSurface() = default;
	~ControlSurface() = default;

	DOWNLINK_CONTROL_SURFACE get_data();

private:
	DOWNLINK_CONTROL_SURFACE msg;
	uORB::Subscription actuator_outputs_sub{ORB_ID(actuator_outputs)};
};
