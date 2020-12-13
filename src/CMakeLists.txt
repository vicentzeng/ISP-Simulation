cmake_minimum_required(VERSION 3.0.0)
project(isp VERSION 0.1.0)

include(CTest)
enable_testing()

#头文件
include_directories(./ ./core)

# 添加子目录
#add_subdirectory(core) 子目录需要有CMakeList.txt

# 添加文件夹，自动添加文件夹中的文件
aux_source_directory(. DIRSRCS)
add_executable(isp ${DIRSRCS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
