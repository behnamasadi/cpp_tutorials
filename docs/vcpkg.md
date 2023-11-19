# vcpkg
First add it as submodule to your peoject:

```
git submodule add https://github.com/microsoft/vcpkg
```

## Search for the Library
```
./vcpkg/vcpkg search opencv
```

## List of dependencies that will be built 

```
./vcpkg/vcpkg depend-info opencv4
```

## Install the Specific Version

```
./vcpkg/vcpkg install <library-name>:<triplet>@<version>
```
- <triplet> refers to the system architecture (like x64-windows, x86-windows, etc.)

## Integrate Installed Libraries
After installation, you might need to integrate the installed libraries with your build system. For most users, the following command should work

```
./vcpkg/vcpkg integrate install
```

## Verify Installation
```
vcpkg list
```



## Building Dependecies with Your CMakeLists.txt
Now add the following to your CMakeLists
```
cmake_minimum_required(VERSION 3.16.3)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
endif()

message("toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
project(your-project-name)

find_package(vtk  9.2)
```

After that, create the following file  `vcpkg.json` next to your `CMakeLists.txt`: 

```
{
  "name": "your-project-name",
  "version-string": "1.1.0",
  "dependencies": [
    "vtk"
  ]
}
```
Now you can run:
```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg//scripts/buildsystems/vcpkg.cmake
```

```
cmake -S . -B build  -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=./vcpkg//scripts/buildsystems/vcpkg.cmake
```



If you need version `8.2.0` of `vtk`, your file would look like:
```
{
  "name": "your-project-name",
  "version-string": "1.1.0",
  "dependencies": [
    {
      "name": "vtk",
      "version>=": "8.2.0"
    }
  ]
}
```

Use Overrides to Pin a Specific Version
```
{
  "name": "your-project-name",
  "version-string": "1.1.0",
  "dependencies": [
    "vtk"
  ],
  "overrides": [
    {
      "name": "vtk",
      "version": "9.2.0"
    }
  ]
}
```
Note: The "overrides" feature is a powerful tool and should be used cautiously. It forces vcpkg to use the specified version, which might lead to compatibility issues with other libraries that require different versions of the same dependency.

## Port File

Port files in vcpkg are a crucial part of how the tool manages and installs C++ libraries. These files define how a specific library (or a "port" in vcpkg terminology) should be downloaded, built, and installed. Here’s a detailed explanation:

### 1. **What is a Port File?**
   
   - **Definition**: A port file in vcpkg is essentially a set of instructions that vcpkg follows to handle a particular library. These instructions are primarily written in CMake language.
   - **Location**: Port files are located in the `ports` directory of your vcpkg installation, with each library having its own subdirectory. For example, the port files for a library named `examplelib` would be in `vcpkg/ports/examplelib/`.

### 2. **Contents of a Port File**

   A typical port file directory for a library might contain the following:

   - **`portfile.cmake`**: This is the main script that vcpkg executes to handle the library. It includes commands for downloading the source code, applying patches, configuring the build process using CMake, building the library, and installing it.
   - **`CONTROL` or `vcpkg.json`**: This file provides metadata about the library, such as its name, version, description, homepage, and a list of its dependencies.
   - **Patches**: Sometimes, additional patch files are included to make necessary modifications to the library’s source code to ensure it works well with vcpkg or to fix bugs.

### 3. **Function of Port Files**

   The port files serve several functions:

   - **Automating Builds**: They automate the process of building a library from source, including resolving and installing dependencies.
   - **Customization**: Through these files, vcpkg allows customization of the build process. Users can edit the port files to change how a library is built.
   - **Cross-Platform Consistency**: Port files help ensure that a library can be built consistently across different platforms and configurations.

### 4. **Editing Port Files**

   Users can edit port files to customize the build process. This might be necessary to:

   - **Apply Custom Patches**: You might have specific requirements or fixes that need to be applied to the library's source.
   - **Change Build Options**: To enable or disable certain features of a library during the build.
   - **Resolve Conflicts**: Sometimes, edits are needed to resolve conflicts with other libraries or with the system environment.

### 5. **Updating and Maintaining Port Files**

   The vcpkg team and community contributors regularly update port files to reflect new versions of libraries and incorporate improvements. Users can also contribute changes back to the vcpkg repository.

Port files are a powerful aspect of vcpkg, allowing for a high degree of flexibility and control over how third-party libraries are integrated into your C++ projects. They are key to vcpkg’s ability to manage a wide array of C++ libraries across different platforms and environments.

## Pass specific CMake parameters to dependencies

To pass specific CMake parameters to dependencies when using vcpkg, you will typically modify the portfile of the library or use overlay ports. vcpkg does not directly support passing custom CMake options through the `vcpkg.json` manifest for third-party libraries. However, there are ways to achieve this:

1. **Modify the Portfile**: The portfile of a library in vcpkg is a CMake script that defines how to download, build, and install the library. You can modify this file to include your specific CMake parameters.

   - **Locate the Portfile**: Find the portfile for `vtk` in the vcpkg directory, usually under `vcpkg/ports/vtk`.
   - **Edit the Portfile**: Add your custom CMake options in the portfile's `vcpkg_configure_cmake` call.
   - **Reinstall the Library**: Run `vcpkg remove vtk` and then `vcpkg install vtk` to apply the changes.

2. **Using Overlay Ports**: Overlay ports allow you to specify custom portfiles for vcpkg packages. This method is useful if you want to maintain your modifications separately from the main vcpkg repository.

   - **Create a Custom Portfile**: Make a new directory outside your vcpkg installation, such as `my-ports`, and copy the `vtk` port directory into it.
   - **Modify the Portfile**: In `my-ports/vtk`, add your CMake parameters as needed.
   - **Use Overlay Ports**: Install `vtk` using your custom portfile by running vcpkg with the `--overlay-ports` option:

     ```bash
     vcpkg install vtk --overlay-ports=path/to/my-ports
     ```

3. **CMake Toolchain File**: If you are using a CMake project, you can set some options through the CMake toolchain file provided by vcpkg. However, this is more limited and generally applies to global settings rather than specific packages.

4. **Feature Flags**: Some packages in vcpkg support feature flags, which can be used to enable or disable certain functionalities in the library. These are specified in the `vcpkg.json` or during installation:

   ```bash
   vcpkg install vtk[feature1,feature2]
   ```

Remember that modifying portfiles or using overlay ports can complicate the update process of vcpkg and the libraries, as you'll need to maintain your custom changes. Always document any changes you make for future reference and maintenance.


To work with vcpkg and handle aspects like the default triplet, specifying build types (debug or release), and using Ninja with multiple configurations, you can follow these guidelines:

## Example Customize VTK

Passing specific CMake parameters to a vcpkg dependency like VTK requires a bit of customization in the vcpkg portfile for that library. This is because vcpkg's standard `vcpkg.json` manifest file doesn't directly support the injection of custom CMake parameters for individual dependencies. Here's how you can do it for VTK:

### 1. **Modify the VTK Portfile**

You need to edit the portfile for VTK within your vcpkg installation. Here are the steps:

1. **Locate the Portfile**: Navigate to the directory where vcpkg is installed, and then go to `ports/vtk`.

2. **Edit the Portfile**: Look for the `portfile.cmake` file in the `vtk` directory. Open this file in a text editor.

3. **Modify the Build Options**: In the `portfile.cmake`, locate the `vcpkg_configure_cmake` command. This is where you can add your custom CMake parameters. You should add your options (`-DVTK_MODULE_ENABLE_VTK_RenderingQt=YES -DVTK_MODULE_ENABLE_VTK_hdf=YES`) in the OPTIONS list of this command.

   It would look something like this:

   ```cmake
   vcpkg_configure_cmake(
       SOURCE_PATH ${SOURCE_PATH}
       PREFER_NINJA
       OPTIONS
           -DVTK_MODULE_ENABLE_VTK_RenderingQt=YES
           -DVTK_MODULE_ENABLE_VTK_hdf=YES
           # ... other existing options ...
   )
   ```

4. **Save the Portfile**: After adding your parameters, save the changes to the `portfile.cmake` file.

### 2. **Rebuild VTK**

After modifying the portfile, you need to rebuild VTK:

1. **Remove the Existing VTK Installation**: Run `vcpkg remove vtk` to remove the currently installed version of VTK.

2. **Install VTK Again**: Run `vcpkg install vtk`. This will build VTK with your custom parameters.

### 3. **Consider Using Overlay Ports**

If you prefer not to modify the vcpkg repository directly, you can use overlay ports:

1. **Create an Overlay Port**: Make a new directory outside your vcpkg installation, copy the `vtk` port directory into it, and then make your modifications in this copy.

2. **Install Using Overlay**: Use the `--overlay-ports` option when running vcpkg to specify your custom ports directory.

   ```bash
   vcpkg install vtk --overlay-ports=path/to/your/overlay-ports
   ```

### 4. **Note on Maintenance**

Modifying portfiles or using overlay ports requires you to manually maintain these changes, especially when updating vcpkg or the specific libraries. It's important to track these changes for future reference.

### 5. **Alternative: Feature Flags**

Before modifying portfiles, it's also worth checking if VTK in vcpkg supports enabling these modules through feature flags. Vcpkg's feature flags can sometimes offer a way to enable or disable specific functionalities in a library without directly modifying the portfile.



## Getting the Default Triplet

vcpkg uses a default triplet based on your system's architecture and platform. To find out the default triplet for your system:

- **Check Environment Variable**: vcpkg respects the `VCPKG_DEFAULT_TRIPLET` environment variable. If set, this variable defines the default triplet.
- **vcpkg Documentation**: Consult the vcpkg documentation or GitHub repository for information on default triplets for various platforms.
- **Check vcpkg Configuration**: By looking at the vcpkg configuration files or running some test installations, you can infer the default triplet.

## Specifying Build Type (Debug or Release)

vcpkg installs both debug and release builds of libraries by default. If you want to specify only one type:

- **Modify vcpkg Command**: Use the `--only` option when installing a package. For example, `vcpkg install <library>:<triplet> --only-release` will install only the release version.
- **CMake Integration**: If you are using CMake, vcpkg's toolchain file usually handles the debug and release configurations automatically based on your CMake build type.

## Using Ninja with Multiple Configurations

To use Ninja with multiple configurations like Debug, Release, and RelWithDebInfo, you need to set up your build system accordingly:

- **CMake Generator**: Ensure your CMake is configured to use Ninja as the generator, and it supports multi-configuration setups. You can specify this when configuring your CMake project:

  ```bash
  cmake -G "Ninja Multi-Config" ..
  ```

- **Build Configurations**: With Ninja Multi-Config, you can build different configurations by specifying the `--config` option. For example:

  ```bash
  cmake --build . --config Debug
  cmake --build . --config Release
  cmake --build . --config RelWithDebInfo
  ```

- **Integrate with vcpkg**: Ensure that your CMakeLists.txt or build setup integrates vcpkg correctly. Use the vcpkg toolchain file in your CMake configuration:

  ```bash
  cmake -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake ..
  ```

Remember, when dealing with multiple configurations, the build system (like CMake with Ninja) is responsible for handling different build types, while vcpkg is responsible for providing the dependencies. The integration between them is crucial to ensure that the correct libraries for the corresponding build type are linked.


## Understanding builtin-baseline
The concept of `builtin-baseline` is closely related to how dependencies and their versions are managed, especially when using the `"version>="` constraint in the `vcpkg.json` manifest file.

### Understanding `builtin-baseline`

1. **Baseline Concept**: A baseline in vcpkg is a specific state or snapshot of the vcpkg repository, which includes all the port versions available at a certain point in time. It essentially locks the versions of all packages in the vcpkg repository as they were at that specific baseline.

2. **Builtin Baseline**: The `builtin-baseline` refers to the baseline that is inherently part of your vcpkg installation or repository clone. This baseline is automatically used by vcpkg to resolve package versions unless you specify a different baseline.

### Role in `"version>="`

When you specify a dependency in your `vcpkg.json` with `"version>="`, you are indicating that your project requires at least a certain version of a library. Here’s how `builtin-baseline` plays a role:

1. **Version Resolution**: When you request a version with `"version>="`, vcpkg looks at the available versions in the context of the current baseline – which, unless specified otherwise, is the `builtin-baseline`.

2. **Ensuring Compatibility**: The baseline ensures that all package versions are compatible with each other. By using `builtin-baseline`, you're relying on a tested and stable set of package versions.

3. **Impact on Version Selection**: If the version you specified with `"version>="` is not available in your `builtin-baseline`, vcpkg will typically use the next available version that is greater than or equal to the one you specified. This is constrained by the versions available at that baseline.

### Updating Baseline

- **Updating vcpkg**: When you update your vcpkg installation, the `builtin-baseline` also updates. This means newer versions of packages become available, and the resolution of `"version>="` might yield different results.

- **Specifying a Different Baseline**: You can specify a different baseline in your `vcpkg.json` if you need to work with a set of package versions different from those in your current `builtin-baseline`.

### Conclusion

The `builtin-baseline` in vcpkg serves as a foundation for consistent and stable package version resolution. When using `"version>="`, it determines which versions of a package are available for installation, ensuring that your project gets a version that is at least as new as what you've specified, but still compatible with the rest of the packages in the vcpkg repository at that baseline.

## Building and Installing Dependencies 
Alternatively you can install everything globally/ locally

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
 If you don't want to type the architecture i.e. `x64-windows`, create a new environmental variable:
```  
Default x64/x32:
Name:  VCPKG_DEFAULT_TRIPLET
Value: x64-windows
```

You can create a `vcpkg.json` next to your cmake file:


All available packages listed [here](https://vcpkg.io/en/packages.html):

and locally they are  stored at:

`vcpkg\ports\ffmpeg\portfile.cmake`

Refs: [1](https://vcpkg.io/en/getting-started.html)

