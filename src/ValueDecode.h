#pragma once

#include <ArduinoJson.h>

/**
 * Helper function to extract Json data from PUT request
 */
template<class T>
bool ValueDecode(JsonObject& input, JsonObject& object, T& value)
{
	if (input.containsKey("value") == false)
	{
		object["error"] = "Input does not contain a 'value' field";
		return false;
	}

	if (input["value"].is<T>() == false)
	{
		object["error"] = String("Input value " + String(input["value"].asString()) + " is not of expected type");
		return false;
	}

	value = input["value"];
	return true;
}
