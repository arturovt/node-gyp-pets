{
  'targets': [
    {
      'target_name': 'libobdii',
      'type': 'static_library',
      'sources': [
        'libobdii/src/OBDII.h',
        'libobdii/src/OBDIICommunication.h',
        'libobdii/src/OBDII.c',
        'libobdii/src/OBDIICommunication.c'
      ],
      'include_dirs': [
        'libobdii/src'
      ],
      'cflags': [
        '-w'
      ]
    }
  ]
}
