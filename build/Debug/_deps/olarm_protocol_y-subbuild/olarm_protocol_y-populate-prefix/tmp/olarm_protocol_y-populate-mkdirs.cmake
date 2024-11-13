# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-src"
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-build"
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix"
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/tmp"
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp"
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src"
  "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/tashriqpandy/Documents/mycode/SubGHz_Phy_PingPong/build/Debug/_deps/olarm_protocol_y-subbuild/olarm_protocol_y-populate-prefix/src/olarm_protocol_y-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
