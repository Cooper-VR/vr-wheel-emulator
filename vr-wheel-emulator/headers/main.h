#pragma once

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL

#include "openvr.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <cmath>

const double PI = 3.14159265358979323846;

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

Vector3 GetRotation(vr::IVRSystem *pSystem) {
    vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];

    pSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, trackedDevicesPose, vr::k_unMaxTrackedDeviceCount);

    if (trackedDevicesPose[vr::k_unTrackedDeviceIndex_Hmd + 4].bPoseIsValid) {
        vr::HmdMatrix34_t mat = trackedDevicesPose[vr::k_unTrackedDeviceIndex_Hmd + 4].mDeviceToAbsoluteTracking;

        // Convert the 3x4 matrix to a 4x4 matrix
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix[0][0] = mat.m[0][0]; matrix[1][0] = mat.m[0][1]; matrix[2][0] = mat.m[0][2]; matrix[3][0] = 0.0f;
        matrix[0][1] = mat.m[1][0]; matrix[1][1] = mat.m[1][1]; matrix[2][1] = mat.m[1][2]; matrix[3][1] = 0.0f;
        matrix[0][2] = mat.m[2][0]; matrix[1][2] = mat.m[2][1]; matrix[2][2] = mat.m[2][2]; matrix[3][2] = 0.0f;
        matrix[0][3] = mat.m[0][3]; matrix[1][3] = mat.m[1][3]; matrix[2][3] = mat.m[2][3]; matrix[3][3] = 1.0f;

        // Extract yaw, pitch, and roll from the matrix (use glm or your preferred math library)
        glm::vec3 euler = glm::eulerAngles(glm::quat_cast(matrix));

        float yaw = glm::degrees(euler.y);
        float pitch = glm::degrees(euler.x);
        float roll = glm::degrees(euler.z);

        // Now you have yaw, pitch, and roll in degrees

        Vector3 Rotation;
        Rotation.x = yaw;
        Rotation.y = pitch;
        Rotation.z = roll;
        return Rotation;
    }
    else return *(new Vector3());
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
