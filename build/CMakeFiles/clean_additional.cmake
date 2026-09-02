# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/ForestBreedMonitor_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ForestBreedMonitor_autogen.dir/ParseCache.txt"
  "ForestBreedMonitor_autogen"
  )
endif()
