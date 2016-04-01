#!/usr/bin/env python
# encoding: utf-8

"""
gtest is a Waf tool for test builds in Ardupilot
"""

from waflib import Utils
from waflib.Configure import conf

import boards

def configure(cfg):
    board = boards.get_board(cfg.env.BOARD)
    if isinstance(board, boards.px4):
        # toolchain is currently broken for gtest
        cfg.msg(
            'Gtest',
            'PX4 boards currently don\'t support compiling gtest',
            color='YELLOW',
        )
        return

    cfg.env.HAS_GTEST = False

    if cfg.env.STATIC_LINKING:
        # gtest uses a function (getaddrinfo) that is supposed to be linked
        # dynamically
        cfg.msg(
            'Gtest',
            'statically linked tests not supported',
            color='YELLOW',
        )
        return

    cfg.env.append_value('GIT_SUBMODULES', 'gtest')
    cfg.env.HAS_GTEST = True

@conf
def libgtest(bld, **kw):
    kw['cxxflags'] = Utils.to_list(kw.get('cxxflags', [])) + ['-Wno-undef']
    kw.update(
        source='modules/gtest/src/gtest-all.cc',
        target='gtest/gtest',
        includes='modules/gtest/ modules/gtest/include',
        export_includes='modules/gtest/include',
        name='GTEST',
    )
    return bld.stlib(**kw)
