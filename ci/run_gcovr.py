#!/usr/bin/env python3
import json
from pathlib import Path
import subprocess
import sys


if len(sys.argv) < 3:
    print(f'{Path(__file__).name} <root directory> <percent>')
    sys.exit(1)

root_dir = Path(sys.argv[1])
percent = int(sys.argv[2])
params = sys.argv[2:]

command = ['gcovr', '-r', str(root_dir), '--json-summary']

result = subprocess.run(command, check=False, stdout=subprocess.PIPE)

if result.returncode != 0:
    sys.exit(result.returncode)

report = json.loads(result.stdout)
line_percent = int(report['line_percent'])

print(
    f'Total lines count = {report["line_total"]}\n'
    f'Coverage lines percent = {line_percent}\n'
    f'Total functions count = {report["function_total"]}\n'
    f'Coverage functions percent = {report["function_covered"]}\n'
    f'Total branches count = {report["branch_total"]}\n'
    f'Coverage branches percent = {report["branch_percent"]}'
)

exit_code = percent - line_percent if line_percent < percent else 0

if exit_code:
    print(f'Warning: coverage analysis was not passed [{percent}% necessary, but only {line_percent}% covered]!')
    files = sorted(report['files'], key=lambda k: int(k['line_total']))[:10]
    print('TOP10 files without coverage:')
    for f in files:
        print(f'  {f["filename"]} ({f["line_percent"]}%)')
else:
    print('Coverage analysis passed.')

sys.exit(exit_code)
