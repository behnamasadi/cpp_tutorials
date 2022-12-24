# vcpkg
First add it as submodule to your peoject:

`git submodule add https://github.com/microsoft/vcpkg`

Then run the following script on windows:

`.\vcpkg\bootstrap-vcpkg.bat`

on the bash:

`./vcpkg/bootstrap-vcpkg.sh`

Now you can install dependencies:

`
.\vcpkg.exe install opencv4:x64-windows
.\vcpkg.exe install curl[openssl]:x64-windows
.\vcpkg.exe  upgrade curl --no-dry-run
.\vcpkg.exe  upgrade remove curl:x64-windows --recurse
`
 If you dont want to type the architecture i.e. `x64-windows`, create a new environmental variable:
```  
Default x64/x32:
Name:  VCPKG_DEFAULT_TRIPLET
Value: x64-windows
```

You can create a `vcpkg.json` next to your cmake file:


All availabe packages listed [here](https://vcpkg.io/en/packages.html):

and localy they are  stored at:

`vcpkg\ports\ffmpeg\portfile.cmake`

Refs: [1](https://vcpkg.io/en/getting-started.html)

