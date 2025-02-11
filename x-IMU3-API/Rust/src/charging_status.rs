use std::fmt;

#[repr(C)]
#[derive(Clone, Copy)]
pub enum ChargingStatus {
    NotConnected,
    Charging,
    ChargingComplete,
    ChargingOnHold,
}

impl From<i32> for ChargingStatus {
    fn from(value: i32) -> Self {
        match value {
            0 => ChargingStatus::NotConnected,
            1 => ChargingStatus::Charging,
            2 => ChargingStatus::ChargingComplete,
            3 => ChargingStatus::ChargingOnHold,
            _ => ChargingStatus::NotConnected,
        }
    }
}

impl From<f32> for ChargingStatus {
    fn from(value: f32) -> Self {
        ChargingStatus::from(value as i32)
    }
}

impl fmt::Display for ChargingStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            ChargingStatus::NotConnected => write!(formatter, "Not connected"),
            ChargingStatus::Charging => write!(formatter, "Charging"),
            ChargingStatus::ChargingComplete => write!(formatter, "Charging complete"),
            ChargingStatus::ChargingOnHold => write!(formatter, "Charging on hold"),
        }
    }
}
