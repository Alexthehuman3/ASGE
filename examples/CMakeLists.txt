add_executable(asgegame helloasge/ASGEGame.cpp
               helloasge/src/MapLoader.cpp
               helloasge/headers/MapLoader.hpp
               helloasge/CustomIncludes/FileWatch.hpp
        )
target_link_libraries(asgegame PRIVATE asge tmxlite)

add_custom_command(
        TARGET asgegame  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/data/tilemap/testTile.tmx
        ${CMAKE_BINARY_DIR}/bin/data)