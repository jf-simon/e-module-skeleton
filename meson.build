project('gad', 'c',
        version         : '0.0.0',
        license         : 'BSD 2 clause',
        default_options : [ 'buildtype=plain', 'c_std=gnu99' ],
        meson_version   : '>= 0.47.0')
name='gad'

## dependencies and options
enlightenment = dependency('enlightenment', version: '>= 0.23.99')
edje = dependency('edje')
edje_cmd = get_option('edje-cc')
if edje_cmd == ''
  edje_cmd = join_paths(edje.get_pkgconfig_variable('prefix'),
                        'bin', 'edje_cc')
endif
home = get_option('home')
if (home == '')
  enlightenment_modules = enlightenment.get_pkgconfig_variable('modules')
else
  enlightenment_modules = join_paths(home, '.e', 'e', 'modules')
endif

enlightenment_module_arch = enlightenment.get_pkgconfig_variable('module_arch')
module_dir = join_paths(enlightenment_modules, name)
module_bin = join_paths(module_dir, enlightenment_module_arch)

subdir('src')
subdir('data')
