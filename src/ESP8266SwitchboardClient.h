#pragma once

#include "SwitchboardDeviceStore_impl.h"

#include <ESP8266WebServer.h>

template<size_t RECEIVE_BUFFER_SIZE, size_t SEND_BUFFER_SIZE>
class ESP8266SwitchboardClient
{
public:
	ESP8266SwitchboardClient(ESP8266WebServer& server)
	  : mServer(server)
	{
		mServer.on("/devices_info", [this]() { HandleGetDevicesInfo(); });
		mServer.on("/devices_value", [this]() { HandleGetDevicesValue(); });
		mServer.on("/device_set", HTTP_PUT, [this]() { HandleSetDeviceValue(); });
	}

	void AddDevice(SwitchboardDevice& device)
	{
		mDeviceStore.AddDevice(device);
	}

	void AddDevice(SwitchboardDevices& devices)
	{
		for (SwitchboardDevice& device : devices.GetDevices())
			AddDevice(device);
	}

	bool Error() { return mDeviceStore.Error(); }
	String GetErrorString() { return mDeviceStore.GetErrorString(); }

private:
	void HandleGetDevicesInfo()
	{
		mDeviceStore.GetDevicesInfo(mRawOutputBuffer, SEND_BUFFER_SIZE);
		mServer.send(200, "application/json", mRawOutputBuffer);
	}

	void HandleGetDevicesValue()
	{
		mDeviceStore.GetDevicesValue(mRawOutputBuffer, SEND_BUFFER_SIZE);
		mServer.send(200, "application/json", mRawOutputBuffer);
	}

	void HandleSetDeviceValue()
	{
		String body = mServer.arg("plain");
		mDeviceStore.SetDeviceValue(body, mRawOutputBuffer, SEND_BUFFER_SIZE);
		mServer.send(200, "application/json", mRawOutputBuffer);
	}

	ESP8266WebServer& mServer;
	char mRawOutputBuffer[SEND_BUFFER_SIZE];
	SwitchboardDeviceStore<RECEIVE_BUFFER_SIZE, SEND_BUFFER_SIZE> mDeviceStore;
};
