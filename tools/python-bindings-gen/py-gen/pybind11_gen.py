#!/usr/bin/env python3
#
# Apache 2 license
#

import argparse
import collections
import re

from CppHeaderParser_l import CppHeaderParser

op_re = re.compile(r'^.*operator\s*\w*\s*(.+)')
operators = {
    '()': '__call__',
    '<': '__lt__',
    '<=': '__le__',
    '>': '__gt__',
    '>=': '__ge__',
    '==': '__eq__',
    '!=': '__ne__',
    '[]': '__getitem__',
    'bool': 'bool',
}

spaces_count = 4

# CppHeaderParser.debug = 1
# CppHeaderParser.debug_trace = 1


MethodHookData = collections.namedtuple('MethodHookData', ['method_name', 'in_params', 'ret_names', 'pre', 'post'])


def _process_enum(enum, class_name=None):
    name = enum['name']
    typename = name
    parent = 'm'

    if class_name:
        typename = '{}::{}'.format(class_name, typename)
        parent = class_name.lower()

    ret = ['py::enum_<{}>({}, "{}")'.format(typename, parent, name)]

    for v in enum['values']:
        k = v['name']
        ret.append('{}.value("{}", {}::{})'.format(' ' * spaces_count, k, typename, k))

    ret[-1] = ret[-1] + ';'
    return ret


def _process_fn(fn):
    return []  # TODO


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

    # Constructor.
    if method_name == class_name:
        params = ', '.join(p['raw_type'] for p in parameters)
        ret.append('{}.def(py::init<{}>())'.format(' ' * spaces_count, params))
    else:
        pre = []
        post = []
        ret_names = []
        if meth['returns'] != 'void':
            ret_names.append('__ret')

        in_params = parameters[:]

        modified = False

        # Data that hooks can modify.
        hook_data = MethodHookData(method_name, in_params, ret_names, pre, post)

        for hook in hooks.get('method_hooks', []):
            if hook(class_name, meth, hook_data):
                modified = True

        py_method_name = hook_data.method_name

        m = op_re.match(method_name)
        if m:
            py_method_name = operators.get(m[1], method_name)

        if modified:
            in_args = ''
            if in_params:
                in_args = ', ' + ', '.join('%(type)s %(name)s' % p for p in in_params)

            ret.append(
                f'{" " * spaces_count}.def("%(py_method_name)s", '
                f'[](%(class_name)s '
                '&__inst%(in_args)s)\n'
                f'{" " * 2 * spaces_count}'
                '{' % locals()
            )

            if pre:
                ret.append(' ' * 2 * spaces_count + '; '.join(pre) + ';')

            meth_params = ', '.join(p['name'] for p in meth['parameters'])

            fnret = 'auto __ret = '

            if '__ret' not in ret_names:
                fnret = ''

            if not post:
                if ret_names == ['__ret']:
                    ret_names = []
                    fnret = 'return '

            ret.append(f'{" " * spaces_count * 2}%(fnret)s__inst.%(method_name)s(%(meth_params)s);' % locals())

            if post:
                ret.append(' ' * spaces_count + '; '.join(post) + ';')

            if len(ret_names) == 0:
                pass
            elif len(ret_names) == 1:
                ret.append(f'{" " * 2 * spaces_count}return %s;' % ret_names[0])
            else:
                ret.append(f'{" " * 2 * spaces_count}return std::make_tuple(%s);' % ', '.join(ret_names))

            ret.append('%s})' % (' ' * spaces_count))

        else:
            overload = ''
            if overloaded:
                # Overloaded method.
                params = ', '.join(p['raw_type'] for p in parameters)
                overload = '({} ({}::*)({}))'.format(meth['returns'], class_name, params)

            ret.append(
                '{}.def("{}", {}&{}::{})'.format(' ' * spaces_count, py_method_name, overload, class_name, method_name)
            )

    return ret


def _process_class(cls, hooks):
    class_name = cls['name']
    varname = class_name.lower()

    ret = ['py::class_<{}> {}(m, "{}");'.format(class_name, varname, class_name)]

    # Collect methods first to determine if there are overloads.
    # We're ignoring base classes here.
    methods = cls['methods']['public']
    if methods:
        ret.append(varname)

        # Collapse them to find overloads.
        meths = collections.OrderedDict()
        for meth in methods:
            meths.setdefault(meth['name'], []).append(meth)

        # Process it.
        for ml in meths.values():
            if len(ml) == 1:
                ret += _process_method(class_name, ml[0], hooks)
            else:
                for mh in ml:
                    ret += _process_method(class_name, mh, hooks, True)

        ret[-1] = ret[-1] + ';'

    for e in cls['enums']['public']:
        ret.append('')
        ret += _process_enum(e, class_name=class_name)

    return ret


def process_header(fname, hooks):
    """Return a list of lines."""

    header = CppHeaderParser.CppHeader(fname)
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


def _reference_hook(class_name, method, hook_data):
    parameters = method['parameters']
    refs = [p for p in parameters if p['reference']]
    if refs:
        hook_data.in_params[:] = [p for p in hook_data.in_params if not p['reference']]
        hook_data.pre.extend('%(raw_type)s %(name)s' % p for p in refs)
        hook_data.ret_names.extend(p['name'] for p in refs)
        return True


def _ctr_hook(class_name, method, hook_data):
    if method['returns'] == 'CTR_Code':
        hook_data.ret_names.remove('__ret')
        hook_data.post.append('CheckCTRCode(__ret)')
        return True


def process_module(module_name, headers, hooks, add_namespaces=None):
    for header in headers:
        print('#include <%s>' % header)  # TODO: Not usually the actual path.

    print()
    print('#include <pybind11/pybind11.h>')
    print()
    print('namespace py = pybind11;')
    if add_namespaces:
        for ns in add_namespaces:
            if not ns:
                continue

            print('Using namespace %s;' % ns)
    print()

    print()
    print('PYBIND11_MODULE(%s, m)\n{' % module_name)
    print()
    print(
        '%sm.doc() = R"pbdoc(\n%s%s\n%s)pbdoc";'
        % (' ' * spaces_count, ' ' * spaces_count * 2, module_name, ' ' * spaces_count)
    )
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

    hooks = dict()
    hooks['method_hooks'] = [_reference_hook, _ctr_hook]

    process_module(args.module_name, args.headers, hooks, args.add_namespaces.split(','))


if __name__ == '__main__':
    main()
