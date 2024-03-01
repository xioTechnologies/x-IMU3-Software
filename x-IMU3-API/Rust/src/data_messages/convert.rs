use crate::data_messages::*;

// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
impl From<RotationMatrixMessage> for QuaternionMessage {
    fn from(message: RotationMatrixMessage) -> Self {
        let xx = message.xx;
        let xy = message.xy;
        let xz = message.xz;
        let yx = message.yx;
        let yy = message.yy;
        let yz = message.yz;
        let zx = message.zx;
        let zy = message.zy;
        let zz = message.zz;

        let tr = xx + yy + zz;
        if tr > 0.0 {
            let s = f32::sqrt(tr + 1.0) * 2.0; // s=4*w
            QuaternionMessage {
                timestamp: message.timestamp,
                w: 0.25 * s,
                x: (zy - yz) / s,
                y: (xz - zx) / s,
                z: (yx - xy) / s,
            }
        } else if (xx > yy) && (xx > zz) {
            let s = f32::sqrt(1.0 + xx - yy - zz) * 2.0; // s=4*x
            QuaternionMessage {
                timestamp: message.timestamp,
                w: (zy - yz) / s,
                x: 0.25 * s,
                y: (xy + yx) / s,
                z: (xz + zx) / s,
            }
        } else if yy > zz {
            let s = f32::sqrt(1.0 + yy - xx - zz) * 2.0; // s=4*y
            QuaternionMessage {
                timestamp: message.timestamp,
                w: (xz - zx) / s,
                x: (xy + yx) / s,
                y: 0.25 * s,
                z: (yz + zy) / s,
            }
        } else {
            let s = f32::sqrt(1.0 + zz - xx - yy) * 2.0; // s=4*z
            QuaternionMessage {
                timestamp: message.timestamp,
                w: (yx - xy) / s,
                x: (xz + zx) / s,
                y: (yz + zy) / s,
                z: 0.25 * s,
            }
        }
    }
}

// Quaternions and Rotation Sequence by Jack B. Kuipers, ISBN 0-691-10298-8, Page 167
impl From<EulerAnglesMessage> for QuaternionMessage {
    fn from(message: EulerAnglesMessage) -> Self {
        let psi = f32::to_radians(message.yaw);
        let theta = f32::to_radians(message.pitch);
        let phi = f32::to_radians(message.roll);

        let cos_half_psi = f32::cos(psi * 0.5);
        let sin_half_psi = f32::sin(psi * 0.5);

        let cos_half_theta = f32::cos(theta * 0.5);
        let sin_half_theta = f32::sin(theta * 0.5);

        let cos_half_phi = f32::cos(phi * 0.5);
        let sin_half_phi = f32::sin(phi * 0.5);

        QuaternionMessage {
            timestamp: message.timestamp,
            w: cos_half_psi * cos_half_theta * cos_half_phi + sin_half_psi * sin_half_theta * sin_half_phi,
            x: cos_half_psi * cos_half_theta * sin_half_phi - sin_half_psi * sin_half_theta * cos_half_phi,
            y: cos_half_psi * sin_half_theta * cos_half_phi + sin_half_psi * cos_half_theta * sin_half_phi,
            z: sin_half_psi * cos_half_theta * cos_half_phi - cos_half_psi * sin_half_theta * sin_half_phi,
        }
    }
}

// Quaternions and Rotation Sequence by Jack B. Kuipers, ISBN 0-691-10298-8, Page 168
impl From<QuaternionMessage> for EulerAnglesMessage {
    fn from(message: QuaternionMessage) -> Self {
        let w = message.w;
        let x = message.x;
        let y = message.y;
        let z = message.z;

        EulerAnglesMessage {
            timestamp: message.timestamp,
            roll: f32::to_degrees(f32::atan2(2.0 * (y * z + w * x), 2.0 * (w * w - 0.5 + z * z))),
            pitch: f32::to_degrees(-1.0 * f32::asin(f32::clamp(2.0 * (x * z - w * y), -1.0, 1.0))),
            yaw: f32::to_degrees(f32::atan2(2.0 * (x * y + w * z), 2.0 * (w * w - 0.5 + x * x))),
        }
    }
}

impl From<RotationMatrixMessage> for EulerAnglesMessage {
    fn from(message: RotationMatrixMessage) -> Self {
        QuaternionMessage::from(message).into()
    }
}

impl From<LinearAccelerationMessage> for EulerAnglesMessage {
    fn from(message: LinearAccelerationMessage) -> Self {
        QuaternionMessage {
            timestamp: message.timestamp,
            w: message.quaternion_w,
            x: message.quaternion_x,
            y: message.quaternion_y,
            z: message.quaternion_z,
        }.into()
    }
}

impl From<EarthAccelerationMessage> for EulerAnglesMessage {
    fn from(message: EarthAccelerationMessage) -> Self {
        QuaternionMessage {
            timestamp: message.timestamp,
            w: message.quaternion_w,
            x: message.quaternion_x,
            y: message.quaternion_y,
            z: message.quaternion_z,
        }.into()
    }
}
