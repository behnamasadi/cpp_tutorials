============================== vcpkg ============================== 
git submodule add https://github.com/microsoft/vcpkg





git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat

.\vcpkg.exe install opencv4:x64-windows
.\vcpkg.exe install curl[openssl]:x64-windows
.\vcpkg.exe  upgrade curl --no-dry-run
.\vcpkg.exe  upgrade remove curl:x64-windows --recurse

Env variable  new 
Default x64/x32:
Name:  VCPKG_DEFAULT_TRIPLET
Value: x64-windows


.\vcpkg.exe integrate remove 



create a vcpkg.json next to your cmake file:


all the possible:
https://vcpkg.io/en/packages.html

are stored at:

vcpkg\ports\ffmpeg\portfile.cmake
