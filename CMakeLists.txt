cmake_minimum_required(VERSION 3.15)
project(MySFMLApp)

set(CMAKE_CXX_STANDARD 17)

# Đường dẫn SFML config nếu cần
set(SFML_DIR "D:/COMPILER/SFML-3.0.0/lib/cmake/SFML")

find_package(SFML 3.0 COMPONENTS System Window Graphics Audio REQUIRED)

add_executable(MySFMLApp Ball-Game.cpp)

target_link_libraries(MySFMLApp PRIVATE SFML::System SFML::Window SFML::Graphics SFML::Audio)
target_include_directories(MySFMLApp PRIVATE ${SFML_INCLUDE_DIRS})
