set(fluky_FOUND YES)

include(CMakeFindDependencyMacro)
find_dependency(fmt)

if(fluky_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/flukyTargets.cmake")
endif()
