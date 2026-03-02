#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/timeb.h> 
#include <sys/time.h>
#include "protocol.h"

uint64_t last_time = 0;

const char *COM_PORT_NAME = "\\\\.\\COM5";
const int BAUD_RATE = 115200;

const uint16_t crc16tab[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

uint16_t GetCRC16(const void *buf, unsigned len)
{
    uint16_t crc;
    const uint8_t *bf;
    bf = (const uint8_t *) buf;
    crc = 0xffff;
    do {
        crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *bf++) & 0xFF];
    } while (--len);
    return ~crc;
}

uint8_t cal_header_checku(const MESSAGE_HEADER_UPLINK* pkt) {
    const uint8_t* ptr = (const uint8_t*)pkt;
    uint16_t sum = 0;
    for(int i = 0; i < 16; i++) {
        sum += ptr[i];
    }
    return (uint8_t)sum;
}

uint8_t cal_header_checkd(const MESSAGE_HEADER_DOWNLINK* pkt) {
    const uint8_t* ptr = (const uint8_t*)pkt;
    uint16_t sum = 0;
    for(int i = 0; i < 16; i++) {
        sum += ptr[i];
    }
    return (uint8_t)sum;
}
int main() {
    int count = 0;
    HANDLE hSerial = CreateFile(COM_PORT_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error: Cannot open %s\n", COM_PORT_NAME);
        return 1;
    }

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(hSerial, &dcb);
    dcb.BaudRate = BAUD_RATE;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    SetCommState(hSerial, &dcb);

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    printf("--- FULL PROTOCOL MONITOR ---\n\n");

    MESSAGE_HEADER_UPLINK header;
    uint8_t rx_buffer[MAX_PACKET_SIZE];
    uint8_t data[MAX_PACKET_SIZE];

    uint64_t last_time_us = 0;
    uint64_t last_time = 0;
    uint64_t last_times = 0;
    uint64_t byte_count = 0;
    uint64_t start_time_us = 0;

    uint8_t buffer[MAX_PACKET_SIZE];
    DWORD bytes_written, bytes_read;

    while (1) {
        struct timeval tv;

        // รับเวลาปัจจุบันในหน่วยวินาทีและไมโครวินาที
        mingw_gettimeofday(&tv, NULL);
        uint64_t current_time = (tv.tv_sec * 1000) + tv.tv_usec / 1000;
        
        if(current_time - last_time >= 20){
            //printf("%u %u %u %u\t\n", tv.tv_sec * 1000, tv.tv_usec / 1000, last_time, current_time);
            //printf("Time sent %u |\n", tv.tv_usec / 1000);
            last_time = current_time;
            
            struct _timeb timebuffer;
            _ftime(&timebuffer);
            
            header.unix_time_stamp = (uint32_t)timebuffer.time;
            header.utc_microsecond_time_stamp = (uint32_t)timebuffer.millitm * 1000;
            
            header.protocol_iden_hi = HI_BYTE;
            header.protocol_iden_low = LOW_BYTE;
            header.protocol_version = 1;
            header.take_control = 1;
            header.source_id = 56;  
            header.destination_id = 255;
            header.sequence = count++;
            header.length = sizeof(UPLINK_DISCO);

            header.data.packet_id = PACKET_ID;
            header.data.length_in_bytes = sizeof(UPLINK_DISCO) - 2;
            header.data.left_flap_cmd = -32000;
            header.data.right_flap_cmd = 32000;
            header.data.left_aileron_cmd = 100;
            header.data.right_aileron_cmd = 5000;
            header.data.left_elevator_cmd = 10000;
            header.data.right_elevator_cmd = -10000;
            header.data.left_rudder_cmd = -3000;
            header.data.right_rudder_cmd = 3000;
            header.data.throttle_cmd = 30000;
            header.data.nose_wheel_cmd = -100;
            header.data.brake_cmd = 10;

            header.header_check = cal_header_checku(&header);
            size_t crc_scope = sizeof(MESSAGE_HEADER_UPLINK) - 2;
            header.checksum = GetCRC16(&header, crc_scope);

            WriteFile(hSerial, &header, sizeof(MESSAGE_HEADER_UPLINK), &bytes_written, NULL);
        }
        
        struct timeval tvs;

        // รับเวลาปัจจุบันในหน่วยวินาทีและไมโครวินาที
        mingw_gettimeofday(&tvs, NULL);
        uint64_t current_times = (tvs.tv_sec * 1000) + tvs.tv_usec / 1000;
        
        if(current_times - last_times >= 40){
            //printf("%u %u %u %u\t\n", tv.tv_sec * 1000, tv.tv_usec / 1000, last_time, current_time);
            printf(" Time read %u\n", tvs.tv_usec / 1000);
            last_times = current_times;
            
            if (ReadFile(hSerial, rx_buffer, sizeof(MESSAGE_HEADER_DOWNLINK), &bytes_read, NULL) && bytes_read > 0) {
                for(DWORD i = 0; i < bytes_read; i++){
                    uint8_t byte = rx_buffer[i];
                    switch(state){
                        case WAIT_IDEN:
                            if(buffer_index == 0){
                                if(byte == 0x5A) buffer[buffer_index++] = byte;
                            }else if(buffer_index == 1){
                                if(byte == 0xA5){
                                    buffer[buffer_index++] = byte;
                                    state = READ_HEADER;
                                }else{
                                    printf("Error header\n");
                                    buffer_index = 0;
                                }
                            }
                            break;
                            
                        case READ_HEADER:
                            buffer[buffer_index++] = byte;
                            if(buffer_index == 16){
                                state = HEADER_CHECKSUM;
                            }
                            break;
                            
                        case HEADER_CHECKSUM:
                            if(cal_header_checkd((const MESSAGE_HEADER_DOWNLINK*)buffer) == byte){
                                buffer[buffer_index++] = byte;
                                state = CHECK_LENGTH;
                            }else{
                                state = WAIT_IDEN;
                                printf("Error header checksum\n");
                                buffer_index = 0;
                            }
                            break;
                            
                        case CHECK_LENGTH:
                            {
                                MESSAGE_HEADER_DOWNLINK* header = (MESSAGE_HEADER_DOWNLINK*)buffer;
                                payload_len = header->length;
                                if(payload_len > (MAX_PACKET_SIZE - 19)){
                                    state = WAIT_IDEN;
                                    printf("Error size\n");
                                    buffer_index = 0;
                                }else{
                                    i--;
                                    printf("Length %u : " ,payload_len);
                                    state = READ_PAYLOAD;
                                }
                            }
                            break;
                            
                        case READ_PAYLOAD:
                            buffer[buffer_index++] = byte;
                            if(buffer_index >= (17 + payload_len + 2)){
                                state = CHECK_CRC16;
                            }
                            break;
                            
                        case CHECK_CRC16:
                            {
                                uint16_t recv_crc = (buffer[buffer_index - 1] << 8) | buffer[buffer_index - 2];
                                uint16_t cal_crc = GetCRC16(buffer, buffer_index - 2);
                                if(cal_crc == recv_crc){
                                    state = COMMIT_DATA;
                                }else{
                                    state = WAIT_IDEN;
                                    printf("Error crc\n");
                                    buffer_index = 0;
                                }
                            }
                            break;
                            
                        case COMMIT_DATA:
                            {
                                MESSAGE_HEADER_DOWNLINK last_data;
                                memcpy(&last_data, buffer, sizeof(MESSAGE_HEADER_DOWNLINK));
                                    
                                // ดึงเวลาปัจจุบัน
                                // struct timeval tv;
                                // gettimeofday(&tv, NULL);
                                
                                // // แสดงผลข้อมูลจาก last_data
                                // printf(" Time : %u ms\n", (unsigned int)(tv.tv_usec / 1000));
                                printf(" Protocol iden hi      : %u\n", last_data.protocol_iden_hi);
                                printf(" Protocol iden low     : %u\n", last_data.protocol_iden_low);
                                printf(" Protocol version      : %u\n", last_data.protocol_version);
                                printf(" Take control          : %u\n", last_data.take_control);
                                printf(" Source ID             : %u\n", last_data.source_id);
                                printf(" Destination ID        : %u\n", last_data.destination_id);
                                printf(" Sequence              : %u\n", last_data.sequence);
                                printf(" Length                : %u\n", last_data.length);
                                printf(" Unix time stamp       : %u\n", last_data.unix_time_stamp);
                                printf(" Utc micro time stamp  : %u\n", last_data.utc_microsecond_time_stamp);
                                printf(" Header checksum       : %u\n", last_data.header_check);
                                printf(" Checksum              : %u\n", last_data.checksum);
                                printf(" FCC-TELEMETRY\n");
                                printf(" Packet ID             : %u\n", last_data.telemetry_data.packet_id);
                                printf(" Length in bytes       : %u\n", last_data.telemetry_data.length_in_bytes);
                                printf(" Tailed number         : %u\n", last_data.telemetry_data.tailed_number);
                                printf(" Flight mode           : %u\n", last_data.telemetry_data.flight_mode);
                                printf(" Air speed             : %f\n", last_data.telemetry_data.air_speed);
                                printf(" Angle of attack       : %f\n", last_data.telemetry_data.angle_of_attack);
                                printf(" Angle of sideslip     : %f\n", last_data.telemetry_data.angle_of_sideslip);
                                printf(" Roll angle            : %f\n", last_data.telemetry_data.roll_angle);
                                printf(" Pitch angle           : %f\n", last_data.telemetry_data.pitch_angle);
                                printf(" Yaw angle             : %f\n", last_data.telemetry_data.yaw_angle);
                                printf(" Roll rate             : %f\n", last_data.telemetry_data.roll_rate);
                                printf(" Pitch rate            : %f\n", last_data.telemetry_data.pitch_rate);
                                printf(" Yaw rate              : %f\n", last_data.telemetry_data.yaw_rate);
                                printf(" Wind speed            : %f\n", last_data.telemetry_data.wind_speed);
                                printf(" Wind direction        : %f\n", last_data.telemetry_data.wind_direction);
                                printf(" Disengage heading     : %u\n", last_data.telemetry_data.disengage_heading);
                                printf(" Disengage altitute    : %u\n", last_data.telemetry_data.disengage_altitute);
                                printf(" Disengage ias         : %u\n", last_data.telemetry_data.disengage_ias);
                                printf(" Spare                 : %u\n", last_data.telemetry_data.spare_1);
                                printf(" Accelerometer x       : %f\n", last_data.telemetry_data.accelerometer_x);
                                printf(" Accelerometer y       : %f\n", last_data.telemetry_data.accelerometer_y);
                                printf(" Accelerometer z       : %f\n", last_data.telemetry_data.accelerometer_z);
                                printf(" Velocity north        : %f\n", last_data.telemetry_data.velocity_north);
                                printf(" Velocity east         : %f\n", last_data.telemetry_data.velocity_east);
                                printf(" Velocity down         : %f\n", last_data.telemetry_data.velocity_down);
                                printf(" Latitude              : %f\n", last_data.telemetry_data.latitude);
                                printf(" Longitude             : %f\n", last_data.telemetry_data.longitude);
                                printf(" Gps height            : %lf\n", (double)last_data.telemetry_data.gps_height);
                                printf(" Baro altitude         : %lf\n", (double)last_data.telemetry_data.baro_altitude);
                                printf(" Spare                 : %u\n", last_data.telemetry_data.spare_2);
                                printf(" UAV control by ID     : %u\n", last_data.telemetry_data.uav_controled_by_id);
                                // printf("-----------------------------------------\n");
                                printf(" FCC-ENGINE-STATUS\n");
                                printf(" Packet ID             : %u\n", last_data.engine_data.packet_id);
                                printf(" Length in bytes       : %u\n", last_data.engine_data.length_in_bytes);
                                printf(" Engine RPM            : %u\n", last_data.engine_data.engine_rpm);
                                printf(" Injection 1 time      : %u\n", last_data.engine_data.injection_1_time);
                                printf(" Injection 2 time      : %u\n", last_data.engine_data.injection_2_time);
                                printf(" Throttle position     : %u\n", last_data.engine_data.throttle_position);
                                printf(" Barometric            : %u\n", last_data.engine_data.barometric);
                                printf(" Battery voltage       : %u\n", last_data.engine_data.battery_voltage);
                                printf(" Cht l                 : %u\n", last_data.engine_data.cht_l);
                                printf(" Cht r                 : %u\n", last_data.engine_data.cht_r);
                                printf(" Egt l                 : %u\n", last_data.engine_data.egt_l);
                                printf(" Egt r                 : %u\n", last_data.engine_data.egt_r);
                                printf(" Fuel pressure         : %u\n", last_data.engine_data.fuel_pressure);
                                printf(" Air temp              : %u\n", last_data.engine_data.air_temp);
                                printf(" Duty pump             : %u\n", last_data.engine_data.duty_pump);
                                printf(" Smot                  : %u\n", last_data.engine_data.smot);
                                printf(" Ignition 1            : %u\n", last_data.engine_data.ignition_1);
                                printf(" Ignition 2            : %u\n", last_data.engine_data.ignition_2);
                                printf(" Ignition 3            : %u\n", last_data.engine_data.ignition_3);
                                printf(" Ignition 4            : %u\n", last_data.engine_data.ignition_4);
                                printf(" FCC-TEMPERATURE-REPORT\n");
                                printf(" Packet ID             : %u\n", last_data.temperature_data.packet_id);
                                printf(" Length in bytes       : %u\n", last_data.temperature_data.length_in_bytes);
                                printf(" Outside air temp      : %d\n", last_data.temperature_data.outside_air_temp);
                                printf(" Fcc temperature       : %d\n", last_data.temperature_data.fcc_temperature);
                                printf(" Fcc humidity          : %u\n", last_data.temperature_data.fcc_humidity);
                                printf(" ACKNOWLEDGE-PACKET\n");
                                printf(" Packet ID             : %u\n", last_data.acknowledge_data.packet_id);
                                printf(" Length in bytes       : %u\n", last_data.acknowledge_data.length_in_bytes);
                                printf(" Acknowledge packet ID : %u\n", last_data.acknowledge_data.acknowledge_packet_id);
                                printf(" Sequence number       : %u\n", last_data.acknowledge_data.sequence_number);
                                printf(" Response code         : %u\n", last_data.acknowledge_data.response_code);
                                printf(" FCC-CONTROL-SURFACE\n");
                                printf(" Packet ID             : %u\n", last_data.control_data.packet_id);
                                printf(" Length in bytes       : %u\n", last_data.control_data.length_in_bytes);
                                printf(" Flap l                : %d\n", last_data.control_data.flap_l);
                                printf(" Flap r                : %d\n", last_data.control_data.flap_r);
                                printf(" Aileron l             : %d\n", last_data.control_data.aileron_l);
                                printf(" Aileron r             : %d\n", last_data.control_data.aileron_r);
                                printf(" Elevator l            : %d\n", last_data.control_data.elevator_l);
                                printf(" Elevator r            : %d\n", last_data.control_data.elevator_r);
                                printf(" Rudder l              : %d\n", last_data.control_data.rudder_l);
                                printf(" Rudder r              : %d\n", last_data.control_data.rudder_r);
                                printf(" Throttle              : %d\n", last_data.control_data.throttle);
                                printf(" Nose wheel angle      : %d\n", last_data.control_data.nose_wheel_angle);
                                printf(" Break                 : %u\n", last_data.control_data.break_ctrl);
                                printf("-----------------------------------------\n");
                                // printf(" Header protocol\n");
                                // printf(" Protocol iden hi      : %u\n", last_data.protocol_iden_hi);
                                // printf(" Protocol iden low     : %u\n", last_data.protocol_iden_low);
                                // printf(" Protocol version      : %u\n", last_data.protocol_version);
                                // printf(" Take control          : %u\n", last_data.take_control);
                                // printf(" Source ID             : %u\n", last_data.source_id);
                                // printf(" Destination ID        : %u\n", last_data.destination_id);
                                // printf(" Sequence              : %u\n", last_data.sequence);
                                // printf(" Length                : %u\n", last_data.length);
                                // printf(" Unix time stamp       : %u\n", last_data.unix_time_stamp);
                                // printf(" Utc micro time stamp  : %u\n", last_data.utc_microsecond_time_stamp);
                                // printf(" Header checksum       : %u\n", last_data.header_check);
                                // printf(" Checksum              : %u\n", last_data.checksum);
                                // printf(" FCC-TELEMETRY\n");
                                // printf(" Packet ID             : %u\n", last_data.telemetry_data.packet_id);
                                // printf(" Length in bytes       : %u\n", last_data.telemetry_data.length_in_bytes);
                                // printf(" Tailed number         : %u\n", last_data.telemetry_data.tailed_number);
                                // printf(" Flight mode           : %u\n", last_data.telemetry_data.flight_mode);
                                // printf(" Air speed             : %u\n", last_data.telemetry_data.air_speed);
                                // printf(" Angle of attack       : %u\n", last_data.telemetry_data.angle_of_attack);
                                // printf(" Angle of sideslip     : %u\n", last_data.telemetry_data.angle_of_sideslip);
                                // printf(" Roll angle            : %u\n", last_data.telemetry_data.roll_angle);
                                // printf(" Pitch angle           : %u\n", last_data.telemetry_data.pitch_angle);
                                // printf(" Yaw angle             : %u\n", last_data.telemetry_data.yaw_angle);
                                // printf(" Roll rate             : %u\n", last_data.telemetry_data.roll_rate);
                                // printf(" Pitch rate            : %u\n", last_data.telemetry_data.pitch_rate);
                                // printf(" Yaw rate              : %u\n", last_data.telemetry_data.yaw_rate);
                                // printf(" Wind speed            : %u\n", last_data.telemetry_data.wind_speed);
                                // printf(" Wind direction        : %u\n", last_data.telemetry_data.wind_direction);
                                // printf(" Disengage heading     : %u\n", last_data.telemetry_data.disengage_heading);
                                // printf(" Disengage altitute    : %u\n", last_data.telemetry_data.disengage_altitute);
                                // printf(" Disengage ias         : %u\n", last_data.telemetry_data.disengage_ias);
                                // printf(" Spare                 : %u\n", last_data.telemetry_data.spare_1);
                                // printf(" Accelerometer x       : %u\n", last_data.telemetry_data.accelerometer_x);
                                // printf(" Accelerometer y       : %u\n", last_data.telemetry_data.accelerometer_y);
                                // printf(" Accelerometer z       : %u\n", last_data.telemetry_data.accelerometer_z);
                                // printf(" Velocity north        : %u\n", last_data.telemetry_data.velocity_north);
                                // printf(" Velocity east         : %u\n", last_data.telemetry_data.velocity_east);
                                // printf(" Velocity down         : %u\n", last_data.telemetry_data.velocity_down);
                                // printf(" Latitude              : %u\n", last_data.telemetry_data.latitude);
                                // printf(" Longitude             : %u\n", last_data.telemetry_data.longitude);
                                // printf(" Gps height            : %llu\n", last_data.telemetry_data.gps_height);
                                // printf(" Baro altitude         : %llu\n", last_data.telemetry_data.baro_altitude);
                                // printf(" Spare                 : %u\n", last_data.telemetry_data.spare_2);
                                // printf(" UAV control by ID     : %u\n", last_data.telemetry_data.uav_controled_by_id);
                                // // printf("-----------------------------------------\n");
                                // printf(" FCC-ENGINE-STATUS\n");
                                // printf(" Packet ID             : %u\n", last_data.engine_data.packet_id);
                                // printf(" Length in bytes       : %u\n", last_data.engine_data.length_in_bytes);
                                // printf(" Engine RPM            : %u\n", last_data.engine_data.engine_rpm);
                                // printf(" Injection 1 time      : %u\n", last_data.engine_data.injection_1_time);
                                // printf(" Injection 2 time      : %u\n", last_data.engine_data.injection_2_time);
                                // printf(" Throttle position     : %u\n", last_data.engine_data.throttle_position);
                                // printf(" Barometric            : %u\n", last_data.engine_data.barometric);
                                // printf(" Battery voltage       : %u\n", last_data.engine_data.battery_voltage);
                                // printf(" Cht l                 : %u\n", last_data.engine_data.cht_l);
                                // printf(" Cht r                 : %u\n", last_data.engine_data.cht_r);
                                // printf(" Egt l                 : %u\n", last_data.engine_data.egt_l);
                                // printf(" Egt r                 : %u\n", last_data.engine_data.egt_r);
                                // printf(" Fuel pressure         : %u\n", last_data.engine_data.fuel_pressure);
                                // printf(" Air temp              : %u\n", last_data.engine_data.air_temp);
                                // printf(" Duty pump             : %u\n", last_data.engine_data.duty_pump);
                                // printf(" Smot                  : %u\n", last_data.engine_data.smot);
                                // printf(" Ignition 1            : %u\n", last_data.engine_data.ignition_1);
                                // printf(" Ignition 2            : %u\n", last_data.engine_data.ignition_2);
                                // printf(" Ignition 3            : %u\n", last_data.engine_data.ignition_3);
                                // printf(" Ignition 4            : %u\n", last_data.engine_data.ignition_4);
                                // printf(" FCC-TEMPERATURE-REPORT\n");
                                // printf(" Packet ID             : %u\n", last_data.temperature_data.packet_id);
                                // printf(" Length in bytes       : %u\n", last_data.temperature_data.length_in_bytes);
                                // printf(" Outside air temp      : %d\n", last_data.temperature_data.outside_air_temp);
                                // printf(" Fcc temperature       : %d\n", last_data.temperature_data.fcc_temperature);
                                // printf(" Fcc humidity          : %u\n", last_data.temperature_data.fcc_humidity);
                                // printf(" ACKNOWLEDGE-PACKET\n");
                                // printf(" Packet ID             : %u\n", last_data.acknowledge_data.packet_id);
                                // printf(" Length in bytes       : %u\n", last_data.acknowledge_data.length_in_bytes);
                                // printf(" Acknowledge packet ID : %u\n", last_data.acknowledge_data.acknowledge_packet_id);
                                // printf(" Sequence number       : %u\n", last_data.acknowledge_data.sequence_number);
                                // printf(" Response code         : %u\n", last_data.acknowledge_data.response_code);
                                // printf(" FCC-CONTROL-SURFACE\n");
                                // printf(" Packet ID             : %u\n", last_data.control_data.packet_id);
                                // printf(" Length in bytes       : %u\n", last_data.control_data.length_in_bytes);
                                // printf(" Flap l                : %d\n", last_data.control_data.flap_l);
                                // printf(" Flap r                : %d\n", last_data.control_data.flap_r);
                                // printf(" Aileron l             : %d\n", last_data.control_data.aileron_l);
                                // printf(" Aileron r             : %d\n", last_data.control_data.aileron_r);
                                // printf(" Elevator l            : %d\n", last_data.control_data.elevator_l);
                                // printf(" Elevator r            : %d\n", last_data.control_data.elevator_r);
                                // printf(" Rudder l              : %d\n", last_data.control_data.rudder_l);
                                // printf(" Rudder r              : %d\n", last_data.control_data.rudder_r);
                                // printf(" Throttle              : %d\n", last_data.control_data.throttle);
                                // printf(" Nose wheel angle      : %d\n", last_data.control_data.nose_wheel_angle);
                                // printf(" Break                 : %u\n", last_data.control_data.break_ctrl);
                                // printf("-----------------------------------------\n");
                                buffer_index = 0;
                                i--;
                                state = WAIT_IDEN;
                                break;
                            }
                    }
                }
            }
        }
    }
    
    CloseHandle(hSerial);
    return 0;
}
