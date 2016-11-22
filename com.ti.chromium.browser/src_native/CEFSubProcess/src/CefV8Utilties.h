#ifndef _CEF_V8_H_
#define _CEF_V8_H_
#pragma once

#include "../../JSON/json.h"

#include <include/cef_app.h>

/**
 * Transfer a V8 value to a List index.
 */
void SetListValue(CefRefPtr<CefListValue> list, int index, CefRefPtr<CefV8Value> value);

/**
 * Transfer a V8 array to a List.
 */
void SetList(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target);

/**
 * Transfer a List value to a V8 array index.
 */
//void SetListValue(CefRefPtr<CefV8Value> list, int index, CefRefPtr<CefListValue> value);

#endif
