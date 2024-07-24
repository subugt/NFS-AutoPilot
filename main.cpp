#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
// added support for linux too ( just for github ) 
#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
const DWORD CAR_X_ADDRESS = 0x0018BDF8;
const DWORD CAR_Y_ADDRESS = 0x0018C078;
#else
const uintptr_t CAR_X_ADDRESS = 0x0018BDF8;
const uintptr_t CAR_Y_ADDRESS = 0x0018C078;
#endif

#ifdef _WIN32
float readFloatFromMemory(HANDLE hProcess, DWORD address) {
    float value = 0;
    SIZE_T bytesRead;
    ReadProcessMemory(hProcess, (LPCVOID)address, &value, sizeof(value), &bytesRead);
    return value;
}

void sendKeyPress(UINT key, int duration) {
    keybd_event(key, 0, 0, 0);
    Sleep(duration);
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}

void setConsoleAlwaysOnTop() {
    HWND hwndConsole = GetConsoleWindow();
    if (hwndConsole != NULL) {
        SetWindowPos(hwndConsole, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <fcntl.h>
#include <cstring>

float readFloatFromMemory(uintptr_t address) {
    return 0.0f;
}

void sendKeyPress(int key, int duration) {
}

void setConsoleAlwaysOnTop() {
}
#endif

void updateDisplay(float carPosX, float carPosY, const std::string& action) {
    std::stringstream ss;
    ss << "Car Position (X, Y): (" << carPosX << ", " << carPosY << ")\n";
    ss << "Action: " << action;

    std::cout << "\033[2J\033[1;1H";
    std::cout << ss.str();
}

void controlCar() {
    const float POSITION_TOLERANCE = 1.0f;
    const int GAS_PRESS_DURATION = 800;
    const int REVERSE_PRESS_DURATION = 500;
    const int MANEUVER_PRESS_DURATION = 500;
    const int CHECK_INTERVAL = 5000;

    float lastPosX = readFloatFromMemory(CAR_X_ADDRESS);
    float lastPosY = readFloatFromMemory(CAR_Y_ADDRESS);
    bool isReversing = false;
    bool lastManeuverLeft = false;
    unsigned long lastCheckTime = 0;

    while (true) {
        float currentPosX = readFloatFromMemory(CAR_X_ADDRESS);
        float currentPosY = readFloatFromMemory(CAR_Y_ADDRESS);

        unsigned long currentTime = std::time(nullptr) * 1000;
        if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
            if (fabs(currentPosX - lastPosX) > POSITION_TOLERANCE || fabs(currentPosY - lastPosY) > POSITION_TOLERANCE) {
                if (isReversing) {
                    isReversing = false;
                    updateDisplay(currentPosX, currentPosY, "Applying Gas (W)");
                    sendKeyPress('W', GAS_PRESS_DURATION);
                } else {
                    updateDisplay(currentPosX, currentPosY, "Applying Gas (W)");
                    sendKeyPress('W', GAS_PRESS_DURATION);
                }
                lastPosX = currentPosX;
                lastPosY = currentPosY;
            } else {
                if (!isReversing) {
                    isReversing = true;
                    updateDisplay(currentPosX, currentPosY, "Reversing and Maneuvering (Z, A, D)");

                    if (lastManeuverLeft) {
                        sendKeyPress('D', MANEUVER_PRESS_DURATION);
                    } else {
                        sendKeyPress('A', MANEUVER_PRESS_DURATION);
                    }

                    sendKeyPress('Z', REVERSE_PRESS_DURATION);

                    updateDisplay(currentPosX, currentPosY, "Moving Forward (W)");

                    if (lastManeuverLeft) {
                        sendKeyPress('A', MANEUVER_PRESS_DURATION);
                    } else {
                        sendKeyPress('D', MANEUVER_PRESS_DURATION);
                    }

                    sendKeyPress('W', GAS_PRESS_DURATION);
                    lastCheckTime = std::time(nullptr) * 1000;
                }
            }
        }
        usleep(100000);
    }
}

int main() {
    setConsoleAlwaysOnTop();

#ifdef _WIN32
    DWORD processID;
    HWND hwnd = FindWindow(NULL, L"Need for Speed™ Most Wanted");
    if (hwnd == NULL) {
        std::cerr << "Game window not found!" << std::endl;
        return -1;
    }

    GetWindowThreadProcessId(hwnd, &processID);
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process!" << std::endl;
        return -1;
    }

    controlCar(hProcess);

    CloseHandle(hProcess);
#else
    controlCar();
#endif

    return 0;
}
