# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\InterfazProyectoProgra_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\InterfazProyectoProgra_autogen.dir\\ParseCache.txt"
  "InterfazProyectoProgra_autogen"
  )
endif()
