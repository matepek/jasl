#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# JASL: For more information see https://github.com/matepek/jasl
#
# Copyright (c) 2018 Mate Pek
#
# This code is licensed under the MIT License (MIT).


class Value(object):

    def __init__(self, name, value, short=None, data=None):
        self.name = name
        self.value = value
        self.short = short if short else name
        self.data = data

    def __call__(self):
        return self.value

    def __eq__(self, other):
        return self.value == other.value

    def __repr__(self):
        return 'Value(' + repr(self.name) + ', ' + repr(self.value) + ', ' + repr(self.short) + ')'

    def __nonzero__(self):
        asser(False)


class StringValue(Value):

    def __init__(self, name, value=None, short=None, data=None):
        if value is None:
            value = '"' + name + '"'
        else:
            assert(not value.startswith('"'))
            assert(not value.endswith('"'))
            value = '"' + value + '"'
        super(StringValue, self).__init__(name, value, short, data)


class BoolValue(Value):

    def __eq__(self, other):
        return bool(self) == bool(other)


class TrueValue(BoolValue):

    def __init__(self):
        super(TrueValue, self).__init__(name='true', value='true', short='t')

    def __nonzero__(self):
        return True


class FalseValue(BoolValue):

    def __init__(self):
        super(FalseValue, self).__init__(name='false', value='false', short='f')

    def __nonzero__(self):
        return False


class AlwaysTrueArg(object):
    '''For development. Should not be used in released code.'''

    def __init__(self, name, short):
        self.name = name
        self.short = short
        self.true = TrueValue()
        self.values = (self.true,)

    def __repr__(self):
        return 'AlwaysTrueArg(' + repr(self.name) + ', ' + repr(self.short) + ')'


class AlwaysFalseArg(object):
    '''For development. Should not be used in released code.'''

    def __init__(self, name, short):
        self.name = name
        self.short = short
        self.false = FalseValue()
        self.values = (self.false,)

    def __repr__(self):
        return 'AlwaysFalseArg(' + repr(self.name) + ', ' + repr(self.short) + ')'


class BooleanArg(object):

    def __init__(self, name, short):
        self.name = name
        self.short = short
        self.true = TrueValue()
        self.false = FalseValue()
        self.values = (self.true, self.false)

    def __repr__(self):
        return 'BooleanArg(' + repr(self.name) + ', ' + repr(self.short) + ')'


class StringArg(object):

    def __init__(self, name, short, values=[]):
        self.name = name
        self.short = short
        self.values = []
        for v in values:
            self.add(v)

    def add(self, value):
        assert(value.name not in self.__dict__)
        self.__dict__[value.name] = value
        self.values.append(value)
        return self

    def __repr__(self):
        return 'StringArg(' + repr(self.name) + ', ' + repr(self.short) + ', ' + repr(list(self.values)) + ')'


class Args(object):
    __slots__ = '_args', '_filters'

    def __init__(self, args=[]):
        self._args = dict()
        self._filters = []
        for a in args:
            self.add(a)

    def add(self, arg):
        assert(arg.name not in self._args)
        self._args[arg.name] = arg
        return arg

    def __getattr__(self, key):
        return self._args[key]

    def __repr__(self):
        return 'Args(' + repr(list(self._args[a] for a in self._args)) + ')'

    def filter(self, func):
        self._filters.append(func)

    def filter_out(self, func):
        self._filters.append(lambda x: not func(x))

    def variants(self):
        class Variant(object):
            __slots__ = '_arg_value_dict',

            class MissingArgError(KeyError):
                pass

            def __init__(self, arg_value_dict={}):
                self._arg_value_dict = arg_value_dict

            def copy(self):
                return Variant(self._arg_value_dict.copy())

            def as_dict(self):
                return {arg.name: self._arg_value_dict[arg].value for arg in self._arg_value_dict}

            def as_dir(self):
                # warning: On windows path name has a limit. We should carefully generate directory names.
                return '_'.join(sorted(arg.short + '' + self._arg_value_dict[arg].short.title() for arg in self._arg_value_dict))

            def as_args(self):
                return ' '.join(sorted(arg.name + '=' + self._arg_value_dict[arg].value for arg in self._arg_value_dict))

            def _set(self, key, value):
                self._arg_value_dict[key] = value

            def __getattr__(self, key):
                key_value = {
                    k.name: self._arg_value_dict[k] for k in self._arg_value_dict}
                if key not in key_value:
                    raise self.MissingArgError(key)
                return key_value[key]

            def __repr__(self):
                return repr({k.name: self._arg_value_dict[k].value for k in self._arg_value_dict})

        variants = [Variant()]
        filters = list(self._filters)
        for arg_name in self._args:
            extended_variants = []
            for variant in variants:
                for value in self._args[arg_name].values:
                    extended_variants.append(variant.copy())
                    extended_variants[-1]._set(self._args[arg_name], value)
            # filtering here makes it much much much times faster
            remaining_filters = []
            for f in filters:
                try:
                    extended_variants = filter(f, extended_variants)
                except Variant.MissingArgError:
                    remaining_filters.append(f)
            filters = remaining_filters
            variants = extended_variants

        class Variants(object):
            __slots__ = '_variants',

            def __init__(self, variants):
                self._variants = variants

            def filter(self, func):
                self._variants = filter(func, self._variants)

            def filter_out(self, func):
                self.filter(lambda x: not func(x))

            def __repr__(self):
                self._variants = list(self._variants)
                return repr(self._variants)

            def __len__(self):
                self._variants = list(self._variants)
                return len(self._variants)

            def __iter__(self):
                return self._variants.__iter__()

        return Variants(variants)
