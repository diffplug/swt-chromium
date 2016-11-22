#ifndef _JSON_UTILTITIES_H_
#define _JSON_UTILTITIES_H_
#pragma once

#include "../JSON/json.h"
#include <include/cef_app.h>

/**
 * Parses the json string and return the root json object.
 */
Json::Value GetJsonValue(std::string json);

/**
 * Creates a JSON array from a CefListValue.
 */
Json::Value CreateJSONArray(CefRefPtr<CefListValue>& list);

/**
 * Creates the evaluated jSON object from the CefListValue.
 */
Json::Value CreateEvalJSONObject(CefRefPtr<CefListValue>& list, int index);

/**
 * Creates a CefV8Value from a JSON value.
 */
CefRefPtr<CefV8Value> CreateV8ValueFromJSON(Json::Value root);
#endif
