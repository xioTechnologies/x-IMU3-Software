import os
import platform

from setuptools import Extension, setup

if platform.system() == "Darwin":
    os.environ["LDFLAGS"] = "-framework cocoa -framework IOKit"
    libraries = ["ximu3"]
elif platform.system() == "Linux":
    libraries = ["ximu3", "udev"]
else:
    libraries = ["ximu3", "ws2_32", "userenv", "setupapi", "advapi32"]

setup(
    ext_modules=[
        Extension(
            "ximu3._core",
            ["x-IMU3-API/Python/ximu3/src/_core.c"],
            library_dirs=["x-IMU3-API/Rust/target/release"],
            libraries=libraries,
            define_macros=[("_CRT_SECURE_NO_WARNINGS", "")],
        )
    ],
)
