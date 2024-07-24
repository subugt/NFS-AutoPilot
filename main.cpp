#include <iostream>
#include <Windows.h>
#include <string>
#include <sstream>
#include <cmath>

const DWORD CAR_X_ADDRESS = 0x0018BDF8; // x adress found via cheat engine.
const DWORD CAR_Y_ADDRESS = 0x0018C078; // y adress found via cheat engine.

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

void updateDisplay(float carPosX, float carPosY, const std::string& action) {
    std::stringstream ss;
    ss << "Car Position (X, Y): (" << carPosX << ", " << carPosY << ")\n";
    ss << "Action: " << action;

   
    system("cls"); // clear console
    std::cout << ss.str();
}

void controlCar(HANDLE hProcess) {
    const float POSITION_TOLERANCE = 1.0f;
    const int GAS_PRESS_DURATION = 800; // ms
    const int REVERSE_PRESS_DURATION = 500; // ms
    const int MANEUVER_PRESS_DURATION = 500; // ms
    const int CHECK_INTERVAL = 5000; // ms

    float lastPosX = readFloatFromMemory(hProcess, CAR_X_ADDRESS);
    float lastPosY = readFloatFromMemory(hProcess, CAR_Y_ADDRESS);
    bool isReversing = false;
    bool lastManeuverLeft = false;
    DWORD lastCheckTime = GetTickCount();

    while (true) {
        float currentPosX = readFloatFromMemory(hProcess, CAR_X_ADDRESS);
        float currentPosY = readFloatFromMemory(hProcess, CAR_Y_ADDRESS);

        DWORD currentTime = GetTickCount();
        if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
            if (fabs(currentPosX - lastPosX) > POSITION_TOLERANCE || fabs(currentPosY - lastPosY) > POSITION_TOLERANCE) {
                if (isReversing) {
                    isReversing = false;
                    updateDisplay(currentPosX, currentPosY, "Applying Gas (W)");
                    sendKeyPress('W', GAS_PRESS_DURATION);
                }
                else {
                    updateDisplay(currentPosX, currentPosY, "Applying Gas (W)");
                    sendKeyPress('W', GAS_PRESS_DURATION);
                }
                lastPosX = currentPosX;
                lastPosY = currentPosY;
            }
            else {
                if (!isReversing) {
                    isReversing = true;
                    updateDisplay(currentPosX, currentPosY, "Reversing and Maneuvering (Z, A, D)");

                    if (lastManeuverLeft) {
                        sendKeyPress('D', MANEUVER_PRESS_DURATION);
                    }
                    else {
                        sendKeyPress('A', MANEUVER_PRESS_DURATION);
                    }

                    sendKeyPress('Z', REVERSE_PRESS_DURATION);

                    updateDisplay(currentPosX, currentPosY, "Moving Forward (W)");

                    if (lastManeuverLeft) {
                        sendKeyPress('A', MANEUVER_PRESS_DURATION);
                    }
                    else {
                        sendKeyPress('D', MANEUVER_PRESS_DURATION);
                    }

                    sendKeyPress('W', GAS_PRESS_DURATION);
                    lastCheckTime = GetTickCount();
                }
            }
        }
        Sleep(100);
    }
}

void setConsoleAlwaysOnTop() {
    HWND hwndConsole = GetConsoleWindow();
    if (hwndConsole != NULL) {
        SetWindowPos(hwndConsole, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}

int main() {
   
    setConsoleAlwaysOnTop();  // Set console to always on top

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

    return 0;
}
