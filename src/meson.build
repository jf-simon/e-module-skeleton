src = [
  'e_mod_main.c', 'e_mod_main.h'
]
shared_module('module', src,
              name_prefix        : '',
              dependencies       : [ enlightenment ],
              install_dir        : module_bin,
              install            : true,
              link_args          : '-Wl,--unresolved-symbols=ignore-all'
             )
