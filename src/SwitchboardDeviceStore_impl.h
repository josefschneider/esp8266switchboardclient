#pragma once

#include "SwitchboardDeviceStore.h"

template<size_t R, size_t S>
void SwitchboardDeviceStore<R, S>::AddDevice(SwitchboardDevice& device)
{
	if (mFirstDevice == nullptr)
	{
		mFirstDevice = &device;
		return;
	}

	SwitchboardDevice* deviceIter = mFirstDevice;
	while (true)
	{
		if (deviceIter->mName == device.mName)
		{
			deviceIter->mError = true;
			deviceIter->mErrorString += "Duplicate device with name '" + device.mName + "'|";
		}

		if (deviceIter->GetNextDevice() == nullptr)
			break;

		deviceIter = deviceIter->GetNextDevice();
	}

	mDeviceError |= deviceIter->mError;

	deviceIter->SetNextDevice(&device);
}

template<size_t R, size_t S>
void SwitchboardDeviceStore<R, S>::GetDevicesInfo(char* rawBuffer, size_t rawBufferSize)
{
	StaticJsonBuffer<S> sendJsonBuffer;
	auto& object = sendJsonBuffer.createObject();

	if (mDeviceError)
	{
		object["error"] = GetErrorString();
	}
	else
	{
		JsonArray& devicesArray = object.createNestedArray("devices");
		for (SwitchboardDevice* deviceIter = mFirstDevice; deviceIter != nullptr; deviceIter = deviceIter->GetNextDevice())
		{
			auto& arrayEntry = devicesArray.createNestedObject();
			arrayEntry["name"] = deviceIter->mName;
			arrayEntry["writeable"] = deviceIter->mWriteable;
			arrayEntry["readable"] = deviceIter->mReadable;
		}
	}

	object.printTo(rawBuffer, rawBufferSize);
}

template<size_t R, size_t S>
void SwitchboardDeviceStore<R, S>::GetDevicesValue(char* rawBuffer, size_t rawBufferSize)
{
	StaticJsonBuffer<S> sendJsonBuffer;
	auto& object = sendJsonBuffer.createObject();

	if (mDeviceError)
	{
		object["error"] = GetErrorString();
	}
	else
	{
		JsonArray& devicesArray = object.createNestedArray("devices");
		for (SwitchboardDevice* deviceIter = mFirstDevice; deviceIter != nullptr; deviceIter = deviceIter->GetNextDevice())
		{
			if (deviceIter->mReadable)
			{
				auto& arrayEntry = devicesArray.createNestedObject();
				arrayEntry["name"] = deviceIter->mName;
				deviceIter->GetValueJson(arrayEntry);
			}
		}
	}

	object.printTo(rawBuffer, rawBufferSize);
}

template<size_t R, size_t S>
void SwitchboardDeviceStore<R, S>::SetDeviceValue(const String& input, char* rawBuffer, size_t rawBufferSize)
{
	StaticJsonBuffer<S> sendJsonBuffer;
	auto& object = sendJsonBuffer.createObject();

	StaticJsonBuffer<R> receiveJsonBuffer;
	auto& inputArgs = receiveJsonBuffer.parseObject(input);

	if (inputArgs.success() == false)
	{
		object["error"] = String("Unable to decode JSON payload: '" + input + "'");
	}
	else if (inputArgs.containsKey("name") == false)
	{
		object["error"] = "Input does not contain a 'name' field";
	}
	else
	{
		String name = inputArgs["name"].asString();
		SwitchboardDevice* device = GetDevice(name);
		if (device == nullptr)
		{
			object["error"] = String("Unknown device '" + name + "'");
		}
		else if (device->mError)
		{
			object["error"] = device->mErrorString;
		}
		else if (!device->mWriteable)
		{
			object["error"] = String("Device '" + name + "' not writeable");
		}
		else
		{
			device->SetValueJson(inputArgs, object);
		}
	}

	object.printTo(rawBuffer, rawBufferSize);
}

template<size_t R, size_t S>
String SwitchboardDeviceStore<R, S>::GetErrorString()
{
	if (!mDeviceError)
		return "";

	String allErrors = "|";
	for (SwitchboardDevice* deviceIter = mFirstDevice; deviceIter != nullptr; deviceIter = deviceIter->GetNextDevice())
	{
		if (deviceIter->mError)
			allErrors += deviceIter->mErrorString;
	}

	return allErrors;
}

template<size_t R, size_t S>
SwitchboardDevice* SwitchboardDeviceStore<R, S>::GetDevice(const String& name)
{
	for (SwitchboardDevice* deviceIter = mFirstDevice; deviceIter != nullptr; deviceIter = deviceIter->GetNextDevice())
	{
		if (deviceIter->mName == name)
			return deviceIter;
	}
	return nullptr;
}
