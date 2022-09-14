#ifdef _WIN32
#pragma comment(lib, "bcrypt")
#pragma comment(lib, "setupapi")
#pragma comment(lib, "userenv")
#pragma comment(lib, "ws2_32")
#endif

#ifndef XIMU3_H
#define XIMU3_H

/* Warning, this file is autogenerated by cbindgen. Don't modify this manually. */

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
namespace ximu3 {
#endif // __cplusplus

#define XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE 256

#define XIMU3_CHAR_ARRAY_SIZE 256

typedef enum XIMU3_ChargingStatus
{
    XIMU3_ChargingStatusNotConnected,
    XIMU3_ChargingStatusCharging,
    XIMU3_ChargingStatusChargingComplete,
} XIMU3_ChargingStatus;

typedef enum XIMU3_ConnectionType
{
    XIMU3_ConnectionTypeUsb,
    XIMU3_ConnectionTypeSerial,
    XIMU3_ConnectionTypeTcp,
    XIMU3_ConnectionTypeUdp,
    XIMU3_ConnectionTypeBluetooth,
    XIMU3_ConnectionTypeFile,
} XIMU3_ConnectionType;

typedef enum XIMU3_DecodeError
{
    XIMU3_DecodeErrorBufferOverrun,
    XIMU3_DecodeErrorInvalidMessageIdentifier,
    XIMU3_DecodeErrorInvalidUtf8,
    XIMU3_DecodeErrorInvalidJson,
    XIMU3_DecodeErrorJsonIsNotAnObject,
    XIMU3_DecodeErrorJsonObjectIsNotASingleKeyValuePair,
    XIMU3_DecodeErrorInvalidEscapeSequence,
    XIMU3_DecodeErrorInvalidBinaryMessageLength,
    XIMU3_DecodeErrorUnableToParseAsciiMessage,
} XIMU3_DecodeError;

typedef enum XIMU3_FileConverterStatus
{
    XIMU3_FileConverterStatusComplete,
    XIMU3_FileConverterStatusFailed,
    XIMU3_FileConverterStatusInProgress,
} XIMU3_FileConverterStatus;

typedef enum XIMU3_Result
{
    XIMU3_ResultOk,
    XIMU3_ResultError,
} XIMU3_Result;

typedef struct XIMU3_Connection XIMU3_Connection;

typedef struct XIMU3_DataLogger XIMU3_DataLogger;

typedef struct XIMU3_FileConverter XIMU3_FileConverter;

typedef struct XIMU3_NetworkAnnouncement XIMU3_NetworkAnnouncement;

typedef struct XIMU3_SerialDiscovery XIMU3_SerialDiscovery;

typedef struct XIMU3_CharArrays
{
    char (*array)[XIMU3_CHAR_ARRAY_SIZE];
    uint32_t length;
    uint32_t capacity;
} XIMU3_CharArrays;

typedef struct XIMU3_UsbConnectionInfo
{
    char port_name[XIMU3_CHAR_ARRAY_SIZE];
} XIMU3_UsbConnectionInfo;

typedef struct XIMU3_SerialConnectionInfo
{
    char port_name[XIMU3_CHAR_ARRAY_SIZE];
    uint32_t baud_rate;
    bool rts_cts_enabled;
} XIMU3_SerialConnectionInfo;

typedef struct XIMU3_TcpConnectionInfo
{
    char ip_address[XIMU3_CHAR_ARRAY_SIZE];
    uint16_t port;
} XIMU3_TcpConnectionInfo;

typedef struct XIMU3_UdpConnectionInfo
{
    char ip_address[XIMU3_CHAR_ARRAY_SIZE];
    uint16_t send_port;
    uint16_t receive_port;
} XIMU3_UdpConnectionInfo;

typedef struct XIMU3_BluetoothConnectionInfo
{
    char port_name[XIMU3_CHAR_ARRAY_SIZE];
} XIMU3_BluetoothConnectionInfo;

typedef struct XIMU3_FileConnectionInfo
{
    char file_path[XIMU3_CHAR_ARRAY_SIZE];
} XIMU3_FileConnectionInfo;

typedef void (*XIMU3_CallbackResult)(enum XIMU3_Result data, void *context);

typedef struct XIMU3_PingResponse
{
    char interface[XIMU3_CHAR_ARRAY_SIZE];
    char device_name[XIMU3_CHAR_ARRAY_SIZE];
    char serial_number[XIMU3_CHAR_ARRAY_SIZE];
} XIMU3_PingResponse;

typedef void (*XIMU3_CallbackCharArrays)(struct XIMU3_CharArrays data, void *context);

typedef struct XIMU3_Statistics
{
    uint64_t timestamp;
    uint64_t data_total;
    uint32_t data_rate;
    uint64_t message_total;
    uint32_t message_rate;
    uint64_t error_total;
    uint32_t error_rate;
} XIMU3_Statistics;

typedef void (*XIMU3_CallbackDecodeError)(enum XIMU3_DecodeError data, void *context);

typedef void (*XIMU3_CallbackStatistics)(struct XIMU3_Statistics data, void *context);

typedef struct XIMU3_InertialMessage
{
    uint64_t timestamp;
    float gyroscope_x;
    float gyroscope_y;
    float gyroscope_z;
    float accelerometer_x;
    float accelerometer_y;
    float accelerometer_z;
} XIMU3_InertialMessage;

typedef void (*XIMU3_CallbackInertialMessage)(struct XIMU3_InertialMessage data, void *context);

typedef struct XIMU3_MagnetometerMessage
{
    uint64_t timestamp;
    float x_axis;
    float y_axis;
    float z_axis;
} XIMU3_MagnetometerMessage;

typedef void (*XIMU3_CallbackMagnetometerMessage)(struct XIMU3_MagnetometerMessage data, void *context);

typedef struct XIMU3_QuaternionMessage
{
    uint64_t timestamp;
    float w_element;
    float x_element;
    float y_element;
    float z_element;
} XIMU3_QuaternionMessage;

typedef void (*XIMU3_CallbackQuaternionMessage)(struct XIMU3_QuaternionMessage data, void *context);

typedef struct XIMU3_RotationMatrixMessage
{
    uint64_t timestamp;
    float xx_element;
    float xy_element;
    float xz_element;
    float yx_element;
    float yy_element;
    float yz_element;
    float zx_element;
    float zy_element;
    float zz_element;
} XIMU3_RotationMatrixMessage;

typedef void (*XIMU3_CallbackRotationMatrixMessage)(struct XIMU3_RotationMatrixMessage data, void *context);

typedef struct XIMU3_EulerAnglesMessage
{
    uint64_t timestamp;
    float roll;
    float pitch;
    float yaw;
} XIMU3_EulerAnglesMessage;

typedef void (*XIMU3_CallbackEulerAnglesMessage)(struct XIMU3_EulerAnglesMessage data, void *context);

typedef struct XIMU3_LinearAccelerationMessage
{
    uint64_t timestamp;
    float w_element;
    float x_element;
    float y_element;
    float z_element;
    float x_axis;
    float y_axis;
    float z_axis;
} XIMU3_LinearAccelerationMessage;

typedef void (*XIMU3_CallbackLinearAccelerationMessage)(struct XIMU3_LinearAccelerationMessage data, void *context);

typedef struct XIMU3_EarthAccelerationMessage
{
    uint64_t timestamp;
    float w_element;
    float x_element;
    float y_element;
    float z_element;
    float x_axis;
    float y_axis;
    float z_axis;
} XIMU3_EarthAccelerationMessage;

typedef void (*XIMU3_CallbackEarthAccelerationMessage)(struct XIMU3_EarthAccelerationMessage data, void *context);

typedef struct XIMU3_HighGAccelerometerMessage
{
    uint64_t timestamp;
    float x_axis;
    float y_axis;
    float z_axis;
} XIMU3_HighGAccelerometerMessage;

typedef void (*XIMU3_CallbackHighGAccelerometerMessage)(struct XIMU3_HighGAccelerometerMessage data, void *context);

typedef struct XIMU3_TemperatureMessage
{
    uint64_t timestamp;
    float temperature;
} XIMU3_TemperatureMessage;

typedef void (*XIMU3_CallbackTemperatureMessage)(struct XIMU3_TemperatureMessage data, void *context);

typedef struct XIMU3_BatteryMessage
{
    uint64_t timestamp;
    float percentage;
    float voltage;
    float charging_status;
} XIMU3_BatteryMessage;

typedef void (*XIMU3_CallbackBatteryMessage)(struct XIMU3_BatteryMessage data, void *context);

typedef struct XIMU3_RssiMessage
{
    uint64_t timestamp;
    float percentage;
    float power;
} XIMU3_RssiMessage;

typedef void (*XIMU3_CallbackRssiMessage)(struct XIMU3_RssiMessage data, void *context);

typedef struct XIMU3_SerialAccessoryMessage
{
    uint64_t timestamp;
    char char_array[XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE];
    size_t number_of_bytes;
} XIMU3_SerialAccessoryMessage;

typedef void (*XIMU3_CallbackSerialAccessoryMessage)(struct XIMU3_SerialAccessoryMessage data, void *context);

typedef struct XIMU3_NotificationMessage
{
    uint64_t timestamp;
    char char_array[XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE];
    size_t number_of_bytes;
} XIMU3_NotificationMessage;

typedef void (*XIMU3_CallbackNotificationMessage)(struct XIMU3_NotificationMessage data, void *context);

typedef struct XIMU3_ErrorMessage
{
    uint64_t timestamp;
    char char_array[XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE];
    size_t number_of_bytes;
} XIMU3_ErrorMessage;

typedef void (*XIMU3_CallbackErrorMessage)(struct XIMU3_ErrorMessage data, void *context);

typedef struct XIMU3_FileConverterProgress
{
    enum XIMU3_FileConverterStatus status;
    float percentage;
    uint64_t bytes_processed;
    uint64_t file_size;
} XIMU3_FileConverterProgress;

typedef void (*XIMU3_CallbackFileConverterProgress)(struct XIMU3_FileConverterProgress data, void *context);

typedef struct XIMU3_NetworkAnnouncementMessage
{
    char device_name[XIMU3_CHAR_ARRAY_SIZE];
    char serial_number[XIMU3_CHAR_ARRAY_SIZE];
    uint32_t rssi;
    uint32_t battery;
    enum XIMU3_ChargingStatus status;
    struct XIMU3_TcpConnectionInfo tcp_connection_info;
    struct XIMU3_UdpConnectionInfo udp_connection_info;
} XIMU3_NetworkAnnouncementMessage;

typedef struct XIMU3_NetworkAnnouncementMessages
{
    struct XIMU3_NetworkAnnouncementMessage *array;
    uint32_t length;
    uint32_t capacity;
} XIMU3_NetworkAnnouncementMessages;

typedef void (*XIMU3_CallbackNetworkAnnouncementMessageC)(struct XIMU3_NetworkAnnouncementMessage data, void *context);

typedef struct XIMU3_DiscoveredSerialDevice
{
    char device_name[XIMU3_CHAR_ARRAY_SIZE];
    char serial_number[XIMU3_CHAR_ARRAY_SIZE];
    enum XIMU3_ConnectionType connection_type;
    struct XIMU3_UsbConnectionInfo usb_connection_info;
    struct XIMU3_SerialConnectionInfo serial_connection_info;
    struct XIMU3_BluetoothConnectionInfo bluetooth_connection_info;
} XIMU3_DiscoveredSerialDevice;

typedef struct XIMU3_DiscoveredSerialDevices
{
    struct XIMU3_DiscoveredSerialDevice *array;
    uint32_t length;
    uint32_t capacity;
} XIMU3_DiscoveredSerialDevices;

typedef void (*XIMU3_CallbackDiscoveredSerialDevices)(struct XIMU3_DiscoveredSerialDevices data, void *context);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void XIMU3_char_arrays_free(struct XIMU3_CharArrays char_arrays);

const char *XIMU3_charging_status_to_string(enum XIMU3_ChargingStatus charging_status);

struct XIMU3_Connection *XIMU3_connection_new_usb(struct XIMU3_UsbConnectionInfo connection_info);

struct XIMU3_Connection *XIMU3_connection_new_serial(struct XIMU3_SerialConnectionInfo connection_info);

struct XIMU3_Connection *XIMU3_connection_new_tcp(struct XIMU3_TcpConnectionInfo connection_info);

struct XIMU3_Connection *XIMU3_connection_new_udp(struct XIMU3_UdpConnectionInfo connection_info);

struct XIMU3_Connection *XIMU3_connection_new_bluetooth(struct XIMU3_BluetoothConnectionInfo connection_info);

struct XIMU3_Connection *XIMU3_connection_new_file(struct XIMU3_FileConnectionInfo connection_info);

void XIMU3_connection_free(struct XIMU3_Connection *connection);

enum XIMU3_Result XIMU3_connection_open(struct XIMU3_Connection *connection);

void XIMU3_connection_open_async(struct XIMU3_Connection *connection, XIMU3_CallbackResult callback, void *context);

void XIMU3_connection_close(struct XIMU3_Connection *connection);

struct XIMU3_PingResponse XIMU3_connection_ping(struct XIMU3_Connection *connection);

struct XIMU3_CharArrays XIMU3_connection_send_commands(struct XIMU3_Connection *connection, const char *const *commands, uint32_t length, uint32_t retries, uint32_t timeout);

void XIMU3_connection_send_commands_async(struct XIMU3_Connection *connection, const char *const *commands, uint32_t length, uint32_t retries, uint32_t timeout, XIMU3_CallbackCharArrays callback, void *context);

enum XIMU3_ConnectionType XIMU3_connection_get_type(struct XIMU3_Connection *connection);

struct XIMU3_UsbConnectionInfo XIMU3_connection_get_info_usb(struct XIMU3_Connection *connection);

struct XIMU3_SerialConnectionInfo XIMU3_connection_get_info_serial(struct XIMU3_Connection *connection);

struct XIMU3_TcpConnectionInfo XIMU3_connection_get_info_tcp(struct XIMU3_Connection *connection);

struct XIMU3_UdpConnectionInfo XIMU3_connection_get_info_udp(struct XIMU3_Connection *connection);

struct XIMU3_BluetoothConnectionInfo XIMU3_connection_get_info_bluetooth(struct XIMU3_Connection *connection);

struct XIMU3_FileConnectionInfo XIMU3_connection_get_info_file(struct XIMU3_Connection *connection);

struct XIMU3_Statistics XIMU3_connection_get_statistics(struct XIMU3_Connection *connection);

uint64_t XIMU3_connection_add_decode_error_callback(struct XIMU3_Connection *connection, XIMU3_CallbackDecodeError callback, void *context);

uint64_t XIMU3_connection_add_statistics_callback(struct XIMU3_Connection *connection, XIMU3_CallbackStatistics callback, void *context);

uint64_t XIMU3_connection_add_inertial_callback(struct XIMU3_Connection *connection, XIMU3_CallbackInertialMessage callback, void *context);

uint64_t XIMU3_connection_add_magnetometer_callback(struct XIMU3_Connection *connection, XIMU3_CallbackMagnetometerMessage callback, void *context);

uint64_t XIMU3_connection_add_quaternion_callback(struct XIMU3_Connection *connection, XIMU3_CallbackQuaternionMessage callback, void *context);

uint64_t XIMU3_connection_add_rotation_matrix_callback(struct XIMU3_Connection *connection, XIMU3_CallbackRotationMatrixMessage callback, void *context);

uint64_t XIMU3_connection_add_euler_angles_callback(struct XIMU3_Connection *connection, XIMU3_CallbackEulerAnglesMessage callback, void *context);

uint64_t XIMU3_connection_add_linear_acceleration_callback(struct XIMU3_Connection *connection, XIMU3_CallbackLinearAccelerationMessage callback, void *context);

uint64_t XIMU3_connection_add_earth_acceleration_callback(struct XIMU3_Connection *connection, XIMU3_CallbackEarthAccelerationMessage callback, void *context);

uint64_t XIMU3_connection_add_high_g_accelerometer_callback(struct XIMU3_Connection *connection, XIMU3_CallbackHighGAccelerometerMessage callback, void *context);

uint64_t XIMU3_connection_add_temperature_callback(struct XIMU3_Connection *connection, XIMU3_CallbackTemperatureMessage callback, void *context);

uint64_t XIMU3_connection_add_battery_callback(struct XIMU3_Connection *connection, XIMU3_CallbackBatteryMessage callback, void *context);

uint64_t XIMU3_connection_add_rssi_callback(struct XIMU3_Connection *connection, XIMU3_CallbackRssiMessage callback, void *context);

uint64_t XIMU3_connection_add_serial_accessory_callback(struct XIMU3_Connection *connection, XIMU3_CallbackSerialAccessoryMessage callback, void *context);

uint64_t XIMU3_connection_add_notification_callback(struct XIMU3_Connection *connection, XIMU3_CallbackNotificationMessage callback, void *context);

uint64_t XIMU3_connection_add_error_callback(struct XIMU3_Connection *connection, XIMU3_CallbackErrorMessage callback, void *context);

void XIMU3_connection_remove_callback(struct XIMU3_Connection *connection, uint64_t callback_id);

const char *XIMU3_usb_connection_info_to_string(struct XIMU3_UsbConnectionInfo connection_info);

const char *XIMU3_serial_connection_info_to_string(struct XIMU3_SerialConnectionInfo connection_info);

const char *XIMU3_tcp_connection_info_to_string(struct XIMU3_TcpConnectionInfo connection_info);

const char *XIMU3_udp_connection_info_to_string(struct XIMU3_UdpConnectionInfo connection_info);

const char *XIMU3_bluetooth_connection_info_to_string(struct XIMU3_BluetoothConnectionInfo connection_info);

const char *XIMU3_file_connection_info_to_string(struct XIMU3_FileConnectionInfo connection_info);

const char *XIMU3_connection_type_to_string(enum XIMU3_ConnectionType connection_type);

struct XIMU3_DataLogger *XIMU3_data_logger_new(const char *directory, const char *name, struct XIMU3_Connection *const *connections, uint32_t length, XIMU3_CallbackResult callback, void *context);

void XIMU3_data_logger_free(struct XIMU3_DataLogger *data_logger);

enum XIMU3_Result XIMU3_data_logger_log(const char *directory, const char *name, struct XIMU3_Connection *const *connections, uint32_t length, uint32_t seconds);

const char *XIMU3_inertial_message_to_string(struct XIMU3_InertialMessage message);

const char *XIMU3_magnetometer_message_to_string(struct XIMU3_MagnetometerMessage message);

const char *XIMU3_quaternion_message_to_string(struct XIMU3_QuaternionMessage message);

const char *XIMU3_rotation_matrix_message_to_string(struct XIMU3_RotationMatrixMessage message);

const char *XIMU3_euler_angles_message_to_string(struct XIMU3_EulerAnglesMessage message);

const char *XIMU3_linear_acceleration_message_to_string(struct XIMU3_LinearAccelerationMessage message);

const char *XIMU3_earth_acceleration_message_to_string(struct XIMU3_EarthAccelerationMessage message);

const char *XIMU3_high_g_accelerometer_message_to_string(struct XIMU3_HighGAccelerometerMessage message);

const char *XIMU3_temperature_message_to_string(struct XIMU3_TemperatureMessage message);

const char *XIMU3_battery_message_to_string(struct XIMU3_BatteryMessage message);

const char *XIMU3_rssi_message_to_string(struct XIMU3_RssiMessage message);

const char *XIMU3_serial_accessory_message_to_string(struct XIMU3_SerialAccessoryMessage message);

const char *XIMU3_notification_message_to_string(struct XIMU3_NotificationMessage message);

const char *XIMU3_error_message_to_string(struct XIMU3_ErrorMessage message);

const char *XIMU3_decode_error_to_string(enum XIMU3_DecodeError decode_error);

const char *XIMU3_file_converter_status_to_string(enum XIMU3_FileConverterStatus status);

const char *XIMU3_file_converter_progress_to_string(struct XIMU3_FileConverterProgress progress);

struct XIMU3_FileConverter *XIMU3_file_converter_new(const char *destination, const char *source, XIMU3_CallbackFileConverterProgress callback, void *context);

void XIMU3_file_converter_free(struct XIMU3_FileConverter *file_converter);

struct XIMU3_FileConverterProgress XIMU3_file_converter_convert(const char *destination, const char *source);

void XIMU3_network_announcement_messages_free(struct XIMU3_NetworkAnnouncementMessages messages);

const char *XIMU3_network_announcement_message_to_string(struct XIMU3_NetworkAnnouncementMessage message);

struct XIMU3_NetworkAnnouncement *XIMU3_network_announcement_new(void);

void XIMU3_network_announcement_free(struct XIMU3_NetworkAnnouncement *network_announcement);

uint64_t XIMU3_network_announcement_add_callback(struct XIMU3_NetworkAnnouncement *network_announcement, XIMU3_CallbackNetworkAnnouncementMessageC callback, void *context);

void XIMU3_network_announcement_remove_callback(struct XIMU3_NetworkAnnouncement *network_announcement, uint64_t callback_id);

struct XIMU3_NetworkAnnouncementMessages XIMU3_network_announcement_get_messages(struct XIMU3_NetworkAnnouncement *network_announcement);

struct XIMU3_NetworkAnnouncementMessages XIMU3_network_announcement_get_messages_after_short_delay(struct XIMU3_NetworkAnnouncement *network_announcement);

const char *XIMU3_ping_response_to_string(struct XIMU3_PingResponse ping_response);

const char *XIMU3_result_to_string(enum XIMU3_Result result);

void XIMU3_discovered_serial_devices_free(struct XIMU3_DiscoveredSerialDevices devices);

const char *XIMU3_discovered_serial_device_to_string(struct XIMU3_DiscoveredSerialDevice device);

struct XIMU3_SerialDiscovery *XIMU3_serial_discovery_new(XIMU3_CallbackDiscoveredSerialDevices callback, void *context);

void XIMU3_serial_discovery_free(struct XIMU3_SerialDiscovery *discovery);

struct XIMU3_DiscoveredSerialDevices XIMU3_serial_discovery_get_devices(struct XIMU3_SerialDiscovery *discovery);

struct XIMU3_DiscoveredSerialDevices XIMU3_serial_discovery_scan(uint32_t milliseconds);

struct XIMU3_DiscoveredSerialDevices XIMU3_serial_discovery_scan_filter(uint32_t milliseconds, enum XIMU3_ConnectionType connection_type);

struct XIMU3_CharArrays XIMU3_serial_discovery_get_available_ports(void);

const char *XIMU3_statistics_to_string(struct XIMU3_Statistics statistics);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ximu3
#endif // __cplusplus

#endif /* XIMU3_H */
