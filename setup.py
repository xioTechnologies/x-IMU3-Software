import os
import platform
import shutil
import sys
from setuptools import setup, Extension

for folder in ["build", "dist", "ximu3.egg-info"]:
    if os.path.exists(folder) and os.path.isdir(folder):
        shutil.rmtree(folder)

if len(sys.argv) == 1:  # if this script was called without arguments
    sys.argv.append("install")
    sys.argv.append("--user")

github_url = "https://github.com/xioTechnologies/x-IMU3-Software"

if platform.system() == "Darwin":
    os.environ["LDFLAGS"] = "-framework cocoa -framework IOKit"
    libraries = ["ximu3"]
elif platform.system() == "Linux":
    libraries = ["ximu3", "udev"]
else:
    libraries = ["ximu3", "ws2_32", "userenv", "setupapi", "advapi32"]

ext_modules = Extension("ximu3",
                        ["x-IMU3-API/Python/Python-C-API/ximu3.c"],
                        library_dirs=["x-IMU3-API/Rust/target/release"],
                        libraries=libraries,
                        define_macros=[("_CRT_SECURE_NO_WARNINGS", "")])

setup(name="ximu3",
      version="1.4.5",
      description="x-IMU3 API",
      long_description="See [github](" + github_url + ") for documentation and examples.",
      long_description_content_type='text/markdown',
      url=github_url,
      author="x-io Technologies Limited",
      author_email="info@x-io.co.uk",
      license="MIT",
      classifiers=["Programming Language :: Python :: 3.10",
                   "Programming Language :: Python :: 3.11",
                   "Programming Language :: Python :: 3.12"],  # versions shown by pyversions badge in README
      ext_modules=[ext_modules])
