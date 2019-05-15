project(GBemu)

set(SRC_FOLDER ${CMAKE_SOURCE_DIR}/src)


set(SOURCE_FILES_LIBGB
    ${SRC_FOLDER}/hardware/System.cpp
    ${SRC_FOLDER}/hardware/Mmu.cpp
    ${SRC_FOLDER}/hardware/Cpu.cpp
)
add_library(gb ${SOURCE_FILES_LIBGB})


set(SOURCE_FILES_GBEMU
    ${SRC_FOLDER}/main.cpp
)
add_executable(GBemu ${SOURCE_FILES_GBEMU})
target_link_libraries(GBemu gb)