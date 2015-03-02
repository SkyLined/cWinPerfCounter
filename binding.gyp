{
  "targets": [
    {
      "target_name": "cWinPerfCounter",
      "sources": [ "src/cWinPerfCounter.cpp" ],
      "libraries": [ "Pdh.lib" ],
      "include_dirs" : [
          "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}