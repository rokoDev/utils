include(FetchContent)

# If dependencies are not available then download it and make available
FetchContent_Declare(
	sysops
	GIT_REPOSITORY https://github.com/rokoDev/sysops.git
	GIT_TAG        b892538a36c4ec55cab20161dfc16816fc9ccbd3
)

FetchContent_Declare(
	boost_pfr
	GIT_REPOSITORY https://github.com/boostorg/pfr.git
	GIT_TAG        f09e6aeae9d050897fff72b93d5f5e866cc5e11a
)

# If necessary it's possible to use sysops from different location instead of downloaded from GitHub
# string(TOUPPER sysops UP_sysops)
# set(FETCHCONTENT_SOURCE_DIR_${UP_sysops} ${CMAKE_SOURCE_DIR}/../sysops)

FetchContent_MakeAvailable(sysops boost_pfr)
target_link_libraries(utils INTERFACE sysops boost_pfr)

# If use IDE add strong_type target into deps source group
set_target_properties(sysops boost_pfr PROPERTIES FOLDER deps)
