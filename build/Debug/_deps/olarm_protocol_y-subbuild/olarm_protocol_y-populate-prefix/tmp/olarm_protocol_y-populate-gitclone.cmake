# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

if(EXISTS "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitclone-lastrun.txt" AND EXISTS "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitinfo.txt" AND
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitclone-lastrun.txt" IS_NEWER_THAN "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitinfo.txt")
  message(STATUS
    "Avoiding repeated git clone, stamp file is up to date: "
    "'/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitclone-lastrun.txt'"
  )
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"
            clone --no-checkout --config "advice.detachedHead=false" "git@github.com:OlarmTech/olarm_protocol_y.git" "olarm_protocol_y-src"
    WORKING_DIRECTORY "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps"
    RESULT_VARIABLE error_code
  )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once: ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'git@github.com:OlarmTech/olarm_protocol_y.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"
          checkout "ULPN-628-Limit-Protocol-Y-Testing-to-only-the-Submodule" --
  WORKING_DIRECTORY "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-src"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'ULPN-628-Limit-Protocol-Y-Testing-to-only-the-Submodule'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git" 
            submodule update --recursive --init 
    WORKING_DIRECTORY "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-src"
    RESULT_VARIABLE error_code
  )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitinfo.txt" "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
)
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/olarm_protocol_y-populate-gitclone-lastrun.txt'")
endif()
