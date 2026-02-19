import json as _json
import shutil
import sys
import time
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
) -> ximu3.Connection | tuple[ximu3.Connection, ximu3.KeepOpen]:
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


_keep_open_status = None


def _keep_open_callback(status: int) -> None:
    global _keep_open_status
    _keep_open_status = status


def _keep_open(
    config: ximu3.ConnectionConfig,
    timeout: int,
) -> tuple[ximu3.Connection, ximu3.KeepOpen]:
    connection = ximu3.Connection(config)

    keep_open = ximu3.KeepOpen(connection, _keep_open_callback)

    start = time.perf_counter()

    while True:
        global _keep_open_status

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
) -> str | None:
    match value:
        case None:
            value = "null"
        case bool():
            value = str(value).lower()
        case str():
            value = f'"{value}"'
        case _:
            value = str(value)

    command = json or f'{{"{key}":{value}}}'

    response = connection.send_command(command, 2, 500)

    if not response:
        raise RuntimeError(f"{command} failed for {connection.get_config()}. No response.")

    if response.error:
        raise RuntimeError(f"{command} failed for {connection.get_config()}. {response.error}.")

    return _json.loads(response.value)


class DataLogger:
    def __init__(
        self,
        connections: Sequence[ximu3.Connection],
        name: str | None = None,
        destination: str | None = None,
        seconds: int | None = None,
        overwrite: bool = False,
    ):
        main_path = Path(sys.modules["__main__"].__file__)

        name = name or main_path.stem

        destination = destination or str(main_path.parent)

        self.__path = Path(destination) / name

        if overwrite:
            self.delete()

        if self.__path.exists():
            raise FileExistsError(f"Directory already exists. {self.__path}")

        if seconds is None:
            self.__wrapped = ximu3.DataLogger(destination, name, connections)
        else:
            ximu3.DataLogger.log(destination, name, connections, seconds)
            self.__wrapped = None

    def stop(self):
        if self.__wrapped is None:
            raise RuntimeError("Data logger already stopped")

        self.__wrapped = None

    def delete(self):
        shutil.rmtree(self.__path, ignore_errors=True)

    def __str__(self):
        return str(self.__path)
