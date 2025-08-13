# IDAKeyGen

A key/licence generator for modern versions of IDA Pro 9.x including all add ons and cleaner formatting of the hexlic file

(for educational purposes only)

---

## Building

### GNU/Linux

  1. Make sure you have standard C and C++ development libraries along with nlohmann json and openssl development packages installed on your distro

   ```bash
   git clone https://github.com/StrafeUltra/IDAKeyGen.git
   cd IDAKeyGen
   make
```

### Windows

  1. Open the included Visual Studio 2022 project in the "Windows" subfolder
  2. Make sure you have vcpkg setup if not check it in the Visual Studio Installer, and then in the Visual Studio shell type in "vcpkg integrate install"
