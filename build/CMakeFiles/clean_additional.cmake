# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/SensorViz_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/SensorViz_autogen.dir/ParseCache.txt"
  "SensorViz_autogen"
  )
endif()
