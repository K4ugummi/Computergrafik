# Sample CMake file for Qt5 OpenGL projects.
# v1.0, 2018, krueckel@fh-aachen.de
# v1.1, 2018, jonas.kulla@alumni.fh-aachen.de

# Set required CMake version.
cmake_minimum_required(VERSION 3.1.0)

# Name of the project.
project(MeinCGProjekt)

# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find Qt modules.
# Note: Qt modules are able to find their own dependencies. For
# non-OpenGL projects, you might only require Qt5Core, Qt5Widgets, or Qt5Gui.
find_package(Qt5OpenGL REQUIRED 5.4)

# Find Assimp.
if (UNIX)
    message("Target: Unix")
    find_package(Assimp REQUIRED)
elseif (WIN32)
    message("Target: Win32")
    set(ASSIMP_INCLUDE_DIRS ../assimp-mingw32-4.1.0/include)
    # Link statically on Windows to avoid PATH hell
    set(ASSIMP_LIBRARIES -L../assimp-mingw32-4.1.0/lib -lassimp -lIrrXML -lzlibstatic)
    # The below line would link dynamically
#    set(ASSIMP_LIBRARIES -L../assimp-mingw32-4.1.0/bin -lassimp)
else (UNIX)
    error(Unsupported platform)
endif (UNIX)

# Automatically collect files.
# Note: this should only be used if you have a single target and Git is used.
# Also, CMake may not notice files that have been added because no changes
# were made to CMakeLists.txt.
file(GLOB HEADER_FILES src/*.h)
file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB QT_FORMS ui/*.ui)
file(GLOB QT_RESOURCES res/*.qrc)

# Set files manually.
# Note: if you have multiple targets, you may not want to use variables and
# list the files in the add_exectuable call instead.
#set(SOURCE_FILES src/main.cpp src/MainWindow.cpp)
# (...)

# Process resources and ui files.
qt5_add_resources(RESOURCES ${QT_RESOURCES})
qt5_wrap_ui(FORMS ${QT_FORMS})

# Define targets.
# Note: target names correspond to resulting executable/library names.
# Also, if you do not collect files automatically, you can easily add
# more targets here.
add_executable(MeinCGProjekt ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCES} ${FORMS})
# (...)

# Add include directories and link libraries.
target_include_directories(MeinCGProjekt PUBLIC include ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(MeinCGProjekt ${ASSIMP_LIBRARIES} Qt5::OpenGL)
