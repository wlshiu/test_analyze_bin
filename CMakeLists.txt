INCLUDE_DIRECTORIES(
    .
    ${INC_DIRS}
    ${PROJECT_SOURCE_DIR}/lib/jsonc
    )

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}
    main.c
    parse_json.c
    parse_json.h
    util.h
    )

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME}
    jsonc
    )

configure_file(
	${PROJECT_SOURCE_DIR}/main/bin_parser.json
    ${CMAKE_BINARY_DIR}/data/private/bin_parser.json
    COPYONLY)
    
# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
