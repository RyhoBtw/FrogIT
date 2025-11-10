add_executable(FrogITApp main.cpp)

target_link_libraries(
  FrogITApp
  PRIVATE FrogIT::FrogIT_options
          FrogIT::FrogIT_warnings)

target_link_system_libraries(
  FrogITApp
  PRIVATE
					SFML::Graphics 
					ImGui-SFML::ImGui-SFML
          fmt::fmt)
					
target_include_directories(FrogITApp PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
