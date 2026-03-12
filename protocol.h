#ifndef PROTOCOL_H  
#define PROTOCOL_H

#include <stdint.h>

#define MAX_PACKET_SIZE 4096
#define HI_BYTE 0x5A
#define LOW_BYTE 0xA5
#define PACKET_ID 0x02
#define TIME_INTERVAL 20
#define FIRST_BYTE 0
#define SECOND_BYTE 1
#define NUM_OF_HEADER 16
#define SIZE_OF_HEADER 19
#define HEADER 17
#define CRC_SIZE 2
#define HEADER_CHECKSUM_SIZE 16

typedef struct __attribute__((packed)){
    uint8_t packet_id;
    uint8_t length_in_bytes;
    int16_t left_flap_cmd;
    int16_t right_flap_cmd;
    int16_t left_aileron_cmd;
    int16_t right_aileron_cmd;
    int16_t left_elevator_cmd;
    int16_t right_elevator_cmd;
    int16_t left_rudder_cmd;
    int16_t right_rudder_cmd;
    int16_t throttle_cmd;
    int16_t nose_wheel_cmd;
    uint8_t brake_cmd;
} UPLINK_DISCO;

typedef struct __attribute__((packed)){
    uint8_t protocol_iden_hi;          
    uint8_t protocol_iden_low;          
    uint8_t protocol_version;        
    uint8_t source_id : 7; 
    uint8_t take_control : 1;
    uint8_t destination_id;         
    uint8_t sequence;       
    uint16_t length;     
    uint32_t unix_time_stamp;      
    uint32_t utc_microsecond_time_stamp;
    uint8_t header_check;
    UPLINK_DISCO data;
    uint16_t checksum;       
} MESSAGE_HEADER_UPLINK;

typedef struct __attribute__((packed)){
	uint8_t packet_id;
	uint8_t length_in_bytes;
	uint32_t tailed_number;
	uint8_t flight_mode;
	float air_speed;
	float angle_of_attack;
	float angle_of_sideslip;
	float roll_angle;
	float pitch_angle;
	float yaw_angle;
	float roll_rate;
	float pitch_rate;
	float yaw_rate;
	float wind_speed;
	float wind_direction;
	// uint32_t air_speed;
	// uint32_t angle_of_attack;
	// uint32_t angle_of_sideslip;
	// uint32_t roll_angle;
	// uint32_t pitch_angle;
	// uint32_t yaw_angle;
	// uint32_t roll_rate;
	// uint32_t pitch_rate;
	// uint32_t yaw_rate;
	// uint32_t wind_speed;
	// uint32_t wind_direction;
	uint32_t spare_1 : 29;
	uint32_t disengage_ias : 1;
	uint32_t disengage_altitute : 1;
	uint32_t disengage_heading : 1;
	float accelerometer_x;
	float accelerometer_y;
	float accelerometer_z;
	float velocity_north;
	float velocity_east;
	float velocity_down;
	float latitude;
	float longitude;
	double gps_height;
	double baro_altitude;
	// uint32_t accelerometer_x;
	// uint32_t accelerometer_y;
	// uint32_t accelerometer_z;
	// uint32_t velocity_north;
	// uint32_t velocity_east;
	// uint32_t velocity_down;
	// uint32_t latitude;
	// uint32_t longitude;
	// uint64_t gps_height;
	// uint64_t baro_altitude;
	uint8_t uav_controled_by_id : 7;
	uint8_t spare_2 : 1;
} DOWNLINK_TELEMETRY;
 
typedef struct __attribute__((packed)){
	uint8_t packet_id;
	uint8_t length_in_bytes;
	uint16_t engine_rpm;
	uint16_t injection_1_time;
	uint16_t injection_2_time;
	uint16_t throttle_position;
	uint16_t barometric;
	uint16_t battery_voltage;
	uint16_t cht_l;
	uint16_t cht_r;
	uint16_t egt_l;
	uint16_t egt_r;
	uint16_t fuel_pressure;
	uint16_t air_temp;
	uint16_t duty_pump;
	uint8_t smot;
	uint8_t ignition_1;
	uint8_t ignition_2;
	uint8_t ignition_3;
	uint8_t ignition_4;
} DOWNLINK_ENGINE_STATUS;
 
typedef struct __attribute__((packed)){
	uint8_t packet_id;
	uint8_t length_in_bytes;
	int8_t outside_air_temp;
	int8_t fcc_temperature;
	uint8_t fcc_humidity;
} DOWNLINK_TEMPERATURE_REPORT;
 
typedef struct __attribute__((packed)){
	uint8_t packet_id;
	uint8_t length_in_bytes;
	uint8_t acknowledge_packet_id;
	uint8_t sequence_number;
	uint8_t response_code;
} DOWNLINK_ACKNOWLEDGE;
 
typedef struct __attribute__((packed)){
	uint8_t packet_id;
	uint8_t length_in_bytes;
	int16_t flap_l;
	int16_t flap_r;
	int16_t aileron_l;
	int16_t aileron_r;
	int16_t elevator_l;
	int16_t elevator_r;
	int16_t rudder_l;
	int16_t rudder_r;
	int16_t throttle;
	int16_t nose_wheel_angle;
	uint8_t break_ctrl;
} DOWNLINK_CONTROL_SURFACE;
 
typedef struct __attribute__((packed)){
	uint8_t protocol_iden_hi;
	uint8_t protocol_iden_low;
	uint8_t protocol_version;
	uint8_t source_id : 7;
	uint8_t take_control : 1;
	uint8_t destination_id;
	uint8_t sequence;
	uint16_t length;
	uint32_t unix_time_stamp;
	uint32_t utc_microsecond_time_stamp;
	uint8_t header_check;
	//user_data//
	DOWNLINK_TELEMETRY telemetry_data;
	DOWNLINK_ENGINE_STATUS engine_data;
	DOWNLINK_TEMPERATURE_REPORT temperature_data;
	DOWNLINK_ACKNOWLEDGE acknowledge_data;
	DOWNLINK_CONTROL_SURFACE control_data;
	uint16_t checksum;
} MESSAGE_HEADER_DOWNLINK;

typedef enum {
    WAIT_IDEN,
    READ_HEADER,
    HEADER_CHECKSUM,
    CHECK_LENGTH,
    READ_PAYLOAD,
    CHECK_CRC16,
    COMMIT_DATA
} PARSER_STATE;

PARSER_STATE current_state = WAIT_IDEN;
int payload_len = 0;
uint16_t buffer_index = 0;
uint8_t buffer[MAX_PACKET_SIZE];

#endif 
