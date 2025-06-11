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

ext_modules = Extension(
    "ximu3",
    ["x-IMU3-API/Python/Python-C-API/ximu3.c"],
    library_dirs=["x-IMU3-API/Rust/target/release"],
    libraries=libraries,
    define_macros=[("_CRT_SECURE_NO_WARNINGS", "")],
)

setup(
    ext_modules=[ext_modules],
    packages=["ximu3-stubs"],
    package_dir={"ximu3-stubs": "x-IMU3-API/Python/ximu3-stubs"},
)
