cmake_minimum_required(VERSION 3.26)
project(Nifti_viewer)

set(CMAKE_CXX_STANDARD 20)

find_package(NIFTI 2.1.0 CONFIG REQUIRED)
add_executable(Nifti_viewer main.cpp
        niftyWrapper/niftiWrapper.cpp
        niftyWrapper/niftiWrapper.h
        niftyWrapper/niftiReader.cpp
        window/MRIWindow.cpp
        window/MRIWindow.h
        Controller.cpp
        Controller.h
        window/ScrollWindow.h
        window/ScrollWindow.cpp
        shell/sshell.h
        shell/sshell.cc
)


target_link_libraries(Nifti_viewer PRIVATE -lsfml-graphics -lsfml-window -lsfml-system -pthread -lX11)
target_link_libraries(Nifti_viewer PUBLIC ${NIFTI_PACKAGE_PREFIX}NIFTI::${NIFTI_PACKAGE_PREFIX}niftiio)

target_compile_features(Nifti_viewer PRIVATE cxx_std_23)
