#!/usr/bin/env python

"""PyLint CI runner."""

import os
import sys
from pathlib import Path

from pylint import run_pylint


SCRIPT_DIR = Path(__file__).parent.absolute()
PROJECT_DIR = WORKING_DIR = Path(sys.argv[1])
PYLINT_DIR = 'pylint_report'
PYLINT_PATH = WORKING_DIR / PYLINT_DIR

WORKING_DIR.cwd()

PYLINT_PATH.mkdir(exist_ok=True)

os.environ['PYTHON_PATH'] = f'{WORKING_DIR}{os.pathsep}{os.environ["PYTHON_PATH"]}'

# sys.argv[0] = re.sub(r'(-script\.pyw|\.exe)?$', '', sys.argv[0])

sys.argv.extend([f'--rcfile={PROJECT_DIR / ".pylintrc"}', '--exit-zero', str(WORKING_DIR)])

run_pylint()

a = r"""
#tee $PYLINT_PATH/pylint-report.txt
#grep -rn --exclude-dir=$PYLINT_DIR pylint: | tee $PYLINT_PATH/pylint-disabled.txt
## Errors and warnings make build fail only
#cat $PYLINT_PATH/pylint-report.txt | grep -P '\[[F,E,W]\d+\(' | tee $PYLINT_PATH/pylint-report-critical-only.txt
#
#if [[ "${BYPASS_BLOCKING_PYLINT}" != "true" ]]; then
#    CRITICAL_REPORT_SIZE=$(du -sb $PYLINT_PATH/pylint-report-critical-only.txt | awk '{ print $1 }')
#    if ((${CRITICAL_REPORT_SIZE}>0)); then
#        echo "Pylint found Errors and Warnings. Fix or explain then rerun."
#        exit 1
#     else
#        echo "No Errors of Warnings were found by pylint."
#    fi
#else
#    echo "WARNING: Pylint blocking mechanism was disabled."
#fi
#
#popd
"""
