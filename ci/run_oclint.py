#!/usr/bin/env python3
"""
@file run_oclint.py
@brief Run OCLint static analyzer.

@kaspersky_support Artiom N.
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

import platform
import subprocess
import sys
import tarfile
from pathlib import Path

import requests


def download_oclint(url: str, oclint_directory: Path) -> None:
    if oclint_directory.exists():
        return

    print(f'Downloading {url}...')
    with requests.get(url, stream=True) as response:
        with tarfile.open(mode='r:gz', fileobj=response.raw) as tf:  # type: ignore
            if hasattr(tarfile, 'tar_filter'):
                tf.extractall(path=oclint_directory, filter=tarfile.tar_filter)  # nosec B202
            else:
                tf.extractall(path=oclint_directory)  # nosec B202

    print('Download finished.')


def get_oclint_archive_url() -> str:
    oc_base_url = 'https://github.com/oclint/oclint/releases/download'
    oc_version = '22.02'
    oc_llvm_version = 'llvm-13.0.1'

    system = platform.system().lower()

    if system.startswith('linux'):
        oc_os = 'linux-ubuntu'
        os_ver = '20.04'
    elif system.startswith('darwin'):
        oc_os = 'darwin-macos'
        os_ver = '12.2-xcode-13.2'
    else:
        raise RuntimeError(f'Cannot download OCLint for the OS "{system}"')

    machine = platform.machine().lower()
    if machine in ['amd64', 'x86_64']:
        oc_arch = 'x86_64'
    elif machine in ['aarch64']:
        oc_arch = 'aarch64'
    elif machine in ['arm64']:
        oc_arch = 'arm64'
    else:
        raise RuntimeError(f'Cannot download OCLint for the machine "{machine}"')

    return f'{oc_base_url}/v{oc_version}/oclint-{oc_version}-{oc_llvm_version}-{oc_arch}-{oc_os}-{os_ver}.tar.gz'


if len(sys.argv) < 3:
    print(f'{Path(__file__).name} <oclint directory> <build directory>')
    sys.exit(1)

oclint_dir = Path(sys.argv[1])
build_dir = Path(sys.argv[2])

if not oclint_dir.exists():
    download_oclint(get_oclint_archive_url(), oclint_dir)

oclint_bin_dir = oclint_dir / 'bin'
oclint_lib_dir = oclint_dir / 'lib'

if not (oclint_bin_dir / 'oclint-json-compilation-database').exists():
    for ocl_dir in oclint_dir.glob('oclint*'):
        if (ocl_dir / 'bin' / 'oclint-json-compilation-database').exists():
            oclint_bin_dir = ocl_dir / 'bin'
            oclint_lib_dir = ocl_dir / 'lib'
            break

command = [
    str(oclint_bin_dir / 'oclint-json-compilation-database'),
    '-p',
    str(build_dir),
    '-e',
    str(build_dir),
    '-e',
    'third-party',
    '-e',
    'examples',
    '-e',
    '/usr/include/',
    '--',
    f'-R={oclint_lib_dir / "oclint" / "rules"}',
    f'-o={build_dir / "oclint.log"}',
]

print('Starting OCLint...')
sys.exit(subprocess.run(command, check=False).returncode)
