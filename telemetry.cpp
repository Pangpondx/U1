#include "telemetry.hpp"

DOWNLINK_TELEMETRY Telemetry::get_data(){
	msg.packet_id = PACKET_ID_TELEMETRY;
	msg.length_in_bytes = sizeof(DOWNLINK_TELEMETRY) - 2;
	static airspeed_s airspeed;
	airspeed_sub.update(&airspeed);
	msg.air_speed = airspeed.true_airspeed_m_s;
	static message_header_s header;
	message_header_sub.update(&header);
	msg.uav_controled_by_id = header.source_id;
	static sensor_combined_s acc;
	sensor_sub.update(&acc);
	msg.accelerometer_x = acc.accelerometer_m_s2[0];
	msg.accelerometer_y = acc.accelerometer_m_s2[1];
	msg.accelerometer_z = acc.accelerometer_m_s2[2];
	static vehicle_air_data_s air_data;
	air_data_sub.update(&air_data);
	msg.baro_altitude = air_data.baro_alt_meter;
	static vehicle_angular_velocity_s angular_data;
	angular_velocity_sub.update(&angular_data);
	msg.roll_rate = angular_data.xyz[0];
	msg.pitch_rate = angular_data.xyz[1];
	msg.yaw_rate = angular_data.xyz[2];
	static vehicle_attitude_s att_data;
	attitude_sub.update(&att_data);
	matrix::Eulerf euler(matrix::Quatf(att_data.q));
	msg.roll_angle = euler.phi();
	msg.pitch_angle = euler.theta();
	msg.yaw_angle = euler.psi();
	static vehicle_global_position_s gb_data;
	global_pos_sub.update(&gb_data);
	msg.latitude = math::radians(gb_data.lat);
	msg.longitude = math::radians(gb_data.lon);
	msg.gps_height = gb_data.alt;
	static vehicle_local_position_s lc_data;
	local_pos_sub.update(&lc_data);
	msg.velocity_north = lc_data.vx;
	msg.velocity_east = lc_data.vy;
	msg.velocity_down = lc_data.vz;
	static vehicle_status_s status_data;
	status_sub.update(&status_data);
	msg.tailed_number = status_data.system_id;
	msg.flight_mode = status_data.nav_state;
	//printf("nav_state = %u\n", (unsigned)status_data.nav_state);
	static wind_s wind;
	wind_sub.update(&wind);
	msg.wind_speed = sqrtf(wind.windspeed_north*wind.windspeed_north + wind.windspeed_east*wind.windspeed_east);
	msg.wind_direction = atan2f(wind.windspeed_east, wind.windspeed_north);
	matrix::Vector3f v_ground(lc_data.vx, lc_data.vy, lc_data.vz);
	matrix::Vector3f v_wind(wind.windspeed_north, wind.windspeed_east, 0.0f);
	matrix::Vector3f v_air_ned = v_ground - v_wind;
	matrix::Quatf q(att_data.q);
	matrix::Dcmf dcm(q);
	matrix::Vector3f v_air_body = dcm.transpose() * v_air_ned;
	float u = v_air_body(0); //x forword
	float v = v_air_body(1); //y side
	float w = v_air_body(2); //z down
	float v_air_sq = u*u + v*v + w*w;
	if(v_air_sq > 1.0f && u > 0.1f){
		float v_air = sqrtf(v_air_sq);
		msg.angle_of_attack = atan2f(w, u);
		msg.angle_of_sideslip = asinf(v / v_air);
	}else{
		msg.angle_of_attack = 0.0f;
		msg.angle_of_sideslip = 0.0f;
	}
	msg.disengage_heading= 0;
	msg.disengage_altitute= 1;
	msg.disengage_ias= 0;
	msg.spare_1 = 0;
	msg.spare_2 = 0;
	return msg;
}
