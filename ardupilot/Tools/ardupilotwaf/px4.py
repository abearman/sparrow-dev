#!/usr/bin/env python
# encoding: utf-8

"""
Waf tool for PX4 build
"""

from waflib import Logs, Task, Utils
from waflib.TaskGen import after_method, before_method, feature

import os
import sys

_dynamic_env_data = {}
def _load_dynamic_env_data(bld):
    bldnode = bld.bldnode.make_node('modules/PX4Firmware')
    for name in ('cxx_flags', 'include_dirs', 'definitions'):
        _dynamic_env_data[name] = bldnode.find_node(name).read().split(';')

    _dynamic_env_data['DEFINES'] = [
        'NUTTX_GIT_VERSION="%s"' % bld.git_submodule_head_hash('PX4NuttX')[:8],
        'PX4_GIT_VERSION="%s"' % bld.git_submodule_head_hash('PX4Firmware')[:8],
    ]

@feature('px4_ap_stlib', 'px4_ap_program')
@before_method('process_source')
def px4_dynamic_env(self):
    # The generated files from configuration possibly don't exist if it's just
    # a list command (TODO: figure out a better way to address that).
    if self.bld.cmd == 'list':
        return

    if not _dynamic_env_data:
        _load_dynamic_env_data(self.bld)

    self.env.append_value('INCLUDES', _dynamic_env_data['include_dirs'])
    self.env.prepend_value('CXXFLAGS', _dynamic_env_data['cxx_flags'])
    self.env.prepend_value('CXXFLAGS', _dynamic_env_data['definitions'])
    self.env.append_value('DEFINES', _dynamic_env_data['DEFINES'])

# Single static library
# NOTE: This only works only for local static libraries dependencies - fake
# libraries aren't supported yet
@feature('px4_ap_program')
@after_method('apply_link')
@before_method('process_use')
def px4_import_objects_from_use(self):
    queue = Utils.to_list(getattr(self, 'use', []))
    names = set()

    while queue:
        name = queue.pop(0)
        if name in names:
            continue
        names.add(name)

        try:
            tg = self.bld.get_tgen_by_name(name)
        except Errors.WafError:
            continue

        tg.post()
        for t in getattr(tg, 'compiled_tasks', []):
            self.link_task.set_inputs(t.outputs)

        queue.extend(Utils.to_list(getattr(tg, 'use', [])))

class px4_copy_lib(Task.Task):
    run_str = '${CP} ${SRC} ${PX4_AP_PROGRAM_LIB}'
    color = 'CYAN'

    def runnable_status(self):
        status = super(px4_copy_lib, self).runnable_status()
        if status != Task.SKIP_ME:
            return status

        pseudo_target = self.get_pseudo_target()
        try:
            if pseudo_target.sig != self.inputs[0].sig:
                status = Task.RUN_ME
        except AttributeError:
            status = Task.RUN_ME

        return status

    def get_pseudo_target(self):
        if hasattr(self, 'pseudo_target'):
            return self.pseudo_target

        bldnode = self.generator.bld.bldnode
        self.pseudo_target = bldnode.make_node(self.env.PX4_AP_PROGRAM_LIB)
        return self.pseudo_target

    def keyword(self):
        return 'PX4: Copying program library'

    def post_run(self):
        super(px4_copy_lib, self).post_run()
        pseudo_target = self.get_pseudo_target()
        pseudo_target.sig = pseudo_target.cache_sig = self.inputs[0].sig

class px4_copy(Task.Task):
    run_str = '${CP} ${SRC} ${TGT}'
    color = 'CYAN'

    def keyword(self):
        return "PX4: Copying %s to" % self.inputs[0].name

class px4_add_git_hashes(Task.Task):
    run_str = '${PYTHON} ${PX4_ADD_GIT_HASHES} --ardupilot ${PX4_APM_ROOT} --px4 ${PX4_ROOT} --nuttx ${PX4_NUTTX_ROOT} --uavcan ${PX4_UAVCAN_ROOT} ${SRC} ${TGT}'
    color = 'CYAN'

    def keyword(self):
        return "PX4: Copying firmware and adding git hashes"

    def __str__(self):
        return self.outputs[0].path_from(self.outputs[0].ctx.launch_node())

def _update_firmware_sig(fw_task, firmware):
    original_post_run = fw_task.post_run
    def post_run():
        original_post_run()
        firmware.sig = firmware.cache_sig = Utils.h_file(firmware.abspath())
    fw_task.post_run = post_run

_cp_px4io = None
_firmware_semaphorish_tasks = []
_upload_task = []

@feature('px4_ap_program')
@after_method('process_source')
def px4_firmware(self):
    global _cp_px4io, _firmware_semaphorish_tasks, _upload_task
    version = self.env.get_flat('PX4_VERSION')

    if self.env.PX4_USE_PX4IO and not _cp_px4io:
        px4io_task = self.create_cmake_build_task('px4', 'fw_io')
        px4io = px4io_task.config_taskgen.get_cmake_bldnode().make_node(
            'src/modules/px4iofirmware/px4io-v%s.bin' % version,
        )
        px4io_task.set_outputs(px4io)

        romfs = self.bld.bldnode.make_node(self.env.PX4_ROMFS_BLD)
        romfs_px4io = romfs.make_node('px4io/px4io.bin')
        romfs_px4io.parent.mkdir()
        _cp_px4io = self.create_task('px4_copy', px4io, romfs_px4io)
        _cp_px4io.keyword = lambda: 'PX4: Copying PX4IO to ROMFS'

    cp_lib = self.create_task('px4_copy_lib', self.link_task.outputs)
    # we need to synchronize because the path PX4_AP_PROGRAM_LIB is used by all
    # ap_programs
    if _firmware_semaphorish_tasks:
        for t in _firmware_semaphorish_tasks:
            cp_lib.set_run_after(t)
    _firmware_semaphorish_tasks = []

    fw_task = self.create_cmake_build_task(
        'px4',
        'build_firmware_px4fmu-v%s' % version,
    )
    fw_task.set_run_after(cp_lib)
    if self.env.PX4_USE_PX4IO and _cp_px4io.generator is self:
        fw_task.set_run_after(_cp_px4io)

    firmware = fw_task.config_taskgen.cmake_bld.make_node(
        'src/firmware/nuttx/nuttx-px4fmu-v%s-apm.px4' % version,
    )
    _update_firmware_sig(fw_task, firmware)

    fw_dest = self.bld.bldnode.make_node(
        os.path.join(self.program_dir, '%s.px4' % self.program_name)
    )
    git_hashes = self.create_task('px4_add_git_hashes', firmware, fw_dest)
    git_hashes.set_run_after(fw_task)
    _firmware_semaphorish_tasks.append(git_hashes)

    if self.bld.options.upload:
        if _upload_task:
            Logs.warn('PX4: upload for %s ignored' % self.name)
            return
        _upload_task = self.create_cmake_build_task('px4', 'upload')
        _upload_task.set_run_after(fw_task)
        _firmware_semaphorish_tasks.append(_upload_task)

def _px4_taskgen(bld, **kw):
    if 'cls_keyword' in kw and not callable(kw['cls_keyword']):
        cls_keyword = str(kw['cls_keyword'])
        kw['cls_keyword'] = lambda tsk: 'PX4: ' + cls_keyword

    if 'cls_str' in kw and not callable(kw['cls_str']):
        cls_str = str(kw['cls_str'])
        kw['cls_str'] = lambda tsk: cls_str

    kw['color'] = 'CYAN'

    return bld(**kw)

def configure(cfg):
    cfg.load('cmake')
    cfg.find_program('cp')

    bldnode = cfg.bldnode.make_node(cfg.variant)
    env = cfg.env

    env.AP_PROGRAM_FEATURES += ['px4_ap_program']
    env.AP_STLIB_FEATURES += ['px4_ap_stlib']

    def srcpath(path):
        return cfg.srcnode.make_node(path).abspath()

    def bldpath(path):
        return bldnode.make_node(path).abspath()

    if env.PX4_VERSION == '1':
        bootloader_name = 'px4fmu_bl.bin'
    else:
        bootloader_name = 'px4fmuv%s_bl.bin' % env.get_flat('PX4_VERSION')

    # TODO: we should move stuff from mk/PX4 to Tools/ardupilotwaf/px4 after
    # stop using the make-based build system
    env.PX4_ROMFS_SRC = 'mk/PX4/ROMFS'
    env.PX4_ROMFS_BLD = 'px4-extra-files/ROMFS'
    env.PX4_BOOTLOADER = 'mk/PX4/bootloader/%s' % bootloader_name

    program_lib_name = cfg.env.cxxstlib_PATTERN % 'ap_program'
    env.PX4_AP_PROGRAM_LIB = os.path.join('px4-extra-files', program_lib_name)

    env.PX4_ADD_GIT_HASHES = srcpath('Tools/scripts/add_git_hashes.py')
    env.PX4_APM_ROOT = srcpath('')
    env.PX4_ROOT = srcpath('modules/PX4Firmware')
    env.PX4_NUTTX_ROOT = srcpath('modules/PX4NuttX')
    env.PX4_UAVCAN_ROOT = srcpath('modules/uavcan')

    env.PX4_CMAKE_VARS = dict(
        CONFIG='nuttx_px4fmu-v%s_apm' % env.get_flat('PX4_VERSION'),
        CMAKE_MODULE_PATH=srcpath('Tools/ardupilotwaf/px4/cmake'),
        UAVCAN_LIBUAVCAN_PATH=env.PX4_UAVCAN_ROOT,
        NUTTX_SRC=env.PX4_NUTTX_ROOT,
        PX4_NUTTX_ROMFS=bldpath(env.PX4_ROMFS_BLD),
        APM_PROGRAM_LIB=bldpath(env.PX4_AP_PROGRAM_LIB),
        EXTRA_CXX_FLAGS=' '.join((
            # NOTE: these "-Wno-error=*" flags should be removed as we update
            # the submodule
            '-Wno-error=double-promotion',
            '-Wno-error=reorder',
            # NOTE: *Temporarily* using this definition so that both
            # PX4Firmware build systems (cmake and legacy make-based) can live
            # together
            '-DCMAKE_BUILD',
            '-I%s' % bldpath('libraries/GCS_MAVLink'),
            '-Wl,--gc-sections',
        )),
        EXTRA_C_FLAGS=' '.join((
            # NOTE: *Temporarily* using this definition so that both
            # PX4Firmware build systems (cmake and legacy make-based) can live
            # together
            '-DCMAKE_BUILD',
        )),
    )

def build(bld):
    version = bld.env.get_flat('PX4_VERSION')
    px4 = bld(
        features='cmake_configure',
        name='px4',
        cmake_src=bld.srcnode.find_dir('modules/PX4Firmware'),
        cmake_vars=bld.env.PX4_CMAKE_VARS,
        group='dynamic_sources',
    )

    px4.cmake_build(
        'msg_gen',
        group='dynamic_sources',
        cmake_output_patterns='src/modules/uORB/topics/*.h',
    )
    px4.cmake_build(
        'prebuild_targets',
        group='dynamic_sources',
        cmake_output_patterns='px4fmu-v%s/NuttX/nuttx-export/**/*.h' % version,
    )

    # ROMFS static files
    romfs_src = bld.srcnode.find_dir(bld.env.PX4_ROMFS_SRC)
    bld.env.PX4_ROMFS_SRC_ABS = romfs_src.abspath()
    romfs_bld = bld.bldnode.make_node(bld.env.PX4_ROMFS_BLD)
    romfs_src_files = romfs_src.ant_glob('**')
    romfs_bld_files = []
    for node in romfs_src_files:
        bld_node = romfs_bld.make_node(node.path_from(romfs_src))
        romfs_bld_files.append(bld_node)

    _px4_taskgen(
        bld,
        name='px4_romfs_static_files',
        cls_keyword='Copying ROMFS to build directory',
        cls_str='%s -> %s' % (bld.env.PX4_ROMFS_SRC, bld.env.PX4_ROMFS_BLD),
        source=romfs_src_files,
        target=romfs_bld_files,
        group='dynamic_sources',
        rule='${CP} -a -T ${PX4_ROMFS_SRC_ABS} ${PX4_ROMFS_BLD}',
    )

    romfs_bootloader = romfs_bld.make_node('bootloader/fmu_bl.bin')
    romfs_bootloader.parent.mkdir()
    _px4_taskgen(
        bld,
        name='px4_romfs_bootloader',
        cls_keyword='Copying bootloader to ROMFS',
        source=bld.env.PX4_BOOTLOADER,
        target=romfs_bootloader,
        group='dynamic_sources',
        rule='${CP} ${SRC} ${TGT}',
    )
