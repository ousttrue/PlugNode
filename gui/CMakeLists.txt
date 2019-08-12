SET(SUB_NAME gui)
SET(DEPENDENCIES_DIR ${CMAKE_CURRENT_LIST_DIR}/../dependencies)

FILE(GLOB SRC
    *.cpp
    **/*.cpp
    ) 

ADD_LIBRARY(${SUB_NAME}
    ${SRC}
    )
TARGET_COMPILE_DEFINITIONS(${SUB_NAME} PUBLIC
    )
TARGET_INCLUDE_DIRECTORIES(${SUB_NAME} PUBLIC
    ${DEPENDENCIES_DIR}/plog/include
    ${DEPENDENCIES_DIR}/imgui
    ${DEPENDENCIES_DIR}/imgui/examples
    ${DEPENDENCIES_DIR}/lua
    ${DEPENDENCIES_DIR}/perilune/include
    ../common
    .
    )
TARGET_LINK_LIBRARIES(${SUB_NAME}
    )
