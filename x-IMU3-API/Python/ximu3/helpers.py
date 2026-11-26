import json as _json
import shutil
import sys
import time
from collections import Counter
from collections.abc import Sequence
from enum import Flag, auto
from fnmatch import fnmatch
from pathlib import Path
from typing import Any, Self

from . import _core as ximu3

_DEFAULT_TIMEOUT = 2

_LOOP_SLEEP = 0.01


class ConnectionType(Flag):
    """Connection types to match when scanning."""

    USB = auto()
    SERIAL = auto()
    TCP = auto()
    UDP = auto()
    BLUETOOTH = auto()

    ALL = USB | SERIAL | TCP | UDP | BLUETOOTH

    ALL_EXCEPT_TCP = ALL & ~TCP

    def __contains__(self, config: ximu3.ConnectionConfig) -> bool:
        match config:
            case ximu3.UsbConnectionConfig():
                return bool(self & ConnectionType.USB)
            case ximu3.SerialConnectionConfig():
                return bool(self & ConnectionType.SERIAL)
            case ximu3.TcpConnectionConfig():
                return bool(self & ConnectionType.TCP)
            case ximu3.UdpConnectionConfig():
                return bool(self & ConnectionType.UDP)
            case ximu3.BluetoothConnectionConfig():
                return bool(self & ConnectionType.BLUETOOTH)
            case _:
                return False


def scanner(
    device_name: str = "*",
    connection_type: ConnectionType = ConnectionType.ALL,
    number_of_connections: int | None = None,
    timeout: int = _DEFAULT_TIMEOUT,
) -> list[ximu3.Device]:
    """Scan for connections.

    The scan ends as soon as `number_of_connections` are found, otherwise
    after `timeout`.

    Parameters
    ----------
    device_name
        Device name pattern, matched using fnmatch. Connections that do not
        match are ignored.
    connection_type
        Connection types to match. Connections that do not match are
        ignored.
    number_of_connections
        Number of connections expected, or None for all connections found.
    timeout
        Timeout (s).

    Returns
    -------
    list[ximu3.Device]
        Connections found.
    """
    port_scanner = ximu3.PortScanner() if connection_type & (ConnectionType.USB | ConnectionType.SERIAL | ConnectionType.BLUETOOTH) else None

    network_announcement = ximu3.NetworkAnnouncement() if connection_type & (ConnectionType.TCP | ConnectionType.UDP) else None

    start = time.perf_counter()

    while True:
        devices: list[ximu3.Device] = []

        if port_scanner:
            devices += port_scanner.get_devices()

        if network_announcement:
            devices += [d for m in network_announcement.get_messages() for d in m.to_devices()]

        rejects: list[ximu3.Device] = []

        for device in devices[:]:
            if (not fnmatch(device.device_name, device_name)) or (device.connection_config not in connection_type):
                devices.remove(device)
                rejects.append(device)

        if number_of_connections and (len(devices) >= number_of_connections):
            break

        if time.perf_counter() > (start + timeout):
            break

        time.sleep(_LOOP_SLEEP)

    if not devices and not rejects:
        raise RuntimeError("No connections found")

    if not devices:
        rejects_string = "\n".join(str(r) for r in rejects)

        raise RuntimeError(f"No matches for {device_name!r} and {connection_type}:\n{rejects_string}")

    if number_of_connections and (number_of_connections != len(devices)):
        devices_string = "\n".join(str(d) for d in devices)

        raise RuntimeError(f"Found {len(devices)} connection(s) when {number_of_connections} expected:\n{devices_string}")

    return devices


class Connection(ximu3.Connection):
    """Connection that reconnects automatically if no data is received."""

    def open(self) -> Self:
        """Open the connection and keep it open.

        Returns
        -------
        Connection
            This connection.
        """
        super().open()

        self._keep_open = ximu3.KeepOpen(self)

        return self

    def close(self) -> Self:
        """Close the connection.

        Returns
        -------
        Connection
            This connection.
        """
        self._keep_open = None  # may return before ximu3.KeepOpen Drop impl completes

        return self


def connect(
    device_name: str = "*",
    connection_type: ConnectionType = ConnectionType.ALL_EXCEPT_TCP,
    timeout: int = _DEFAULT_TIMEOUT,
) -> Connection:
    """Connect to a single device.

    Exactly one matching connection must be found.

    Parameters
    ----------
    device_name
        Device name pattern, matched using fnmatch. Connections that do not
        match are ignored.
    connection_type
        Connection types to match. Connections that do not match are
        ignored.
    timeout
        Timeout (s) of each of the scan and the open.

    Returns
    -------
    Connection
        Open connection.
    """
    return _open(scanner(device_name, connection_type, 1, timeout)[0], timeout)


def connect_list(
    device_name: str = "*",
    connection_type: ConnectionType = ConnectionType.ALL_EXCEPT_TCP,
    number_of_connections: int | None = None,
    timeout: int = _DEFAULT_TIMEOUT,
) -> list[Connection]:
    """Connect to multiple devices.

    Parameters
    ----------
    device_name
        Device name pattern, matched using fnmatch. Connections that do not
        match are ignored.
    connection_type
        Connection types to match. Connections that do not match are
        ignored.
    number_of_connections
        Number of connections expected, or None for all connections found.
    timeout
        Timeout (s) of each of the scan and the open.

    Returns
    -------
    list[Connection]
        Open connections.
    """
    return [_open(d, timeout) for d in scanner(device_name, connection_type, number_of_connections, timeout)]


def connect_dict(
    device_name: str = "*",
    connection_type: ConnectionType = ConnectionType.ALL_EXCEPT_TCP,
    number_of_connections: int | None = None,
    timeout: int = _DEFAULT_TIMEOUT,
) -> dict[str, Connection]:
    """Connect to multiple devices, keyed by device name.

    Device names must be unique.

    Parameters
    ----------
    device_name
        Device name pattern, matched using fnmatch. Connections that do not
        match are ignored.
    connection_type
        Connection types to match. Connections that do not match are
        ignored.
    number_of_connections
        Number of connections expected, or None for all connections found.
    timeout
        Timeout (s) of each of the scan and the open.

    Returns
    -------
    dict[str, Connection]
        Open connections, keyed by device name.
    """
    return {d.device_name: _open(d, timeout) for d in _unique(scanner(device_name, connection_type, number_of_connections, timeout))}


def _open(device: ximu3.Device, timeout: int) -> Connection:
    start = time.perf_counter()

    while True:
        try:
            connection = Connection(device.connection_config).open()
            break
        except Exception:
            if time.perf_counter() > (start + timeout):
                raise

        time.sleep(_LOOP_SLEEP)

    if isinstance(device.connection_config, ximu3.UdpConnectionConfig):
        ping(connection)

    return connection


def _unique(devices: list[ximu3.Device]) -> list[ximu3.Device]:
    counts = Counter(d.device_name for d in devices)

    duplicates = [d for d in devices if counts[d.device_name] > 1]

    if duplicates:
        duplicates_string = "\n".join(str(d) for d in duplicates)

        raise RuntimeError(f"Duplicate device name(s):\n{duplicates_string}")

    return devices


def ping(connection: ximu3.Connection) -> ximu3.PingResponse:
    """Ping a device.

    Parameters
    ----------
    connection
        Connection to the device.

    Returns
    -------
    ximu3.PingResponse
        Ping response.
    """
    response = connection.ping()

    if not response:
        raise RuntimeError(f"No ping response for {connection.get_config()}")

    return response


def send_command(
    connection: ximu3.Connection,
    key: str | None = None,
    value: Any = None,
    json: str | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> Any:
    """Send a command.

    The command is created from `key` and `value`, unless `json` is given.

    Parameters
    ----------
    connection
        Connection to the device.
    key
        Command key.
    value
        Command value.
    json
        Command as JSON, or None to create the command from `key` and
        `value`.
    retries
        Number of retries.
    timeout
        Timeout (ms) of each attempt.

    Returns
    -------
    Any
        Response value, decoded from JSON.
    """
    command = json or f"{{{_json.dumps(key)}:{_json.dumps(value)}}}"

    response = connection.send_command(command, retries, timeout)

    if not response:
        raise RuntimeError(f"No response to {command} for {connection.get_config()}")

    if response.error:
        raise RuntimeError(f"Error response to {command} for {connection.get_config()}: {response.error}")

    return _json.loads(response.value)


class DataLogger:
    """Data logger for one or more connections.

    If `seconds` is given, logging blocks for that duration. Otherwise,
    logging is non-blocking and continues until `stop` is called.

    Parameters
    ----------
    connections
        Connection or connections to log.
    name
        Name of the log directory, or None to use the name of the main
        script.
    destination
        Directory in which to create the log directory, or None to use the
        directory of the main script.
    seconds
        Duration (s) to log for, or None to log until `stop` is called.
    overwrite
        True to delete an existing log directory first.

    Attributes
    ----------
    name
    destination
    path
    """

    def __init__(
        self,
        connections: ximu3.Connection | Sequence[ximu3.Connection],
        name: str | None = None,
        destination: Path | str | None = None,
        seconds: int | None = None,
        overwrite: bool = False,
    ) -> None:
        if isinstance(connections, ximu3.Connection):
            connections = (connections,)

        main_path = Path(sys.modules["__main__"].__file__)

        self._name = name or main_path.stem

        self._destination = Path(destination).absolute() if destination else main_path.parent

        self._path = _verify_destination(self._destination, self._name, overwrite)

        if seconds is None:
            self._wrapped = ximu3.DataLogger(str(self._destination), self._name, connections)
        else:
            ximu3.DataLogger.log(str(self._destination), self._name, connections, seconds)
            self._wrapped = None

    @property
    def name(self) -> str:
        """Name of the log directory."""
        return self._name

    @property
    def destination(self) -> Path:
        """Directory containing the log directory."""
        return self._destination

    @property
    def path(self) -> Path:
        """Path of the log directory."""
        return self._path

    def stop(self) -> None:
        """Stop logging."""
        if self._wrapped is None:
            raise RuntimeError("Data logger already stopped")

        self._wrapped = None  # may return before ximu3.DataLogger Drop impl completes

    def delete(self) -> None:
        """Delete the log directory."""
        shutil.rmtree(self._path, ignore_errors=True)


def convert(
    file_path: Path | str,
    name: str | None = None,
    destination: Path | str | None = None,
    overwrite: bool = False,
) -> Path:
    """Convert a .ximu3 file to a log directory.

    Parameters
    ----------
    file_path
        Path of the .ximu3 file.
    name
        Name of the log directory, or None to use the name of the file
        without its suffix.
    destination
        Directory in which to create the log directory, or None to use the
        directory of the file.
    overwrite
        True to delete an existing log directory first.

    Returns
    -------
    Path
        Path of the log directory.
    """
    file_path: Path = Path(file_path).absolute()

    name = name or file_path.stem

    destination = Path(destination).absolute() if destination else file_path.parent

    return convert_together([file_path], name, destination, overwrite)


def convert_together(
    file_paths: Sequence[Path | str],
    name: str,
    destination: Path | str | None = None,
    overwrite: bool = False,
) -> Path:
    """Convert multiple .ximu3 files to one log directory.

    Each file is converted as one connection, as if logged together by
    `DataLogger`.

    Parameters
    ----------
    file_paths
        Paths of the .ximu3 files.
    name
        Name of the log directory.
    destination
        Directory in which to create the log directory, or None to use the
        directory of the files, which must be the same for all files.
    overwrite
        True to delete an existing log directory first.

    Returns
    -------
    Path
        Path of the log directory.
    """
    if not file_paths:
        raise ValueError("No files provided")

    file_paths: list[Path] = [Path(f).absolute() for f in file_paths]

    for file_path in file_paths:
        if not file_path.exists():
            raise FileNotFoundError(f"File not found: {file_path}")

    if destination:
        destination = Path(destination).absolute()
    else:
        parents = {f.parent for f in file_paths}

        if len(parents) > 1:
            parents_string = "\n".join(str(p) for p in parents)

            raise ValueError(f"Conflicting destinations:\n{parents_string}")

        destination = next(iter(parents))

    path = _verify_destination(destination, name, overwrite)

    ximu3.FileConverter.convert(str(destination), name, [str(f) for f in file_paths])

    return path


def _verify_destination(destination: Path, name: str, overwrite: bool) -> Path:
    if not destination.exists():
        raise FileNotFoundError(f"Directory not found: {destination}")

    if not destination.is_dir():
        raise NotADirectoryError(f"Not a directory: {destination}")

    path: Path = destination / name

    if overwrite:
        shutil.rmtree(path, ignore_errors=True)

    if path.exists():
        raise FileExistsError(f"Directory already exists: {path}")

    return path


def mux_scanner(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> list[ximu3.Device]:
    """Scan for mux connections.

    The scan ends as soon as `number_of_channels` are found, otherwise
    after all attempts.

    Parameters
    ----------
    connection
        Connection hosting the mux connections.
    number_of_channels
        Number of channels expected, or None for all channels found.
    retries
        Number of retries.
    timeout
        Timeout (ms) of each attempt.

    Returns
    -------
    list[ximu3.Device]
        Connections found.
    """
    devices = ximu3.MuxScanner.scan(connection, number_of_channels or ximu3.MAX_NUMBER_OF_MUX_CHANNELS, retries, timeout)

    if not devices:
        raise RuntimeError("No mux connections found")

    if number_of_channels and (number_of_channels != len(devices)):
        devices_string = "\n".join(str(d) for d in devices)

        raise RuntimeError(f"Found {len(devices)} mux channel(s) when {number_of_channels} expected:\n{devices_string}")

    return devices


def mux_connect(
    connection: ximu3.Connection,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> ximu3.Connection:
    """Connect to a single device through a mux.

    Exactly one channel must be found.

    Parameters
    ----------
    connection
        Connection hosting the mux connections.
    retries
        Number of retries.
    timeout
        Timeout (ms) of each attempt.

    Returns
    -------
    ximu3.Connection
        Open connection.
    """
    return ximu3.Connection(mux_scanner(connection, 1, retries, timeout)[0].connection_config).open()


def mux_connect_list(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> list[ximu3.Connection]:
    """Connect to multiple devices through a mux.

    Parameters
    ----------
    connection
        Connection hosting the mux connections.
    number_of_channels
        Number of channels expected, or None for all channels found.
    retries
        Number of retries.
    timeout
        Timeout (ms) of each attempt.

    Returns
    -------
    list[ximu3.Connection]
        Open connections.
    """
    return [ximu3.Connection(d.connection_config).open() for d in mux_scanner(connection, number_of_channels, retries, timeout)]


def mux_connect_dict(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> dict[str, ximu3.Connection]:
    """Connect to multiple devices through a mux, keyed by device name.

    Device names must be unique.

    Parameters
    ----------
    connection
        Connection hosting the mux connections.
    number_of_channels
        Number of channels expected, or None for all channels found.
    retries
        Number of retries.
    timeout
        Timeout (ms) of each attempt.

    Returns
    -------
    dict[str, ximu3.Connection]
        Open connections, keyed by device name.
    """
    return {d.device_name: ximu3.Connection(d.connection_config).open() for d in _unique(mux_scanner(connection, number_of_channels, retries, timeout))}


def backup(
    connection: ximu3.Connection,
    file_path: Path | str | None = None,
    overwrite: bool = False,
) -> Path:
    """Back up device settings to a file.

    Parameters
    ----------
    connection
        Connection to the device.
    file_path
        Path of the backup file, or None to name the file automatically
        using the device serial number, in the directory of the main
        script.
    overwrite
        True to delete an existing backup file first.

    Returns
    -------
    Path
        Path of the backup file.
    """
    if file_path:
        file_path = Path(file_path).absolute()
    else:
        response = ping(connection)

        directory = Path(sys.modules["__main__"].__file__).parent

        file_path = directory / f"{response.device_name} ({response.model} {response.serial_number})"

    file_path = _with_json_suffix(file_path)

    if overwrite:
        file_path.unlink(missing_ok=True)

    if file_path.exists():
        raise FileExistsError(f"File already exists: {file_path}")

    ximu3.settings_backup(str(file_path), connection)

    return file_path


def restore(
    connection: ximu3.Connection,
    file_path: Path | str | None = None,
) -> Path:
    """Restore device settings from a file.

    Parameters
    ----------
    connection
        Connection to the device.
    file_path
        Path of the backup file, or None to find the file by device serial
        number, in the directory of the main script.

    Returns
    -------
    Path
        Path of the backup file.
    """
    if file_path:
        file_path = Path(file_path).absolute()
    else:
        response = ping(connection)

        directory = Path(sys.modules["__main__"].__file__).parent

        backups = [f for f in directory.glob("*.json") if response.serial_number in f.name]

        if len(backups) > 1:
            backups_string = "\n".join(f.as_posix() for f in backups)

            raise RuntimeError(f"Multiple backups found for {response.serial_number}:\n{backups_string}")

        if not backups:
            raise FileNotFoundError(f"No backups found for {response.serial_number} in {directory}")

        file_path: Path = backups[0]

    file_path = _with_json_suffix(file_path)

    if not file_path.exists():
        raise FileNotFoundError(f"File not found: {file_path}")

    ximu3.settings_restore(str(file_path), connection)

    return file_path


def _with_json_suffix(file_path: Path) -> Path:
    if file_path.suffix == ".json":
        return file_path

    return file_path.with_name(f"{file_path.name}.json")
