#include "temperature_report.hpp"

DOWNLINK_TEMPERATURE_REPORT TemperatureReport::get_data(){
	msg.packet_id = PACKET_ID_TEMPERATURE;
	msg.length_in_bytes = sizeof(DOWNLINK_TEMPERATURE_REPORT) - 2;
	msg.outside_air_temp = 127;
	msg.fcc_temperature = 100;
	msg.fcc_humidity = 50;
	return msg;
}
