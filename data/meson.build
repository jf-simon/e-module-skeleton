cmd = [ edje_cmd,
        '-id', join_paths(meson.source_root(), 'data'),
        '@INPUT@', '@OUTPUT@'
      ]
custom_target('module-icon-' + name + '.edj',
              input        : 'module-icon.edc',
              output       : 'e-module-' + name + '.edj',
              command      : cmd,
              install_dir  : module_dir,
              install_mode : 'rw-r--r--',
              install      : true
             )
install_data('module.desktop',
             install_dir   : module_dir)
