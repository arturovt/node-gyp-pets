{
  'targets': [
    {
      'target_name': 'node_clhash',
      'cflags!': ['-fno-exceptions'],
      'cflags_cc!': ['-fno-exceptions'],
      'include_dirs': [
        'libclhash/include',
        '<!@(node -p \'require("node-addon-api").include\')',
      ],
      'sources': ['src/node_clhash.h', 'src/node_clhash.cc'],
      'cflags_cc': ['-std=c++14', '-Wall', '-Wextra'],
      'dependencies': [
        'libclhash.gypi:libclhash',
        '<!(node -p \'require("node-addon-api").gyp\')'
      ],
      'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS']
    }
  ]
}
