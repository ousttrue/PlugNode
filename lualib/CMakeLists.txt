SET(SUB_NAME lualib)
SET(DEPENDENCIES_DIR ${CMAKE_CURRENT_LIST_DIR}/../dependencies)
SET(LUA_DIR ${DEPENDENCIES_DIR}/lua)

ADD_LIBRARY(${SUB_NAME}
    # lua core
    ${LUA_DIR}/lapi.c
    ${LUA_DIR}/lcode.c
    ${LUA_DIR}/lctype.c
    ${LUA_DIR}/ldebug.c
    ${LUA_DIR}/ldo.c
    ${LUA_DIR}/ldump.c
    ${LUA_DIR}/lfunc.c
    ${LUA_DIR}/lgc.c
    ${LUA_DIR}/llex.c
    ${LUA_DIR}/lmem.c
    ${LUA_DIR}/lobject.c
    ${LUA_DIR}/lopcodes.c
    ${LUA_DIR}/lparser.c
    ${LUA_DIR}/lstate.c
    ${LUA_DIR}/lstring.c
    ${LUA_DIR}/ltable.c
    ${LUA_DIR}/ltm.c
    ${LUA_DIR}/lundump.c
    ${LUA_DIR}/lvm.c
    ${LUA_DIR}/lzio.c
    # lua lib
    ${LUA_DIR}/lauxlib.c
    ${LUA_DIR}/lbaselib.c
    ${LUA_DIR}/lbitlib.c
    ${LUA_DIR}/lcorolib.c
    ${LUA_DIR}/ldblib.c
    ${LUA_DIR}/liolib.c
    ${LUA_DIR}/lmathlib.c
    ${LUA_DIR}/loslib.c
    ${LUA_DIR}/lstrlib.c
    ${LUA_DIR}/ltablib.c
    ${LUA_DIR}/lutf8lib.c
    ${LUA_DIR}/loadlib.c
    ${LUA_DIR}/linit.c
    )
TARGET_COMPILE_DEFINITIONS(${SUB_NAME} PUBLIC
    )
TARGET_INCLUDE_DIRECTORIES(${SUB_NAME} PUBLIC
    ${LUA_DIR}
    )
TARGET_LINK_LIBRARIES(${SUB_NAME}
    )
