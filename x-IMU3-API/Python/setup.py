import os
import platform
import shutil
import sys
from setuptools import setup, Extension

# Build
sys.argv.append("bdist_wheel")

if platform.system() == "Darwin":  # if macOS
    os.environ["LDFLAGS"] = "-framework cocoa -framework IOKit"
    libraries = ["ximu3"]
else:
    libraries = ["ximu3", "ws2_32", "userenv", "setupapi", "advapi32"]

ext_modules = Extension("ximu3",
                        ["Python-C-API/ximu3.c"],
                        library_dirs=["../../x-IMU3-API/Rust/target/release"],
                        libraries=libraries,
                        define_macros=[("_CRT_SECURE_NO_WARNINGS", "")])

github_url = "https://github.com/xioTechnologies/x-IMU3-Software"

setup(name="ximu3",
      version="0.1.0",
      author="x-io Technologies Limited",
      author_email="info@x-io.co.uk",
      url=github_url,
      description="x-IMU3 API",
      long_description="See [github](" + github_url + ") for documentation and examples.",
      long_description_content_type='text/markdown',
      license="MIT",
      ext_modules=[ext_modules])

# Upload (prerequisite: "pip install wheel" and "pip install twine")
os.system("python -m twine upload --verbose dist/* -u __token__ -p pypi-" + open("token.txt").read())

# Clean up
shutil.rmtree("build")
shutil.rmtree("dist")
shutil.rmtree("ximu3.egg-info")
