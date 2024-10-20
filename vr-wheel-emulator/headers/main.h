#pragma once

#include <iostream>
#include "openvr.h"
#include <cmath>
#include <chrono>
#include <thread>
#include <Windows.h>
#include "headers/public.h"
#include "headers/vjoyinterface.h"

const double PI = 3.14159265358979323846;
const float gear_shift_sense = 0.25;
const float mode_change_sense = 0.2;
double deltaTime = 0.0;
float wheelAngle = 0;
const float wheelReboundSpeed = 300;
vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];
float triggerValue_right;
float triggerValue_left;
LONG vJoyAxisX;
LONG vJoyAxisY;
LONG vJoyAxisZ = 0;
LONG vJoyAxisXY = 0;
LONG vJoyAxisXZ = 0;

UINT upShiftButton = 14;
UINT downShiftButton = 12;
UINT headlightButton = 3;
UINT hornButton = 4;

/// <summary>
/// an easier way to store three values like x, y, z
/// </summary>
class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3(float x_value, float y_value, float z_value) {
        x = x_value;
        y = y_value;
        z = z_value;
    }

    Vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }
};

/// <summary>
/// meant mainly for axis movement
/// </summary>
class Vector2 {
public:
    float x, y;

    Vector2(float x_value, float y_value) {
        x = x_value;
        y = y_value;
    }

    Vector2()
    {
        x = 0;
        y = 0;
    }

    float GetMagnitude() {
        return hypot(x, y);
    }
};


/// <summary>
/// gets the angular velocity of a device
/// </summary>
/// <param name="pSystem">the openvr system</param>
/// <param name="index">index of the tracked device. this is the device you are getting angular velocity of</param>
/// <returns>return a vector3 of the angular velocity</returns>
Vector3 getAngularVelocity(vr::IVRSystem *pSystem, int index) {
    pSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, trackedDevicesPose, vr::k_unMaxTrackedDeviceCount);

    if (trackedDevicesPose[index].bPoseIsValid) {

        Vector3 angularVelocity;

        angularVelocity.x = trackedDevicesPose[index].vAngularVelocity.v[0];
        angularVelocity.y = trackedDevicesPose[index].vAngularVelocity.v[1];
        angularVelocity.z = trackedDevicesPose[index].vAngularVelocity.v[2];

        return angularVelocity;
    }
    return *(new Vector3());
}
/// <summary>
/// pretty much just prints a vector3 easier
/// </summary>
/// <param name="rotation">any vector3 will work here</param>
void PrintRotation(Vector3 rotation) {
	std::cout << "rotation: " << rotation.x << ' ' << rotation.y << ' ' << rotation.z << '\n';
}

/// <summary>
/// converts the vector3 from radian to degrees 
/// </summary>
/// <param name="Rotation">vector3 to change</param>
void ConvertToDegrees(Vector3* Rotation) {
    Rotation->x *= (180 / (float)PI);
    Rotation->y *= (180 / (float)PI);
    Rotation->z *= (180 / (float)PI);
}

/// <summary>
/// defferent modes like braking, etc
/// </summary>
enum leftHandModes {
    brake = 10,
    handBrake = 11,
    headlights = 12,
    horn = 13,
    clutch = 14,
	extra = 15,
    none = 9
};

void setButton(UINT device, UINT button) {
    int i = 0;
    SetBtn(true, device, button);
    while (i < 50) {
		i++;
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
	SetBtn(false, device, button);

}
void setConfigAxis(UINT device, JOYSTICK_POSITION_V2* report) {
	std::cout << "setting axisX\n";
	report->wAxisX = 16384;
    UpdateVJD(device, report);
	while (report->wAxisX > 0) {
		report->wAxisX -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisX < 16384) {
		report->wAxisX += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	report->wAxisX = 16384;
    UpdateVJD(device, report);
	while (report->wAxisX < 32768) {
		report->wAxisX += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisX > 16384) {
		report->wAxisX -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	report->wAxisX = 16384;


    std::cout << "setting axisY\n";
	report->wAxisY = 16384;
    UpdateVJD(device, report);
	while (report->wAxisY > 0) {
		report->wAxisY -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisY < 16384) {
		report->wAxisY += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	report->wAxisY = 16384;
    UpdateVJD(device, report);
	while (report->wAxisY < 32768) {
		report->wAxisY += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisY > 16384) {
		report->wAxisY -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	report->wAxisY = 16384;
	setButton(device, 9);



	std::cout << "setting axisZ\n";
	report->wAxisZ = 16384;
    UpdateVJD(device, report);
	while (report->wAxisZ > 0) {
		report->wAxisZ -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisZ < 16384) {
		report->wAxisZ += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	report->wAxisZ = 16384;
    UpdateVJD(device, report);
	while (report->wAxisZ < 32768) {
		report->wAxisZ += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisZ > 16384) {
		report->wAxisZ -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	report->wAxisZ = 16384;


	report->wAxisZRot = 16384;
    UpdateVJD(device, report);
	while (report->wAxisZRot < 32768) {
		report->wAxisZRot += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisZRot > 16384) {
		report->wAxisZRot -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	report->wAxisZRot = 16384;

	report->wAxisZRot = 16384;
    UpdateVJD(device, report);
	while (report->wAxisZRot > 0) {
		report->wAxisZRot -= 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	while (report->wAxisZRot < 16384) {
		report->wAxisZRot += 30;
		UpdateVJD(device, report);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	report->wAxisZ = 16384;


}


void steamCalibrate(UINT device,JOYSTICK_POSITION_V2 *report) {
    int i = 0;

	std::cout << "setting buttons\n";
    for (UINT d = 1; d <= 8; d++)
    {
		setButton(device, d);
    }

	for (size_t i = 1; i < 16; i++)
	{
		SetBtn(false, device, i);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));


    setConfigAxis(device, report);

    //trigger
	for (size_t i = 10; i <= 18; i++)
	{
		if (i == 12) {
			std::cout << "setting trigger\n";
			report->wAxisXRot = 0;
			UpdateVJD(device, report);
			while (report->wAxisXRot < 32768) {
				report->wAxisXRot += 30;
				UpdateVJD(device, report);
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			while (report->wAxisXRot > 0) {
				report->wAxisXRot -= 30;
				UpdateVJD(device, report);
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			report->wAxisXRot = 0;

		}
		else {
			setButton(device, i);
		}
	}
}

leftHandModes currentMode = none;
Vector3 angularVelocity_left;
Vector3 angularVelocity_right;
vr::TrackedDeviceIndex_t rightControllerIndex;
vr::TrackedDeviceIndex_t leftControllerIndex;
vr::VRControllerState_t state;
vr::VRControllerState_t rightControllerState;
vr::VRControllerState_t leftControllerState;
Vector2 axis_left;
Vector2 axis_right;
