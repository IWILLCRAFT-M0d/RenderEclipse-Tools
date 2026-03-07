@echo off

set /A rebuild=0
set /A reeBuild=0

for %%a in (%*) do (
    if "%%a" == "Rebuild" (
        set /A rebuild=1
    )
    if "%%a" == "REE" (
        set /A reeBuild=1
    )
)


if %rebuild% == 1 (
    if %reeBuild% == 1 (
        cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_REE=ON
    ) else (
        cmake -B build -DCMAKE_BUILD_TYPE=Release
    )
)

if %reeBuild% == 1 (
    cmake --build build --target RenderEclipseTools -j4
) else (
    cmake --build build -j4
)

