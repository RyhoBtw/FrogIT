# Building (Windows)

#### Run in powershell as admin
```powershell
curl.exe -LJO "https://github.com/aminya/setup-cpp/releases/download/v1.7.1/setup-cpp-x64-windows.exe"
```
#### Install dependencies
```powershell
./setup-cpp-x64-windows --compiler msvc --cmake true --ninja true --ccache true --cppcheck true
```
#### Generate Visual Studio Solution
```powershell
git clone https://github.com/RyhoBtw/FrogIT
cd FrogIT
cmake -S . -B ./build -G "Visual Studio 17 2022"
```
#### Open and build in Visual Studio 2022

# Building (Ubuntu)

#### Run in shell
```bash
curl -LJO "https://github.com/aminya/setup-cpp/releases/download/v1.7.1/setup-cpp-x64-linux"
chmod +x setup-cpp-x64-linux
```
#### Install dependencies
```bash
sudo ./setup-cpp-x64-linux --compiler gcc --cmake true --ninja true --ccache true --cppcheck true
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