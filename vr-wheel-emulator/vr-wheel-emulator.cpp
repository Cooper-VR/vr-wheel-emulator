#include <iostream>
#include "headers/openvr.h"
//#include "headers/openvr_driver.h"

int main()
{
    vr::EVRInitError eError = vr::VRInitError_None;
    vr::IVRSystem* pSystem = vr::VR_Init(&eError, vr::VRApplication_Background);

    if (eError != vr::VRInitError_None) {
        std::cout << "Failed to iniitalize, is SteamVR running\n";
        return -1;
    }
    std::cout << "OpenVR initalized\n";

    //std::cout << pSystem->IsTrackedDeviceConnected(vr::k_unTrackedDeviceIndex_Hmd+1) << '\n';

    vr::ETrackedDeviceProperty deviceProp = vr::Prop_RenderModelName_String;

    vr::ETrackedPropertyError dError;
    int BufferSize = 256;
    char *propString = new char[BufferSize];
    
    //for me  +3 is left controller, +4 is right, +1||+2 are base stations
    pSystem->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd + 4, deviceProp, propString, BufferSize, &dError) << '\n';
    if (dError != vr::TrackedProp_Success) {
        std::cout << "failed?: " << dError;
        return -2;
    }
    std::string trackingSystemName(propString);
    std::cout << trackingSystemName << '\n';
    //can use render models ot tell difference

    vr::TrackedDevicePose_t trackedDevicesPose[vr::k_unMaxTrackedDeviceCount];

    pSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, trackedDevicesPose, vr::k_unMaxTrackedDeviceCount);

    if (trackedDevicesPose[vr::k_unTrackedDeviceIndex_Hmd + 4].bPoseIsValid) {
        vr::HmdMatrix34_t matPose = trackedDevicesPose[vr::k_unTrackedDeviceIndex_Hmd + 4].mDeviceToAbsoluteTracking;

        // Extract rotation (in the form of a quaternion) from the matrix
        float qw = sqrt(fmax(0, 1 + matPose.m[0][0] + matPose.m[1][1] + matPose.m[2][2])) / 2;
        float qx = sqrt(fmax(0, 1 + matPose.m[0][0] - matPose.m[1][1] - matPose.m[2][2])) / 2;
        float qy = sqrt(fmax(0, 1 - matPose.m[0][0] + matPose.m[1][1] - matPose.m[2][2])) / 2;
        float qz = sqrt(fmax(0, 1 - matPose.m[0][0] - matPose.m[1][1] + matPose.m[2][2])) / 2;

        qx = copysign(qx, matPose.m[2][1] - matPose.m[1][2]);
        qy = copysign(qy, matPose.m[0][2] - matPose.m[2][0]);
        qz = copysign(qz, matPose.m[1][0] - matPose.m[0][1]);

        std::cout << "rotation: " << qx << ' ' << qy << ' ' << qz << '\n';
    }


    std::cout << "Hello World!\n";
}
