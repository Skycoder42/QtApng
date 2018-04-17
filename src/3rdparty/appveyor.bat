@echo on
setlocal

:: update qmake conf
echo CONFIG += libpng_static >> .qmake.conf

cd "%~dp0"

:: get patch tool
powershell -Command "Invoke-WebRequest https://kent.dl.sourceforge.net/project/gnuwin32/patch/2.5.9-7/patch-2.5.9-7-bin.zip -OutFile patch.zip" || exit /B 1
dir
powershell -Command "(Get-FileHash -Algorithm MD5 -Path patch.zip).Hash.equals('b9c8b31d62f4b2e4f1887bbb63e8a905')" || exit /B 1
7z x patch.zip || exit /B 1

:: get zlib
powershell -Command "Invoke-WebRequest https://www.zlib.net/zlib%ZLIB_VERSION_P%.zip -OutFile zlib.zip" || exit /B 1
powershell -Command "(Get-FileHash -Algorithm SHA256 -Path zlib.zip).Hash.equals('%ZLIB_CHECKSUM%')" || exit /B 1
7z x zlib.zip || exit /B 1
del zlib.zip
ren zlib-%ZLIB_VERSION% zlib

:: get libpng
powershell -Command "Invoke-WebRequest https://prdownloads.sourceforge.net/libpng/lpng%LIBPNG_VERSION_P%.zip?download -OutFile libpng.zip" || exit /B 1
powershell -Command "(Get-FileHash -Algorithm MD5 -Path libpng.zip).Hash.equals('%LIBPNG_CHECKSUM%')" || exit /B 1
7z x libpng.zip || exit /B 1
del libpng.zip
ren libpng-%LIBPNG_VERSION% libpng

:: get apng patch
powershell -Command "Invoke-WebRequest https://downloads.sourceforge.net/sourceforge/libpng-apng/libpng-%LIBPNG_VERSION%-apng.patch.gz -OutFile libpng-apng.patch.gz" || exit /B 1
7z x libpng-apng.patch.gz || exit /B 1
cd libpng
.\bin\patch.exe -Np1 -i "../libpng-apng.patch"
copy scripts/pnglibconf.h.prebuilt pnglibconf.h
cd ..

:: copy pro files
copy zlib.pro zlib/zlib.pro
copy libpng.pro libpng/libpng.pro
