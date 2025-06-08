cmake_minimum_required(VERSION 3.5)

project(ae)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)
find_package(EnTT 3.16 REQUIRED)
find_package(tomlplusplus)
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)

add_subdirectory(3rd/embed)

add_library(ae SHARED
    3rd/sigslot/signal.hpp
    3rd/stb/stb_image.h
    3rd/stb/stb_image_write.h
    3rd/stb/stb_vorbis.c
    ae/animation.h ae/animation.cpp
    ae/animation_manager.h ae/animation_manager.cpp
    ae/engine.h ae/engine.cpp
    ae/engine_context.h ae/engine_context.cpp
    ae/engine_context_object.h ae/engine_context_object.cpp
    ae/engine_data.h
    ae/assets/asset_loader.h
    ae/assets/asset_loaders.h
    ae/assets/assets.h
    ae/assets/font_loader.h
    ae/assets/model_loader.h
    ae/assets/shader_loader.h
    ae/assets/sound_buffer_loader.h
    ae/assets/texture_loader.h
    ae/audio/audio_device.h ae/audio/audio_device.cpp
    ae/audio/audio_stream.h ae/audio/audio_stream.cpp
    ae/audio/enums.h
    ae/audio/music.h ae/audio/music.cpp
    ae/audio/sound.h ae/audio/sound.cpp
    ae/audio/sound_buffer.h ae/audio/sound_buffer.cpp
    ae/audio/sound_source.h ae/audio/sound_source.cpp
    ae/audio/utils.h ae/audio/utils.cpp
    ae/collisions/colliders.h
    ae/collisions/collision_result.h
    ae/collisions/collisions.h ae/collisions/collisions.cpp
    ae/common/consts.h
    ae/common/glm_utils.h ae/common/glm_utils.cpp
    ae/common/utils.h ae/common/utils.cpp
    ae/config.h ae/config.cpp
    ae/game_state.h ae/game_state.cpp
    ae/game_state_stack.h ae/game_state_stack.cpp
    ae/geometry/frustum.h ae/geometry/frustum.cpp
    ae/geometry/geometry_utils.h ae/geometry/geometry_utils.cpp
    ae/geometry/primitives.h ae/geometry/primitives.cpp
    ae/geometry/triangles_node.h
    ae/graphics/common/enums.h
    ae/graphics/common/utils.h ae/graphics/common/utils.cpp
    ae/graphics/core/batch_2d.h ae/graphics/core/batch_2d.cpp
    ae/graphics/core/buffer.h ae/graphics/core/buffer.cpp
    ae/graphics/core/color.h ae/graphics/core/color.cpp
    ae/graphics/core/default_shaders.h ae/graphics/core/default_shaders.cpp
    ae/graphics/core/font.h ae/graphics/core/font.cpp
    ae/graphics/core/glyph.h
    ae/graphics/core/material.h
    ae/graphics/core/quad.h ae/graphics/core/quad.cpp
    ae/graphics/core/render_state.h
    ae/graphics/core/render_target.h ae/graphics/core/render_target.cpp
    ae/graphics/core/render_texture.h ae/graphics/core/render_texture.cpp
    ae/graphics/core/shader.h ae/graphics/core/shader.cpp
    ae/graphics/core/texture.h ae/graphics/core/texture.cpp
    ae/graphics/core/vertex.h
    ae/graphics/core/vertex_array.h ae/graphics/core/vertex_array.cpp
    ae/graphics/core/vertex_attrib.h
    ae/graphics/scene/assimp_helper.h ae/graphics/scene/assimp_helper.cpp
    ae/graphics/scene/drawable.h ae/graphics/scene/drawable.cpp
    ae/graphics/scene/mesh.h ae/graphics/scene/mesh.cpp
    ae/graphics/scene/model.h ae/graphics/scene/model.cpp
    ae/graphics/scene/model_instance.h ae/graphics/scene/model_instance.cpp
    ae/graphics/scene/pose.h ae/graphics/scene/pose.cpp
    ae/graphics/scene/pose_animation.h ae/graphics/scene/pose_animation.cpp
    ae/graphics/scene/pose_animator.h ae/graphics/scene/pose_animator.cpp
    ae/graphics/scene/shape.h ae/graphics/scene/shape.cpp
    ae/graphics/scene/skeleton.h ae/graphics/scene/skeleton.cpp
    ae/graphics/scene/skybox.h ae/graphics/scene/skybox.cpp
    ae/graphics/scene/sprite.h ae/graphics/scene/sprite.cpp
    ae/gui/button_base.h ae/gui/button_base.cpp
    ae/gui/buttons_group.h ae/gui/buttons_group.cpp
    ae/gui/control.h ae/gui/control.cpp
    ae/gui/gui.h ae/gui/gui.cpp
    ae/gui/image.h ae/gui/image.cpp
    ae/gui/input_text_base.h ae/gui/input_text_base.cpp
    ae/gui/label.h ae/gui/label.cpp
    ae/gui/progress_bar_base.h ae/gui/progress_bar_base.cpp
    ae/input_action_manager.h ae/input_action_manager.cpp
    ae/level.h ae/level.cpp
    ae/scene/bvh.h
    ae/scene/components.h
    ae/scene/draw_s.h ae/scene/draw_s.cpp
    ae/scene/lights_s.h ae/scene/lights_s.cpp
    ae/scene/movement_s.h ae/scene/movement_s.cpp
    ae/scene/multi_component_watcher.h
    ae/scene/player_s.h ae/scene/player_s.cpp
    ae/scene/scene.h ae/scene/scene.cpp
    ae/scene/scene_context.h ae/scene/scene_context.cpp
    ae/scene/scene_data.h
    ae/scene/shadows_s.h ae/scene/shadows_s.cpp
    ae/scene/system.cpp
    ae/scene/system.h
    ae/system/clock.cpp ae/system/clock.h ae/system/time.cpp ae/system/time.h
    ae/system/files.h ae/system/files.cpp
    ae/system/log.h
    ae/system/memory.h
    ae/system/string.h ae/system/string.cpp
    ae/task.h ae/task.cpp
    ae/task_manager.h ae/task_manager.cpp
    ae/window/input.h ae/window/input.cpp
    ae/window/window.h ae/window/window.cpp
    shaders/gui.frag
    shaders/gui.vert
    shaders/light.inc
    shaders/main.frag
    shaders/main.vert
    shaders/material.inc
    shaders/old_terminal_screen_quad.frag
    shaders/screen_quad.frag
    shaders/screen_quad.vert
    shaders/skybox.frag
    shaders/skybox.vert
)

target_link_libraries(ae PRIVATE
    glfw
    OpenGL::OpenGL
    GLEW::glew
    glm::glm
    spdlog::spdlog
    assimp::assimp
    EnTT::EnTT
    tomlplusplus::tomlplusplus
    Freetype::Freetype
    OpenAL::OpenAL
)

b_embed(ae shaders/main.vert)
b_embed(ae shaders/main.frag)
b_embed(ae shaders/light.inc)
b_embed(ae shaders/material.inc)
b_embed(ae shaders/skybox.vert)
b_embed(ae shaders/skybox.frag)
b_embed(ae shaders/screen_quad.vert)
b_embed(ae shaders/screen_quad.frag)
b_embed(ae shaders/old_terminal_screen_quad.frag)
b_embed(ae shaders/gui.vert)
b_embed(ae shaders/gui.frag)

b_embed(ae fonts/default.ttf)


