#!/usr/bin/env python
# encoding: utf-8

APPLICATION_NAME='JackClient'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_cxx doxygen')

def configure(conf):
    conf.load('compiler_cxx doxygen')

    conf.env.CXXFLAGS = ['-Wall', '-O3']

    conf.check_cfg(package='jack', args='--cflags --libs', uselib_store='JACK')

def build(bld):
    
    # Build library
    bld.stlib(
        features    = 'cxx',
        source      = bld.path.ant_glob('src/**.cc'),
        includes    = 'include',
        use         = 'JACK',
        target      = APPLICATION_NAME
    )

    # Build example
    bld.program(
        features = 'cxx',
        source   = bld.path.ant_glob('examples/simple_client/**.cc'),
        includes = 'include .',
        use      = 'JackClient',
        target   = 'simple_client',
    )

    # Generate documentation
    bld(features='doxygen', doxyfile='Doxyfile')
