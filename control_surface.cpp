#include "control_surface.hpp"

DOWNLINK_CONTROL_SURFACE ControlSurface::get_data(){
	msg.packet_id = PACKET_ID_CONTROL;
	msg.length_in_bytes = sizeof(DOWNLINK_CONTROL_SURFACE) - 2;
	msg.flap_l = -30000;
	msg.flap_r = 30000;
	msg.aileron_l = -20000;
	msg.aileron_r = 20000;
	msg.elevator_l = -10000;
	msg.elevator_r = 10000;
	msg.rudder_l = -5000;
	msg.rudder_r = 5000;
	msg.throttle = 2000;
	msg.nose_wheel_angle = 1000;
	msg.break_ctrl = 200;
	return msg;
}
