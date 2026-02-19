import json as _json
import shutil
import sys
import time
from collections import Counter
from collections.abc import Sequence
from fnmatch import fnmatch
from pathlib import Path
from typing import Any

import ximu3

_LOOP_SLEEP = 0.01


def quick_connect(
    name: str = "*",
    keep_open: bool = False,
    timeout: int = 2,
) -> ximu3.Connection | tuple[ximu3.Connection, ximu3.KeepOpen]:  # del ximu3.KeepOpen to close connection
    port_scanner = ximu3.PortScanner(lambda _: None)

    rejected_names: set[str] = set()

    start = time.perf_counter()

    while True:
        config = _match(name, port_scanner.get_devices(), rejected_names)

        if config:
            if keep_open:
                return _keep_open(config, timeout)
            else:
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


def _open(
    config: ximu3.ConnectionConfig,
    timeout: int,
) -> ximu3.Connection:
    start = time.perf_counter()

    while True:
        try:
            return ximu3.Connection(config).open()
        except Exception:
            if time.perf_counter() > (start + timeout):
                raise

        time.sleep(_LOOP_SLEEP)


def _keep_open(
    config: ximu3.ConnectionConfig,
    timeout: int,
) -> tuple[ximu3.Connection, ximu3.KeepOpen]:
    connection = ximu3.Connection(config)

    _keep_open_status: int | None = None

    def callback(status: int) -> None:
        nonlocal _keep_open_status
        _keep_open_status = status

    keep_open = ximu3.KeepOpen(connection, callback)

    start = time.perf_counter()

    while True:
        if (_keep_open_status is not None) and (_keep_open_status == ximu3.CONNECTION_STATUS_CONNECTED):
            return connection, keep_open

        if time.perf_counter() > (start + timeout):
            raise RuntimeError(f"Unable to open {connection.get_config()} after {timeout} second(s)")

        time.sleep(_LOOP_SLEEP)


def send_command(
    connection: ximu3.Connection,
    key: str | None = None,
    value: Any = None,
    json: str | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> str | None:
    command = json or f'{{"{key}":{_json.dumps(value)}}}'

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

        self.__name = name or main_path.stem

        self.__destination = Path(destination).absolute() if destination else main_path.parent

        if not self.__destination.exists():
            raise FileNotFoundError(f"Destination does not exist. {self.__destination}")

        if not self.__destination.is_dir():
            raise NotADirectoryError(f"Destination is not a directory. {self.__destination}")

        self.__path = self.__destination / self.__name

        if overwrite:
            self.delete()

        if self.__path.exists():
            raise FileExistsError(f"Directory already exists. {self.__path}")

        if seconds is None:
            self.__wrapped = ximu3.DataLogger(str(self.__destination), self.__name, connections)
        else:
            ximu3.DataLogger.log(str(self.__destination), self.__name, connections, seconds)
            self.__wrapped = None

    @property
    def name(self) -> str:
        return self.__name

    @property
    def destination(self) -> Path:
        return self.__destination

    @property
    def path(self) -> Path:
        return self.__path

    def stop(self) -> None:
        if self.__wrapped is None:
            raise RuntimeError("Data logger already stopped")

        self.__wrapped = None

    def delete(self) -> None:
        shutil.rmtree(self.__path, ignore_errors=True)


def mux_scanner(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
) -> list[ximu3.Device]:
    devices = ximu3.MuxScanner.scan(connection, number_of_channels or ximu3.MAX_NUMBER_OF_MUX_CHANNELS, retries, timeout)

    if number_of_channels and (number_of_channels != len(devices)):
        raise RuntimeError(f"Found {len(devices)} mux channel(s) when {number_of_channels} expected")

    return devices


def mux_connect(
    connection: ximu3.Connection,
    number_of_channels: int | None = None,
    retries: int = ximu3.DEFAULT_RETRIES,
    timeout: int = ximu3.DEFAULT_TIMEOUT,
    dictionary: bool = False,
) -> list[ximu3.Connection] | dict[str, ximu3.Connection]:
    devices = mux_scanner(connection, number_of_channels, retries, timeout)

    if not devices:
        raise RuntimeError("No mux connections found")

    if not dictionary:
        return [ximu3.Connection(d.connection_config).open() for d in devices]

    device_names = [d.device_name for d in devices]

    duplicates = [name for name, count in Counter(device_names).items() if count > 1]

    if duplicates:
        raise RuntimeError(f"Dupliate device name(s) {duplicates}")

    return {d.device_name: ximu3.Connection(d.connection_config).open() for d in devices}
