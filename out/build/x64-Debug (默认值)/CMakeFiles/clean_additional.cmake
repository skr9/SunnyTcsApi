# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SunnyTcsApi_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SunnyTcsApi_autogen.dir\\ParseCache.txt"
  "SunnyTcsApi_autogen"
  )
endif()
