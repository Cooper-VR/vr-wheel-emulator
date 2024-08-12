#pragma once

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL

#include "openvr.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <cmath>
#include <chrono>
#include <thread>

const double PI = 3.14159265358979323846;
const float gear_shift_sense = 0.25;
const float mode_change_sense = 0.2;
double deltaTime = 0.0;
float wheelAngle = 0;
const float wheelReboundSpeed = 100;
vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];

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

void PrintRotation(Vector3 rotation) {
	std::cout << "rotation: " << rotation.x << ' ' << rotation.y << ' ' << rotation.z << '\n';
}

/// <summary>
/// radians * (180 /pi) = degree (not needed anymore because glm stuff)
/// </summary>
/// <param name="Rotation"></param>
void ConvertToDegrees(Vector3* Rotation) {
    Rotation->x *= (180 / (float)PI);
    Rotation->y *= (180 / (float)PI);
    Rotation->z *= (180 / (float)PI);
}

enum leftHandModes {
    brake = 10,
    handBrake = 11,
    headlights = 12,
    horn = 13,
    clutch = 14,
    none = 9
};

leftHandModes currentMode = none;
Vector3 angularVelocity_left;
Vector3 angularVelocity_right;
vr::TrackedDeviceIndex_t rightControllerIndex;
vr::TrackedDeviceIndex_t leftControllerIndex;
vr::VRControllerState_t state;
vr::VRControllerState_t rightControllerState;
vr::VRControllerState_t leftControllerState;
float triggerValue_right;
float triggerValue_left;
Vector2 axis_left;
Vector2 axis_right;
