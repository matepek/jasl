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


class TrueValue(Value):
    def __init__(self):
        super(TrueValue, self).__init__(name='true', value='true', short='t')

    def __nonzero__(self):
        return True


class FalseValue(Value):
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


class GN(object):

    def __init__(self, args=[]):
        self._args = dict()
        self._filters = []
        for a in args:
            self.add(a)

    def add(self, arg):
        assert(arg.name not in self.__dict__)
        assert(arg.name not in self._args)
        self.__dict__[arg.name] = arg
        self._args[arg.name] = arg
        return arg

    def __repr__(self):
        return 'GN(' + repr(list(self._args[a] for a in self._args)) + ')'

    def filter(self, func):
        self._filters.append(func)

    def filter_out(self, func):
        self._filters.append(lambda x: not func(x))

    def variants(self):
        class Variant:

            def __init__(self, arg_value_dict={}):
                self.arg_value_dict = arg_value_dict

            def copy(self):
                return Variant(self.arg_value_dict.copy())

            def as_dict(self):
                return {arg.name: self.arg_value_dict[arg].value for arg in self.arg_value_dict}

            def as_dir(self):
                # warning: On windows path name has a limit. We should carefully generate directory names.
                return '_'.join(sorted(arg.short + '' + self.arg_value_dict[arg].short.title() for arg in self.arg_value_dict))

            def as_args(self):
                return ' '.join(sorted(arg.name + '=' + self.arg_value_dict[arg].value for arg in self.arg_value_dict))

            def __getattr__(self, key):
                key_value = {
                    k.name: self.arg_value_dict[k] for k in self.arg_value_dict}
                assert(key in key_value)
                return key_value[key]

            def __repr__(self):
                return repr({k.name: self.arg_value_dict[k].value for k in self.arg_value_dict})

        variants = [Variant()]
        for arg_name in self._args:
            extended_variants = []
            for variant in variants:
                for value in self._args[arg_name].values:
                    extended_variants.append(variant.copy())
                    extended_variants[-1].arg_value_dict[self._args[arg_name]] = value
            variants = extended_variants

        class Variants:

            def __init__(self, variants):
                self.variants = variants

            def filter(self, func):
                self.variants = filter(func, self.variants)

            def filter_out(self, func):
                self.filter(lambda x: not func(x))

            def __repr__(self):
                self.variants = list(self.variants)
                return repr(self.variants)

            def __len__(self):
                self.variants = list(self.variants)
                return len(self.variants)

            def __iter__(self):
                return self.variants.__iter__()

        return Variants(filter(lambda v: all(f(v) for f in self._filters), variants))
