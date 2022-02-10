use ximu3::connection::*;
use ximu3::connection_info::*;
use ximu3::data_messages::*;
use ximu3::decode_error::*;
use ximu3::statistics::*;
use crate::helpers;

pub fn run(connection_info: ConnectionInfo) {

    // Create connection
    let mut connection = Connection::new(connection_info);

    connection.add_decode_error_closure(Box::new(decode_error_closure));
    connection.add_statistics_closure(Box::new(statistics_closure));

    if helpers::yes_or_no("Print data messages?") {
        connection.add_inertial_closure(Box::new(inertial_closure));
        connection.add_magnetometer_closure(Box::new(magnetometer_closure));
        connection.add_quaternion_closure(Box::new(quaternion_closure));
        connection.add_rotation_matrix_closure(Box::new(rotation_matrix_closure));
        connection.add_euler_angles_closure(Box::new(euler_angles_closure));
        connection.add_linear_acceleration_closure(Box::new(linear_acceleration_closure));
        connection.add_earth_acceleration_closure(Box::new(earth_acceleration_closure));
        connection.add_high_g_accelerometer_closure(Box::new(high_g_accelerometer_closure));
        connection.add_temperature_closure(Box::new(temperature_closure));
        connection.add_battery_closure(Box::new(battery_closure));
        connection.add_rssi_closure(Box::new(rssi_closure));
        connection.add_serial_accessory_closure(Box::new(serial_accessory_closure));
        connection.add_notification_closure(Box::new(notification_closure));
        connection.add_error_closure(Box::new(error_closure));
    }

    // Open connection
    println!("Connecting to {}", connection.get_info().to_string());

    if connection.open().is_err() {
        println!("Unable to open connection");
        return;
    }
    println!("Connection successful");

    // Send command to strobe LED
    connection.send_commands(vec!["{\"strobe\":null}"], 2, 500);

    // Close connection
    helpers::wait(-1);
    connection.close();
}

macro_rules! timestamp_format {() => { "{:>8} us" }}

macro_rules! int_format {() => { " {:>8}" }}

macro_rules! float_format {() => { " {:>8.3}" }}

macro_rules! string_format {() => { " \"{}\"" }}

pub fn decode_error_closure(decode_error: DecodeError) {
    println!("{}", decode_error);
}

pub fn statistics_closure(statistics: Statistics) {
    println!(concat!(timestamp_format!(), int_format!(), " bytes", int_format!(), " bytes/s", int_format!(), " messages", int_format!(), " messages/s", int_format!(), " errors", int_format!(), " errors/s"),
             statistics.timestamp,
             statistics.data_total,
             statistics.data_rate,
             statistics.message_total,
             statistics.message_rate,
             statistics.error_total,
             statistics.error_rate);
    // println!("{}", statistics); // alternative to above
}

pub fn inertial_closure(message: InertialMessage) {
    println!(concat!(timestamp_format!(), float_format!(), " deg/s", float_format!(), " deg/s", float_format!(), " deg/s", float_format!(), " g", float_format!(), " g", float_format!(), " g"),
             message.timestamp,
             message.gyroscope_x,
             message.gyroscope_y,
             message.gyroscope_z,
             message.accelerometer_x,
             message.accelerometer_y,
             message.accelerometer_z);
    // println!("{}", message); // alternative to above
}

pub fn magnetometer_closure(message: MagnetometerMessage) {
    println!(concat!(timestamp_format!(), float_format!(), " a.u.", float_format!(), " a.u.", float_format!(), " a.u."),
             message.timestamp,
             message.x_axis,
             message.y_axis,
             message.z_axis);
    // println!("{}", message); // alternative to above
}

pub fn quaternion_closure(message: QuaternionMessage) {
    println!(concat!(timestamp_format!(), float_format!(), float_format!(), float_format!(), float_format!()),
             message.timestamp,
             message.w_element,
             message.x_element,
             message.y_element,
             message.z_element);
    // println!("{}", message); // alternative to above
}

pub fn rotation_matrix_closure(message: RotationMatrixMessage) {
    println!(concat!(timestamp_format!(), float_format!(), float_format!(), float_format!(), float_format!(), float_format!(), float_format!(), float_format!(), float_format!(), float_format!()),
             message.timestamp,
             message.xx_element,
             message.xy_element,
             message.xz_element,
             message.yx_element,
             message.yy_element,
             message.yz_element,
             message.zx_element,
             message.zy_element,
             message.zz_element);
    // println!("{}", message); // alternative to above
}

pub fn euler_angles_closure(message: EulerAnglesMessage) {
    println!(concat!(timestamp_format!(), float_format!(), " deg", float_format!(), " deg", float_format!(), " deg"),
             message.timestamp,
             message.roll,
             message.pitch,
             message.yaw);
    // println!("{}", message); // alternative to above
}

pub fn linear_acceleration_closure(message: LinearAccelerationMessage) {
    println!(concat!(timestamp_format!(), float_format!(), float_format!(), float_format!(), float_format!(), float_format!(), " g", float_format!(), " g", float_format!(), " g"),
             message.timestamp,
             message.w_element,
             message.x_element,
             message.y_element,
             message.z_element,
             message.x_axis,
             message.y_axis,
             message.z_axis);
    // println!("{}", message); // alternative to above
}

pub fn earth_acceleration_closure(message: EarthAccelerationMessage) {
    println!(concat!(timestamp_format!(), float_format!(), float_format!(), float_format!(), float_format!(), float_format!(), " g", float_format!(), " g", float_format!(), " g"),
             message.timestamp,
             message.w_element,
             message.x_element,
             message.y_element,
             message.z_element,
             message.x_axis,
             message.y_axis,
             message.z_axis);
    // println!("{}", message); // alternative to above
}

pub fn high_g_accelerometer_closure(message: HighGAccelerometerMessage) {
    println!(concat!(timestamp_format!(), float_format!(), " g", float_format!(), " g", float_format!(), " g"),
             message.timestamp,
             message.x_axis,
             message.y_axis,
             message.z_axis);
    // println!("{}", message); // alternative to above
}

pub fn temperature_closure(message: TemperatureMessage) {
    println!(concat!(timestamp_format!(), float_format!(), " degC"),
             message.timestamp,
             message.temperature);
    // println!("{}", message); // alternative to above
}

pub fn battery_closure(message: BatteryMessage) {
    println!(concat!(timestamp_format!(), float_format!(), " %", float_format!(), " V", float_format!()),
             message.timestamp,
             message.percentage,
             message.voltage,
             message.charging_status);
    // println!("{}", message); // alternative to above
}

pub fn rssi_closure(message: RssiMessage) {
    println!(concat!(timestamp_format!(), float_format!(), " %", float_format!(), " dBm"),
             message.timestamp,
             message.percentage,
             message.power);
    // println!("{}", message); // alternative to above
}

pub fn serial_accessory_closure(message: SerialAccessoryMessage) {
    println!(concat!(timestamp_format!(), string_format!()),
             message.timestamp,
             message.char_array_as_string());
    // println!("{}", message); // alternative to above
}

pub fn notification_closure(message: NotificationMessage) {
    println!(concat!(timestamp_format!(), string_format!()),
             message.timestamp,
             message.char_array_as_string());
    // println!("{}", message); // alternative to above
}

pub fn error_closure(message: ErrorMessage) {
    println!(concat!(timestamp_format!(), string_format!()),
             message.timestamp,
             message.char_array_as_string());
    // println!("{}", message); // alternative to above
}
