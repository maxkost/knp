"""
@file setup.py
@brief Configurator for python build.

@author Artiom N.
@license Apache 2.0 License.
@copyright © 2024 AO Kaspersky Lab
@date 28.10.2024.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import pathlib

from setuptools import setup

here = pathlib.Path(__file__).parent.resolve()

# requirements = [r for r in (here / 'requirements.txt').read_text(encoding='utf-8').split()
#                 if r and not r.lstrip().startswith('#')]

setup(
    # install_requires=requirements,
    tests_require=['pytest==7.2.2'],
    # package_data={'knp': [str(so_path) for so_path in pathlib.Path('.').glob('**/*.so')]}
    # scripts=['markdown_tool.py'],
    # entry_points={
    #     'console_scripts': [
    #         'markdown_tool = markdown_tool:main',
    #     ],
    # }
)
