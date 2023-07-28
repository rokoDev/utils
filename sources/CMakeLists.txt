target_sources(utils
	PRIVATE
    include/utils/utils.h
    include/utils/type_list.h
    include/utils/value_list.h
    include/utils/function_traits.h
    include/utils/detector.h
    include/utils/memory.h
    include/utils/ctz.h
    include/utils/common.h
  )

set_property(TARGET utils APPEND PROPERTY SRC_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(utils INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
