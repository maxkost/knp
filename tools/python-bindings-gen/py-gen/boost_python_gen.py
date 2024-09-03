#!/usr/bin/env python3
#
# Apache 2 license
#

import argparse
import collections
import re
from typing import Callable, Dict, List

from CppHeaderParser_l.CppHeaderParser import CppHeader  # type: ignore

operators = {
    '()': '__call__',
    '<': 'py::self < py::self',
    '<=': 'py::self <= py::self',
    '>': 'py::self > py::self',
    '>=': 'py::self >= py::self',
    '==': 'py::self == py::self',
    '!=': 'py::self != py::self',
    '[]': '__getitem__',
    'bool': 'py::bool_(py::self)',
    'std::string': 'py::str(py::self)',
    '<<': 'py::str(py::self)',
    'float': 'py::float_(py::self)',
}

spaces_count = 4

# CppHeaderParser.debug = 1
# CppHeaderParser.debug_trace = 1


MethodHookData = collections.namedtuple('MethodHookData', ['method_name', 'in_params', 'ret_names', 'pre', 'post'])

op_re = re.compile(r'^.*operator\s*([^\s\(\)].+)')

# Doxygen-related regexes.
brief_regex = re.compile(r'\s*\*+\s+@brief\s+(.*)')
text_regex = re.compile(r'^\s*\*+\s+[^@](.*)$')


def process_docstring(ds):
    in_brief = False
    lines = []

    for ln in ds.split('\n'):
        if not in_brief:
            brief = brief_regex.match(ln)
            if brief is not None:
                in_brief = True
                lines.append(brief.group(1))
            else:
                continue
        if in_brief:
            txt = text_regex.match(ln)
            if txt is not None:
                lines.append(txt.group(1))
            else:
                break

    result = ' '.join(lines)

    return f'"{result}"'


def _process_enum(enum, class_name=None):
    name = enum['name']
    typename = name

    if class_name:
        typename = f'{class_name}::{typename}'
        class_name.lower()

    ret = [f'py::enum_<{typename}>("{name}")']

    for k in enum['values']:
        # v['name']
        ret.append(f'{" " * spaces_count}.value("{k["name"]}", {typename}::{k["value"]})')

    ret[-1] = ret[-1] + ';'
    return ret


def _process_constructor(method):
    doxygen = method.get('doxygen')
    brk = ')' if doxygen is None else f', {process_docstring(doxygen)})'
    params = ', '.join(p['raw_type'] for p in method['parameters'])

    return f'{" " * spaces_count}.def(py::init<{params}>(){brk}'


def _process_general_method(class_name, meth, py_method_name):
    ret = []
    method_name: str = meth['name']

    # in_params = '' if not hook_data.in_params else ', ' + ', '.join('%(type)s %(name)s' % p \
    # for p in hook_data.in_params)

    doxygen = meth.get('doxygen')

    brk = ')' if doxygen is None else f', {process_docstring(doxygen)})'
    ret.append(f'{" " * spaces_count}.def("{py_method_name}", &{class_name}::{method_name}{brk}')

    return ret


def _process_operator(class_name, meth, py_method_name, hook_data):
    ret = []
    doxygen = meth.get('doxygen')
    brk = ')' if doxygen is None else f', {process_docstring(doxygen)})'

    method_body = operators.get(py_method_name)

    if method_body is not None:
        ret.append(f'{" " * spaces_count}.def({method_body}{brk}')
    else:
        # ret.extend(_process_general_method(class_name, meth, py_method_name, in_params, ret_names, pre, post))
        ret.extend(_process_overloaded_method(class_name, meth, py_method_name, hook_data.in_params))
        return ret

    return ret


def _process_overloaded_method(class_name, meth, py_method_name, parameters):
    ret = []
    method_name: str = meth['name']

    doxygen = meth.get('doxygen')
    brk = ')' if doxygen is None else f', {process_docstring(doxygen)})'

    # Overloaded method.
    params = ', '.join(p['raw_type'] for p in parameters)
    overload = f'({meth["returns"]} ({class_name}::*)({params}))'

    ret.append(f'{" " * spaces_count}.def("{py_method_name}", {overload}&{class_name}::{method_name}{brk}')

    return ret


def _process_method(class_name, meth, hooks, overloaded=False):
    # Skip destructor.
    if meth.get('destructor', False):
        return []

    ret = []
    method_name: str = meth['name']
    parameters = meth['parameters']

    # Fix types that are enums.
    for p in parameters:
        if p.get('enum'):
            p['raw_type'] = p['enum']
            p['type'] = p['enum']

    meth.get('doxygen')

    # Constructor.
    if method_name == class_name:
        ret.append(_process_constructor(meth))
    else:
        ret_names = []
        if meth['returns'] != 'void':
            ret_names.append('__ret')

        in_params = parameters[:]

        modified = False

        # Data that hooks can modify.
        hook_data = MethodHookData(method_name, in_params, ret_names, [], [])

        for hook in hooks.get('method_hooks', []):
            if hook(class_name, meth, hook_data):
                modified = True

        py_method_name = hook_data.method_name

        m = op_re.match(method_name)
        if m:
            # Operator.
            ret.extend(_process_operator(class_name, meth, m[1], hook_data))
        else:
            if overloaded:
                ret.extend(_process_overloaded_method(class_name, meth, py_method_name, parameters))
            if modified:
                ret.extend(_process_general_method(class_name, meth, py_method_name))
    return ret


def _process_class(cls, hooks):
    class_name = cls['name']
    class_name.lower()

    brk = ')' if 'doxygen' not in cls else f', {process_docstring(cls["doxygen"])})'
    ret = [f'py::class_<{class_name}>("{class_name}"{brk}']

    # Collect methods first to determine if there are overloads.
    # We're ignoring base classes here.
    methods = cls['methods']['public']
    if methods:
        # Collapse them to find overloads.
        meths: Dict[str, List[Callable]] = {}
        for meth in methods:
            meths.setdefault(meth['name'], []).append(meth)

        # Process it.
        for ml in meths.values():
            for mh in ml:
                ret += _process_method(class_name, mh, hooks, len(ml) != 1)

        ret[-1] += ';'

    for e in cls['enums']['public']:
        ret.append('')
        ret += _process_enum(e, class_name=class_name)

    return ret


def process_header(fname, hooks):
    """Return a list of lines."""

    header = CppHeader(fname)
    output = []

    for e in header.enums:
        output += _process_enum(e)
        output.append('')

    # for fn in header.functions:
    #    output += _process_fn(fn)
    #    output.append('')

    for cls in sorted(header.classes.values(), key=lambda c: c['line_number']):
        output += _process_class(cls, hooks)
        output.append('')

    return output


#
# Hooks.
#


# Method hook parameters:
#   class_name: name of the class
#   method: method dictionary from cppheaderparser
#   in_params: copy of method['parameters']
#   ret_names: variables to return
#   pre: statements to insert before function call
#   post: statements to insert after function call
#   .. returns True if method hook did something
def _reference_hook(_, method, hook_data):
    parameters = method['parameters']
    refs = [p for p in parameters if p['reference']]
    if refs:
        hook_data.in_params[:] = [p for p in hook_data.in_params if not p['reference']]
        hook_data.pre.extend(f'{p["raw_type"]} {p["name"]}' for p in refs)
        hook_data.ret_names.extend(p['name'] for p in refs)
        return True

    return False


def _ctr_hook(_, method, hook_data):
    if method['returns'] == 'CTR_Code':
        hook_data.ret_names.remove('__ret')
        hook_data.post.append('CheckCTRCode(__ret)')
        return True

    return False


def process_module(module_name, headers, hooks, add_namespaces=None):
    for header in headers:
        print(f'#include <{header}>')  # TODO: Not usually the actual path.

    print('\n')
    print('#include <boost/python.hpp>')
    print('\n')
    print('namespace py = boost::python;')

    if add_namespaces:
        for ns in add_namespaces:
            if not ns:
                continue

            print(f'Using namespace {ns};')

    print('\n\n')

    print('BOOST_PYTHON_MODULE(%s)\n{' % module_name)

    if 'doxygen' in headers:
        docstring = headers['doxygen']
        print(f'python::scope().attr("__doc__") = "{docstring}"')

    print()

    for header in headers:
        print(' ' * spaces_count + f'\n{" " * spaces_count}'.join(process_header(header, hooks)))
        print()

    print('}')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('module_name')
    parser.add_argument('-n', '--add-namespaces', default='', help='Additional namespaces')
    parser.add_argument('headers', nargs='+')

    args = parser.parse_args()

    hooks = {'method_hooks': [_reference_hook, _ctr_hook]}

    process_module(args.module_name, args.headers, hooks, args.add_namespaces.split(','))


if __name__ == '__main__':
    main()
