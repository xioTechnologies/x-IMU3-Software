use std::fmt;

#[repr(C)]
#[derive(Clone, Copy, PartialEq)]
pub enum ChargingStatus {
    NotConnected,
    Charging,
    ChargingComplete,
}

impl From<i32> for ChargingStatus {
    fn from(value: i32) -> Self {
        match value {
            0 => ChargingStatus::NotConnected,
            1 => ChargingStatus::Charging,
            2 => ChargingStatus::ChargingComplete,
            _ => ChargingStatus::NotConnected,
        }
    }
}

impl fmt::Display for ChargingStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            ChargingStatus::NotConnected => write!(formatter, "Not connected"),
            ChargingStatus::Charging => write!(formatter, "Charging"),
            ChargingStatus::ChargingComplete => write!(formatter, "Charging complete"),
        }
    }
}
