#pragma once

#define UART_PORT "/dev/ttyS2"
#define UART_BAUDRATE B115200

#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform_common/log.h>
#include <uORB/Publication.hpp>

#include "downlink_protocol.h"
#include "telemetry.hpp"
#include "engine_status.hpp"
#include "temperature_report.hpp"
#include "acknowledge.hpp"
#include "control_surface.hpp"


class DownlinkProtocol : public ModuleBase<DownlinkProtocol>,public px4::ScheduledWorkItem
{
public:
	DownlinkProtocol();
	~DownlinkProtocol() override;

	static int task_spawn(int argc, char *argv[]);
	static int custom_command(int argc, char *argv[]);
	static int print_usage(const char *reason = nullptr);

private:
	void Run() override;
	bool Init();
	int open_uart();
	int fd = {-1};
	uint8_t cal_header_check(const uint8_t* pkt);
	uint16_t GetCRC16(const void *buf, unsigned len);
	static const uint16_t crc16tab[256];
	DOWNLINK_PROTOCOL msg;
	Telemetry telem;
	EngineStatus engine;
	TemperatureReport temp;
	Acknowledge ack;
	ControlSurface ctrlsf;
};

