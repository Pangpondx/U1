#include "engine_status.hpp"

DOWNLINK_ENGINE_STATUS EngineStatus::get_data(){
	msg.packet_id = PACKET_ID_ENGINE;
	msg.length_in_bytes = sizeof(DOWNLINK_ENGINE_STATUS) - 2;
	msg.engine_rpm = 1000;
	msg.injection_1_time = 10000;
	msg.injection_2_time = 20000;
	msg.throttle_position = 1;
	msg.barometric = 2;
	msg.battery_voltage = 12;
	msg.cht_l = 10;
	msg.cht_r = 20;
	msg.egt_l = 30;
	msg.egt_r = 40;
	msg.fuel_pressure = 50;
	msg.air_temp = 60;
	msg.duty_pump = 70;
	msg.smot = 0;
	msg.ignition_1 = 1;
	msg.ignition_2 = 2;
	msg.ignition_3 = 3;
	msg.ignition_4 = 4;
	return msg;
}
