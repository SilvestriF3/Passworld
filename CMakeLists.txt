cmake_minimum_required(VERSION 3.2)
if(WIN32)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()
project(Passworld)

add_subdirectory(external)

add_subdirectory(src/Passworld)
