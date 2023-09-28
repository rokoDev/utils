include(FetchContent)

# Setup directory to which dependencies will be downloaded
cmake_path(APPEND FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/../" "deps_content" "${CMAKE_GENERATOR_NAME_WITHOUT_SPACES}")

# If dependencies are not available then download it and make available
FetchContent_Declare(
	sysops
	GIT_REPOSITORY https://github.com/rokoDev/sysops.git
	GIT_TAG        257fe5aff4971087e37ad480bf4e5ced2059dde9
)

# If necessary it's possible to use sysops from different location instead of downloaded from GitHub
# string(TOUPPER sysops UP_sysops)
# set(FETCHCONTENT_SOURCE_DIR_${UP_sysops} ${CMAKE_SOURCE_DIR}/../sysops)

FetchContent_MakeAvailable(sysops)
target_link_libraries(utils INTERFACE sysops)

# If use IDE add strong_type target into deps source group
set_target_properties(utils PROPERTIES FOLDER deps)
