/*****************************************************************
 * Copyright (c) 2012 Texas Instruments and others
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Patrick Chuong (Texas Instruments) - Initial API and implementation
 *****************************************************************/
package com.ti.chromium.browser.cef;

import org.json.JSONObject;

import com.ti.chromium.browser.CEFBrowserPlugin;

/**
 * Instances of this class represent java-side "functions" that
 * are invokable from javascript.  Browser clients define these
 * functions by subclassing <code>BrowserFunction</code> and
 * overriding its <code>function(Object[])</code> method.  This
 * method will be invoked whenever javascript running in the
 * Browser makes a call with the function's name.
 */
public abstract class CefBrowserFunction {
	private final CefBrowser browser;
	private final String name;
	
	public CefBrowserFunction(CefBrowser browser, String name) {
		this.browser = browser;
		this.name = name;
		
		browser.createBrowserFunction(name, this);
	}
	
	public void dispose() {
		browser.destroyBrowserFunction(name);
	}
	
	public abstract Object function(Object[] arguments);
	
	/* package */ String execute(Object[] arguments) {
		Object result = function(arguments);
		
		if (result != null) {
			try {
				JSONObject root = new JSONObject();
				root.put("result", result);
				return root.toString();
			} catch (Throwable t) {
				CEFBrowserPlugin.log(t);
			}
		}
		return null;
	}	
}
