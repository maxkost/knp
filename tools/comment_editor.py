#!/usr/bin/env python3

"""
@file comment_editor.py
@brief Doxygen heading comment fixer.

@author Artiom N.
@license Apache 2.0 License.
@copyright Kaspersky(c)2024
@date 11.06.24.
"""

import fileinput
import sys
from pathlib import Path
from typing import Iterable

import regex as re

tag_regex = re.compile(r'[[:space:]]*\*?[[:space:]]*(?P<tag_name>\@[[:alpha:]][[:alnum:]]+)? (?P<tag_value_part>.*)')


def parse_header(header_data: str) -> dict[str, list[str]]:
    prev_tag_name: str | None = None
    tag_map: dict[str, list[str]] = {}

    for line in header_data.split('\n'):
        s_res = tag_regex.search(line)
        if s_res is None:
            continue

        tag_name = s_res.groups()[0]
        tag_value = s_res.groups()[1]

        if tag_name is not None:
            # Remove leading '@'.
            prev_tag_name = tag_name[1:]

        tag_name = prev_tag_name

        if tag_value.strip() in ['*/']:
            continue

        tag_map.setdefault(tag_name, []).append(tag_value)

    return tag_map


def process_header(
    parsed_header: dict[str, list[str]],
    add_tags: dict[str, str] | None = None,
    del_tags: list[str] | None = None,
    replace: bool = False,
) -> dict[str, str]:
    result: dict[str, str] = {}

    for tag_name, tag_val in parsed_header.items():
        if del_tags is not None and tag_name in del_tags:
            continue

        result[tag_name] = ' '.join(tag_val)

    existing_names = parsed_header.keys()

    if add_tags is not None:
        for tag_name, tag_val in add_tags.items():  # type: ignore
            if not replace and tag_name in existing_names:
                continue

            result[tag_name] = tag_val  # type: ignore

    return result


def format_header(processed_header: dict[str, str]) -> str:
    result: list[str] = ['/**']

    for tag_name, tag_val in processed_header.items():
        result.append(f' * @{tag_name} {tag_val}')

    result.append(' */')

    return '\n'.join(result)


def get_header(file_name: Path) -> tuple[int, int, str]:
    header_started: bool = False
    header_lines: list[str] = []
    header_start = 0
    header_end = 0

    with open(file_name, encoding='utf8') as f:
        for ln in f:
            header_end += 1
            if header_started:
                header_lines.append(ln)
                if ln.strip().endswith('*/'):
                    break
                continue

            header_start += 1

            if ln.strip().startswith('/**'):
                header_lines.append(ln)
                header_started = True

    return header_start, header_end, ''.join(header_lines)


def find_files(root_path: Path) -> Iterable[Path]:
    for file_name in root_path.iterdir():
        if file_name.is_dir():
            yield from find_files(file_name)
        else:
            if file_name.suffix in ['.h', '.cpp', '.in']:
                yield file_name


def process_path(root_path: Path | str, /, **add_tags: str) -> None:
    for f_name in find_files(Path(root_path)):
        header_start_ln, header_end_ln, doxy_file_header = get_header(f_name)
        assert header_start_ln <= header_end_ln

        line_no = 0
        header_printed = False

        print(f'Processing file {f_name}')

        with fileinput.input(f_name, inplace=True) as f_data:
            for f_line in f_data:
                line_no += 1
                if header_start_ln <= line_no <= header_end_ln:
                    if not header_printed:
                        print(format_header(process_header(parse_header(doxy_file_header), add_tags=add_tags)))
                        header_printed = True
                else:
                    print(f_line, end='')


if '__main__' == __name__:
    if len(sys.argv) < 3:
        print(f'{Path(sys.argv[0]).name} <root directory> {{tag_name:tag_value}}')
        sys.exit(1)

    process_path(sys.argv[1], **dict([i.split(':', 1) for i in sys.argv[2:]]))
