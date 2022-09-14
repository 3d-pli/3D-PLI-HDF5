# gets version from [0-9].[0-9].[0-9] tag
function(gitversion)
  if(NOT GIT_EXECUTABLE)
    find_package(Git REQUIRED)
  endif()

  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --long
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE git_describe
    ERROR_VARIABLE git_error
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
  if(NOT git_result EQUAL 0)
    message(WARNING "No git tags available. Please run git fetch --all --tags.")
    set(git_describe "v0.0.0-000-00000000")
  endif()

  string(REPLACE "-" ";" git_list ${git_describe})

  list(GET git_list 0 project_git_version)
  list(GET git_list 1 project_git_commit)
  list(GET git_list 2 project_git_hash)
  string(REPLACE "v" "" project_git_version ${project_git_version})

  set(project_git_version
      ${project_git_version}
      PARENT_SCOPE)
  set(project_git_commit
      ${project_git_commit}
      PARENT_SCOPE)
  set(project_git_hash
      ${project_git_hash}
      PARENT_SCOPE)
endfunction()
