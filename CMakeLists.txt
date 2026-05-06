cmake_minimum_required(VERSION 3.18)
project(FunctionalInterpreter VERSION 1.0.0)

# Force use of the specified Python3_EXECUTABLE if provided
if(DEFINED Python3_EXECUTABLE)
    set(Python3_EXECUTABLE "${Python3_EXECUTABLE}" CACHE FILEPATH "Python3 executable" FORCE)
    set(PYTHON_EXECUTABLE "${Python3_EXECUTABLE}" CACHE FILEPATH "Python executable" FORCE)
    message(STATUS "Using explicitly provided Python: ${Python3_EXECUTABLE}")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable lto by default
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Emit compile_commands.json for use with clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories(include include/interpreter include/lexer include/parser
    include/interpreter/valuesystem include/interpreter/typesystem)

# Emscripten / WebAssembly build
# Usage:
#   emcmake cmake -S . -B build-wasm -DCMAKE_BUILD_TYPE=Release
#   cmake --build build-wasm --target interpreter_web
if(EMSCRIPTEN)
    message(STATUS "Building for Emscripten/WASM (build type: ${CMAKE_BUILD_TYPE})")

    add_executable(interpreter_web tools/bindings.cpp ${SOURCES})
    set_target_properties(interpreter_web PROPERTIES SUFFIX ".js")

    target_compile_options(interpreter_web PRIVATE
        -fvisibility=hidden -fvisibility-inlines-hidden
        -msimd128
        -fwasm-exceptions)

    target_link_options(interpreter_web PRIVATE
        --bind
        -fwasm-exceptions
        "SHELL:-s EXPORT_ES6=1"
        "SHELL:-s MODULARIZE=1"
        "SHELL:-s EXPORT_NAME=createInterpreterModule"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1")

    # Skip all the native-only machinery below (pybind11, Julia, shell tools).
    return()
endif()

option(AUTUMN_DEV_MODE "Enable development mode with debug prints" OFF)
option(AUTUMN_BUILD_PYTHON_MODULE "Build the pybind11 Python extension (interpreter_module)" ON)

if(AUTUMN_DEV_MODE)
    add_definitions(-DAUTUMN_DEV_MODE)
endif()

# SOURCES and include_directories are declared near the top of this file so
# both the WASM and native branches can share them.

add_executable(sexp_parser tools/sexp_parser.cpp src/parser/sexpresso.cpp)
add_executable(generate_ast tools/generate_ast.cpp)
add_executable(lexer tools/lexer.cpp src/Token.cpp)
add_executable(parser tools/parser.cpp src/Token.cpp src/parser/sexpresso.cpp)


add_library(AutumnLib ${SOURCES})
target_include_directories(AutumnLib PUBLIC
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(parser PRIVATE AutumnLib)

add_executable(TokenTypeTest
    test_suites/test_token_type.cpp
)

target_link_libraries(TokenTypeTest PRIVATE AutumnLib)

enable_testing()
add_test(NAME TokenTypeTest COMMAND TokenTypeTest)

message("SOURCES: ${SOURCES}")
# Create the native CLI executable
add_executable(interpreter tools/main.cpp ${SOURCES})

if(AUTUMN_BUILD_PYTHON_MODULE)
    # Set python executable path
    # If Python3_EXECUTABLE was passed to CMake, use it exclusively
    if(NOT DEFINED Python3_EXECUTABLE OR NOT Python3_EXECUTABLE)
        # Prefer in-tree venv at <source>/.venv, then Homebrew, then system Python
        set(VENV_PYTHON "${CMAKE_SOURCE_DIR}/.venv/bin/python")

        if(EXISTS "${VENV_PYTHON}")
            set(Python3_EXECUTABLE "${VENV_PYTHON}" CACHE FILEPATH "Python3 executable" FORCE)
            message(STATUS "Auto-detected venv Python: ${Python3_EXECUTABLE}")
        elseif(EXISTS "/opt/homebrew/bin/python")
            set(Python3_EXECUTABLE "/opt/homebrew/bin/python" CACHE FILEPATH "Python3 executable" FORCE)
            message(STATUS "Auto-detected Homebrew Python: ${Python3_EXECUTABLE}")
        else()
            # Fall back to system Python
            find_program(Python3_EXECUTABLE python3 python)
            if(Python3_EXECUTABLE)
                message(STATUS "Auto-detected system Python: ${Python3_EXECUTABLE}")
            endif()
        endif()
    endif()

    # Set PYTHON_EXECUTABLE for backwards compatibility
    set(PYTHON_EXECUTABLE "${Python3_EXECUTABLE}")
    message(STATUS "Final Python for build: ${Python3_EXECUTABLE}")

    # Find Python3 FIRST with our specified executable
    # This locks in the Python version before pybind11 tries to find it
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
    message(STATUS "Locked Python3: ${Python3_EXECUTABLE} (version ${Python3_VERSION})")

    # Find pybind11 - it will now use the Python3 we just locked in
    find_package(pybind11 REQUIRED)

    # Create the Python module
    pybind11_add_module(interpreter_module tools/bindings_python.cpp ${SOURCES})

    install(
      TARGETS interpreter_module
      LIBRARY DESTINATION MARA/autumn_cpp
      COMPONENT python_extension
    )

    install(
      FILES python_pkg/__init__.py
      DESTINATION MARA/autumn_cpp
      COMPONENT python_extension
    )

    install(
      FILES python_pkg/autumnstdlib/__init__.py autumnstdlib/stdlib.sexp
      DESTINATION MARA/autumn_cpp/autumnstdlib
      COMPONENT python_extension
    )
endif()

# Try to find Julia
find_package(Julia QUIET)
if(NOT Julia_FOUND)
    # Try to find Julia manually if find_package failed
    find_program(Julia_EXECUTABLE julia DOC "Julia interpreter")
    if(Julia_EXECUTABLE)
        execute_process(
            COMMAND ${Julia_EXECUTABLE} --startup-file=no -e "print(dirname(Sys.BINDIR))"
            OUTPUT_VARIABLE Julia_DIR
        )
        execute_process(
            COMMAND ${Julia_EXECUTABLE} --startup-file=no -e "print(joinpath(Sys.BINDIR, \"../lib\"))"
            OUTPUT_VARIABLE Julia_LIBRARY_DIR
        )
        execute_process(
            COMMAND ${Julia_EXECUTABLE} --startup-file=no -e "print(joinpath(Sys.BINDIR, \"../include/julia\"))"
            OUTPUT_VARIABLE Julia_INCLUDE_DIRS
        )

        find_library(Julia_LIBRARY
            NAMES julia libjulia
            PATHS ${Julia_LIBRARY_DIR}
            NO_DEFAULT_PATH
        )

        if(Julia_LIBRARY)
            set(Julia_FOUND TRUE)
            message(STATUS "Found Julia library: ${Julia_LIBRARY}")
            message(STATUS "Julia include dirs: ${Julia_INCLUDE_DIRS}")
        endif()
    endif()
endif()

if(Julia_FOUND)
    message(STATUS "Found Julia: Building Julia bindings")
    find_package(JlCxx QUIET)

    if(JlCxx_FOUND)
        # Get Julia package directory
        execute_process(
            COMMAND ${Julia_EXECUTABLE} -e "using Pkg; print(joinpath(first(DEPOT_PATH), \"packages\"))"
            OUTPUT_VARIABLE Julia_PACKAGE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Create AutumnInterpreter directory if it doesn't exist
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory ${Julia_PACKAGE_DIR}/AutumnInterpreter
        )

        # Add Julia bindings
        add_library(autumn_julia SHARED
            tools/bindings_julia.cpp
        )

        target_link_libraries(autumn_julia
            PRIVATE
            AutumnLib
            JlCxx::cxxwrap_julia
            ${Julia_LIBRARY}
        )

        target_include_directories(autumn_julia
            PRIVATE
            ${Julia_INCLUDE_DIRS}
            ${JlCxx_DIR}/../../../include
        )

        # Copy the library to Julia package directory
        add_custom_command(TARGET autumn_julia POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:autumn_julia> ${Julia_PACKAGE_DIR}/AutumnInterpreter/libautumn_julia${CMAKE_SHARED_LIBRARY_SUFFIX}
        )

        # First, generate a UUID using Julia
        execute_process(
            COMMAND ${Julia_EXECUTABLE} -e "using UUIDs; print(uuid4())"
            OUTPUT_VARIABLE PACKAGE_UUID
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Then use the generated UUID in the Project.toml
        file(WRITE ${Julia_PACKAGE_DIR}/AutumnInterpreter/Project.toml
"name = \"AutumnInterpreter\"
uuid = \"${PACKAGE_UUID}\"
version = \"0.1.0\"

[deps]
CxxWrap = \"1f15a43c-97ca-5a2a-ae31-89f07a497df4\"
")

        file(WRITE ${Julia_PACKAGE_DIR}/AutumnInterpreter/src/AutumnInterpreter.jl
"module AutumnInterpreter

using CxxWrap

# Function to get library path
function get_library_path()
    return joinpath(@__DIR__, \"../libautumn_julia\")
end

# Load the library using the function
@wrapmodule(() -> get_library_path())

function __init__()
    @initcxx
end

export Interpreter,
    run_script,
    step,
    click,
    left,
    right,
    up,
    down,
    get_trigger_state,
    render_all,
    get_environment_string,
    restore_environment,
    tmp_execute_stmt,
    set_verbose,
    get_verbose

end
")
    else()
        message(STATUS "JlCxx not found: Skipping Julia bindings")
    endif()
else()
    message(STATUS "Julia not found: Skipping Julia bindings")
endif()
