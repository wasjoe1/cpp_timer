# C++ Project Boilerplate

Boilerplate files for bootstrapping new C++ projects with CMake + vcpkg.

## File Structure

```
project_folder/
├── CMakeLists.txt           # build config: project name, C++ std, packages, linking
├── CMakePresets.json        # vcpkg preset: Ninja generator, toolchain, compile commands
├── vcpkg.json               # dependency list
├── vcpkg-configuration.json # vcpkg registry + baseline pin
└── src/
    └── main.cpp             # entrypoint
```

## Starting a New Project (Quick start)

### 1. Init

`cd` into your new project folder, then run:

```bash
initcpp
```

This copies all boilerplate files from the template directory into the current project folder.

### 2. Configure

Update the following files to match your project:

**`CMakeLists.txt`**
- Change `project(project_name)` to your project name
- Update `add_executable(project_name ...)` to match your project name
- Add/remove `find_package(...)` calls for your dependencies
- Update `target_link_libraries(project_name ...)` to match your dependencies
- [Optional] Change `CMAKE_CXX_STANDARD` if needed (default: 20)

**`vcpkg.json`**
- Add/remove packages under `"dependencies"`
- [Mandatory] remove the comment on line 1

**`src/main.cpp`**
- Add `#include` directives for your dependencies
- Write your code in `main()`

### 3. Build & Run

```bash
runcpp     # build and run normally
debugcpp   # build and run in Debug mode
```

Both commands:
1. Run `cmake --preset vcpkg` to configure and resolve dependencies
2. Run `cmake --build build` to compile
3. Execute the binary (name is read from `CMakeCache.txt`)

* in debug mode (debugcpp), `cmake --preset vcpkg` is called with `--DCMAKE_BUILD_TYPE=Debug`

## TODOs When Starting Fresh

- [ ] `CMakeLists.txt` — update project name, C++ version, packages, and linked libraries
- [ ] `vcpkg.json` — update dependencies list
- [ ] `src/main.cpp` — add includes and write code


## Glossary
- bootstrap (a program) - to use a simple process to initiate a more complex one OR set up phase before the main program runs

- CmakeLists.txt - cmake reads this to know: project name, cpp standard, which packages, src files, libs to link against, etc.
- CMakePreset.json - presets file, tells cmake which build tool to use (i.e. ninja), where to put build files, etc.
- vcpkg.json - tells vcpkg which dependency => similar to package.json
- vcpkg-configuration.json - locks down dependencies version => similar to package-lock.json

- cmake - build system generator; not a build tool => reads cmake config & generate instructions for actual build tool to execute
- vcpkg.cmake - tool chain file => cmake loads this to know 2 things (1. calls vcpkg binary to install missing pacakges from vcpkg.json && 2. tell cmake where to find .cmake config files that vcpkg generated)
- vcpkg - pacakge manager for cpp libs => downloads & builds src code of dependency libraries (comiles them inito .a/.dylib files)
- ninja - build tool => compiles & links the cpp files

## Under the hood steps
cmake --preset vcpkg
1. cmake reads CMakePresets.json => knows build tool (ninja), output build dir, & tool chain file path
2. vcpkg toolchain (vcpkg.cmake) is invoked & reads vcpkg.json, then download & build any missing package, then registers them => find_package() can find them after registration
3. cmake reads CMakeLists.txt => then now knows all the config details
    - find_package() - cmake executes this, to find the already built .cmake config files (vcpkg generatres these which tells Cmake where the headers & compiled binaries are)
    - target_link_libraries() - cmake executes this wires the found library(headers & binaries) into specific executable
    * no actual compilation & linkage of code, other than lib binaries compiled
4. build files then generated

cmake --build build
1. cmake tells build tool (ninja) to execute the commands (actual compilation & linkage)
