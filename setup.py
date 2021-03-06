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

if platform.system() == "Darwin":
    os.environ["LDFLAGS"] = "-framework cocoa -framework IOKit"
    libraries = ["ximu3"]
elif platform.system() == "Linux":
    libraries = ["ximu3"]
else:
    libraries = ["ximu3", "ws2_32", "userenv", "setupapi", "advapi32"]

ext_modules = Extension("ximu3",
                        ["x-IMU3-API/Python/Python-C-API/ximu3.c"],
                        library_dirs=["x-IMU3-API/Rust/target/release"],
                        libraries=libraries,
                        define_macros=[("_CRT_SECURE_NO_WARNINGS", "")])

github_url = "https://github.com/xioTechnologies/x-IMU3-Software"

setup(name="ximu3",
      version="0.6.0",
      author="x-io Technologies Limited",
      author_email="info@x-io.co.uk",
      url=github_url,
      description="x-IMU3 API",
      long_description="See [github](" + github_url + ") for documentation and examples.",
      long_description_content_type='text/markdown',
      license="MIT",
      ext_modules=[ext_modules],
      classifiers=["Programming Language :: Python :: 3.8",
                   "Programming Language :: Python :: 3.9",
                   "Programming Language :: Python :: 3.10"])  # versions shown by pyversions badge in README
