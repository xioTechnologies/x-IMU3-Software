import json as _json
import shutil
import sys
import time
from collections import Counter
from collections.abc import Sequence
from enum import Flag, auto
from fnmatch import fnmatch
from pathlib import Path
from typing import Any

from . import _core as ximu3

_DEFAULT_TIMEOUT = 2

_LOOP_SLEEP = 0.01


class ConnectionType(Flag):
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
    device_name: str = "*",  # device names that do not match (using fnmatch) will be ignored
    connection_type: ConnectionType = ConnectionType.ALL,  # connection types that do not match will be ignored
    number_of_connections: int | None = None,  # scan will end as soon as number_of_connections is found, else scan will end after timeout
    timeout: int = _DEFAULT_TIMEOUT,
) -> list[ximu3.Device]:
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
    def open(self) -> "Connection":
        super().open()

        self._keep_open = ximu3.KeepOpen(self)

        return self

    def close(self) -> "Connection":
        self._keep_open = None  # may return before ximu3.KeepOpen Drop impl completes

        return self


def connect(
    device_name: str = "*",  # device names that do not match (using fnmatch) will be ignored
    connection_type: ConnectionType = ConnectionType.ALL_EXCEPT_TCP,  # connection types that do not match will be ignored
    timeout: int = _DEFAULT_TIMEOUT,  # timeout for scan and open
) -> Connection:
    return _open(scanner(device_name, connection_type, 1, timeout)[0], timeout)


def connect_list(
    device_name: str = "*",  # device names that do not match (using fnmatch) will be ignored
    connection_type: ConnectionType = ConnectionType.ALL_EXCEPT_TCP,  # connection types that do not match will be ignored
    number_of_connections: int | None = None,  # None to open all matching connections, else number_of_connections will be enforced
    timeout: int = _DEFAULT_TIMEOUT,  # timeout for scan and open
) -> list[Connection]:
    return [_open(d, timeout) for d in scanner(device_name, connection_type, number_of_connections, timeout)]


def connect_dict(
    device_name: str = "*",  # device names that do not match (using fnmatch) will be ignored
    connection_type: ConnectionType = ConnectionType.ALL_EXCEPT_TCP,  # connection types that do not match will be ignored
    number_of_connections: int | None = None,  # None to open all matching connections, else number_of_connections will be enforced
    timeout: int = _DEFAULT_TIMEOUT,  # timeout for scan and open
) -> dict[str, Connection]:
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
    response = connection.ping()

    if not response:
        raise RuntimeError(f"No ping response for {connection.get_config()}")

    return response


def send_command(
    connection: ximu3.Connection,
    key: str | None = None,
    value: Any = None,
    json: str | None = None,  # None to create from key and value, else key and value will be ignored
    retries: int = ximu3.DEFAULT_RETRIES,  # ximu3.Connection.send_command parameter
    timeout: int = ximu3.DEFAULT_TIMEOUT,  # ximu3.Connection.send_command parameter
) -> Any:
    command = json or f"{{{_json.dumps(key)}:{_json.dumps(value)}}}"

    response = connection.send_command(command, retries, timeout)

    if not response:
        raise RuntimeError(f"No response to {command} for {connection.get_config()}")

    if response.error:
        raise RuntimeError(f"Error response to {command} for {connection.get_config()}: {response.error}")

    return _json.loads(response.value)


class DataLogger:
    def __init__(
        self,
        connections: ximu3.Connection | Sequence[ximu3.Connection],
        name: str | None = None,  # None to use main script name
        destination: Path | str | None = None,  # None to use main script directory
        seconds: int | None = None,  # None for non-blocking, else blocks until logging completes
        overwrite: bool = False,  # True to delete destination/name directory first
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
        return self._name

    @property
    def destination(self) -> Path:
        return self._destination

    @property
    def path(self) -> Path:
        return self._path

    def stop(self) -> None:
        if self._wrapped is None:
            raise RuntimeError("Data logger already stopped")

        self._wrapped = None  # may return before ximu3.DataLogger Drop impl completes

    def delete(self) -> None:
        shutil.rmtree(self._path, ignore_errors=True)


def convert(
    file_path: Path | str,
    name: str | None = None,  # None to use file_path name
    destination: Path | str | None = None,  # None to use file_path directory
    overwrite: bool = False,  # True to delete existing destination/name directory
) -> Path:
    file_path: Path = Path(file_path).absolute()

    name = name or file_path.stem

    destination = Path(destination).absolute() if destination else file_path.parent

    return convert_together([file_path], name, destination, overwrite)


def convert_together(
    file_paths: Sequence[Path | str],
    name: str,
    destination: Path | str | None = None,  # None to use file_paths directory
    overwrite: bool = False,  # True to delete existing destination/name directory
) -> Path:
    if not file_paths:
        raise ValueError("No files provided")

    file_paths: list[Path] = [Path(f).absolute() for f in file_paths]

    for file_path in file_paths:
        if not file_path.exists():
            raise FileNotFoundError(f"File does not exist: {file_path}")

    if destination:
        destination = Path(destination).absolute()
    else:
        parents = {f.parent for f in file_paths}

        if len(parents) > 1:
            parents_string = "\n".join(str(p) for p in parents)

            raise ValueError(f"Conflicting destinations:\n{parents_string}")

        destination = next(iter(parents))

    path = _verify_destination(destination, name, overwrite)

    progress = ximu3.FileConverter.convert(str(destination), name, [str(f) for f in file_paths])

    if progress.status != ximu3.FILE_CONVERTER_STATUS_COMPLETE:
        raise RuntimeError(f"Unexpected file converter status: {ximu3.file_converter_status_to_string(progress.status)}")

    return path


def _verify_destination(destination: Path, name: str, overwrite: bool) -> Path:
    if not destination.exists():
        raise FileNotFoundError(f"Destination does not exist: {destination}")

    if not destination.is_dir():
        raise NotADirectoryError(f"Destination is not a directory: {destination}")

    path: Path = destination / name

    if overwrite:
        shutil.rmtree(path, ignore_errors=True)

    if path.exists():
        raise FileExistsError(f"Directory already exists: {path}")

    return path


def mux_scanner(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,  # scan will end as soon as number_of_channels is found, else scan will end after timeout
    retries: int = ximu3.DEFAULT_RETRIES,  # ximu3.MuxScanner.scan parameter
    timeout: int = ximu3.DEFAULT_TIMEOUT,  # ximu3.MuxScanner.scan parameter
) -> list[ximu3.Device]:
    devices = ximu3.MuxScanner.scan(connection, number_of_channels or ximu3.MAX_NUMBER_OF_MUX_CHANNELS, retries, timeout)

    if not devices:
        raise RuntimeError("No mux connections found")

    if number_of_channels and (number_of_channels != len(devices)):
        devices_string = "\n".join(str(d) for d in devices)

        raise RuntimeError(f"Found {len(devices)} mux channel(s) when {number_of_channels} expected:\n{devices_string}")

    return devices


def mux_connect(
    connection: ximu3.Connection,
    retries: int = ximu3.DEFAULT_RETRIES,  # ximu3.MuxScanner.scan parameter
    timeout: int = ximu3.DEFAULT_TIMEOUT,  # ximu3.MuxScanner.scan parameter
) -> ximu3.Connection:
    return ximu3.Connection(mux_scanner(connection, 1, retries, timeout)[0].connection_config).open()


def mux_connect_list(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,  # None to open all channels, else number_of_channels will be enforced
    retries: int = ximu3.DEFAULT_RETRIES,  # ximu3.MuxScanner.scan parameter
    timeout: int = ximu3.DEFAULT_TIMEOUT,  # ximu3.MuxScanner.scan parameter
) -> list[ximu3.Connection]:
    return [ximu3.Connection(d.connection_config).open() for d in mux_scanner(connection, number_of_channels, retries, timeout)]


def mux_connect_dict(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,  # None to open all channels, else number_of_channels will be enforced
    retries: int = ximu3.DEFAULT_RETRIES,  # ximu3.MuxScanner.scan parameter
    timeout: int = ximu3.DEFAULT_TIMEOUT,  # ximu3.MuxScanner.scan parameter
) -> dict[str, ximu3.Connection]:
    return {d.device_name: ximu3.Connection(d.connection_config).open() for d in _unique(mux_scanner(connection, number_of_channels, retries, timeout))}


def backup(
    connection: ximu3.Connection,
    file_path: Path | str | None = None,  # None to backup to file in main script directory, with file name containing device serial number
    overwrite: bool = False,  # True to delete file_path first
) -> Path:
    if file_path:
        file_path = Path(file_path).absolute()
    else:
        response = ping(connection)

        directory = Path(sys.modules["__main__"].__file__).parent

        file_path = directory / f"{response.device_name} {response.serial_number}"

    file_path: Path = file_path.with_suffix(".json")

    if overwrite:
        file_path.unlink(missing_ok=True)

    if file_path.exists():
        raise FileExistsError(f"File already exists: {file_path}")

    ximu3.settings_backup(str(file_path), connection)

    return file_path


def restore(
    connection: ximu3.Connection,
    file_path: Path | str | None = None,  # None to restore from file in main script directory, with file name containing device serial number
) -> Path:
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

    file_path = file_path.with_suffix(".json")

    if not file_path.exists():
        raise FileNotFoundError(f"File does not exist: {file_path}")

    ximu3.settings_restore(str(file_path), connection)

    return file_path
