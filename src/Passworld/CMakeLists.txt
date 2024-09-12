add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 20
    )
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED) 
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw imgui-opengl3 imgui-glfw)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
target_sources(${PROJECT_NAME} PUBLIC Passworld.cpp
 PRIVATE
 generator.cpp
 generator.h
 rule.cpp
 rule.h
 saveHandler.cpp
 saveHandler.h
 wordlist.cpp
 wordlist.h
 validator.cpp
 validator.h
 globalConst.h
 ImGui/resource.h
 ImGui/applicationGui.cpp
 ImGui/applicationGui.h
 ImGui/guiUtil.cpp
 ImGui/guiUtil.h
 ImGui/initGui.cpp
 ImGui/initGui.h
 ImGui/stb_image.h
 ImGui/tinyfiledialogs.cpp
 ImGui/tinyfiledialogs.h)

target_include_directories(${PROJECT_NAME} PUBLIC
 ${PROJECT_SOURCE_DIR}/external/imgui ${PROJECT_SOURCE_DIR}/external/imgui/backends)
target_link_directories(${PROJECT_NAME} PUBLIC
 ${PROJECT_SOURCE_DIR}/external/imgui ${PROJECT_SOURCE_DIR}/external/imgui/backends)
target_link_libraries(${PROJECT_NAME} imgui)
