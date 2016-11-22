/*****************************************************************
 * Copyright (c) 2012, 2013 Texas Instruments and others
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Patrick Chuong (Texas Instruments) - Initial API and implementation
 *****************************************************************/
package com.ti.chromium.browser.cef;

import static org.json.JSONObject.NULL;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.widgets.Widget;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.ti.chromium.browser.CEFBrowserPlugin;
import com.ti.chromium.browser.internal.cef.N_KeyboardListener;
import com.ti.chromium.browser.internal.cef.N_LocationListener;
import com.ti.chromium.browser.internal.cef.N_OpenWindowListener;
import com.ti.chromium.browser.internal.cef.N_ProgressListener;

/**
 * JNI CefBrowser interface.
 * 
 * @since 5.5
 * @noextend This interface is not intended to be extended by clients.
 * @noinstantiate This class is not intended to be instantiated by clients.
 */
public class N_CefBrowser {
	/* Browser function map */
	Map<String, CefBrowserFunction> functions = new HashMap<String, CefBrowserFunction>();
	
	/* Browser context menu map */
	Map<Integer, CefBrowserContextMenu> contextMenus = new HashMap<Integer, CefBrowserContextMenu>();
	
	private final Widget widget;
	
	public N_CefBrowser(Widget widget) {
		this.widget = widget;
		
		N_ProgressListener = new N_ProgressListener(this.widget);
		N_LocationListener = new N_LocationListener(this.widget, N_ProgressListener);
		N_OpenWindowListener = new N_OpenWindowListener(this.widget);
		N_KeyboardListener = new N_KeyboardListener(this.widget);
	}	
	
	/*
	 * Cef browser handle
	 */
	long N_CefHandle = 0;
	String N_EvalRespond;
	
	/*
	 * Callback handlers
	 */
	N_ProgressListener N_ProgressListener;
	N_LocationListener N_LocationListener;
	N_OpenWindowListener N_OpenWindowListener;
	N_KeyboardListener N_KeyboardListener;
	
	
	/**
	 * Convert the JSON object to native java object.
	 * 
	 * @param obj a Json object
	 * @return native java object
	 * @throws JSONException
	 */
	Object createNativeJavaObject(Object obj) throws JSONException {
		if (obj instanceof JSONArray) {
			JSONArray array = ((JSONArray)obj);
			ArrayList<Object> list = new ArrayList<Object>();
			for (int i = 0; i < array.length(); ++i) {
				list.add(createNativeJavaObject(array.get(i)));
			}
			return list.toArray(new Object[0]);
		} else if (NULL.equals(obj)) {
			return null;
		}
		return obj;
	}
	
	/*
	 * Callback methods 
	 */
	private String executeFunction(String name, String arguments) {
		CefBrowserFunction function = functions.get(name);
		if (function != null) {
			try {
				JSONObject root = new JSONObject(arguments);
				Object object = root.get("result"); // this is the root node of the JSON object
				Object nativeObject = createNativeJavaObject(object);
				
				if (nativeObject instanceof Object[])
					return function.execute( (Object[]) nativeObject);
				else 
					return function.execute(new Object[] {nativeObject});
				
			} catch (JSONException e) {
				CEFBrowserPlugin.log(e);
			}
		}
		return null;
	}	
	
	private void onContextMenu(int id, int detail) {
		CefBrowserContextMenu menu = contextMenus.get(id);
		if (menu != null) {
			menu.widgetSelected(detail);
		}
	}
	
	/*
	 * Cef browser methods
	 */
	final native boolean N_CreateBrowser(int hParentWnd, String url); 
	final native void N_Destroy();
	final native void N_SetWindowSize(int x, int y, int width, int height);
	final native void N_SetUrl(String url);
	final native String N_GetUrl();
	final native boolean N_IsBackEnabled();
	final native void N_Back();
	final native boolean N_IsForwardEnabled();
	final native void N_Forward();
	final native void N_Refresh();
	final native void N_Stop();
	final native boolean N_Execute(String code);
	final native void N_Evaluate(String code);
	final native void N_CreateBrowserFunction(String name, CefBrowserFunction function);
	final native void N_DestroyBrowserFunction(String name);
	final native void N_AddContextMenu(int id, String label, CefBrowserContextMenu menu);
	final native void N_RemoveContextMenu(int id);
	final native double N_GetZoomLevel();
	final native void N_SetZoomLevel(double level);
}
