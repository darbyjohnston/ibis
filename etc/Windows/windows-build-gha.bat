set BUILD_TYPE=%1

cmake ^
    -S ibis\cmake\SuperBuild ^
    -B sbuild-%BUILD_TYPE% ^
    -Dibis_OCIO=%IBIS_OCIO% ^
    -Dibis_JPEG=%IBIS_JPEG% ^
    -Dibis_TIFF=%IBIS_TIFF% ^
    -Dibis_EXR=%IBIS_EXR% ^
    -Dibis_FFMPEG=%IBIS_FFMPEG% ^
    -Dibis_FFMPEG_MINIMAL=%IBIS_FFMPEG_MINIMAL% ^
    -Dibis_OIIO=%IBIS_OIIO% ^
    -Dibis_USD=%IBIS_USD% ^
    -Dibis_PYTHON=%IBIS_PYTHON% ^
    -Dftk_API=%FTK_API% ^
    -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_INSTALL_PREFIX=%CD%/install-%BUILD_TYPE% ^
    -DCMAKE_PREFIX_PATH="%CD%/install-%BUILD_TYPE%"
cmake --build sbuild-%BUILD_TYPE% -j %JOBS% --config %BUILD_TYPE%

cmake ^
    -S ibis ^
    -B build-%BUILD_TYPE% ^
    -Dibis_OCIO=%IBIS_OCIO% ^
    -Dibis_OIIO=%IBIS_OIIO% ^
    -Dibis_USD=%IBIS_USD% ^
    -Dibis_PYTHON=%IBIS_PYTHON% ^
    -Dibis_TESTS=%IBIS_TESTS% ^
    -Dibis_GCOV=%IBIS_GCOV% ^
    -Dftk_API=%FTK_API% ^
    -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_INSTALL_PREFIX=%CD%/install-%BUILD_TYPE% ^
    -DCMAKE_PREFIX_PATH="%CD%/install-%BUILD_TYPE%"
cmake --build build-%BUILD_TYPE% -j %JOBS% --config %BUILD_TYPE%
cmake --build build-%BUILD_TYPE% --config %BUILD_TYPE% --target install
