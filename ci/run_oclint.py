#!/usr/bin/env python3

from pathlib import Path
import subprocess
import sys
import tarfile
import requests


def download_oclint(url: str, oclint_directory: Path):
    if oclint_directory.exists():
        return

    print(f'Downloading {url}...')
    with requests.get(url, stream=True) as response:
        with tarfile.open(mode='r:gz', fileobj=response.raw) as tf:
            tf.extractall(path=oclint_directory.parent)


def get_oclint_archive_url():
    pass


if len(sys.argv) < 3:
    print(f'{Path(__file__).name} <oclint directory> <build directory> [oclint_url]')
    sys.exit(1)

oclint_dir = Path(sys.argv[1])
build_dir = Path(sys.argv[2])

if 4 == len(sys.argv):
    download_oclint(sys.argv[3], oclint_dir)


command = [
    str(oclint_dir / 'bin' / 'oclint-json-compilation-database'),
    '-p',
    str(build_dir),
    '-e',
    'third-party',
    '--',
    f'-R={oclint_dir / "lib" / "oclint" / "rules"}',
]

sys.exit(subprocess.run(command, check=False).returncode)
