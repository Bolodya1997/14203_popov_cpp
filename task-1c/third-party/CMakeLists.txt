include(ExternalProject)

##################################################
#   gtest
##################################################

find_package(Threads REQUIRED)

# Use GIT if you can, if not - use URL
find_package(Git)
if(GIT_FOUND)
    ExternalProject_Add(gtest
            GIT_REPOSITORY  https://github.com/google/googletest.git
            GIT_TAG         master
            PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/gtest
            # Disable install step
            INSTALL_COMMAND ""
            )
else()
    ExternalProject_Add(gtest
            URL             https://github.com/google/googletest/archive/master.zip
            PREFIX          ${CMAKE_CURRENT_BINARY_DIR}/gtest
            # Disable install step
            INSTALL_COMMAND ""
            )
endif()

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

# Set libgmock properties
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Interface variables
set(GTEST_LIB
        libgtest
        libgmock PARENT_SCOPE)
set(GTEST_INCLUDE
        ${source_dir}/googletest/include
        ${source_dir}/googlemock/include PARENT_SCOPE)