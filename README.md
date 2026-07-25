# Sauced

This repository contains the source code for **DnkSaus**, an audio plugin built with JUCE and CMake.

## Building the plugin (for non-technical collaborators)

### 1. Install the required tools

- **Xcode Command Line Tools** (provides the compiler). Open the Terminal app and run:
  ```
  xcode-select --install
  ```
  If a window pops up asking to install, click "Install" and wait for it to finish.

- **CMake**. The easiest way is with [Homebrew](https://brew.sh):
  ```
  brew install cmake
  ```

### 2. Get the code

If you haven't already, clone this repository and move into its folder:
```
git clone git@github.com:rainbowcircuit/Sauced.git
cd Sauced
```

### 3. Build the plugin

From inside the `Sauced` folder, run these two commands:
```
cmake -B build
cmake --build build
```

- The first command sets up the project in a folder called `build/` (this folder is created automatically and is not part of the repository).
- The second command compiles the plugin. This can take several minutes the first time, since it also downloads the JUCE framework.

### 4. Find the built plugin

Once the build finishes, the compiled plugin files (VST3, AU, and a Standalone app) will be located inside:
```
build/Dnks_artefacts/
```

`COPY_PLUGIN_AFTER_BUILD` is enabled, so the VST3 and AU versions are also automatically copied to your system's plugin folders, ready to be used in a DAW (Logic, Ableton, etc.).

### Rebuilding after changes

If someone updates the source code, you don't need to redo step 1 or 2 — just run step 3 again:
```
cmake --build build
```
