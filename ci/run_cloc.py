#!/usr/bin/env python3
"""
@file run_cloc.py
@brief Run code lines counter.

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
