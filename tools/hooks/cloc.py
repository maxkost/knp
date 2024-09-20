#!/usr/bin/env python3

"""Wrapper script for Cloc code lines counter."""
import json
import sys
from typing import List

from tools_lib.static_analyzer_cmd import StaticAnalyzerCmd  # pylint: disable = E0401


class ClocCmd(StaticAnalyzerCmd):
    """Class for the cloc command."""

    command = 'cloc'
    max_code_lines = 350
    max_text_lines = 500

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
        """Run cloc."""

        self.run_command(self.args + self.files)
        self.exit_on_error()

        for n, v in {n: v for n, v in json.loads(self.stdout.decode()).items() if n not in ['header', 'SUM']}.items():
            code_lines_count = v['code']
            text_lines_count = v['comment'] + code_lines_count

            if self.include_blanks:
                text_lines_count += v['blank']

            if code_lines_count >= self.max_code_lines:
                self.raise_error(
                    f'File "{n}" code line count exceeds code line count limit.',
                    f'File contains {code_lines_count} lines of code, but maximum is {self.max_code_lines}.',
                )

            if text_lines_count >= self.max_text_lines:
                self.raise_error(
                    f'File "{n}" line count exceeds line count limit.',
                    f'File contains {code_lines_count} lines of text, but maximum is {self.max_text_lines}.',
                )


def main(argv: List[str]):
    cmd = ClocCmd(argv)
    cmd.run()


if __name__ == '__main__':
    main(sys.argv)
