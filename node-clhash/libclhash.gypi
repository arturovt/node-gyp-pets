{
  'targets': [
    {
      'target_name': 'libclhash',
      'type': 'static_library',
      'include_dirs': ['libclhash/include'],
      'sources': ['libclhash/src/clhash.c'],
      'defines': ['NDEBUG'],
      'cflags': [
        '-O3',
        '-Wall',
        '-Wextra',
        '-msse4.2',
        '-mpclmul',
        '-march=native',
        '-funroll-loops'
      ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-msse4.2',
              '-mpclmul',
              '-march=native'
            ]
          }
        }]
      ]
    }
  ]
}
