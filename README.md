# NFS-AutoPilot
autopilot project still in development

This project is developed to use autopilot in the game Need for Speed™ Most Wanted by tracking the car's position. Written in cpp it reads the car's position information and automatically applies specific actions (accelerating, reversing, maneuvering).

## Showcase
https://streamable.com/k1iha7 
## Features

- **Read Car Position:** Reads the X and Y coordinates of the car within the game.
- **Apply Actions:** Automatically applies actions such as accelerating, reversing, and maneuvering based on changes in the car's position.
- **Console Display:** Updates the console with the current car position and the action being performed.

## Requirements

- Windows operating system
- Visual Studio or a C++ compiler
- Developer privileges (administrator rights may be required)

## Usage

1. In the `main` function, verify the game window name and ensure the necessary permissions are granted.
2. Use the `CMakeLists.txt` file to compile and run the code.

## Building and Running

1. Install [CMake](https://cmake.org/download/) and a C++ compiler.
2. Run the following commands in the terminal or command prompt in the project directory:

    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

3. Once the build is complete, you can start controlling the car automatically by running `CarControlAutomation.exe`.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT). Please refer to the LICENSE file for details.

## Contact

For questions or contributions, please reach out to [discord:subugt](https://discord.com/users/1088544030205431859).
