cmake_minimum_required(VERSION 3.10)
project(bb84_simulation)

##root
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)
include(${ROOT_USE_FILE})

ROOT_GENERATE_DICTIONARY(G__Qbit Qbit.h LINKDEF)
add_library(Qbit SHARED Qbit.cxx G__Qbit.cxx)
target_link_libraries(Qbit ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__Buddy Buddy.h LINKDEF)
add_library(Buddy SHARED Buddy.cxx G__Buddy.cxx)
target_link_libraries(Buddy Qbit ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__Channel Channel.h LINKDEF)
add_library(Channel SHARED Channel.cxx G__Channel.cxx)
target_link_libraries(Channel Qbit ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__Phone Phone.h LINKDEF)
add_library(Phone SHARED Phone.cxx G__Phone.cxx)
target_link_libraries(Phone Qbit ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__ConfigSimulation ConfigSimulation.h LINKDEF)
add_library(ConfigSimulation SHARED ConfigSimulation.cxx G__ConfigSimulation.cxx)
target_link_libraries(ConfigSimulation Buddy ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__Simulator Simulator.h LINKDEF)
add_library(Simulator SHARED Simulator.cxx G__Simulator.cxx)
target_link_libraries(Simulator Channel Phone ConfigSimulation ${ROOT_LIBRARIES})

ROOT_GENERATE_DICTIONARY(G__Analyzer Analyzer.h LINKDEF)
add_library(Analyzer SHARED Analyzer.cxx G__Analyzer.cxx)
target_link_libraries(Analyzer Simulator ${ROOT_LIBRARIES})

add_executable(
        bb84
        main.cxx
        StopWatch.cxx
)

target_link_libraries(
        bb84
        Analyzer
)

add_executable(various #DO NOT COMPILE
        compile.C
        bb84_simulation.cxx
        )
