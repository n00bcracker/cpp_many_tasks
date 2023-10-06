option(TEST_SOLUTION "Build solution" OFF)
option(ENABLE_PRIVATE_TESTS "Enable private tests" OFF)

function(patch_include_directories TARGET PATH_TO_TASK)
  if (TEST_SOLUTION)
    target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/tests/${PATH_TO_TASK})
  endif()

  target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(prepend VAR PREFIX)
  set(LIST_VAR "")
  foreach(ELEM ${ARGN})
    list(APPEND LIST_VAR "${PREFIX}/${ELEM}")
  endforeach()
  set(${VAR} "${LIST_VAR}" PARENT_SCOPE)
endfunction()

function(add_shad_executable NAME)
  set(MULTI_VALUE_ARGS PRIVATE_TESTS SOLUTION_SRCS)
  cmake_parse_arguments(SHAD_LIBRARY "" "" "${MULTI_VALUE_ARGS}" ${ARGN})

  file(RELATIVE_PATH PATH_TO_TASK ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  if (TEST_SOLUTION)
    prepend(SHAD_LIBRARY_SOLUTION_SRCS
      ${PROJECT_SOURCE_DIR}/tests/${PATH_TO_TASK}
      ${SHAD_LIBRARY_SOLUTION_SRCS})
  endif()

  if (ENABLE_PRIVATE_TESTS)
    prepend(SHAD_LIBRARY_PRIVATE_TESTS
      ${PROJECT_SOURCE_DIR}/tests/${PATH_TO_TASK}
      ${SHAD_LIBRARY_PRIVATE_TESTS})
  endif()

  add_executable(${NAME}
    ${SHAD_LIBRARY_UNPARSED_ARGUMENTS}
    ${SHAD_LIBRARY_SOLUTION_SRCS}
    ${SHAD_LIBRARY_PRIVATE_TESTS})
  
  set_target_properties(${NAME} PROPERTIES COMPILE_FLAGS "-Wall -Werror -Wextra -Wpedantic")
  patch_include_directories(${NAME} ${PATH_TO_TASK})
endfunction()

add_custom_target(test-all)

function(add_catch TARGET)
  add_shad_executable(${TARGET} ${ARGN})
  target_link_libraries(${TARGET} PRIVATE Catch2::Catch2WithMain)

  if (TEST_SOLUTION)
    add_custom_target(
      run_${TARGET}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${TARGET}
      COMMAND ${CMAKE_BINARY_DIR}/${TARGET})
    add_dependencies(test-all run_${TARGET})
  endif()
endfunction()
