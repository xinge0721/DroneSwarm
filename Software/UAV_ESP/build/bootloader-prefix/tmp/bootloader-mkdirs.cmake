# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/esp-idf/components/bootloader/subproject"
  "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader"
  "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader-prefix"
  "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader-prefix/tmp"
  "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader-prefix/src/bootloader-stamp"
  "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader-prefix/src"
  "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "G:/bianchen/xiangmu/DroneSwarm/Software/UAV_ESP/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
