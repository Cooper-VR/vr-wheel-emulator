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

char path[] = "E:/cooperBower/github/vr-wheel-emulator/vr-wheel-emulator/";
char basePath[] = "images/base.png";
char brakePath[] = "images/brake.png";
char handbrakePath[] = "images/handbrake.png";
char unusedPath[] = "images/unused.png";
char lightPath[] = "images/lights.png";
char clutchPath[] = "images/clutch.png";
char currentPath[512];

float speed = 2.2;

bool modeSet = false;

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main()
{	
	std::cout << "Enter path to images folder\n";
	std::cin >> path;

	bool brakeSet = false;
	bool handbrakeSet = false;
	bool clutchSet = false;
	bool lightsSet = false;
	bool unusedSet = false;

	bool shifted = false;
	bool ungrippedLeft = true;
	bool ungrippedRight = true;

	bool turnedOff = true;
	bool turnedOn = false;

	bool gripPrintedLeft = false;
	bool gripPrintedRight = false;

	bool buttonsPressed[16];

	vr::EVRInitError eError = vr::VRInitError_None;
	vr::IVRSystem* pSystem = vr::VR_Init(&eError, vr::VRApplication_Overlay);

	if (eError != vr::VRInitError_None) {
		std::cout << "Failed to iniitalize, is SteamVR running\n";
		std::cin.get();
		return -1;
	}
	std::cout << "OpenVR initalized\n";

	vr::IVROverlay *oOverlay = (vr::IVROverlay*)vr::VR_GetGenericInterface(vr::IVROverlay_Version, &eError);
	if (eError != vr::VRInitError_None || !oOverlay) {
		std::cerr << "Failed to get overlay interface\n";
		vr::VR_Shutdown();
		std::cin.get();
		return -2;
	}

	vr::VROverlayHandle_t handle;
	//vr::VROverlayHandle_t iconHandle;

	const char* overlayKey = "com.cooper.vr-wheel-emulator";
	const char* overlayName = "vr-wheel-emulator";
	oOverlay->CreateOverlay(overlayKey, overlayName, &handle);

	oOverlay->SetOverlayColor(handle, 1, 1, 1);
	oOverlay->SetOverlayAlpha(handle, 1);
	oOverlay->SetOverlayWidthInMeters(handle, 0.1f);
	vr::VRTextureBounds_t bounds;
	bounds.uMin = 1;
	bounds.uMax = 0;
	bounds.vMin = 0;
	bounds.vMax = 1;

	oOverlay->SetOverlayTextureBounds(handle, &bounds);
	oOverlay->SetOverlayFromFile(handle, basePath);

	if (!vJoyEnabled()) {
		std::cerr << "vJoy driver not enabled: Failed to find vJoy device!" << std::endl;
		std::cin.get();
		return -2;
	}
	std::cout << "vjoy enabled\n";

	UINT iInterface = 1; // First vJoy device

	VjdStat status = GetVJDStatus(iInterface);
	if (status != VJD_STAT_FREE && status != VJD_STAT_OWN) {
		std::cerr << "vJoy device " << iInterface << " not available.\n";
		std::cin.get();
		return -1;
	}
	if (!AcquireVJD(iInterface)) {
		std::cerr << "Failed to acquire vJoy device " << iInterface << ".\n";
		std::cin.get();
		return -1;
	}
	std::cout << "vjoy all setup\n";

	vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];
	rightControllerIndex = pSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
	leftControllerIndex = pSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	auto lastTime = std::chrono::high_resolution_clock::now();

	vJoyAxisZ = 0;
	vJoyAxisXY = 0;
	vJoyAxisXZ = 0;

	upShiftButton = 14;
	downShiftButton = 12;
	headlightButton = 3;
	hornButton = 4;

	vr::HmdMatrix34_t transform;

	sprintf_s(currentPath, "%s%s", path, basePath);
	oOverlay->SetOverlayFromFile(handle, currentPath);

	for (size_t i = 0; i < 16; i++)
	{
		buttonsPressed[i] = false;
	}

	while (1) {
		transform.m[0][0] = 1.0f;  transform.m[0][1] = 0.0f;  transform.m[0][2] = 1.0f;  transform.m[0][3] = -0.2f;
		transform.m[1][0] = 0.0f;  transform.m[1][1] = 1.0f;  transform.m[1][2] = 0.0f;  transform.m[1][3] = -0.1f;
		transform.m[2][0] = 0.0f;  transform.m[2][1] = 0.0f;  transform.m[2][2] = 1.0f;  transform.m[2][3] = -0.7f; // 0.5 meters in front


		oOverlay->SetOverlayTransformTrackedDeviceRelative(handle, vr::k_unTrackedDeviceIndex_Hmd, &transform);
		oOverlay->ShowOverlay(handle);
		//for vjoy
		vJoyAxisY = static_cast<LONG>((triggerValue_right * -32767) + 32767);


		JOYSTICK_POSITION_V2 iReport;
		iReport.bDevice = iInterface;
		iReport.wAxisX = vJoyAxisX; // Set the X-axis to the normalized value
		iReport.wAxisY = vJoyAxisY;
		iReport.wAxisZ = vJoyAxisZ;
		iReport.wAxisXRot = vJoyAxisXY;
		iReport.wAxisYRot = vJoyAxisXZ;

		for (int i = 0; i < 128; ++i) {
			iReport.lButtons &= ~(1 << i);  // Set each button to false
		}


		SetBtn(false, iInterface, 2);

		if (!modeSet) {
			char mode;
			std::cout << "Set up input? y or n";
			std::cin >> mode;

			if (mode == 'y') {
				for (size_t i = 1; i <= 16; i++)
				{
					buttonsPressed[i-1] = false;
					SetBtn(false, iInterface, i);
				}


				steamCalibrate(iInterface, &iReport);
				modeSet = true;

				std::cout << "Calibration complete\n";
			}
			else {
				modeSet = true;
			}
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
		
		pSystem->GetControllerState(rightControllerIndex, &state, sizeof(state));

		if (rightControllerIndex != vr::k_unTrackedDeviceIndexInvalid) {
			
			if (pSystem->GetControllerState(rightControllerIndex, &rightControllerState, sizeof(rightControllerState))) {
				triggerValue_right = rightControllerState.rAxis[1].x;

				if (axis_right.GetMagnitude() > 0.1f) {
					if (axis_right.y > gear_shift_sense && !shifted) {
						std::cout << "up-shift\n";
						buttonsPressed[upShiftButton] = true;
						buttonsPressed[downShiftButton] = false;
						shifted = true;
					}
					else if (axis_right.y < -gear_shift_sense && !shifted) {
						std::cout << "down-shift\n";
						buttonsPressed[downShiftButton] = true;
						buttonsPressed[upShiftButton] = false;
						shifted = true;
					}
				}
				else {
					buttonsPressed[upShiftButton] = false;
					buttonsPressed[downShiftButton] = false;
					shifted = false;
				}

				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					axis_right.x = rightControllerState.rAxis[0].x;
					axis_right.y = rightControllerState.rAxis[0].y;

				}
				else {
					axis_right.x = 0;
					axis_right.y = 0;
				}
				if (rightControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					if (!gripPrintedRight) {
						std::cout << "gripping wheel\n";
						gripPrintedRight = true;
					}
					ungrippedRight = false;
					wheelAngle += angularVelocity_right.z * deltaTime * speed;
				}
				else {
					gripPrintedRight = false;
					ungrippedRight = true;
				}
			}

		}
		if (leftControllerIndex != vr::k_unTrackedDeviceIndexInvalid) {

			if (pSystem->GetControllerState(leftControllerIndex, &leftControllerState, sizeof(leftControllerState))) {
				triggerValue_left = leftControllerState.rAxis[1].x;
				//std::cout << "trigger amount: " << triggerValue_left << std::endl;

				if (triggerValue_left > 0.8 && currentMode == headlights) {
					buttonsPressed[headlightButton] = true;
					buttonsPressed[hornButton] = false;
				}
				else if (triggerValue_left > 0.8 && currentMode == horn) {
					buttonsPressed[hornButton] = true;
					buttonsPressed[headlightButton] = false;

				}
				else if (triggerValue_left <= 0.8){
					buttonsPressed[hornButton] = false;
					buttonsPressed[headlightButton] = false;
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

					if ((angle < 36 || angle > 360 - 36) && !clutchSet) {
						clutchSet = true;
						lightsSet = false;
						unusedSet = false;
						handbrakeSet = false;
						brakeSet = false;
						currentMode = clutch;
						sprintf_s(currentPath,  "%s%s", path, clutchPath);
						oOverlay->SetOverlayFromFile(handle, currentPath);
					}
					else if ((angle >= 36 && angle < 108) && !lightsSet) {
						currentMode = headlights;
						clutchSet = false;
						lightsSet = true;
						unusedSet = false;
						handbrakeSet = false;
						brakeSet = false;
						sprintf_s(currentPath, "%s%s", path, lightPath);

						oOverlay->SetOverlayFromFile(handle, currentPath);

					}
					else if ((angle >= 108 && angle < 180) && !unusedSet) {
						currentMode = horn;
						clutchSet = false;
						lightsSet = false;
						unusedSet = true;
						handbrakeSet = false;
						brakeSet = false;
						sprintf_s(currentPath, "%s%s", path, unusedPath);

						oOverlay->SetOverlayFromFile(handle, currentPath);

					}
					else if ((angle >= 180 && angle < 252) && !handbrakeSet) {
						currentMode = handBrake;
						clutchSet = false;
						lightsSet = false;
						unusedSet = false;
						handbrakeSet = true;
						brakeSet = false;
						sprintf_s(currentPath, "%s%s", path, handbrakePath);

						oOverlay->SetOverlayFromFile(handle, currentPath);

					}
					else if ((angle >= 252 && angle < 360 - 36) && !brakeSet){
						currentMode = extra;
						clutchSet = false;
						lightsSet = false;
						unusedSet = false;
						handbrakeSet = false;
						brakeSet = true;
						sprintf_s(currentPath, "%s%s", path, brakePath);

						oOverlay->SetOverlayFromFile(handle, currentPath);
						currentMode = brake;

					}

					
					std::cout << angle << ":" << currentMode << '\n';
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					std::cout << "b-button-left pressed\n";
				}	
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					axis_left.x = leftControllerState.rAxis->x;
					axis_left.y = leftControllerState.rAxis->y;

				}
				else {
					axis_left.x = 0;
					axis_left.y = 0;
				}
				if (leftControllerState.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					if (!gripPrintedLeft) {
						std::cout << "gripping wheel\n";
						gripPrintedLeft = true;
					}
					ungrippedLeft = false;
					wheelAngle += angularVelocity_left.z * deltaTime * speed;
				}
				else {
					gripPrintedLeft = false;
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
				SetBtn(false, iInterface, 9);
				buttonsPressed[6] = false;
				buttonsPressed[10] = false;
				vJoyAxisZ = static_cast<LONG>(triggerValue_left * 2 * 16384) + 16384;
				vJoyAxisXZ = 16383;
				break;
			case handBrake:
				buttonsPressed[6] = true;
				buttonsPressed[10] = false;
				vJoyAxisZ = 16383;
				vJoyAxisXZ = 16384;
				break;
			case clutch:
				buttonsPressed[6] = false;
				buttonsPressed[10] = false;
				vJoyAxisZ = 16383;
				vJoyAxisXZ = static_cast<LONG>(triggerValue_left * 2 * 16384) + 16384;
				break;
			case extra:
				buttonsPressed[10] = true;
				buttonsPressed[6] = false;
				vJoyAxisZ = 16383;
				vJoyAxisXZ = 16383;
				break;
			}
		}
		else {
			buttonsPressed[6] = false;
			buttonsPressed[10] = false;
			vJoyAxisZ = 16383;
			vJoyAxisXZ = 16383;
		}
		iReport.bDevice = iInterface;
		iReport.wAxisY = vJoyAxisY;
		iReport.wAxisZ = vJoyAxisZ;
		vJoyAxisX = static_cast<LONG>((wheelAngle / 90.0) * (16383 * 0.3)) + 16384;
		iReport.wAxisX = vJoyAxisX;
		iReport.wAxisYRot = vJoyAxisXY;
		iReport.wAxisZRot = vJoyAxisXZ;

		UpdateVJD(iInterface, &iReport);

		for (size_t i = 1; i <= 16; i++)
		{
			SetBtn(buttonsPressed[i-1], iInterface, i);
		}	

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	return 1;
}
