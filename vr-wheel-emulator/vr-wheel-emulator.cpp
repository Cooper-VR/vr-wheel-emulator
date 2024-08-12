#include <iostream>
#include "headers/openvr.h"
#include "headers/main.h"
#include <chrono>
#include <thread>


int main()
{
	bool shifted = false;
	bool ungrippedLeft = true;
	bool ungrippedRight = true;

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
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (1) {
		//for deltatime
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;

		angularVelocity_left = getAngularVelocity(pSystem, leftControllerIndex);
		angularVelocity_right = getAngularVelocity(pSystem, rightControllerIndex);

		ConvertToDegrees(&angularVelocity_right);
		ConvertToDegrees(&angularVelocity_left);
		std::cout << wheelAngle << '\n';
		
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
					if (axis_right.y > gear_shift_sense && !shifted) {
						std::cout << "up-shift\n";
						shifted = true;
					}
					else if (axis_right.y < -gear_shift_sense && !shifted) {
						std::cout << "down-shift\n";
						shifted = true;
					}
				}
				else {
					shifted = false;
				}

				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					std::cout << "b-button-right pressed\n";
				}
				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					//std::cout << "joystick Pressed in\n";
				}
				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					std::cout << "gripping wheel\n";
					ungrippedRight = false;
					wheelAngle += angularVelocity_right.z * deltaTime;
				}
				else {
					ungrippedRight = true;
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
				if (axis_left.GetMagnitude() > mode_change_sense) {

					float angle = std::atan(axis_left.y / axis_left.x);
					angle *= (180 / PI);

					//for handling the restricted domain of arctan
					if (axis_left.x < 0 && axis_left.y < 0) {
						angle += 180;
					}
					else if (axis_left.x < 0) {
						angle += 180;
					}else if (axis_left.y < 0){
						angle += 360;
					}	

					if (angle < 36 && angle > 360 - 36) {
						currentMode = clutch;
					}
					else if (angle >= 36 && angle < 108) {
						currentMode = headlights;
					}
					else if (angle >= 108 && angle < 180) {
						currentMode = horn;
					}
					else if (angle >= 180 && angle < 252) {
						currentMode = handBrake;
					}
					else {
						currentMode = brake;
					}

					
					std::cout << angle << ":" << currentMode << '\n';
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					std::cout << "b-button-left pressed\n";
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					//std::cout << "joystick Pressed in\n";
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					std::cout << "gripping\n";
					ungrippedLeft = false;
					wheelAngle += angularVelocity_left.z * deltaTime;
				}
				else {
					ungrippedLeft = true;
				}
			}
		}

		//handles when wheel is let go
		if (ungrippedLeft && ungrippedRight && (wheelAngle < -0.8 || wheelAngle > 0.8)) {
			if (wheelAngle > 0) {
				wheelAngle -= wheelReboundSpeed * deltaTime;
			}
			else if (wheelAngle < 0) {
				wheelAngle += wheelReboundSpeed * deltaTime;
			}
		}
		else if (ungrippedLeft && ungrippedRight) {
			wheelAngle = 0;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}


	return 1;
}
