#pragma once

#include <SwitchboardDevice.h>
#include <ValueDecode.h>

#include <IRremoteESP8266.h>
#include <IRremoteInt.h>

#define RAW_SEND_ARRAY_SIZE	100

class IRSender : public SwitchboardDevices
{
public:
	IRSender(int pin):
		SwitchboardDevices(mTVOnOff, mSendRawIRCode),
		mTVOnOff("tv_on_off.o", [this](JsonObject& input, JsonObject& object) { TVOnOff(input, object); }),
		mSendRawIRCode("raw_ir_code.o", [this](JsonObject& input, JsonObject& object) { SendRawIRCode(input, object); }),
		mIRSsend(pin)
	{
		mIRSsend.begin();
	}
	
	void TVOnOff(JsonObject& input, JsonObject& object)
	{
		const char* value;
		if (ValueDecode<const char*>(input, object, value) == false)
			return;
		
		if (String(value) == "0")
			mIRSsend.sendSAMSUNG(0xE0E019E6, 32);
		else
			mIRSsend.sendSAMSUNG(0xE0E09966, 32);
	}
	
	void SendRawIRCode(JsonObject& input, JsonObject& object)
	{
		const char* charValue;
		if (ValueDecode<const char*>(input, object, charValue) == false)
			return;
		
		String value(charValue);
		int currentIndex = 0;
		int arrayIdx = 0;
		int nextIndex;
		unsigned int codeArray[RAW_SEND_ARRAY_SIZE];

		do
		{
			if (arrayIdx >= RAW_SEND_ARRAY_SIZE)
			{
				object["error"] = String("Raw input '" + value + "'array too long (max " + String(RAW_SEND_ARRAY_SIZE) + ")");
				return;
			}
			
			nextIndex = value.indexOf(',', currentIndex);
			String intStr;
			if (nextIndex == -1)
				intStr = value.substring(currentIndex, value.length());
			else
				intStr = value.substring(currentIndex, nextIndex);
			
			codeArray[arrayIdx] = static_cast<unsigned int>(intStr.toInt());
			arrayIdx++;
			
			currentIndex = nextIndex + 1;    
		} while (nextIndex > 0);

		mIRSsend.sendGC(codeArray, arrayIdx);
	}

private:
	SwitchboardOutputDevice mTVOnOff;
	SwitchboardOutputDevice mSendRawIRCode;

	IRsend mIRSsend;
};