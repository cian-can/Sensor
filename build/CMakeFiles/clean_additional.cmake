# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Sensor_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Sensor_autogen.dir/ParseCache.txt"
  "Sensor_autogen"
  )
endif()
