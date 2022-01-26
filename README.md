# Live Link Antilatency

This is a plugin for the Unreal Engine allows to use Antilatency Alt trackers with UE LiveLink. 

Antilatency SDK version: 3.5.0
Unreal Engine version: 4.27

## Connection settings
* LocalUpdateRateInHz - a trackers update rate per second.
* UsbDeviceFilters - an USB device filter that will be used by Antilatency Device Network. By default it matches all Antilatency Devices.
* IpDeviceFilters - an IP device filter that will be used by Antilatency Device Network.

## Source Settings
* TrackerSettings - a map with settings for each Alt tracker.

The key of this map is a tag property of the socket to which the Alt is connected. Tracking will not run on sockets with tags that are not specified in the settings file. The tag can be set and viewed in AntilatencyService in the Tag property of the selected socket. Each tracker has following settings:
* ExtrapolationTime - extrapolation time in seconds. The larger this value is, the less certain the extrapolation result will be. It's rarely beneficial to extrapolate further than a second to the future.
* EnvironmentCode - a Base64-encoded string represents the Alt Environment. If empty, the environment that marked as default in the AntilatencyService will be used for this tracker.
* PlacementCode - a Base64-encoded string represents the placement of Alt tracker. If empty, the placement that marked as default in the AntilatencyService will be used for this tracker.

To get the Base64-encoded string for environment and placement follow this steps:
1. Click on "..." button next to the environment or placement name in the AntilatencyService.
2. Click on "Share" or "Copy link".
3. Paste the link as the value and remove excess information, leaving only the Placement code (the value between data= and &name).