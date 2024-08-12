#include <iostream>
#include "headers/openvr.h"
#include "headers/main.h"
#include <chrono>
#include <thread>
#include <Windows.h>
#include "headers/public.h"
#include "headers/vjoyinterface.h"
#include "headers/glad.h"
#include "GLFW/glfw3.h"

char path[] = "E:/Capture1.PNG";

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main()
{	
	bool shifted = false;
	bool ungrippedLeft = true;
	bool ungrippedRight = true;

	bool turnedOff = true;
	bool turnedOn = false;

	vr::EVRInitError eError = vr::VRInitError_None;
	vr::IVRSystem* pSystem = vr::VR_Init(&eError, vr::VRApplication_Overlay);

	if (eError != vr::VRInitError_None) {
		std::cout << "Failed to iniitalize, is SteamVR running\n";
		return -1;
	}
	std::cout << "OpenVR initalized\n";

	vr::IVROverlay *oOverlay = (vr::IVROverlay*)vr::VR_GetGenericInterface(vr::IVROverlay_Version, &eError);
	if (eError != vr::VRInitError_None || !oOverlay) {
		std::cerr << "Failed to get overlay interface\n";
		vr::VR_Shutdown();
		return -2;
	}

	vr::VROverlayHandle_t* handle = new vr::VROverlayHandle_t();
	oOverlay->SetOverlayWidthInMeters(*handle, 1);
	vr::VRTextureBounds_t bounds;
	bounds.uMin = 1;
	bounds.uMax = 0;
	bounds.vMin = 0;
	bounds.vMax = 1;
	oOverlay->GetOverlayTextureBounds(*handle, &bounds);

	oOverlay->SetOverlayFromFile(*handle, path);
	vr::VROverlayHandle_t* iconHandle = new vr::VROverlayHandle_t;

	oOverlay->CreateDashboardOverlay("com.cooper.vr-wheel-emulator", "vr-wheel-emulator", handle, iconHandle);
	oOverlay->ShowOverlay(*handle);

	if (!vJoyEnabled()) {
		std::cerr << "vJoy driver not enabled: Failed to find vJoy device!" << std::endl;
		return -2;
	}
	std::cout << "vjoy enabled";

	UINT iInterface = 1; // First vJoy device

	VjdStat status = GetVJDStatus(iInterface);
	if (status != VJD_STAT_FREE && status != VJD_STAT_OWN) {
		std::cerr << "vJoy device " << iInterface << " not available.\n";
		return -1;
	}
	if (!AcquireVJD(iInterface)) {
		std::cerr << "Failed to acquire vJoy device " << iInterface << ".\n";
		return -1;
	}
	std::cout << "vjoy all setup";

	vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];
	rightControllerIndex = pSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
	leftControllerIndex = pSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	auto lastTime = std::chrono::high_resolution_clock::now();

	LONG vJoyAxisX;
	LONG vJoyAxisY;
	LONG vJoyAxisZ = 0;
	LONG vJoyAxisXY = 0;
	LONG vJoyAxisXZ = 0;

	UINT upShiftButton = 1;
	UINT downShiftButton = 2;
	UINT headlightButton = 3;
	UINT hornButton = 4;
	while (1) {

		oOverlay->ShowOverlay(*handle);
		//for vjoy
		vJoyAxisX = static_cast<LONG>((wheelAngle / 90.0) * (16383 * 0.3)) + 16384;
		vJoyAxisY = static_cast<LONG>(triggerValue_right * 32767);
			

		JOYSTICK_POSITION_V2 iReport;
		iReport.bDevice = iInterface;
		iReport.wAxisX = vJoyAxisX; // Set the X-axis to the normalized value
		iReport.wAxisY = vJoyAxisY;
		iReport.wAxisZ = vJoyAxisZ;
		iReport.wAxisXRot = vJoyAxisXY;
		iReport.wAxisYRot = vJoyAxisXZ;
		


		// Update the vJoy device
		if (!UpdateVJD(iInterface, &iReport)) {
			std::cerr << "Failed to update vJoy device.\n";
		}

		//for deltaTime
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = currentTime - lastTime;
		deltaTime = elapsed.count();
		lastTime = currentTime;

		angularVelocity_left = getAngularVelocity(pSystem, leftControllerIndex);
		angularVelocity_right = getAngularVelocity(pSystem, rightControllerIndex);

		ConvertToDegrees(&angularVelocity_right);
		ConvertToDegrees(&angularVelocity_left);
		//std::cout << wheelAngle << '\n';
		
		pSystem->GetControllerState(rightControllerIndex, &state, sizeof(state));

		if (rightControllerIndex != vr::k_unTrackedDeviceIndexInvalid) {
			
			if (pSystem->GetControllerState(rightControllerIndex, &rightControllerState, sizeof(rightControllerState))) {
				triggerValue_right = rightControllerState.rAxis[1].x;

				if (triggerValue_right > 0) {
					std::cout << "trigger amount: " << triggerValue_right << std::endl;
									}
				if (axis_right.GetMagnitude() > 0.1f) {
					if (axis_right.y > gear_shift_sense && !shifted) {
						std::cout << "up-shift\n";
						SetBtn(true, iInterface, upShiftButton);
						shifted = true;
					}
					else if (axis_right.y < -gear_shift_sense && !shifted) {
						std::cout << "down-shift\n";
						SetBtn(true, iInterface, downShiftButton);
						shifted = true;
					}
				}
				else {
					SetBtn(false, iInterface, upShiftButton);
					SetBtn(false, iInterface, downShiftButton);
					shifted = false;
				}

				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					std::cout << "b-button-right pressed\n";
				}
				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					std::cout << "joystick Pressed in\n";
					axis_right.x = rightControllerState.rAxis[0].x;
					axis_right.y = rightControllerState.rAxis[0].y;

				}
				else {
					axis_right.x = 0;
					axis_right.y = 0;
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
				//std::cout << "trigger amount: " << triggerValue_left << std::endl;

				if (triggerValue_left > 0) {

					if (currentMode == headlights && !turnedOn) {
						SetBtn(false, iInterface, headlightButton);
						std::cout << "headlights: on\n";
						turnedOn = true;
						turnedOff = false;
					}
				}
				else if (!turnedOff){
					std::cout << "headlights: off\n";
					SetBtn(true, iInterface, headlightButton);
					turnedOff = true;
					turnedOn = false;
				}
				if (triggerValue_left > 0.8) {
					SetBtn(true, iInterface, hornButton);

				}
				else {
					SetBtn(false, iInterface, hornButton);

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

					if (angle < 36 || angle > 360 - 36) {
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
					//SetBtn(true, iInterface, headlightButton);
					std::cout << "b-button-left pressed\n";
				}
				else {
					//SetBtn(false, iInterface, headlightButton);
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					std::cout << "joystick Pressed in\n";
					axis_left.x = leftControllerState.rAxis->x;
					axis_left.y = leftControllerState.rAxis->y;

				}
				else {
					axis_left.x = 0;
					axis_left.y = 0;
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


		if (triggerValue_left > 0) {
			switch (currentMode) {
			case brake:
				vJoyAxisZ = static_cast<LONG>(triggerValue_left * 32767);
				vJoyAxisXY = 0;
				vJoyAxisXZ = 0;
				break;
			case handBrake:
				vJoyAxisXY = static_cast<LONG>(triggerValue_left * 32767);
				vJoyAxisZ = 0;
				vJoyAxisXZ = 0;
				break;
			case clutch:
				vJoyAxisXZ = static_cast<LONG>(triggerValue_left * 32767);
				vJoyAxisZ = 0;
				vJoyAxisXY = 0;
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}


	return 1;
}
