@echo off

set /A rebuild=0
set /A camBuild=0

for %%a in (%*) do (
    if "%%a" == "Rebuild" (
        set /A rebuild=1
    )
    if "%%a" == "cam" (
        set /A camBuild=1
    )
)


if %rebuild% == 1 (
    cmake -B build -DCMAKE_BUILD_TYPE=Release
)

:compilation

if %camBuild% == 1 (
    cmake --build build --target CAM -j4
) else (
    cmake --build build -j4
)
