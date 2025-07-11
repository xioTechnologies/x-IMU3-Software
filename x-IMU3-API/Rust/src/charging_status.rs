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
            0 => Self::NotConnected,
            1 => Self::Charging,
            2 => Self::ChargingComplete,
            3 => Self::ChargingOnHold,
            _ => Self::NotConnected,
        }
    }
}

impl From<f32> for ChargingStatus {
    fn from(value: f32) -> Self {
        Self::from(value as i32)
    }
}

impl fmt::Display for ChargingStatus {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Self::NotConnected => write!(formatter, "Not connected"),
            Self::Charging => write!(formatter, "Charging"),
            Self::ChargingComplete => write!(formatter, "Charging complete"),
            Self::ChargingOnHold => write!(formatter, "Charging on hold"),
        }
    }
}
