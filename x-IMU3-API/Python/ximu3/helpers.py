import json as _json
import shutil
import sys
import time
from collections import Counter
from collections.abc import Sequence
from fnmatch import fnmatch
from pathlib import Path
from typing import Any

from . import _core as ximu3

_LOOP_SLEEP = 0.01


class Connection(ximu3.Connection):
    def open(self) -> "Connection":
        super().open()

        self._keep_open = ximu3.KeepOpen(self)

        return self

    def close(self) -> None:
        self._keep_open = None  # TODO: this line may return before ximu3.KeepOpen Drop impl


# TODO: connect (calls connect_list with number_of_connections=1)
# TODO: connect_list
# TODO: connect_dict


def connect(
    name: str = "*",
    keep_open: bool = True,  # TODO
    timeout: int = 2,
) -> Connection:
    port_scanner = ximu3.PortScanner()

    # TODO: add ximu3.NetworkAnnouncement to support network connection

    rejected_names: set[str] = set()

    start = time.perf_counter()

    while True:
        config = _match(name, port_scanner.get_devices(), rejected_names)

        if config:
            return _open(config, timeout)

        if time.perf_counter() > (start + timeout):
            break

        time.sleep(_LOOP_SLEEP)

    if rejected_names:
        error = f"{repr(name)} does not match {rejected_names}"
    else:
        error = "Please check hardware"

    raise RuntimeError(f"No connections found after {timeout} second(s). {error}.")


def _match(
    name: str,
    devices: list[ximu3.Device],
    rejected_names: set[str],
) -> ximu3.ConnectionConfig | None:
    for device in devices:
        if fnmatch(device.device_name, name):
            return device.connection_config
        else:
            rejected_names.add(device.device_name)

    return None


def _open(
    config: ximu3.ConnectionConfig,
    timeout: int,
) -> Connection:
    start = time.perf_counter()

    while True:
        try:
            return Connection(config).open()
        except Exception:
            if time.perf_counter() > (start + timeout):
                raise

        time.sleep(_LOOP_SLEEP)


def send_command(
    connection: ximu3.Connection,
    key: str | None = None,
    value: Any = None,
    json: str | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> str | None:
    command = json or f"{{{_json.dumps(key)}:{_json.dumps(value)}}}"

    response = connection.send_command(command, retries, timeout)

    if not response:
        raise RuntimeError(f"{command} failed for {connection.get_config()}. No response.")

    if response.error:
        raise RuntimeError(f"{command} failed for {connection.get_config()}. {response.error}.")

    return _json.loads(response.value)


class DataLogger:
    def __init__(
        self,
        connections: ximu3.Connection | Sequence[ximu3.Connection],
        name: str | None = None,
        destination: str | None = None,
        seconds: int | None = None,
        overwrite: bool = False,
    ):
        if isinstance(connections, ximu3.Connection):
            connections = (connections,)

        main_path = Path(sys.modules["__main__"].__file__)

        self._name = name or main_path.stem

        self._destination = Path(destination).absolute() if destination else main_path.parent

        if not self._destination.exists():
            raise FileNotFoundError(f"Destination does not exist. {self._destination}")

        if not self._destination.is_dir():
            raise NotADirectoryError(f"Destination is not a directory. {self._destination}")

        self._path = self._destination / self._name

        if overwrite:
            self.delete()

        if self._path.exists():
            raise FileExistsError(f"Directory already exists. {self._path}")

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

        self._wrapped = None  # TODO: this line may return before ximu3.DataLogger Drop impl

    def delete(self) -> None:
        shutil.rmtree(self._path, ignore_errors=True)


def mux_scanner(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> list[ximu3.Device]:
    devices = ximu3.MuxScanner.scan(connection, number_of_channels or ximu3.MAX_NUMBER_OF_MUX_CHANNELS, retries, timeout)

    if not devices:
        raise RuntimeError("No mux connections found")

    if number_of_channels and (number_of_channels != len(devices)):
        raise RuntimeError(f"Found {len(devices)} mux channel(s) when {number_of_channels} expected")

    return devices


# TODO: mux_connect (calls connect_list with number_of_channels=1)
# TODO: mux_connect_list
# TODO: mux_connect_dict


def mux_connect(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> list[ximu3.Connection]:
    return [ximu3.Connection(d.connection_config).open() for d in mux_scanner(connection, number_of_channels, retries, timeout)]


def mux_connect_dict(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> dict[str, ximu3.Connection]:
    devices = mux_scanner(connection, number_of_channels, retries, timeout)

    duplicates = [name for name, count in Counter(d.device_name for d in devices).items() if count > 1]

    if duplicates:
        raise RuntimeError(f"Duplicate device name(s) {duplicates}")

    return {d.device_name: ximu3.Connection(d.connection_config).open() for d in devices}
