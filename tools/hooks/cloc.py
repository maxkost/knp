#!/usr/bin/env python3

"""Wrapper script for Cloc code lines counter."""
import json
import sys
from typing import List

from tools_lib.static_analyzer_cmd import StaticAnalyzerCmd  # pylint: disable = E0401


class ClocCmd(StaticAnalyzerCmd):
    """Class for the cloc command."""

    command = 'cloc'
    max_lines = 350

    def __init__(self, args: List[str]):
        super().__init__(self.command, '', args)
        self.check_installed()
        self.parse_args(args)
        self.add_if_missing(['--json'])
        self.add_if_missing(['--by-file'])
        # self.limits_file = Path('.cccc.json')

        self.include_blanks = '--include-blanks' in self.args
        if self.include_blanks:
            self.args.remove('--include-blanks')

    def run(self):
        """Run cloc"""

        self.run_command(self.args + self.files)
        self.exit_on_error()

        for n, v in {n: v for n, v in json.loads(self.stdout.decode()).items() if n not in ['header', 'SUM']}.items():
            result = v['comment'] + v['code']

            if self.include_blanks:
                result += v['blank']

            if result >= self.max_lines:
                self.raise_error(
                    f'file "{n}" exceeds line count limit',
                    f'File contains {result} lines, but maximum is {self.max_lines}',
                )


def main(argv: List[str]):
    cmd = ClocCmd(argv)
    cmd.run()


if __name__ == '__main__':
    main(sys.argv)
