#!/usr/bin/env python3

"""Wrapper script for CCCC."""

from json import load
import sys
from pathlib import Path
from typing import List
from xml.etree import ElementTree

from tools_lib.static_analyzer_cmd import StaticAnalyzerCmd  # pylint: disable = E0401


class CcccCmd(StaticAnalyzerCmd):
    """Class for the cccc command."""

    command = 'cccc'

    def __init__(self, args: List[str]):
        super().__init__(self.command, '', args)
        self.check_installed()
        self.parse_args(args)
        self.artifacts_dir = Path().cwd() / '.cccc'
        self.config_name = 'cccc.xml'
        self.limits_file = Path('.cccc.json')

        for a in self.args:
            opt_name, opt_val = a.split('=')
            if opt_name.startswith('--outdir'):
                self.artifacts_dir = Path.cwd() / opt_val
            elif opt_name.startswith('--xml_outfile'):
                self.config_name = opt_val
            elif opt_name.startswith('--limits_file'):
                self.limits_file = Path(opt_val)
                self.args.remove(a)
        self.config_file = self.artifacts_dir / self.config_name

    def run(self):
        """Run cccc."""

        self.artifacts_dir.mkdir(parents=True, exist_ok=True)

        with open(self.limits_file, encoding='utf8') as f:
            limits = load(f)

        for file in self.files:
            self.run_command(self.args + [file])
            self.exit_on_error()

            try:
                tree = ElementTree.parse(self.config_file)
            except ElementTree.ParseError as e:
                print(f'WARNING: CCCC BUG: {e}.')
                return
            root = tree.getroot()

            for module in root.findall('./procedural_summary/'):
                module_data = {}
                module_name = ''

                for param in module:
                    if 'name' == param.tag:
                        module_name = param.text
                    elif 'lines_of_code' == param.tag:
                        module_data['locf_max'] = int(param.attrib['value'])
                    elif 'McCabes_cyclomatic_complexity' == param.tag:
                        module_data['mvg_function_max'] = int(param.attrib['value'])
                    elif 'McCabes_cyclomatic_complexity_per_module' == param.tag:
                        module_data['mvg_file_max'] = int(param.attrib['value'])
                    elif 'rejected_lines_of_code' == param.tag:
                        module_data['rejected_lines_of_code'] = int(param.attrib['value'])

                for i, v in limits.items():
                    if module_data.get(i, 0) > v:
                        self.raise_error(
                            f'{i} in "{file}" [{module_name}] == {module_data[i]}', f'This exceeds the limit of {v}.'
                        )
                        sys.exit(1)


def main(argv: List[str]):
    cmd = CcccCmd(argv)
    cmd.run()


if __name__ == '__main__':
    main(sys.argv)
