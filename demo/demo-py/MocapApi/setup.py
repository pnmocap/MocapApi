from setuptools import setup, find_packages
from mocap_api.mocap_api import __version__

packages = find_packages()

package_data = {
    'mocap_api': [
        'lib/amd64/*.dll',
        'lib/amd64/*.lib',
        'lib/arm64/*.so',
        'lib/x86_64/*.so',
        'include/MocapApi/*.h',
    ]
}

setup(
    name="mocap_api",
    version=__version__,
    packages=packages,
    package_data=package_data,
    include_package_data=True,
    description='mocap_api with dynamic library support'
)