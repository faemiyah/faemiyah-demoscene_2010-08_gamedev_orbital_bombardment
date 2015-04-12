
EnsureSConsVersion(1, 3, 0)

from os import sys, environ

sys.path.insert(0, "./scons") # Add path to addiotional tools

import boost 
import opengl
import sdl
import pkgconfig

def copy_from_env(key, e):
  """
  Copies environment variables form the system to the scons environment
  """
  if environ.has_key(key):
    e.Append(ENV = {key: environ[key]})

#
# Build options
#
opts = Variables('.scons-options-cache')
opts.AddVariables( 
    ('CC', 'C compiler'),
    ('CXX', 'C++ compiler'), 
    ('CCFLAGS', 'Optional flags passed to the C++ compiler'),

    # Library directories and their include dirs
    # This may be a handy on Windows where there's no
    # standard location for libs and headers.
    PathVariable('boost_inc_dir', 'Path to boost headers', None),
    PathVariable('boost_lib_dir', 'Path to boost libs', None),
    PathVariable('sdl_inc_dir', 'Path to SDL headers', None),
    PathVariable('sdl_lib_dir', 'Path to SDL libs', None),
    PathVariable('libpng_inc_dir', 'Path to libpng headers', None),
    PathVariable('libpng_lib_dir', 'Path to libpng libs', None),
    PathVariable('jpeglib_inc_dir', 'Path to jpeglib headers', None),
    PathVariable('jpeglib_lib_dir', 'Path to jpeglib libs', None),
    )

#
# Construct the build environment
#
env = Environment(options = opts)
opts.Save('.scons-options-cache', env)

map(lambda n: copy_from_env(n, env), ['CPATH', 'LIBRARY_PATH', 'PATH'])
map(lambda k: env.Append(\
  CPPPATH = env.get(k+'_inc_dir'),\
  LIBPATH = env.get(k+'_lib_dir')),\
  [
    'boost',
    'sdl',
    'libpng',
    'jpeglib',
  ])

#
# Help
#
Help(opts.GenerateHelpText(env))

if GetOption('help'):
  Return()

#
# Configure environment
#
configure = Configure(env,
    custom_tests = {
      'CheckBoostVersion': boost.CheckBoostVersion,
      'CheckBoostLib': boost.CheckBoostLib,
      'CheckOpenGL': opengl.CheckOpenGL,
      'CheckSDL' : sdl.CheckSDL,
      'CheckPkgConfig' : pkgconfig.CheckPkgConfig,
      'CheckPkg' : pkgconfig.CheckPkg,
      },
    config_h = 'src/config.h',
    )

boost_headers = [
    'program_options',
    'property_tree/ptree',
    'filesystem',
    'smart_ptr',
    'foreach',
    'throw_exception',
    'tuple/tuple',
    'gil/image',
    'gil/extension/io/png_io',
    'gil/extension/io/jpeg_io',
    ]

boost_libs = [
    'system',
    'program_options',
    ]

from operator import and_

if not configure.CheckCXX():
  print "Error: \tWorking C++ compiler needed."
  Exit(1)

has_pkg_config = configure.CheckPkgConfig('0.23')

if not configure.CheckBoostVersion('1.40'):
  print "Error: \tBoost 1.40.0 or newer required"
  Exit(1)

if not reduce(and_, map(lambda hdr:
  configure.CheckCXXHeader('boost/' + hdr + '.hpp'),
  boost_headers )):
  print "Error: \tInstall Boost libraries and try again"
  print "\tNote that you need also png/jpeg io extensions for Boost.GIL"
  Exit(1)

if not reduce(and_, map(lambda lib:
  configure.CheckBoostLib(lib),
  boost_libs)):
  print "Error: \tBoost required..."
  Exit(1)

if not configure.CheckLibWithHeader('glew', 'GL/glew.h', 'C', 'glewInit;'):
  print "Error: \tGlew required..."
  Exit(1)

if not configure.CheckOpenGL():
  print "Error: \tOpenGL SDK required to compile"
  Exit(1)

if not configure.CheckSDL((1,2,8)):
  print "Error: \tSDL required"
  Exit(1)

pkgs = []

if has_pkg_config:
  for pkg in (
      ('libpng12', '1.2'),
      ):
    if configure.CheckPkg(*pkg):
      pkgs.append(pkg[0]) 

if not ( 'libpng12' in pkgs ):
  if not ( \
      configure.CheckLib('z') and \
      configure.CheckLibWithHeader('png12', 'png.h', 'C++', 'png_sig_cmp;') ):
        print "Error: \tLibpng required..."
        Exit(1)

if not configure.CheckLibWithHeader('jpeg', ['stdio.h', 'jpeglib.h'], 'C++'):
  print "Error: \tjpeg library required..."
  Exit(1)

env = configure.Finish()

#
# Master environtment set-up
# Export and build subdirs
#

env.Append(CPPPATH = '#/src')
Export('env')
env.SConscript(dirs=['src'])

