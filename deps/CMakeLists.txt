include(FetchContent)

# If dependencies are not available then download it and make available
FetchContent_Declare(
	sysops
	GIT_REPOSITORY https://github.com/rokoDev/sysops.git
	GIT_TAG        b892538a36c4ec55cab20161dfc16816fc9ccbd3
)

# If necessary it's possible to use sysops from different location instead of downloaded from GitHub
# string(TOUPPER sysops UP_sysops)
# set(FETCHCONTENT_SOURCE_DIR_${UP_sysops} ${CMAKE_SOURCE_DIR}/../sysops)

FetchContent_MakeAvailable(sysops)
target_link_libraries(utils INTERFACE sysops)

# If use IDE add strong_type target into deps source group
set_target_properties(sysops PROPERTIES FOLDER deps)
