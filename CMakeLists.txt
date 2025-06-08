cmake_minimum_required(VERSION 3.5)

project(Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

add_subdirectory(ae)
include_directories(ae)

# find_package(glfw3 3.3 REQUIRED)
# find_package(OpenGL REQUIRED COMPONENTS OpenGL)
# find_package(GLEW REQUIRED)
# find_package(glm CONFIG REQUIRED)
# find_package(spdlog REQUIRED)

# add_subdirectory(src/ae/3rd/embed)

add_executable(Game src/main.cpp
    src/game/game.h src/game/game.cpp
    src/game/gameplay_state.h src/game/gameplay_state.cpp
    src/game/main_menu_state.h src/game/main_menu_state.cpp
    src/game/first_state.h src/game/first_state.cpp

    src/game/gui/gui_theme.h src/game/gui/gui_theme.cpp
    src/game/gui/main_menu_gui.h src/game/gui/main_menu_gui.cpp
    src/game/gui/controls/frame.h src/game/gui/controls/frame.cpp
    src/game/gui/controls/dialog.h src/game/gui/controls/dialog.cpp
    src/game/gui/controls/button.h src/game/gui/controls/button.cpp
    src/game/gui/controls/input_text.h src/game/gui/controls/input_text.cpp
    src/game/gui/settings_dialog.h src/game/gui/settings_dialog.cpp
    src/game/load_level_state.h src/game/load_level_state.cpp
    src/game/gui/controls/progress_bar.h src/game/gui/controls/progress_bar.cpp
    src/game/gui/load_level_gui.h src/game/gui/load_level_gui.cpp
    src/game/gui/first_state_gui.h src/game/gui/first_state_gui.cpp
    src/game/gui/controls/tab_button.h src/game/gui/controls/tab_button.cpp
    src/game/gui/controls/fake_terminal.h src/game/gui/controls/fake_terminal.cpp
    src/game/gui/controls/fake_terminal_button.h src/game/gui/controls/fake_terminal_button.cpp
    src/game/gui/controls/tabs_container.h src/game/gui/controls/tabs_container.cpp
    src/game/gui/style.h src/game/gui/style.cpp


)

target_link_libraries(Game PRIVATE ae
    spdlog::spdlog
    assimp::assimp
    OpenGL::OpenGL
    GLEW::glew
)

# target_link_libraries(Game PRIVATE
#     glfw
#     OpenGL::OpenGL
#     GLEW::glew
#     glm::glm
#     spdlog::spdlog
# )

include(GNUInstallDirs)
install(TARGETS Game
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
