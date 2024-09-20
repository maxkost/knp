import subprocess as sp
import sys
from typing import List

from .command import Command


class StaticAnalyzerCmd(Command):
    """Commmands that analyze code and are not formatters.s"""

    def __init__(self, command: str, look_behind: str, args: List[str]):
        super().__init__(command, look_behind, args)

    def run_command(self, args: List[str]):
        """Run the command and check for errors. Arguments include options and filepaths."""
        args = [self.command, *args]
        sp_child = sp.run(args, stdout=sp.PIPE, stderr=sp.PIPE)
        self.stdout += sp_child.stdout
        self.stderr += sp_child.stderr
        self.returncode = sp_child.returncode

    def exit_on_error(self):
        if self.returncode != 0:
            sys.stderr.buffer.write(self.stdout + self.stderr)
            sys.exit(self.returncode)
