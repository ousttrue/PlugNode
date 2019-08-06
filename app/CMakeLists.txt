SET(SUB_NAME plugnode)
FILE(GLOB SRC
    *.cpp
    ) 

SET(DEPENDENCIES_DIR ${CMAKE_CURRENT_LIST_DIR}/../dependencies)
SET(LUA_DIR ${DEPENDENCIES_DIR}/lua)

ADD_EXECUTABLE(${SUB_NAME}
    ${SRC}
    # imgui
    ${DEPENDENCIES_DIR}/imgui/imgui.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_draw.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_widgets.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_demo.cpp
    ${DEPENDENCIES_DIR}/imgui/examples/imgui_impl_dx11.cpp
    ${DEPENDENCIES_DIR}/imgui/examples/imgui_impl_win32.cpp
    )
TARGET_COMPILE_DEFINITIONS(${SUB_NAME} PUBLIC
    )
TARGET_INCLUDE_DIRECTORIES(${SUB_NAME} PUBLIC
    ${DEPENDENCIES_DIR}/nlohmann-json/include
    ${DEPENDENCIES_DIR}/plog/include
    ${DEPENDENCIES_DIR}/sol2/include
    ${DEPENDENCIES_DIR}/im3d/examples/common
    ${DEPENDENCIES_DIR}/imgui
    ${DEPENDENCIES_DIR}/imgui/examples
    ${DEPENDENCIES_DIR}/perilune/include
    ${LUA_DIR}
    ../gui
    )
TARGET_LINK_LIBRARIES(${SUB_NAME}
    lualib
    winmm
    d3d11
    d3dcompiler
    gui
    )
