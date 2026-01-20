use crate::helpers;
use ximu3::charging_status::*;
use ximu3::connection::*;
use ximu3::connection_info::*;
use ximu3::data_messages::*;
use ximu3::receive_error::*;
use ximu3::statistics::*;

pub fn run(connection_info: &ConnectionInfo) {
    // Create connection
    let connection = Connection::new(&connection_info);

    connection.add_receive_error_closure(Box::new(receive_error_closure));
    connection.add_statistics_closure(Box::new(statistics_closure));

    if helpers::yes_or_no("Print data messages?") {
        connection.add_inertial_closure(Box::new(inertial_closure));
        connection.add_magnetometer_closure(Box::new(magnetometer_closure));
        connection.add_quaternion_closure(Box::new(quaternion_closure));
        connection.add_rotation_matrix_closure(Box::new(rotation_matrix_closure));
        connection.add_euler_angles_closure(Box::new(euler_angles_closure));
        connection.add_linear_acceleration_closure(Box::new(linear_acceleration_closure));
        connection.add_earth_acceleration_closure(Box::new(earth_acceleration_closure));
        connection.add_ahrs_status_closure(Box::new(ahrs_status_closure));
        connection.add_high_g_accelerometer_closure(Box::new(high_g_accelerometer_closure));
        connection.add_temperature_closure(Box::new(temperature_closure));
        connection.add_battery_closure(Box::new(battery_closure));
        connection.add_rssi_closure(Box::new(rssi_closure));
        connection.add_serial_accessory_closure(Box::new(serial_accessory_closure));
        connection.add_notification_closure(Box::new(notification_closure));
        connection.add_error_closure(Box::new(error_closure));
    }

    connection.add_end_of_file_closure(Box::new(end_of_file_closure));

    // Open connection
    if let Err(error) = connection.open() {
        println!("Unable to open {}. {error}.", connection.get_info());
        return;
    }

    // Send command to strobe LED
    connection.send_commands(vec!["{\"strobe\":null}".into()], DEFAULT_RETRIES, DEFAULT_TIMEOUT);

    // Close connection
    std::thread::sleep(std::time::Duration::from_secs(60));

    connection.close();
}

macro_rules! timestamp_fmt {
    () => {
        "{:>8} us"
    };
}

macro_rules! int_fmt {
    () => {
        " {:>8}"
    };
}

macro_rules! float_fmt {
    () => {
        " {:>8.3}"
    };
}

macro_rules! string_fmt {
    () => {
        " \"{}\""
    };
}

pub fn receive_error_closure(error: ReceiveError) {
    println!("{error}");
}

pub fn statistics_closure(statistics: Statistics) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), int_fmt!(), " bytes", int_fmt!(), " bytes/s", int_fmt!(), " messages", int_fmt!(), " messages/s", int_fmt!(), " errors", int_fmt!(), " errors/s"),
             statistics.timestamp,
             statistics.data_total,
             statistics.data_rate,
             statistics.message_total,
             statistics.message_rate,
             statistics.error_total,
             statistics.error_rate);
    // println!("{statistics}"); // alternative to above
}

pub fn inertial_closure(message: InertialMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), " deg/s", float_fmt!(), " deg/s", float_fmt!(), " deg/s", float_fmt!(), " g", float_fmt!(), " g", float_fmt!(), " g"),
             message.timestamp,
             message.gyroscope_x,
             message.gyroscope_y,
             message.gyroscope_z,
             message.accelerometer_x,
             message.accelerometer_y,
             message.accelerometer_z);
    // println!("{message}"); // alternative to above
}

pub fn magnetometer_closure(message: MagnetometerMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), " a.u.", float_fmt!(), " a.u.", float_fmt!(), " a.u."),
             message.timestamp,
             message.x,
             message.y,
             message.z);
    // println!("{message}"); // alternative to above
}

pub fn quaternion_closure(message: QuaternionMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!()),
             message.timestamp,
             message.w,
             message.x,
             message.y,
             message.z);
    // println!("{message}"); // alternative to above
    println!("{}", EulerAnglesMessage::from(message));
}

pub fn rotation_matrix_closure(message: RotationMatrixMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!()),
             message.timestamp,
             message.xx,
             message.xy,
             message.xz,
             message.yx,
             message.yy,
             message.yz,
             message.zx,
             message.zy,
             message.zz);
    // println!("{message}"); // alternative to above
    println!("{}", EulerAnglesMessage::from(message));
}

pub fn euler_angles_closure(message: EulerAnglesMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), " deg", float_fmt!(), " deg", float_fmt!(), " deg"),
             message.timestamp,
             message.roll,
             message.pitch,
             message.yaw);
    // println!("{message}"); // alternative to above
    println!("{}", QuaternionMessage::from(message));
}

pub fn linear_acceleration_closure(message: LinearAccelerationMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), " g", float_fmt!(), " g", float_fmt!(), " g"),
             message.timestamp,
             message.quaternion_w,
             message.quaternion_x,
             message.quaternion_y,
             message.quaternion_z,
             message.acceleration_x,
             message.acceleration_y,
             message.acceleration_z);
    // println!("{message}"); // alternative to above
    println!("{}", EulerAnglesMessage::from(message));
}

pub fn earth_acceleration_closure(message: EarthAccelerationMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), " g", float_fmt!(), " g", float_fmt!(), " g"),
             message.timestamp,
             message.quaternion_w,
             message.quaternion_x,
             message.quaternion_y,
             message.quaternion_z,
             message.acceleration_x,
             message.acceleration_y,
             message.acceleration_z);
    // println!("{message}"); // alternative to above
    println!("{}", EulerAnglesMessage::from(message));
}

pub fn ahrs_status_closure(message: AhrsStatusMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), float_fmt!(), float_fmt!(), float_fmt!()),
             message.timestamp,
             message.initialising,
             message.angular_rate_recovery,
             message.acceleration_recovery,
             message.magnetic_recovery);
    // println!("{message}"); // alternative to above
}

pub fn high_g_accelerometer_closure(message: HighGAccelerometerMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), " g", float_fmt!(), " g", float_fmt!(), " g"),
             message.timestamp,
             message.x,
             message.y,
             message.z);
    // println!("{message}"); // alternative to above
}

pub fn temperature_closure(message: TemperatureMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), " degC"),
             message.timestamp,
             message.temperature);
    // println!("{message}"); // alternative to above
}

pub fn battery_closure(message: BatteryMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), " %", float_fmt!(), " V", float_fmt!(), " ({})"),
             message.timestamp,
             message.percentage,
             message.voltage,
             message.charging_status,
             ChargingStatus::from(message.charging_status));
    // println!("{message} ({})", ChargingStatus::from(message.charging_status)); // alternative to above
}

pub fn rssi_closure(message: RssiMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), float_fmt!(), " %", float_fmt!(), " dBm"),
             message.timestamp,
             message.percentage,
             message.power);
    // println!("{message}"); // alternative to above
}

pub fn serial_accessory_closure(message: SerialAccessoryMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), string_fmt!()),
             message.timestamp,
             message.char_array_as_string());
    // println!("{message}"); // alternative to above
}

pub fn notification_closure(message: NotificationMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), string_fmt!()),
             message.timestamp,
             message.char_array_as_string());
    // println!("{message}"); // alternative to above
}

pub fn error_closure(message: ErrorMessage) {
    #[rustfmt::skip]
    println!(concat!(timestamp_fmt!(), string_fmt!()),
             message.timestamp,
             message.char_array_as_string());
    // println!("{message}"); // alternative to above
}

pub fn end_of_file_closure() {
    println!("End of file");
}
