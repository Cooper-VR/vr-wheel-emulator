#include <iostream>
#include "headers/openvr.h"
#include "headers/main.h"
#include <chrono>
#include <thread>


int main()
{
	vr::EVRInitError eError = vr::VRInitError_None;
	vr::IVRSystem* pSystem = vr::VR_Init(&eError, vr::VRApplication_Background);

	if (eError != vr::VRInitError_None) {
		std::cout << "Failed to iniitalize, is SteamVR running\n";
		return -1;
	}
	std::cout << "OpenVR initalized\n";

	
	vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];
	rightControllerIndex = pSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
	leftControllerIndex = pSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);

	while (1) {
		rotation_right = GetRotation(pSystem);
		rotation_left = GetRotation(pSystem);

		
		pSystem->GetControllerState(rightControllerIndex, &state, sizeof(state));
		
		if (rightControllerIndex != vr::k_unTrackedDeviceIndexInvalid) {
			
			if (pSystem->GetControllerState(rightControllerIndex, &rightControllerState, sizeof(rightControllerState))) {
				triggerValue_right = rightControllerState.rAxis[1].x;
				axis_right.x = rightControllerState.rAxis->x;
				axis_right.y = rightControllerState.rAxis->y;

				if (triggerValue_right > 0) {
					std::cout << "trigger amount: " << triggerValue_right << std::endl;
				}
				if (axis_right.GetMagnitude() > 0.1f) {
					std::cout << "joystick amount: " << axis_right.x << ", " << axis_right.y << '\n';
				}
				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					std::cout << "b-button-right pressed\n";
				}
				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					std::cout << "joystick Pressed in\n";
				}
				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					std::cout << "gripping\n";
				}
			}

		}
		if (leftControllerIndex != vr::k_unTrackedDeviceIndexInvalid) {

			if (pSystem->GetControllerState(leftControllerIndex, &leftControllerState, sizeof(leftControllerState))) {
				triggerValue_left = leftControllerState.rAxis[1].x;
				axis_left.x = leftControllerState.rAxis->x;
				axis_left.y = leftControllerState.rAxis->y;
				
				if (triggerValue_left > 0) {
					std::cout << "trigger amount: " << triggerValue_left << std::endl;
				}
				if (axis_left.GetMagnitude() > 0.1f) {
					std::cout << "joystick amount: " << axis_left.x << ", " << axis_left.y << '\n';
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					std::cout << "b-button-left pressed\n";
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					std::cout << "joystick Pressed in\n";
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					std::cout << "gripping\n";
				}


			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}


	return 1;
}
