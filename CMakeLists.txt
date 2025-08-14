cmake_minimum_required(VERSION 3.15)
project(Ball-Game)

set(CMAKE_CXX_STANDARD 17)

# Thay bằng đường dẫn SFML của bạn nếu cần
set(SFML_DIR "D:/COMPILER/SFML-3.0.0/lib/cmake/SFML")
find_package(SFML 3.0 COMPONENTS System Window Graphics Audio REQUIRED)

# --- ĐIỂM QUAN TRỌNG: xuất exe vào thư mục nguồn/Ball-Game ---
set(EXE_OUTPUT_DIR "${CMAKE_SOURCE_DIR}")

# Đặt cho cả single-config và multi-config (Debug/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXE_OUTPUT_DIR})

add_executable(Ball-Game Ball-Game.cpp)

target_link_libraries(Ball-Game PRIVATE SFML::System SFML::Window SFML::Graphics SFML::Audio)
target_include_directories(Ball-Game PRIVATE ${SFML_INCLUDE_DIRS})

# Đảm bảo target properties (một lần nữa, an toàn cho mọi generator)
set_target_properties(Ball-Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXE_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXE_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXE_OUTPUT_DIR}"
)

# --- (TÙY CHỌN) Copy các DLL của SFML về cùng thư mục exe để chạy được trên Windows ---
# Điều chỉnh SFML_BIN_DIR nếu SFML .dll nằm ở chỗ khác (thường là .../bin)
if(NOT DEFINED SFML_BIN_DIR)
    set(SFML_BIN_DIR "D:/COMPILER/SFML-3.0.0/bin")
endif()

if(EXISTS "${SFML_BIN_DIR}")
    file(GLOB SFML_DLLS "${SFML_BIN_DIR}/*.dll")
    if(SFML_DLLS)
        add_custom_command(TARGET Ball-Game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_DLLS} "${EXE_OUTPUT_DIR}"
        )
    endif()
endif()
