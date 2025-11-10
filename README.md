## Building (Windows)

#### Install Visual Studio 2022: 
https://visualstudio.microsoft.com/de/

#### Run in powershell as admin:
```powershell
curl.exe -LJO "https://github.com/aminya/setup-cpp/releases/download/v1.7.1/setup-cpp-x64-windows.exe"
```

#### Install remaining dependencies
```powershell
./setup-cpp-x64-windows --compiler msvc --cmake true --ninja true --ccache true --cppcheck true
```

#### Generate Visual Studio Solution in new a shell
```powershell
git clone https://github.com/RyhoBtw/FrogIT
cd FrogIT
cmake -S . -B ./build -G "Visual Studio 17 2022"
```

#### Open solution and build inside Visual Studio 2022 using ctrl + shift + b

## Building (Linux)

#### Run in shell
```bash
curl -LJO "https://github.com/aminya/setup-cpp/releases/download/v1.7.1/setup-cpp-x64-linux"
chmod +x setup-cpp-x64-linux
```
#### Install dependencies
```bash
sudo ./setup-cpp-x64-linux --compiler gcc --cmake true --ninja true --ccache true --cppcheck true
```

#### Install remaining dependencies
```bash
sudo apt update
sudo apt install \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libfreetype-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev \
    libfreetype-dev
```

#### Generate build files
```bash
git clone https://github.com/RyhoBtw/FrogIT
cd FrogIT
cmake -S . -B ./build
```
#### Build project
```bash
cmake --build ./build -- -j$(nproc)
```