# Require at least cmake 2.6.0
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Name of this project
project(Snapshots)

# Include the RunMacros.cmake file that will setup most of everything for us!
include("$ENV{QWANALYSIS}/Extensions/Compton/RunMacros/cmake/modules/RunMacros.cmake")
