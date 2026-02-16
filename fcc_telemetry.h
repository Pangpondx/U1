#pragma once

#include <px4_platform_common/module.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

#define UART_PORT "/dev/ttyS1"
#define UART_BAUDRATE B115200

#define HI_BYTE 0x5A
#define LOW_BYTE 0xA5
#define SRC_ID 0x38
#define DES_ID 0xFF
#define PACKET_ID 0x80
#define UAV_CTRL 0x7E

typedef struct __attribute__((packed)){
	uint8_t packet_id;
	uint8_t length_in_bytes;
	uint32_t tailed_number;
	uint8_t flight_mode;
	uint32_t air_speed;
	uint32_t angle_of_attack;
	uint32_t angle_of_sideslip;
	uint32_t roll_angle;
	uint32_t pitch_angle;
	uint32_t yaw_angle;
	uint32_t roll_rate;
	uint32_t pitch_rate;
	uint32_t yaw_rate;
	uint32_t wind_speed;
	uint32_t wind_direction;
	uint32_t spare_1 : 29;
	uint32_t disengage_ias : 1;
	uint32_t disengage_altitute : 1;
	uint32_t disengage_heading : 1;
	uint32_t accelerometer_x;
	uint32_t accelerometer_y;
	uint32_t accelerometer_z;
	uint32_t velocity_north;
	uint32_t velocity_east;
	uint32_t velocity_down;
	uint32_t latitude;
	uint32_t longitude;
	uint64_t gps_height;
	uint64_t baro_altitude;
	uint8_t uav_controled_by_id : 7;
	uint8_t spare_2 : 1;
} DOWNLINK_TELEMETRY;

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
	DOWNLINK_TELEMETRY user_data;
	uint16_t checksum;
} MESSAGE_HEADER;

class TelemLink : public ModuleBase<TelemLink>, public px4::ScheduledWorkItem
{
public:
	TelemLink();
	~TelemLink() override;
	static TelemLink *instantiate(int argc,char *argv[]);
	static int task_spawn(int argc,char *argv[]);
	static int custom_command(int argc,char *argv[]);
	static int print_usage(const char *reason = nullptr);
private:
	bool Init();
	void Run() override;
	int open_uart();
	uint8_t cal_header_check(const uint8_t* pkt);
	uint16_t GetCRC16(const void *buf, unsigned len);
	static const uint16_t crc16tab[256];
	int fd{-1};
	bool should_exit{false};
};
