import difflib
import os
import subprocess as sp
from typing import List

from .command import Command


class FormatterCmd(Command):
    """Commands that format code: clang-format, uncrustify."""

    def __init__(self, command: str, look_behind: str, args: List[str]):
        super().__init__(command, look_behind, args)
        self.no_diff_flag = "--no-diff" in self.args
        self.file_flag = None

    def set_diff_flag(self):
        if self.no_diff_flag:
            self.args.remove("--no-diff")

    def compare_to_formatted(self, filename_str: str) -> None:
        """Compare the expected formatted output to file contents."""
        # This string encode is from argparse, so we should be able to trust it.
        filename = filename_str.encode()
        actual = self.get_filelines(filename_str)
        expected = self.get_formatted_lines(filename_str)
        if self.edit_in_place:
            # If edit in place is used, the formatter will fix in place with
            # no stdout. So compare the before and after files for hook pass or fail.
            expected = self.get_filelines(filename_str)
        diff = list(
            difflib.diff_bytes(difflib.unified_diff, actual, expected, fromfile=b"original", tofile=b"formatted")
        )
        if len(diff) > 0:
            if not self.no_diff_flag:
                header = filename + b"\n" + 20 * b"=" + b"\n"
                self.stderr += header + b"\n".join(diff) + b"\n"
            self.returncode = 1

    def get_filename_opts(self, filename: str):
        """uncrustify, like clang-format, requires the -f flag to get stdout."""
        if self.file_flag and not self.edit_in_place:
            return [self.file_flag, filename]
        return [filename]

    def get_formatted_lines(self, filename: str) -> List[bytes]:
        """Get the expected output for a command applied to a file."""
        filename_opts = self.get_filename_opts(filename)
        args = [self.command, *self.args, *filename_opts]
        child = sp.run(args, stdout=sp.PIPE, stderr=sp.PIPE)
        if len(child.stderr) > 0 or child.returncode != 0:
            problem = f"Unexpected Stderr/return code received when analyzing {filename}.\nArgs: {args}."
            self.raise_error(problem, child.stdout.decode() + child.stderr.decode())
        if child.stdout == b"":
            return []
        return child.stdout.split(b"\x0a")

    def get_filelines(self, filename: str):
        """Get the lines in a file."""
        if not os.path.exists(filename):
            self.raise_error(f"File {filename} not found", "Check your path to the file.")
        with open(filename, "rb") as f:
            filetext = f.read()
        return filetext.split(b"\x0a")
