use std::os::raw::{c_char, c_void};
use crate::connection::*;
use crate::connection_info::*;
use crate::connection_type::*;
use crate::data_messages::*;
use crate::decode_error::*;
use crate::ffi::callback::*;
use crate::ffi::connection_info::*;
use crate::ffi::helpers::*;
use crate::ffi::ping_response::*;
use crate::ffi::result::*;
use crate::statistics::*;

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_usb(connection_info: UsbConnectionInfoC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionInfo::UsbConnectionInfo(connection_info.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_serial(connection_info: SerialConnectionInfoC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionInfo::SerialConnectionInfo(connection_info.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_tcp(connection_info: TcpConnectionInfoC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionInfo::TcpConnectionInfo(connection_info.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_udp(connection_info: UdpConnectionInfoC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionInfo::UdpConnectionInfo(connection_info.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_bluetooth(connection_info: BluetoothConnectionInfoC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionInfo::BluetoothConnectionInfo(connection_info.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_new_file(connection_info: FileConnectionInfoC) -> *mut Connection {
    Box::into_raw(Box::new(Connection::new(&ConnectionInfo::FileConnectionInfo(connection_info.into()))))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_free(connection: *mut Connection) {
    unsafe { Box::from_raw(connection) };
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_open(connection: *mut Connection) -> Result {
    let connection: &mut Connection = unsafe { &mut *connection };
    match connection.open() {
        Ok(()) => Result::Ok,
        Err(_) => Result::Error,
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_open_async(connection: *mut Connection, callback: Callback<Result>, context: *mut c_void) {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.open_async(Box::new(move |result| {
        match result {
            Ok(()) => callback(Result::Ok, void_ptr.0),
            Err(_) => callback(Result::Error, void_ptr.0),
        }
    }));
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_close(connection: *mut Connection) {
    let connection: &mut Connection = unsafe { &mut *connection };
    connection.close();
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_ping(connection: *mut Connection) -> PingResponseC {
    let connection: &mut Connection = unsafe { &mut *connection };
    if let Ok(ping_response) = connection.ping() {
        ping_response.into()
    } else {
        Default::default()
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_send_commands(connection: *mut Connection, commands: *const *const c_char, length: u32, retries: u32, timeout: u32) -> CharArrays {
    let connection: &mut Connection = unsafe { &mut *connection };
    connection.send_commands(char_ptr_array_to_vec_str(commands, length), retries, timeout).into()
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_send_commands_async(connection: *mut Connection, commands: *const *const c_char, length: u32, retries: u32, timeout: u32, callback: Callback<CharArrays>, context: *mut c_void) {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    let closure = Box::new(move |responses: Vec<String>| { callback(responses.into(), void_ptr.0) });
    connection.send_commands_async(char_ptr_array_to_vec_str(commands, length), retries, timeout, closure);
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_type(connection: *mut Connection) -> ConnectionType {
    let connection: &mut Connection = unsafe { &mut *connection };
    match connection.get_info() {
        ConnectionInfo::UsbConnectionInfo(_) => ConnectionType::Usb,
        ConnectionInfo::SerialConnectionInfo(_) => ConnectionType::Serial,
        ConnectionInfo::TcpConnectionInfo(_) => ConnectionType::Tcp,
        ConnectionInfo::UdpConnectionInfo(_) => ConnectionType::Udp,
        ConnectionInfo::BluetoothConnectionInfo(_) => ConnectionType::Bluetooth,
        ConnectionInfo::FileConnectionInfo(_) => ConnectionType::File,
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_info_usb(connection: *mut Connection) -> UsbConnectionInfoC {
    let connection: &mut Connection = unsafe { &mut *connection };
    match &connection.get_info() {
        ConnectionInfo::UsbConnectionInfo(connection_info) => connection_info.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_info_serial(connection: *mut Connection) -> SerialConnectionInfoC {
    let connection: &mut Connection = unsafe { &mut *connection };
    match &connection.get_info() {
        ConnectionInfo::SerialConnectionInfo(connection_info) => connection_info.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_info_tcp(connection: *mut Connection) -> TcpConnectionInfoC {
    let connection: &mut Connection = unsafe { &mut *connection };
    match &connection.get_info() {
        ConnectionInfo::TcpConnectionInfo(connection_info) => connection_info.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_info_udp(connection: *mut Connection) -> UdpConnectionInfoC {
    let connection: &mut Connection = unsafe { &mut *connection };
    match &connection.get_info() {
        ConnectionInfo::UdpConnectionInfo(connection_info) => connection_info.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_info_bluetooth(connection: *mut Connection) -> BluetoothConnectionInfoC {
    let connection: &mut Connection = unsafe { &mut *connection };
    match &connection.get_info() {
        ConnectionInfo::BluetoothConnectionInfo(connection_info) => connection_info.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_info_file(connection: *mut Connection) -> FileConnectionInfoC {
    let connection: &mut Connection = unsafe { &mut *connection };
    match &connection.get_info() {
        ConnectionInfo::FileConnectionInfo(connection_info) => connection_info.into(),
        _ => Default::default(),
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_get_statistics(connection: *mut Connection) -> Statistics {
    let connection: &mut Connection = unsafe { &mut *connection };
    connection.get_statistics()
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_decode_error_callback(connection: *mut Connection, callback: Callback<DecodeError>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_decode_error_closure(Box::new(move |decode_error: DecodeError| callback(decode_error, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_statistics_callback(connection: *mut Connection, callback: Callback<Statistics>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_statistics_closure(Box::new(move |statistics: Statistics| callback(statistics, void_ptr.0)))
}

// Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_inertial_callback(connection: *mut Connection, callback: Callback<InertialMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_inertial_closure(Box::new(move |message: InertialMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_magnetometer_callback(connection: *mut Connection, callback: Callback<MagnetometerMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_magnetometer_closure(Box::new(move |message: MagnetometerMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_quaternion_callback(connection: *mut Connection, callback: Callback<QuaternionMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_quaternion_closure(Box::new(move |message: QuaternionMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_rotation_matrix_callback(connection: *mut Connection, callback: Callback<RotationMatrixMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_rotation_matrix_closure(Box::new(move |message: RotationMatrixMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_euler_angles_callback(connection: *mut Connection, callback: Callback<EulerAnglesMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_euler_angles_closure(Box::new(move |message: EulerAnglesMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_linear_acceleration_callback(connection: *mut Connection, callback: Callback<LinearAccelerationMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_linear_acceleration_closure(Box::new(move |message: LinearAccelerationMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_earth_acceleration_callback(connection: *mut Connection, callback: Callback<EarthAccelerationMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_earth_acceleration_closure(Box::new(move |message: EarthAccelerationMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_high_g_accelerometer_callback(connection: *mut Connection, callback: Callback<HighGAccelerometerMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_high_g_accelerometer_closure(Box::new(move |message: HighGAccelerometerMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_temperature_callback(connection: *mut Connection, callback: Callback<TemperatureMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_temperature_closure(Box::new(move |message: TemperatureMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_battery_callback(connection: *mut Connection, callback: Callback<BatteryMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_battery_closure(Box::new(move |message: BatteryMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_rssi_callback(connection: *mut Connection, callback: Callback<RssiMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_rssi_closure(Box::new(move |message: RssiMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_serial_accessory_callback(connection: *mut Connection, callback: Callback<SerialAccessoryMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_serial_accessory_closure(Box::new(move |message: SerialAccessoryMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_notification_callback(connection: *mut Connection, callback: Callback<NotificationMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_notification_closure(Box::new(move |message: NotificationMessage| callback(message, void_ptr.0)))
}

#[no_mangle]
pub extern "C" fn XIMU3_connection_add_error_callback(connection: *mut Connection, callback: Callback<ErrorMessage>, context: *mut c_void) -> u64 {
    let connection: &mut Connection = unsafe { &mut *connection };
    let void_ptr = VoidPtr(context);
    connection.add_error_closure(Box::new(move |message: ErrorMessage| callback(message, void_ptr.0)))
}
// End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#[no_mangle]
pub extern "C" fn XIMU3_connection_remove_callback(connection: *mut Connection, callback_id: u64) {
    let connection: &mut Connection = unsafe { &mut *connection };
    connection.remove_closure(callback_id);
}
