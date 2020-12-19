cmake_minimum_required(VERSION 3.0.0)
project(isp VERSION 0.1.0)

include(CTest)
enable_testing()

#设置平台 x86
set(platform x86)

#头文件
set(OpenCV_INCLUDE_DIRS H:/project/algo/opencv/OpenCV-MinGW-Build-OpenCV-4.1.1-x64/include/)
include_directories(./ ./core ${OpenCV_INCLUDE_DIRS})

# 添加子目录
#add_subdirectory(core) 子目录需要有CMakeList.txt

# 添加文件夹及其文件，自动添加文件夹中的文件
aux_source_directory(. DIRSRCS)
add_executable(isp ${DIRSRCS})

#################################################opencv 部分 方式1 find #####################################
#指定OpenCV版本，代码如下
#find_package(OpenCV 3.3 REQUIRED)
#如果不需要指定OpenCV版本，代码如下

#set(CMAKE_MODULE_PATH  H:\\project\\algo\\opencv\\OpenCV-MinGW-Build-OpenCV-3.4.5\\x86\\mingw\\lib\\)
#set(OpenCV_DIR H:\\project\\algo\\opencv\\OpenCV-MinGW-Build-OpenCV-3.4.5\\x86\\mingw\\lib\\)
#find_package(OpenCV REQUIRED)

#显示OpenCV_INCLUDE_DIRS的值
#message(${OpenCV_INCLUDE_DIRS})

# 添加一个可执行程序
# 语法：add_executable( 程序名 源代码文件 ）
#add_executable( isp ${DIRSRCS} )

# 将库文件链接到可执行程序上
#target_link_libraries( isp  ${OpenCV_LIBS})
#################################################opencv 部分 方式2 #####################################
# 依赖库
# 寻找OpenCV库
set(OpenCV_DIR H:/project/algo/opencv/OpenCV-MinGW-Build-OpenCV-4.1.1-x64/x64/mingw/bin/)

SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
set(CMAKE_MODULE_PATH  ${OpenCV_DIR})
set(CMAKE_PREFIX_PATH ${OpenCV_DIR})

# 链接OpenCV库路径
link_directories(${OpenCV_DIR})

#find_library(OpenCV_LIBS libopencv_core411 ${OpenCV_DIR} ) libopencv_imgproc411

find_library(OpenCV_LIBS1
    NAMES libopencv_core411
    PATHS ${OpenCV_DIR}
    NO_DEFAULT_PATH)

find_library(OpenCV_LIBS
    NAMES libopencv_imgproc411
    PATHS ${OpenCV_DIR}
    NO_DEFAULT_PATH)

find_library(OpenCV_LIBS2
    NAMES libopencv_imgcodecs411
    PATHS ${OpenCV_DIR}
    NO_DEFAULT_PATH)

find_library(OpenCV_LIBS3
    NAMES libopencv_highgui411
    PATHS ${OpenCV_DIR}
    NO_DEFAULT_PATH)

#message(STATUS "test")
message("${OpenCV_LIBS}")
message("${OpenCV_LIBS1}")
message("${OpenCV_LIBS2}")
# 链接OpenCV库
#SET(CMAKE_EXE_LINKER_FLAGS " -no-pie")

target_link_libraries(isp ${OpenCV_LIBS} ${OpenCV_LIBS1} ${OpenCV_LIBS2} ${OpenCV_LIBS3})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
