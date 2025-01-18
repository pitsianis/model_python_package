import os
import subprocess
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class MesonBuild(build_ext):
    def run(self):
        # Define build directory
        build_temp = os.path.abspath(self.build_temp)
        os.makedirs(build_temp, exist_ok=True)

        # Run Meson setup
        meson_setup_cmd = [
            "meson",
            "setup",
            build_temp,
            "--buildtype=release",
            "--prefix",
            os.path.abspath(self.build_lib)
        ]
        subprocess.check_call(meson_setup_cmd)

        # Run Ninja build
        subprocess.check_call(["meson", "compile", "-C", build_temp])

        # Install to the Python package's lib directory
        subprocess.check_call(["meson", "install", "-C", build_temp])

# Define the setup configuration
setup(
    name="my_python_package",
    version="0.1.0",
    description="Python package with a C library built using Meson and FFTW",
    ext_modules=[Extension('mylib', sources=[])],
    cmdclass={"build_ext": MesonBuild},
)
