#!/usr/bin/env python3

"""PyLint CI runner."""

import io
import os
import re
import sys
from pathlib import Path
from typing import Dict, List, NoReturn, Tuple, Union

from pylint.lint.run import Run as PylintRun
from pylint.reporters.text import ParseableTextReporter


class TeeIO(io.StringIO):
    """Combined output: console and file."""

    def __init__(self, filepath: Path):
        super().__init__()
        self._filepath = filepath
        self._file = None

    def __enter__(self) -> 'TeeIO':
        self._file = open(self._filepath, 'w', encoding='utf8')  # type: ignore
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):  # type: ignore
        if self._file is not None:
            self._file.close()

    def write(self, text: str) -> None:
        print(text, end='')
        if self._file is None:
            raise OSError('File must be opened.')
        self._file.write(text)


class ExitSuppressor:
    real_exit = sys.exit

    @staticmethod
    def exit_function(code: Union[str, int, None] = 0) -> NoReturn:
        pass


def get_pylint_suppressors(
    starting_directory: Path, current_directory: Path | None = None, lines: Dict[Path, List[Tuple[int, str]]] = None
) -> Dict[Path, List[Tuple[int, str]]]:
    """Print pylint suppressions."""

    if lines is None:
        lines = {}

    if current_directory is None:
        current_directory = starting_directory

    for gf in current_directory.rglob('*'):
        if gf.is_dir():
            get_pylint_suppressors(starting_directory, gf, lines)
        elif gf.is_file() and gf.name.endswith('.py'):
            with open(gf, encoding='utf8') as pf:
                for n, ln in enumerate(pf.readlines()):
                    # This is not totally correct.
                    if 'pylint:' not in ln:
                        continue
                    lines_list = lines.setdefault(gf.relative_to(starting_directory), [])
                    lines_list.append((n, ln))
    return lines


if len(sys.argv) < 3:
    print(f'{Path(__file__).name} <working_directory> <out_directory>')
    sys.exit(1)

SCRIPT_DIR = Path(__file__).parent.absolute()

PROJECT_DIR = WORKING_DIR = Path(sys.argv[1])
RC_FILE = PROJECT_DIR / '.pylintrc'

if not RC_FILE.exists():
    RC_FILE = PROJECT_DIR.parent / '.pylintrc'

PYLINT_DIR = 'pylint_report'
PYLINT_PATH = Path(sys.argv[2]) / PYLINT_DIR
PYLINT_REPORT_FILE = PYLINT_PATH / 'pylint-report.txt'

WORKING_DIR.cwd()
PYLINT_PATH.mkdir(exist_ok=True, parents=True)
sys.exit = ExitSuppressor.exit_function

os.environ['PYTHON_PATH'] = f'{WORKING_DIR}:{os.environ.get("PYTHON_PATH", "")}'
argv = sys.argv[3:] + [f'--rcfile={RC_FILE}', '--exit-zero', str(WORKING_DIR)]

suppressors = get_pylint_suppressors(WORKING_DIR)

if suppressors:
    print(f'Pylint suppressed\n{"=" * 17}\n')
    with TeeIO(PYLINT_PATH / 'pylint-disabled.txt') as tf:
        for fn, skip_list in suppressors.items():
            tf.write(f'{str(fn)}:\n')
            for line_no, line in skip_list:
                tf.write(f'  {line_no}: {line}')
    print('-' * 70)

try:
    with TeeIO(PYLINT_REPORT_FILE) as rf:
        PylintRun(argv, reporter=ParseableTextReporter(output=rf))
except KeyboardInterrupt:
    sys.exit(1)

expr_regex = re.compile(r'\[[FEW,]\d+\(')

print('Analyzing report file...')
with open(PYLINT_REPORT_FILE, encoding='utf8') as f:
    critical_lines = []

    # Create critical report.
    while line := f.readline():
        if expr_regex.search(line):
            critical_lines.append(line.strip())

    print(f'Error count = {len(critical_lines)}')

    with TeeIO(PYLINT_PATH / 'pylint-report-critical-only.txt') as crf:
        crf.write('\n'.join(critical_lines))

    # Errors and warnings make the build only fail.
    if critical_lines:
        print('Pylint found Errors and Warnings. Fix or explain, then rerun.')
        if os.environ.get('BYPASS_BLOCKING_PYLINT') is None:
            ExitSuppressor.real_exit(1)
        else:
            print('WARNING: Pylint blocking mechanism was disabled.')
    else:
        print('No Errors of Warnings were found by Pylint.')
