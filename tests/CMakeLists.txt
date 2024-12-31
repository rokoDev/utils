cmake_minimum_required(VERSION ${cmake_version})

set(ProjectName ${ProjectName}_tests)
project(${ProjectName})

include(FetchContent)

FetchContent_Declare(
  googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG 9d43b27f7a873596496a2ea70721b3f9eb82df01
)

FetchContent_Declare(
  type_name
  GIT_REPOSITORY https://github.com/rokoDev/type_name.git
  GIT_TAG        2272efbcdc5c7707f5dd30df2fa3185fdb499ad0
)

FetchContent_Declare(
  user_literals
  GIT_REPOSITORY https://github.com/rokoDev/user_literals.git
  GIT_TAG        4064baa2bacba91eb424a4fa5bbdb9ae019c0e0b
)

FetchContent_MakeAvailable(googletest type_name user_literals)

add_library(tests_main STATIC)
target_sources(tests_main
  PRIVATE
  src/main_tests.cpp
  )
target_link_libraries(tests_main PUBLIC gtest type_name user_literals)
get_target_property(tests_main_sources tests_main SOURCES)
source_group(
  TREE   ${CMAKE_CURRENT_SOURCE_DIR}/src
  FILES  ${tests_main_sources}
)

if (WIN32)
	option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
	option(GTEST_DISABLE_PTHREADS "Disable uses of pthreads in gtest." ON)
  option(GTEST_FORCE_SHARED_CRT "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
endif (WIN32)


function(package_add_test)
  set(prefix ARG)
  set(noValues)
  set(singleValues TEST_TARGET_NAME)
  set(multiValues
    TEST_SOURCES
    EXTRA_TARGETS
    DEFS
    )

  cmake_parse_arguments(${prefix}
                        "${noValues}"
                        "${singleValues}"
                        "${multiValues}"
                        ${ARGN})

  foreach(arg IN LISTS noValues singleValues multiValues)
      set(${arg} ${${prefix}_${arg}})
  endforeach()

  add_executable(${TEST_TARGET_NAME})
  target_sources(${TEST_TARGET_NAME} PRIVATE ${TEST_SOURCES})
  target_include_directories(${TEST_TARGET_NAME} PUBLIC include)
  foreach(target_to_link IN LISTS EXTRA_TARGETS)
      target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${target_to_link})
  endforeach()

  foreach(define IN LISTS DEFS)
      target_compile_definitions(${TEST_TARGET_NAME} PRIVATE ${define})
  endforeach()

  add_test(NAME ${TEST_TARGET_NAME} COMMAND ${TEST_TARGET_NAME})

  # Create groups in the IDE which mirrors directory structure on the hard disk
  get_target_property(rabbit_tests_src ${TEST_TARGET_NAME} SOURCES)
  source_group(
    TREE   ${CMAKE_CURRENT_SOURCE_DIR}#/src
    FILES  ${rabbit_tests_src}
  )

  # Place all tests targets under "test" source group in IDE
  set_target_properties(${TEST_TARGET_NAME} PROPERTIES FOLDER tests)

  # If colours in googletest are not disabled then in some versions of commandline tools which is used in Xcode 13.3 this
  # can lead to output with escape sequences. Because of that its better to disable colours in googletest.
  set_target_properties(${TEST_TARGET_NAME} PROPERTIES XCODE_GENERATE_SCHEME ON)
  set_target_properties(${TEST_TARGET_NAME} PROPERTIES XCODE_SCHEME_ARGUMENTS "--gtest_color=no")
endfunction()

set(test_src
  src/utils_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_common_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main gmock
  )

set(test_src
  src/type_list_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_type_list_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/value_list_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_value_list_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/function_traits_tests.cpp
  src/function_traits_member_type_checkers_tests.cpp
  src/function_traits_method_checkers_tests.cpp
  src/function_traits_static_method_checkers_tests.cpp
  src/function_traits_member_data_checkers_tests.cpp
  src/function_traits_static_member_data_checkers_tests.cpp
  src/function_traits_free_function_checkers_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_function_traits_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/memvcmp_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_memvcmp_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/memory_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_memory_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main gmock
  )

set(test_src
  src/ctz_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_ctz_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/clz_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_clz_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/simple_resource_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_simple_resource_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main gmock
  )

set(test_src
  src/timer_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_timer_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/fast_pimpl_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_fast_pimpl_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main
  )

set(test_src
  src/monotonic_buffer_resource_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_monotonic_buffer_resource_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main gmock
  )

set(test_src
  src/allocator_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_allocator_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main gmock
  )

set(test_src
  src/padding_checker_tests.cpp
  )
package_add_test(
  TEST_TARGET_NAME utils_padding_checker_tests
  TEST_SOURCES ${test_src}
  EXTRA_TARGETS utils tests_main gmock
  )

# If use IDE add gtest, gmock, gtest_main and gmock_main targets into deps/googletest group
set_target_properties(gtest gmock gtest_main gmock_main PROPERTIES FOLDER tests/deps/googletest)
set_target_properties(type_name user_literals PROPERTIES FOLDER tests/deps)
