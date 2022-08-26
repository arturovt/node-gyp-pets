{
  'target_defaults': {
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {
        'defines': [
          'DEBUG'
        ]
      }
    }
  },
  'targets': [
    {
      'target_name': 'node_obdii',
      'cflags!': [
        '-fno-exceptions'
      ],
      'cflags_cc!': [
        '-fno-exceptions'
      ],
      'sources': [
        'src/node_obdii.h',
        'src/node_obdii.cc'
      ],
      'cflags_cc': [
        '-std=c++14'
      ],
      'include_dirs': [
        '<!@(node -p \'require("node-addon-api").include\')',
        '<(module_root_dir)/libobdii/src'
      ],
      'dependencies': [
        'libobdii.gypi:libobdii',
        '<!(node -p \'require("node-addon-api").gyp\')'
      ],
      'defines': [
        'NAPI_DISABLE_CPP_EXCEPTIONS'
      ]
    }
  ]
}
