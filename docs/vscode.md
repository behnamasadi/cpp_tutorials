**important file you can put inside your `.vscode` directory**  when working with **C++ and CMake projects** (especially if you’re using `CMakePresets.json` and `CMakeLists.txt`):


---

Here is my  (simplified) `CMakePresets.json`

```json

{
  "version": 3,
  "configurePresets": [
    {
      "name": "ninja-multi",
      "generator": "Ninja Multi-Config",
      "binaryDir": "${sourceDir}/build/",
      "cacheVariables": {
        "CMAKE_POLICY_DEFAULT_CMP0048": "NEW",
        "CMAKE_CONFIGURATION_TYPES": "Debug;Release;RelWithDebInfo;MinSizeRel"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "ninja-multi-debug",
      "configurePreset": "ninja-multi",
      "configuration": "Debug"
    },
    {
      "name": "ninja-multi-release",
      "configurePreset": "ninja-multi",
      "configuration": "Release"
    },
    ...
  ]
}
```



## 1. tasks.json

Defines **custom build tasks** you can run with `Ctrl+Shift+B` or from the Command Palette.

Even though you have `CMakePresets.json`, sometimes you still want tasks for:

* running `ninja` or `make` manually
* generating compile commands
* running custom scripts

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "CMake Configure (ninja-multi)",
      "type": "shell",
      "command": "cmake",
      "args": [
        "--preset",
        "ninja-multi"
      ],
      "group": "build",
      "problemMatcher": []
    },
    {
      "label": "CMake Build Debug",
      "type": "shell",
      "command": "cmake",
      "args": [
        "--build",
        "build",
        "--config",
        "Debug"
      ],
      "group": "build",
      "problemMatcher": ["$gcc"]
    },
    {
      "label": "CMake Build Release",
      "type": "shell",
      "command": "cmake",
      "args": [
        "--build",
        "build",
        "--config",
        "Release"
      ],
      "group": "build",
      "problemMatcher": ["$gcc"]
    }
  ]
}

```

If you want to limit parallel jobs (e.g. 8 jobs max)

```bash
cmake --build build --config Release --parallel 8
```
So in tasks.json, adjust your build tasks like this:

```json

{
  "label": "CMake Build Release",
  "type": "shell",
  "command": "cmake",
  "args": [
    "--build",
    "build",
    "--config",
    "Release",
    "--parallel",
    "8"
  ],
  "group": "build",
  "problemMatcher": ["$gcc"]
}


```


**Important things to put:**

* `label`: Name shown in the task list.
* `command`: The tool you want to run (e.g., `cmake`, `ninja`, `make`).
* `args`: Arguments (e.g., presets).
* `problemMatcher`: Helps VS Code parse compiler errors and warnings.
* `group`: (e.g., `build`) so it appears in *Run Build Task* menu.

---

## 2. settings.json

This file **overrides workspace settings** just for this folder.
When using C++ and CMake, this is very important for:

* IntelliSense configuration
* Default CMake presets
* Code formatting
* Compiler paths

 **Example**:

```json
{
  // Configure on open
  "cmake.configureOnOpen": true,

  // Pick your default configure and build presets
  "cmake.defaultConfigurePreset": "ninja-multi",
  "cmake.defaultBuildPreset": "ninja-multi-release",

  // Let IntelliSense know about the compiler (optional but good for accuracy)
  "C_Cpp.default.compilerPath": "/usr/bin/clang++", // or gcc if you use gcc

  // Optional - adjust include paths for IntelliSense if you have custom headers
  "C_Cpp.default.includePath": [
    "${workspaceFolder}/include",
    "${workspaceFolder}/external"
  ],

  // Optional - default IntelliSense mode
  "C_Cpp.default.intelliSenseMode": "linux-clang-x64", // or linux-gcc-x64

  // Optional - use clang-format for formatting
  "C_Cpp.clang_format_style": "file",

  // Optional - build directory pattern so VS Code finds the right place
  "cmake.buildDirectory": "${workspaceFolder}/build"
}

```

**Important things to put:**

* `cmake.defaultConfigurePreset` and `cmake.defaultBuildPreset`
* `C_Cpp.default.compilerPath`
* `C_Cpp.default.includePath`
* `C_Cpp.default.defines`
* Formatting and code style options
* Enable `cmake.configureOnOpen`

---

##  3. launch.json

Defines **debug launch configurations**. Since we are using presets, VS Code can grab the executable path automatically using:

```json
"${command:cmake.launchTargetPath}"
```



 **Example**:
Here’s an example for debugging your app built in Release:

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug My App (Release)",
      "type": "cppdbg",
      "request": "launch",
      "program": "${command:cmake.launchTargetPath}",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "externalConsole": true,
      "MIMode": "gdb",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ]
    }
  ]
}

```

**Important things to put:**

* `program`: Path to your executable (usually in `build/{preset}`).
* `args`: Command-line arguments.
* `cwd`: Working directory.
* `MIMode`: `gdb` (Linux) or `lldb` (macOS) or `cppvsdbg` (Windows).
* `setupCommands`: Enable pretty printing for better variable visualization.
* `environment`: Any custom environment variables.

---

##  Integrate CMakePresets

Since you use `CMakePresets.json`, you can **directly refer to presets**:

**Launch.json** with `cmake.buildDirectory`:

```json
"program": "${command:cmake.launchTargetPath}"
```

This will pick the executable from your active CMake preset.

**settings.json**:

```json
{
  // Configure on open
  "cmake.configureOnOpen": true,

  // Pick your default configure and build presets
  "cmake.defaultConfigurePreset": "ninja-multi",
  "cmake.defaultBuildPreset": "ninja-multi-release",

  // Let IntelliSense know about the compiler (optional but good for accuracy)
  "C_Cpp.default.compilerPath": "/usr/bin/clang++", // or gcc if you use gcc

  // Optional - adjust include paths for IntelliSense if you have custom headers
  "C_Cpp.default.includePath": [
    "${workspaceFolder}/include",
    "${workspaceFolder}/external"
  ],

  // Optional - default IntelliSense mode
  "C_Cpp.default.intelliSenseMode": "linux-clang-x64", // or linux-gcc-x64

  // Optional - use clang-format for formatting
  "C_Cpp.clang_format_style": "file",

  // Optional - build directory pattern so VS Code finds the right place
  "cmake.buildDirectory": "${workspaceFolder}/build"
}

```

This makes everything auto-sync.

---