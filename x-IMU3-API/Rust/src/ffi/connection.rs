use crate::command_message::*;
use crate::connection::*;
use crate::connection_config::*;
use crate::data_messages::*;
use crate::ffi::callback::*;
use crate::ffi::command_message::*;
use crate::ffi::connection_config::*;
use crate::ffi::connection_type::*;
use crate::ffi::helpers::*;
use crate::ffi::ping_response::*;
use crate::ffi::result::*;
use crate::ping_response::*;
use crate::receive_error::*;
use crate::statistics::*;
use std::os::raw::{c_char, c_void};

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_usb(config: UsbConnectionConfigC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionConfig::UsbConnectionConfig(config.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_serial(config: SerialConnectionConfigC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionConfig::SerialConnectionConfig(config.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_tcp(config: TcpConnectionConfigC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionConfig::TcpConnectionConfig(config.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_udp(config: UdpConnectionConfigC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionConfig::UdpConnectionConfig(config.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_bluetooth(config: BluetoothConnectionConfigC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionConfig::BluetoothConnectionConfig(config.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_file(config: FileConnectionConfigC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionConfig::FileConnectionConfig(config.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_mux(config: *const MuxConnectionConfig) -> *mut Connection {
    let config: &MuxConnectionConfig = unsafe { &*config };
    Box::into_raw(Box::new(Connection::new(&ConnectionConfig::MuxConnectionConfig(config.clone()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_free(connection: *mut Connection) {
    unsafe { drop(Box::from_raw(connection)) };
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_open(connection: *mut Connection) -> Result {
    let connection = unsafe { &*connection };
    Result::from(&connection.open())
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_open_async(connection: *mut Connection, callback: Callback<Result>, context: *mut c_void) {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.open_async(Box::new(move |result| callback(Result::from(&result), void_ptr.0)));
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_close(connection: *mut Connection) {
    let connection = unsafe { &*connection };
    connection.close();
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_ping(connection: *mut Connection) -> PingResponseC {
    let connection = unsafe { &*connection };
    connection.ping().into()
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_ping_async(connection: *mut Connection, callback: Callback<PingResponseC>, context: *mut c_void) {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    let closure = Box::new(move |response: Option<PingResponse>| callback(response.into(), void_ptr.0));
    connection.ping_async(closure);
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_send_command(connection: *mut Connection, command: *const c_char, retries: u32, timeout: u32) -> CommandMessageC {
    let connection = unsafe { &*connection };
    let command = unsafe { char_ptr_to_bytes(command) };
    connection.send_command(command, retries, timeout).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_send_commands(connection: *mut Connection, commands: *const *const c_char, length: u32, retries: u32, timeout: u32) -> CommandMessages {
    let connection = unsafe { &*connection };
    let commands = unsafe { char_ptr_array_to_vec_bytes(commands, length) };
    connection.send_commands(commands, retries, timeout).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_send_command_async(connection: *mut Connection, command: *const c_char, retries: u32, timeout: u32, callback: Callback<CommandMessageC>, context: *mut c_void) {
    let connection = unsafe { &*connection };
    let command = unsafe { char_ptr_to_bytes(command) };
    let void_ptr = VoidPtr(context);
    let closure = Box::new(move |response: Option<CommandMessage>| callback(response.into(), void_ptr.0));
    connection.send_command_async(command, retries, timeout, closure);
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_send_commands_async(connection: *mut Connection, commands: *const *const c_char, length: u32, retries: u32, timeout: u32, callback: Callback<CommandMessages>, context: *mut c_void) {
    let connection = unsafe { &*connection };
    let commands = unsafe { char_ptr_array_to_vec_bytes(commands, length) };
    let void_ptr = VoidPtr(context);
    let closure = Box::new(move |responses: Vec<Option<CommandMessage>>| callback(responses.into(), void_ptr.0));
    connection.send_commands_async(commands, retries, timeout, closure);
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_type(connection: *mut Connection) -> ConnectionType {
    let connection = unsafe { &*connection };
    ConnectionType::from(&connection.get_config())
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_usb(connection: *mut Connection) -> UsbConnectionConfigC {
    let connection = unsafe { &*connection };
    match &connection.get_config() {
        ConnectionConfig::UsbConnectionConfig(config) => config.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_serial(connection: *mut Connection) -> SerialConnectionConfigC {
    let connection = unsafe { &*connection };
    match &connection.get_config() {
        ConnectionConfig::SerialConnectionConfig(config) => config.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_tcp(connection: *mut Connection) -> TcpConnectionConfigC {
    let connection = unsafe { &*connection };
    match &connection.get_config() {
        ConnectionConfig::TcpConnectionConfig(config) => config.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_udp(connection: *mut Connection) -> UdpConnectionConfigC {
    let connection = unsafe { &*connection };
    match &connection.get_config() {
        ConnectionConfig::UdpConnectionConfig(config) => config.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_bluetooth(connection: *mut Connection) -> BluetoothConnectionConfigC {
    let connection = unsafe { &*connection };
    match &connection.get_config() {
        ConnectionConfig::BluetoothConnectionConfig(config) => config.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_file(connection: *mut Connection) -> FileConnectionConfigC {
    let connection = unsafe { &*connection };
    match &connection.get_config() {
        ConnectionConfig::FileConnectionConfig(config) => config.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_mux(connection: *mut Connection) -> *mut MuxConnectionConfig {
    let connection: &Connection = unsafe { &*connection };
    match connection.get_config() {
        ConnectionConfig::MuxConnectionConfig(config) => Box::into_raw(Box::new(config)),
        _ => panic!("ConnectionConfig is not MuxConnectionConfig"),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_config_string(connection: *mut Connection) -> *const c_char {
    let connection: &Connection = unsafe { &*connection };
    str_to_char_ptr(&connection.get_config().to_string())
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_statistics(connection: *mut Connection) -> Statistics {
    let connection = unsafe { &*connection };
    connection.get_statistics()
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_receive_error_callback(connection: *mut Connection, callback: Callback<ReceiveError>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_receive_error_closure(Box::new(move |receive_error| callback(receive_error, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_statistics_callback(connection: *mut Connection, callback: Callback<Statistics>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_statistics_closure(Box::new(move |statistics| callback(statistics, void_ptr.0)))
}

// Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_inertial_callback(connection: *mut Connection, callback: Callback<InertialMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_inertial_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_magnetometer_callback(connection: *mut Connection, callback: Callback<MagnetometerMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_magnetometer_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_quaternion_callback(connection: *mut Connection, callback: Callback<QuaternionMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_quaternion_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_rotation_matrix_callback(connection: *mut Connection, callback: Callback<RotationMatrixMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_rotation_matrix_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_euler_angles_callback(connection: *mut Connection, callback: Callback<EulerAnglesMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_euler_angles_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_linear_acceleration_callback(connection: *mut Connection, callback: Callback<LinearAccelerationMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_linear_acceleration_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_earth_acceleration_callback(connection: *mut Connection, callback: Callback<EarthAccelerationMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_earth_acceleration_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_ahrs_status_callback(connection: *mut Connection, callback: Callback<AhrsStatusMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_ahrs_status_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_high_g_accelerometer_callback(connection: *mut Connection, callback: Callback<HighGAccelerometerMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_high_g_accelerometer_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_temperature_callback(connection: *mut Connection, callback: Callback<TemperatureMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_temperature_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_battery_callback(connection: *mut Connection, callback: Callback<BatteryMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_battery_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_rssi_callback(connection: *mut Connection, callback: Callback<RssiMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_rssi_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_serial_accessory_callback(connection: *mut Connection, callback: Callback<SerialAccessoryMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_serial_accessory_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_notification_callback(connection: *mut Connection, callback: Callback<NotificationMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_notification_closure(Box::new(move |message| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_error_callback(connection: *mut Connection, callback: Callback<ErrorMessage>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_error_closure(Box::new(move |message| callback(message, void_ptr.0)))
}
// End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_end_of_file_callback(connection: *mut Connection, callback: extern "C" fn(context: *mut c_void), context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_end_of_file_closure(Box::new(move || callback(void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_remove_callback(connection: *mut Connection, callback_id: u64) {
    let connection = unsafe { &*connection };
    connection.remove_closure(callback_id);
}
