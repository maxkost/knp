#!/usr/bin/env python3

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
        with tarfile.open(mode='r:gz', fileobj=response.raw) as tf:
            if hasattr(tarfile, 'tar_filter'):
                tf.extractall(filter=tarfile.tar_filter)  # nosec B202
            else:
                tf.extractall()  # nosec B202

    print('Downloading finished.')


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
        raise RuntimeError(f'Can\'t download OCLint for the OS "{system}"')

    machine = platform.machine().lower()
    if machine in ['amd64', 'x86_64']:
        oc_arch = 'x86_64'
    elif machine in ['aarch64']:
        oc_arch = 'aarch64'
    elif machine in ['arm64']:
        oc_arch = 'arm64'
    else:
        raise RuntimeError(f'Can\'t download OCLint for the machine "{machine}"')

    return f'{oc_base_url}/v{oc_version}/oclint-{oc_version}-{oc_llvm_version}-{oc_arch}-{oc_os}-{os_ver}.tar.gz'


if len(sys.argv) < 3:
    print(f'{Path(__file__).name} <oclint directory> <build directory>')
    sys.exit(1)

oclint_dir = Path(sys.argv[1])
build_dir = Path(sys.argv[2])

if not oclint_dir.exists():
    download_oclint(get_oclint_archive_url(), oclint_dir)


command = [
    str(oclint_dir / 'bin' / 'oclint-json-compilation-database'),
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
    f'-R={oclint_dir / "lib" / "oclint" / "rules"}',
]

print('Starting OCLint...')
sys.exit(subprocess.run(command, check=False).returncode)
