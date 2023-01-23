#!/usr/bin/env python3

from pathlib import Path
import subprocess
import sys


if len(sys.argv) < 2:
    print(f'{Path(__file__).name} <root directory>')
    sys.exit(1)

root_dir = Path(sys.argv[1])
params = sys.argv[1:]

command = ['-r', str(root_dir), '--json-summary']

result = subprocess.run(command, check=False)

if result.returncode != 0:
    sys.exit(result.returncode)
