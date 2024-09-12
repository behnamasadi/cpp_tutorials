CPack supports a wide range of packaging formats for different platforms. Here is a list of the most common ones:

### 1. **TGZ (Tar Gzip Archive)**
   - **Usage**: Unix-based systems (Linux, macOS).
   - **Command**: `cpack -G TGZ`
   - **Output**: Compressed tarball (e.g., `myapp-1.0.0-Linux.tar.gz`).

### 2. **TBZ2 (Tar BZip2 Archive)**
   - **Usage**: Unix-based systems.
   - **Command**: `cpack -G TBZ2`
   - **Output**: Bzip2 compressed tarball (e.g., `myapp-1.0.0-Linux.tar.bz2`).

### 3. **ZIP (ZIP Archive)**
   - **Usage**: Cross-platform.
   - **Command**: `cpack -G ZIP`
   - **Output**: ZIP archive (e.g., `myapp-1.0.0-Windows.zip`).

### 4. **DEB (Debian Package)**
   - **Usage**: Debian-based Linux distributions (e.g., Ubuntu).
   - **Command**: `cpack -G DEB`
   - **Output**: `.deb` package (e.g., `myapp-1.0.0-Linux.deb`).
   - **Requires**: The `dpkg-deb` tool to be installed on the system.

### 5. **RPM (Red Hat Package Manager)**
   - **Usage**: Red Hat-based Linux distributions (e.g., Fedora, CentOS).
   - **Command**: `cpack -G RPM`
   - **Output**: `.rpm` package (e.g., `myapp-1.0.0-Linux.rpm`).
   - **Requires**: The `rpmbuild` tool to be installed.

### 6. **NSIS (Nullsoft Scriptable Install System)**
   - **Usage**: Windows installer (alternative to MSI).
   - **Command**: `cpack -G NSIS`
   - **Output**: NSIS executable installer (e.g., `myapp-1.0.0-Windows.exe`).
   - **Requires**: NSIS (Nullsoft Scriptable Install System) to be installed.

### 7. **WIX (Windows Installer)**
   - **Usage**: Windows MSI installers.
   - **Command**: `cpack -G WIX`
   - **Output**: `.msi` package (e.g., `myapp-1.0.0-Windows.msi`).
   - **Requires**: WiX Toolset to be installed.

### 8. **DMG (Apple Disk Image)**
   - **Usage**: macOS.
   - **Command**: `cpack -G DragNDrop`
   - **Output**: `.dmg` disk image (e.g., `myapp-1.0.0-macOS.dmg`).
   - **Requires**: macOS system to build.

### 9. **STGZ (Self-extracting Tar Gzip Archive)**
   - **Usage**: Unix-based systems (self-extracting).
   - **Command**: `cpack -G STGZ`
   - **Output**: Self-extracting archive (e.g., `myapp-1.0.0-Linux.sh`).

### 10. **SH (Shell Script)**
   - **Usage**: Linux/Unix installer script.
   - **Command**: `cpack -G SH`
   - **Output**: Shell script installer (e.g., `myapp-1.0.0-Linux.sh`).

### 11. **7Z (7-Zip Archive)**
   - **Usage**: Cross-platform.
   - **Command**: `cpack -G 7Z`
   - **Output**: `.7z` archive (e.g., `myapp-1.0.0-Windows.7z`).
   - **Requires**: 7-Zip installed for extraction.

### 12. **TXZ (Tar XZ Archive)**
   - **Usage**: Unix-based systems.
   - **Command**: `cpack -G TXZ`
   - **Output**: XZ compressed tarball (e.g., `myapp-1.0.0-Linux.tar.xz`).

### 13. **External tools**
   - CPack can also work with **custom generators** if you need a different format not natively supported. You can use the `CPACK_GENERATOR` variable to specify multiple generators if you need multiple formats from a single configuration.

---

These options allow CPack to create a wide range of installers and package formats, providing flexibility in packaging software for different platforms and distribution channels.




To create an MSI installer with **CMake** and **CPack**, follow these steps. CPack is a packaging system that comes with CMake, and it supports various package formats, including MSI for Windows.

### 1. **Set up your CMake project**
Make sure you have a working CMake project. Here's a simple example:

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

add_executable(MyApp main.cpp)
```

This will generate an executable called `MyApp`.

### 2. **Configure CPack for MSI**
Add the necessary configuration to your `CMakeLists.txt` to tell CPack to generate an MSI installer:

```cmake
# Include CPack module
include(CPack)

# Set CPack variables
set(CPACK_GENERATOR "WIX")  # Specify the generator as WIX for MSI
set(CPACK_PACKAGE_NAME "MyApp")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "MyCompany")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MyApp Installer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "MyApp")  # Default installation directory
set(CPACK_WIX_UPGRADE_GUID "C011FA2D-5F0D-43F7-BE94-5D6D77403435")  # Replace with your own GUID

# Include Wix toolset variables if needed
set(CPACK_WIX_PRODUCT_GUID "YOUR-GUID-HERE")  # Optional, for product identification

# Other useful CPack options
set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/license.rtf")  # License file (optional)
set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/images/banner.bmp")  # Custom banner (optional)
set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/images/dialog.bmp")  # Custom dialog image (optional)
```

### 3. **Install targets**
Tell CMake which files to install. This is important so that CPack knows what to include in the installer. You can specify the executable, libraries, and other resources like this:

```cmake
install(TARGETS MyApp DESTINATION bin)
install(FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION .)
```

### 4. **Generate the MSI with CPack**
Once you’ve set up your project, you can use **CPack** to generate the MSI package. To do this, follow these steps:

1. **Configure your project with CMake**:
   ```bash
   cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release .
   ```

2. **Build your project**:
   ```bash
   cmake --build . --config Release
   ```

3. **Run CPack to create the MSI installer**:
   ```bash
   cpack -G WIX
   ```

This will generate an MSI package for your project in the build directory.

### Additional Notes:
- **WIX Toolset**: CPack uses the **WiX (Windows Installer XML)** toolset to generate MSI files. Ensure that you have WiX installed and in your system PATH for CPack to create MSI installers. You can download WiX from [WiX Toolset](https://wixtoolset.org/).
- **GUIDs**: Windows Installer uses GUIDs to uniquely identify products and upgrades. You need to provide a unique `CPACK_WIX_PRODUCT_GUID` and `CPACK_WIX_UPGRADE_GUID` for your project. You can generate GUIDs using tools like `uuidgen`.

Once you run CPack successfully, you’ll find an MSI installer for your application in the build directory.
