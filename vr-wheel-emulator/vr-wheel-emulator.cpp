#include <iostream>
#include "headers/openvr.h"
//#include "headers/openvr_driver.h"
#include "headers/main.h"



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

    Vector3 rotation;

    while (1) {
        rotation = GetRotation(pSystem);
        //ConvertToDegrees(&rotation);
        PrintRotation(rotation);
    }

    std::cout << "Hello World!\n";
}
