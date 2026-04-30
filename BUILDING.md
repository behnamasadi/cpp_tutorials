# Building and Installation

## Updating CMake

Navigate to [CMake's official](https://cmake.org/download/) website to get the link for the latest source tarball, then:

```sh
./bootstrap
make -j$(nproc)
sudo make install
sudo update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force
```

## Build on your machine

Configure:

```sh
cmake -G "Ninja Multi-Config" -S . -B build
```

Or specify an install prefix:

```sh
cmake -G "Ninja Multi-Config" -S . -B build -DCMAKE_INSTALL_PREFIX=/path/to/install
```

Build:

```sh
cmake --build build --config Release
# or
cmake --build build --config Debug
# or be more specific:
cmake --build build --target all --config Release
```

If you prefer presets:

```sh
cmake --preset ninja-multi
cmake --build --preset ninja-multi-debug
cmake --build --preset ninja-multi-release
```

## Build via Docker

### 1. Build the image

```sh
docker build -t cpp_tutorials .
```

### 2. Create the container

Mount the checked-out code into the container:

```sh
docker run --name <container-name> \
  -v <checked-out-path-on-host>:<path-in-the-container> \
  -it <docker-image-name>
```

For instance:

```sh
docker run --name cpp_container \
  -v /home/behnam/workspace/cpp_tutorials:/cpp_tutorials \
  -it cpp_tutorials
```

### 3. Start an existing container

```sh
docker start -i cpp_container
```

### 4. Remove unused images and containers

```sh
docker image prune -a
docker container prune
```

### GUI applications inside Docker

Run with X11 forwarding:

```sh
docker run --name cpp_container \
  -v /home/behnam/workspace/cpp_tutorials:/cpp_tutorials \
  --env="DISPLAY" --env="QT_X11_NO_MITSHM=1" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  -it cpp_tutorials
```

On the host (every time you run the container):

```sh
export containerId=$(docker ps -l -q)
xhost +local: docker inspect --format='{{ .Config.Hostname }}' $containerId
```

More background: [ros-developer.com Docker guide](https://ros-developer.com/2017/11/08/docker/).

## VSCode: use Ninja Multi-Config for all CMake projects

1. Open your VSCode workspace.
2. Press `Ctrl + ,` to open Settings (or File → Preferences → Settings).
3. Search for "cmake generator".
4. Click **Edit in settings.json**, add or modify:

```json
{
  "cmake.generator": "Ninja Multi-Config"
}
```

This makes Ninja Multi-Config the default generator for every CMake project in the workspace.
