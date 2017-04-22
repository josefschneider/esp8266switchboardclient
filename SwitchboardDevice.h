#pragma once

#include <functional>
#include <WString.h>
#include <ArduinoJson.h>

#include <vector>

using ReadCallback = std::function<void(JsonObject&)>;
using WriteCallback = std::function<void(JsonObject&, JsonObject&)>;

struct SwitchboardDevice
{
	SwitchboardDevice(String name, ReadCallback readCallback, WriteCallback writeCallback, bool readable, bool writeable, const char* typeId)
	  : mName(name),
		mReadable(readable),
		mWriteable(writeable),
		mError(false),
		mErrorString(""),
		mNextDevicePtr(nullptr),
	    mReadCallback(readCallback),
	    mWriteCallback(writeCallback)
	{
		if (mName.endsWith(typeId) == false)
		{
			mError = true;
			mErrorString += "Invalid name '" + mName + "' for device type expecting suffix '" + typeId + "'|";
		}
	}

	virtual ~SwitchboardDevice() { }

	virtual const char* GetDeviceTypeID() = 0;

	void SetNextDevice(SwitchboardDevice* nextDevice) { mNextDevicePtr = nextDevice; }
	SwitchboardDevice* GetNextDevice() { return mNextDevicePtr; }

	virtual void GetValueJson(JsonObject& object) { mReadCallback(object); }
	virtual void SetValueJson(JsonObject& input, JsonObject& object) { mWriteCallback(input, object); }

	const String mName;
	const bool mReadable;
	const bool mWriteable;

	bool mError;
	String mErrorString;

protected:
	const ReadCallback mReadNotImplemented = [this](JsonObject& object)
	{
		return MethodNotImplemented(object);
	};

	const WriteCallback mWriteNotImplemented = [this](JsonObject&, JsonObject& object)
	{
		return MethodNotImplemented(object);
	};

private:
	SwitchboardDevice* mNextDevicePtr;

	const ReadCallback mReadCallback;
	const WriteCallback mWriteCallback;

	void MethodNotImplemented(JsonObject& object)
	{
		object["error"] = "Method not implemented";
	}
};

struct SwitchboardIODevice : public SwitchboardDevice
{
	SwitchboardIODevice(String name, ReadCallback readCallback, WriteCallback writeCallback)
	  : SwitchboardDevice(name, readCallback, writeCallback, true, true, GetDeviceTypeID())
	{

	}

	const char* GetDeviceTypeID() override final { return ".io"; }
};

struct SwitchboardInputDevice : public SwitchboardDevice
{
	SwitchboardInputDevice(String name, ReadCallback readCallback)
	  : SwitchboardDevice(name, readCallback, mWriteNotImplemented, true, false, GetDeviceTypeID())
	{

	}

	const char* GetDeviceTypeID() override final { return ".i"; }
	void SetValueJson(JsonObject& input, JsonObject& object) override final
	{
		(void)input;
		object["error"] = "Cannot set device value as it is not writable";
	}
};

struct SwitchboardOutputDevice : public SwitchboardDevice
{
	SwitchboardOutputDevice(String name, WriteCallback writeCallback)
	  : SwitchboardDevice(name, mReadNotImplemented, writeCallback, false, true, GetDeviceTypeID())
	{

	}

	const char* GetDeviceTypeID() override final { return ".o"; }
	void GetValueJson(JsonObject& object) override final
	{
		object["error"] = "Cannot get device value as it is not readable";
	}
};

struct SwitchboardDevices
{
	SwitchboardDevices() { }

	template<typename... Args>
	SwitchboardDevices(Args&... args)
	{
		AddDevice(args...);
	}

	template<typename... Args>
	void AddDevice(SwitchboardDevice& device, Args&... args)
	{
		mDevices.push_back(device);
		AddDevice(args...);
	}

	void AddDevice(SwitchboardDevice& device)
	{
		mDevices.push_back(device);
	}

	std::vector<std::reference_wrapper<SwitchboardDevice>> GetDevices()
	{
		return mDevices;
	}

	std::vector<std::reference_wrapper<SwitchboardDevice>> mDevices;
};
