#pragma once

#include "SwitchboardDevice.h"

template<size_t RECEIVE_BUFFER_SIZE, size_t SEND_BUFFER_SIZE>
class SwitchboardDeviceStore
{
public:
	SwitchboardDeviceStore(): mFirstDevice(nullptr), mDeviceError(false) {}

	virtual void AddDevice(SwitchboardDevice& device);

	void GetDevicesInfo(char* rawBuffer, size_t rawBufferSize);
	void GetDevicesValue(char* rawBuffer, size_t rawBufferSize);
	void SetDeviceValue(const String& input, char* rawBuffer, size_t rawBufferSize);

	bool Error() { return mDeviceError; }
	String GetErrorString();

private:
	SwitchboardDevice* GetDevice(const String& name);

	SwitchboardDevice* mFirstDevice;
	bool mDeviceError; // Set to true if at least one device has an error
};
