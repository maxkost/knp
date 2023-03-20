#!/usr/bin/env python3

import subprocess
import sys
from pathlib import Path

if len(sys.argv) < 2:
    print(f'{Path(__file__).name} <root directory>')
    sys.exit(1)

root_dir = Path(sys.argv[1])
params = sys.argv[1:]

command = ['cloc', '--exclude-dir=third-party,build,test,autodoc', '--vcs=git', *params, str(root_dir)]

result = subprocess.run(command, check=False)

if result.returncode != 0:
    sys.exit(result.returncode)
