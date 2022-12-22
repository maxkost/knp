#!/usr/bin/env python3
"""Wrapper script for cpplint."""
import sys
from typing import List

from .static_analyzer_cmd import StaticAnalyzerCmd


class CpplintCmd(StaticAnalyzerCmd):
    """Class for the cpplint command."""

    command = "cpplint"
    lookbehind = "cpplint "

    def __init__(self, args: List[str]):
        super().__init__(self.command, self.lookbehind, args)
        self.parse_args(args)
        self.add_if_missing(["--verbose=0"])

    def run(self):
        """Run cpplint"""
        for filename in self.files:
            self.run_command(self.args + [filename])  # cpplint is unique in requiring args before filename
            self.exit_on_error()


def main(argv: List[str]):
    cmd = CpplintCmd(argv)
    cmd.run()


if __name__ == "__main__":
    main(sys.argv)
