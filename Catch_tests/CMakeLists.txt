# First try to find system-installed Catch2
find_package(Catch2 3 QUIET)

# If not found, fetch it
if(NOT Catch2_FOUND)
	include(FetchContent)
	FetchContent_Declare(
			Catch2
			GIT_REPOSITORY https://github.com/catchorg/Catch2.git
			GIT_TAG v3.4.0
	)
	FetchContent_MakeAvailable(Catch2)
endif()

# Create the test executable
add_executable(Catch_tests_run JsonExprEvalTest.cpp)
target_link_libraries(Catch_tests_run PRIVATE json_cpp Catch2::Catch2WithMain)

# Enable test discovery
include(Catch)
catch_discover_tests(Catch_tests_run)