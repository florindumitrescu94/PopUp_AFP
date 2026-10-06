cmake_minimum_required(VERSION 3.16)
project(indi_popup_afp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── INDI ──────────────────────────────────────────────────────────────────────
find_package(PkgConfig REQUIRED)
pkg_check_modules(INDI REQUIRED libindi)
include_directories(${INDI_INCLUDE_DIRS})
link_directories(${INDI_LIBRARY_DIRS})
set(INDI_DATA_DIR "/usr/share/indi")

# ── Driver target ─────────────────────────────────────────────────────────────
add_executable(indi_popup_afp
    popup_afp.cpp
)

target_link_libraries(indi_popup_afp
    indidriver
    indiclient
    pthread
)

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS indi_popup_afp DESTINATION bin)
install(FILES indi_popup_afp.xml DESTINATION ${INDI_DATA_DIR})
