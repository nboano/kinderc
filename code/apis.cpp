#pragma once
#include "../kinderc.hpp"

exported void __geolocation_handler(int lambda_number, double latitude, double longitude, double altitude, double accuracy, double altitudeAccuracy, double heading, double speed) {
    GeolocationData gd = {
        latitude, longitude, altitude, accuracy, altitudeAccuracy, heading, speed
    };
    __lambda_call(lambda_number, &gd);
}

Property<bool> Geolocation::IsSupported = Property<bool>(
    []() {
        return (int)JavaScript::Eval("!!navigator.geolocation?1:0") == 1;
    }
);

bool Geolocation::HighAccuracy = false;

int Geolocation::Request(void(*sh)(GeolocationData), void(*eh)(GeolocationError), bool watch) {
    static void(*onsuccess)(GeolocationData);

    onsuccess = sh;

    void(*hndl)(GeolocationData*) = [](GeolocationData* gd) {
        onsuccess(*gd);
    };

    Handler successhandler = EventHandler((void(*)(void*))hndl);
    Handler errorhandler = EventHandler((void(*)(void*))eh);

    return (int)navigator["geolocation"][watch ? "watchPosition" : "getCurrentPosition"]((string)"(e)=>__geolocation_handler(" + (string)successhandler.LambdaIndex +  ",e.coords.latitude,e.coords.longitude,e.coords.altitude,e.coords.accuracy,e.coords.altitudeAccuracy,e.coords.heading,e.coords.speed)", (string)"(e)=>__lambda_call(" + (string)errorhandler.LambdaIndex + ",e.code)", (string)"{enableHighAccuracy:" + (HighAccuracy ? "true" : "false") + "}");
}

void Geolocation::GetPosition(void(*sh)(GeolocationData), void(*eh)(GeolocationError)) {
    Geolocation::Request(sh, eh, false);
}

int Geolocation::WatchPosition(void(*sh)(GeolocationData), void(*eh)(GeolocationError)) {
    return Geolocation::Request(sh, eh, true);
}

void Geolocation::ClearWatch(int watch_number) {
    navigator["geolocation"]["clearWatch"]((string)watch_number);
}