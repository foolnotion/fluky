cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    fluky
    VERSION 0.1.0
    DESCRIPTION "A collection of pseudorandom number generators"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES NONE
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(fluky_fluky INTERFACE)
add_library(fluky::fluky ALIAS fluky_fluky)

set_property(
    TARGET fluky_fluky PROPERTY
    EXPORT_NAME fluky
)

target_include_directories(
    fluky_fluky ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(fluky_fluky INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT fluky_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of fluky"
  )
endif()

include(cmake/dev-mode.cmake)
