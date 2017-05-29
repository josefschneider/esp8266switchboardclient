#pragma once

#include <SwitchboardDevice.h>
#include <DHT.h>

class TemperatureSensor : public SwitchboardDevices
{
public:
	TemperatureSensor(int pin, int sensorType, int cycleCount):
		SwitchboardDevices(mTempDevice, mHumidDevice),
		mTempDevice("temperature.i", [this](JsonObject& object) { GetValue(object, mTemp_c); }),
		mHumidDevice("humidity.i", [this](JsonObject& object) { GetValue(object, mHumidity); }),
		mDht(pin, sensorType, cycleCount),
		mPreviousMillis(0),
		mInterval(2000),
		mError(false),
		mErrorMsg("Failed to read from DHT sensor")
	{
		mDht.begin();
	}

	void GetValue(JsonObject& object, float value)
	{
		UpdateSensorValues();
		if (mError)
			object["error"] = mErrorMsg;
		else
			object["value"] = String((int)value);
	}

private:
	void UpdateSensorValues()
	{
		// Wait at least 2 seconds seconds between measurements.
		// if the difference between the current time and last time you read
		// the sensor is bigger than the interval you set, read the sensor
		// Works better than delay for things happening elsewhere also
		unsigned long currentMillis = millis();

		if(currentMillis - mPreviousMillis >= mInterval)
		{
			// save the last time you read the sensor
			mPreviousMillis = currentMillis;

			// Reading temperature for humidity takes about 250 milliseconds!
			// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
			mHumidity = mDht.readHumidity();          // Read humidity (percent)
			mTemp_c = mDht.readTemperature(false);     // Read temperature
			// Check if any reads failed and exit early (to try again).
			if (isnan(mHumidity) || isnan(mTemp_c)) {
				mError = true;
			}
			mError = false;
		}
	}

	SwitchboardInputDevice mTempDevice;
	SwitchboardInputDevice mHumidDevice;

	DHT mDht;
	unsigned long mPreviousMillis;        // will store last temp was read
	float mHumidity, mTemp_c;  // Values read from sensor
	const unsigned long mInterval;              // interval at which to read sensor

	bool mError;
	String mErrorMsg;
};
