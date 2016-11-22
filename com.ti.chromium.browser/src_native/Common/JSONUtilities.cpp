#include "JSONUtilities.h"
#include "ProcessMessage.h"

Json::Value GetJsonValue(std::string json) {
	Json::Value root;
	Json::Reader reader;
	bool succeed = reader.parse(json, root);
	if (!succeed) {
		//std::cout << "Failed to parse json object\n" << reader.getFormattedErrorMessages() + "\n";
	}

	return root;
}

Json::Value CreateJSONArray(CefRefPtr<CefListValue>& list) {
	Json::Value json(Json::arrayValue);

	int size = list->GetSize();
	for (int i = 0; i < size; ++i) {
		CefValueType type = list->GetType(i);
		CefRefPtr<CefListValue> cefList;
		switch (type) {
		case VTYPE_LIST:
			cefList = list->GetList(i);
			json.append(CreateJSONArray(cefList));
			break;
		case VTYPE_BOOL:
			json.append(list->GetBool(i));
			break;
		case VTYPE_STRING:
			json.append(std::string(list->GetString(i)));
			break;
		case VTYPE_DOUBLE:
			json.append(list->GetDouble(i));
			break;
		case VTYPE_INT:
			json.append(list->GetInt(i));
			break;
		case VTYPE_NULL:
			json.append(Json::nullValue);
			break;
		default:
			break;
		}
	}
	return json;
}

Json::Value CreateEvalJSONObject(CefRefPtr<CefListValue>& list, int index) {
	Json::Value root(Json::objectValue);

	CefValueType type = list->GetType(index);
	switch (type) {
	case VTYPE_LIST: {
			CefRefPtr<CefListValue> childList = list->GetList(index);
			root[JSON_ROOT] = CreateJSONArray(childList);
		}
		break;
	case VTYPE_BOOL:
		root[JSON_ROOT] = list->GetBool(index);
		break;
	case VTYPE_STRING:
		root[JSON_ROOT] = std::string(list->GetString(index));
		break;
	case VTYPE_DOUBLE:
		root[JSON_ROOT] = list->GetDouble(index);
		break;
	case VTYPE_INT:
		root[JSON_ROOT] = list->GetInt(index);
		break;
	case VTYPE_NULL:
		root[JSON_ROOT] = Json::nullValue;
		break;
	default:
		root = Json::nullValue;
		break;
	}

	return root;
}

CefRefPtr<CefV8Value> CreateV8ValueFromJSON(Json::Value root) {
	CefRefPtr<CefV8Value> result = 0;
	if (root.isArray()) {
		int i = 0;
		CefRefPtr<CefV8Value> v8Array = CefV8Value::CreateArray(root.size());
		Json::Value::iterator it = root.begin();
		for (; it != root.end(); ++it, ++i) {
			v8Array->SetValue(i, CreateV8ValueFromJSON(*it));
		}
		result = v8Array;
	} else if (root.isBool()) {
		result = CefV8Value::CreateBool(root.asBool());
	} else if (root.isString()) {
		result = CefV8Value::CreateString(root.asString());
	} else if (root.isDouble()) {
		result = CefV8Value::CreateDouble(root.asDouble());
	} else if (root.isInt()) {
		result = CefV8Value::CreateInt(root.asInt());
	} else if (root.isNull()) {
		result = CefV8Value::CreateNull();
	}
	return result;
}
