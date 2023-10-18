# Target definition
add_executable(Tetris "Tetris.cpp" "Row.h" "Tetris.h" "Sound.h" "Text.h" "ttf2mesh/ttf2mesh.h" "ttf2mesh/ttf2mesh.c" "Text.cpp")

# Target properties
set_target_properties(
	Tetris
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED true
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

add_custom_command(
        TARGET Tetris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/Theme.wav
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Theme.wav)
add_custom_command(
        TARGET Tetris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/Click.wav
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Click.wav)
add_custom_command(
        TARGET Tetris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/GameOver.wav
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GameOver.wav)
add_custom_command(
        TARGET Tetris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/Tetris.wav
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tetris.wav)
add_custom_command(
        TARGET Tetris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/Row.wav
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Row.wav)

# Linking
target_link_libraries(Tetris
	PRIVATE Miracle
)
