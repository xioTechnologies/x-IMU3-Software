use std::io::ErrorKind::*;
use std::os::raw::c_char;

#[repr(C)]
pub enum Result {
    Ok,
    AddrInUse,
    AddrNotAvailable,
    AlreadyExists,
    ArgumentListTooLong,
    BrokenPipe,
    ConnectionAborted,
    ConnectionRefused,
    ConnectionReset,
    Deadlock,
    DirectoryNotEmpty,
    ExecutableFileBusy,
    FileTooLarge,
    HostUnreachable,
    Interrupted,
    InvalidData,
    InvalidInput,
    IsADirectory,
    NetworkDown,
    NetworkUnreachable,
    NotADirectory,
    NotConnected,
    NotFound,
    NotSeekable,
    Other,
    OutOfMemory,
    PermissionDenied,
    ReadOnlyFilesystem,
    ResourceBusy,
    StaleNetworkFileHandle,
    StorageFull,
    TimedOut,
    TooManyLinks,
    UnexpectedEof,
    Unsupported,
    WouldBlock,
    WriteZero,
    UnknownError,
} // duplication of std::io::ErrorKind with the addition of Ok and UnknownError

impl From<&std::io::Error> for Result {
    fn from(error: &std::io::Error) -> Self {
        match error.kind() {
            AddrInUse => Result::AddrInUse,
            AddrNotAvailable => Result::AddrNotAvailable,
            AlreadyExists => Result::AlreadyExists,
            ArgumentListTooLong => Result::ArgumentListTooLong,
            BrokenPipe => Result::BrokenPipe,
            ConnectionAborted => Result::ConnectionAborted,
            ConnectionRefused => Result::ConnectionRefused,
            ConnectionReset => Result::ConnectionReset,
            Deadlock => Result::Deadlock,
            DirectoryNotEmpty => Result::DirectoryNotEmpty,
            ExecutableFileBusy => Result::ExecutableFileBusy,
            FileTooLarge => Result::FileTooLarge,
            HostUnreachable => Result::HostUnreachable,
            Interrupted => Result::Interrupted,
            InvalidData => Result::InvalidData,
            InvalidInput => Result::InvalidInput,
            IsADirectory => Result::IsADirectory,
            NetworkDown => Result::NetworkDown,
            NetworkUnreachable => Result::NetworkUnreachable,
            NotADirectory => Result::NotADirectory,
            NotConnected => Result::NotConnected,
            NotFound => Result::NotFound,
            NotSeekable => Result::NotSeekable,
            Other => Result::Other,
            OutOfMemory => Result::OutOfMemory,
            PermissionDenied => Result::PermissionDenied,
            ReadOnlyFilesystem => Result::ReadOnlyFilesystem,
            ResourceBusy => Result::ResourceBusy,
            StaleNetworkFileHandle => Result::StaleNetworkFileHandle,
            StorageFull => Result::StorageFull,
            TimedOut => Result::TimedOut,
            TooManyLinks => Result::TooManyLinks,
            UnexpectedEof => Result::UnexpectedEof,
            Unsupported => Result::Unsupported,
            WouldBlock => Result::WouldBlock,
            WriteZero => Result::WriteZero,
            _ => Result::UnknownError,
        }
    }
}

impl<T> From<&std::io::Result<T>> for Result {
    fn from(result: &std::io::Result<T>) -> Self {
        match result {
            Ok(_) => Result::Ok,
            Err(error) => error.into(),
        }
    }
}

#[no_mangle]
pub extern "C" fn XIMU3_result_to_string(result: Result) -> *const c_char {
    macro_rules! str_as_char_ptr {
        ($lit:expr) => {{
            static CHAR_ARRAY: &[u8] = concat!($lit, "\0").as_bytes();
            CHAR_ARRAY.as_ptr() as *const c_char
        }};
    }

    match result {
        Result::Ok => str_as_char_ptr!("OK"),
        Result::AddrInUse => str_as_char_ptr!("Address in use"),
        Result::AddrNotAvailable => str_as_char_ptr!("Address not available"),
        Result::AlreadyExists => str_as_char_ptr!("Entity already exists"),
        Result::ArgumentListTooLong => str_as_char_ptr!("Argument list too long"),
        Result::BrokenPipe => str_as_char_ptr!("Broken pipe"),
        Result::ConnectionAborted => str_as_char_ptr!("Connection aborted"),
        Result::ConnectionRefused => str_as_char_ptr!("Connection refused"),
        Result::ConnectionReset => str_as_char_ptr!("Connection reset"),
        Result::Deadlock => str_as_char_ptr!("Deadlock"),
        Result::DirectoryNotEmpty => str_as_char_ptr!("Directory not empty"),
        Result::ExecutableFileBusy => str_as_char_ptr!("Executable file busy"),
        Result::FileTooLarge => str_as_char_ptr!("File too large"),
        Result::HostUnreachable => str_as_char_ptr!("Host unreachable"),
        Result::Interrupted => str_as_char_ptr!("Operation interrupted"),
        Result::InvalidData => str_as_char_ptr!("Invalid data"),
        Result::InvalidInput => str_as_char_ptr!("Invalid input parameter"),
        Result::IsADirectory => str_as_char_ptr!("Is a directory"),
        Result::NetworkDown => str_as_char_ptr!("Network down"),
        Result::NetworkUnreachable => str_as_char_ptr!("Network unreachable"),
        Result::NotADirectory => str_as_char_ptr!("Not a directory"),
        Result::NotConnected => str_as_char_ptr!("Not connected"),
        Result::NotFound => str_as_char_ptr!("Entity not found"),
        Result::NotSeekable => str_as_char_ptr!("Seek on unseekable file"),
        Result::Other => str_as_char_ptr!("Other error"),
        Result::OutOfMemory => str_as_char_ptr!("Out of memory"),
        Result::PermissionDenied => str_as_char_ptr!("Permission denied"),
        Result::ReadOnlyFilesystem => str_as_char_ptr!("Read-only filesystem or storage medium"),
        Result::ResourceBusy => str_as_char_ptr!("Resource busy"),
        Result::StaleNetworkFileHandle => str_as_char_ptr!("Stale network file handle"),
        Result::StorageFull => str_as_char_ptr!("No storage space"),
        Result::TimedOut => str_as_char_ptr!("Timed out"),
        Result::TooManyLinks => str_as_char_ptr!("Too many links"),
        Result::UnexpectedEof => str_as_char_ptr!("Unexpected end of file"),
        Result::Unsupported => str_as_char_ptr!("Unsupported"),
        Result::WouldBlock => str_as_char_ptr!("Operation would block"),
        Result::WriteZero => str_as_char_ptr!("Write zero"),
        Result::UnknownError => str_as_char_ptr!("Unknown error"),
    }
}
