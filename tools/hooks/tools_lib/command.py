#!/usr/bin/env python
"""fns for clang-format, clang-tidy, oclint"""
import os
import re
import shutil
import subprocess as sp
import sys
from typing import List


class Command:
    """Super class that all commands inherit"""

    def __init__(self, command: str, look_behind: str, args: List[str]):
        self.args = args
        self.look_behind = look_behind
        self.command = command
        # Will be [] if not run using pre-commit or if there are no committed files
        self.files = self.get_added_files()
        self.edit_in_place = False

        self.stdout = b""
        self.stderr = b""
        self.returncode = 0

    def check_installed(self):
        """Check if command is installed and fail exit if not."""
        path = shutil.which(self.command)
        if path is None:
            website = "https://github.com/pocc/pre-commit-hooks#example-usage"
            problem = self.command + " not found"
            details = """Make sure {} is installed and on your PATH.\nFor more info: {}""".format(
                self.command, website
            )  # noqa: E501
            self.raise_error(problem, details)

    def get_added_files(self):
        """Find added files using git."""
        added_files = sys.argv[1:]  # 1: don't include the hook file
        # cfg files are used by uncrustify and won't be source files
        added_files = [f for f in added_files if os.path.exists(f) and not f.endswith(".cfg")]

        # Taken from https://github.com/pre-commit/pre-commit-hooks/blob/master/pre_commit_hooks/util.py
        # If no files are provided and if this is used as a command,
        # Find files the same way pre-commit does.
        if len(added_files) == 0:
            cmd = ["git", "diff", "--staged", "--name-only", "--diff-filter=A"]
            sp_child = sp.run(cmd, stdout=sp.PIPE, stderr=sp.PIPE)
            if sp_child.stderr or sp_child.returncode != 0:
                self.raise_error(
                    "Problem determining which files are being committed using git.", sp_child.stderr.decode()
                )
            added_files = sp_child.stdout.decode().splitlines()
        return added_files

    def parse_args(self, args: List[str]):
        """Parse the args into usable variables"""
        self.args = list(args[1:])  # don't include calling function
        for arg in args:
            if arg in self.files and not arg.startswith("-"):
                self.args.remove(arg)
            if arg.startswith("--version"):
                # If --version is passed in as 2 arguments, where the second is version
                if arg == "--version" and args.index(arg) != len(args) - 1:
                    expected_version = args[args.index(arg) + 1]
                # Expected split of --version=8.0.0 or --version 8.0.0 with as many spaces as needed
                else:
                    expected_version = arg.replace(" ", "").replace("=", "").replace("--version", "")
                actual_version = self.get_version_str()
                self.assert_version(actual_version, expected_version)
        # All commands other than clang-tidy or oclint require files, --version ok
        is_cmd_clang_analyzer = self.command == "clang-tidy" or self.command == "oclint"
        has_args = self.files or self.args or "version" in self.args
        if not has_args and not is_cmd_clang_analyzer:
            self.raise_error("Missing arguments", "No file arguments found and no files are pending commit.")

    def add_if_missing(self, new_args: List[str]):
        """Add a default if it's missing from the command. This library
        exists to force checking, so prefer those options.
        len(new_args) should be 1, or 2 for options like --key=value

        If first arg is missing, add new_args to command's args
        Do not change an option - in those cases return."""
        new_arg_key = new_args[0].split("=")[0]
        for arg in self.args:
            existing_arg_key = arg.split("=")[0]
            if existing_arg_key == new_arg_key:
                return
        self.args += new_args

    def assert_version(self, actual_ver: str, expected_ver: str):
        """--version hook arg enforces specific versions of tools."""
        expected_len = len(expected_ver)  # allows for fuzzy versions
        if expected_ver not in actual_ver[:expected_len]:
            problem = "Version of " + self.command + " is wrong"
            details = """Expected version: {}
Found version: {}
Edit your pre-commit config or use a different version of {}.""".format(
                expected_ver, actual_ver, self.command
            )
            self.raise_error(problem, details)
        # If the version is correct, exit normally
        sys.exit(0)

    def raise_error(self, problem: str, details: str):
        """Raise a formatted error."""
        format_list = [self.command, problem, details]
        stderr_str = """Problem with {}: {}\n{}\n""".format(*format_list)
        # All strings are generated by this program, so decode should be safe
        self.stderr = stderr_str.encode()
        self.returncode = 1
        sys.stderr.buffer.write(self.stderr)
        sys.exit(self.returncode)

    def get_version_str(self):
        """Get the version string like 8.0.0 for a given command."""
        args = [self.command, "--version"]
        sp_child = sp.run(args, stdout=sp.PIPE, stderr=sp.PIPE)
        version_str = str(sp_child.stdout, encoding="utf-8")
        # After version like `8.0.0` is expected to be '\n' or ' '
        regex = self.look_behind + r"((?:\d+\.)+[\d+_\+\-a-z]+)"
        search = re.search(regex, version_str)
        if not search:
            details = """The version format for this command has changed.
Create an issue at github.com/pocc/pre-commit-hooks."""
            self.raise_error("getting version", details)
        version = search.group(1)
        return version
