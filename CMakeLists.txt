cmake_minimum_required(VERSION 4.2)
project(Brass-Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External libraries
find_package(SFML COMPONENTS Graphics Window System Audio REQUIRED)
find_package(EnTT REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Source files of the project
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.h
)
add_executable(Brass-Engine main.cpp ${SOURCE_FILES}
        src/Model/BuildSystem/MarkerId.h
        src/Model/BuildSystem/CellContents.h
        src/Model/BuildSystem/MarkerRegistry.cpp
        src/Model/BuildSystem/MarkerRegistry.h
        src/Model/BuildSystem/Predicates/ICellPredicate.h
        src/Model/BuildSystem/Predicates/CellPredicates.cpp
        src/Model/BuildSystem/Predicates/CellPredicates.h
        src/Model/BuildSystem/BuildableLoader.cpp
        src/Model/BuildSystem/BuildableLoader.h
        src/Core/JsonUtil.h
        src/Model/Behaviours/Kinetic/KineticDef.h
        src/Model/Database/KineticDatabase.cpp
        src/Model/Database/KineticDatabase.h
        src/Model/Behaviours/IBehaviourModule.h
        src/Model/Behaviours/Kinetic/KineticModule.cpp
        src/Model/Behaviours/Kinetic/KineticModule.h
        src/View/ECS/Systems/KineticFrameSystem.h
        src/Core/Graphics/AtlasPacker.cpp
        src/Core/Graphics/AtlasPacker.h
        src/Core/Graphics/TextureAtlas.cpp
        src/Core/Graphics/TextureAtlas.h
        src/Model/ECS/Components/KineticComponent.h
        src/Model/ECS/Systems/KineticSystem.cpp
        src/Model/ECS/Systems/KineticSystem.h
        src/Model/Behaviours/Kinetic/KineticPreview.cpp
        src/Model/Behaviours/Kinetic/KineticPreview.h
        src/Model/Behaviours/Kinetic/KineticDebug.h
        src/View/Gameplay/TileMap/BuildableVisuals.h
        src/View/Gameplay/TileMap/BuildableVisualLoader.cpp
        src/View/Gameplay/TileMap/BuildableVisualLoader.h
        src/View/ECS/Systems/AnimationAttachSystem.h
        src/View/Gameplay/Camera.h
        src/Controller/World/CameraController.cpp
        src/Controller/World/CameraController.h
        src/Controller/World/BuildController.cpp
        src/Controller/World/BuildController.h
        src/View/Gameplay/TileMap/BuildCursor.h
        "src/View/States Renderers/FactoryRenderer.cpp"
        "src/View/States Renderers/FactoryRenderer.h"
        src/Model/Map/WorldBounds.h
)

# PCH
target_precompile_headers(Brass-Engine PRIVATE
        <SFML/Graphics.hpp>
        <SFML/Window.hpp>
        <SFML/System.hpp>
        <nlohmann/json.hpp>
        <vector>
        <unordered_map>
        <map>
        <memory>
        <cstdint>
        <string>
        <algorithm>
        <functional>
        <iostream>
        <thread>
        <mutex>
        <atomic>
)

target_link_libraries(Brass-Engine PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio EnTT::EnTT nlohmann_json::nlohmann_json)

# Resources
file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/Resources/*)


add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/resources.stamp
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:Brass-Engine>/Resources
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/resources.stamp
        DEPENDS ${RESOURCE_FILES}
        COMMENT "Copying Resources to build directory"
)

add_custom_target(CopyResources DEPENDS ${CMAKE_BINARY_DIR}/resources.stamp)
add_dependencies(Brass-Engine CopyResources)