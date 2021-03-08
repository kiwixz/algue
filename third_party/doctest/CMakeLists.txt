cmake_minimum_required(VERSION 3.12)
include("algue/third_party")

third_party("doctest" REPO "onqtam/doctest" TAG "2.4.1")
system_include_dirs("doctest")
