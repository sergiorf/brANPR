project(brANPR)
cmake_minimum_required (VERSION 2.6)
set (CMAKE_PREFIX_PATH "C:\\Qt5\\5.5\\msvc2013_64\\")

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

find_package(Qt5Widgets)

set(UI_FILES mainwindow.ui)
set(QT_WRAP mainwindow.h)
set(HEADERS mainwindow.h anprengine.h utils.h detectregions.h plate.h OCR.h)
set(CXX_FILES main.cpp mainwindow.cpp anprengine.cpp utils.cpp detectregions.cpp plate.cpp trainSVM.cpp OCR.cpp trainOCR.cpp)

QT5_WRAP_UI(UISrcs ${UI_FILES})
QT5_WRAP_CPP(MOCSrcs ${QT_WRAP})

set(OpenCV_DIR "C:\\dev\\opencv-2.4.11\\build")
find_package(OpenCV REQUIRED core imgproc highgui ml)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(brANPR ${CXX_FILES} ${HEADERS} ${UISrcs} ${MOCSrcs})
qt5_use_modules(brANPR Widgets)
target_link_libraries(brANPR ${QT_LIBRARIES} ${OpenCV_LIBS})
