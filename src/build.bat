@echo off

set /A rebuild=0
set /A camBuild=0
set /A deboog=0

for %%a in (%*) do (
    if "%%a" == "rebuild" (
        set /A rebuild=1
    )
    if "%%a" == "cam" (
        set /A camBuild=1
    )
    if "%%a" == "deboog" (
        set /A deboog=1
    )
)


if %rebuild% == 1 (
    if %deboog% == 1 (
        cmake -B build/deboog -DCMAKE_BUILD_TYPE=Debug
    ) else (
        cmake -B build/release -DCMAKE_BUILD_TYPE=Release
    )
)

:compilation

if %camBuild% == 1 (
    if %deboog% == 1 (
        cmake --build build/deboog --target CAM -j4
    ) else (
        cmake --build build/release --target CAM -j4
    )
) else (
    if %deboog% == 1 (
        cmake --build build/deboog -j4
    ) else (
        cmake --build build/release -j4
    )
)
