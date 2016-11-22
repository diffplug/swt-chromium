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
package com.ti.chromium.browser.views;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;
import org.eclipse.ui.progress.UIJob;
import org.json.JSONArray;
import org.json.JSONException;

import com.ti.chromium.browser.CEFBrowserPlugin;
import com.ti.chromium.browser.cef.CefBrowser;
import com.ti.chromium.browser.cef.CefBrowserFunction;

public class ChromiumBrowserView extends ViewPart {
	private CefBrowser browser;

	/**
	 * The ID of the view as specified by the extension.
	 */
	public static final String ID = "com.ti.chromium.browser.views.ChromiumBrowserView";

	/**
	 * The constructor.
	 */
	public ChromiumBrowserView() {
	}

	/**
	 * This is a callback that will allow us to create the viewer and initialize
	 * it.
	 */
	public void createPartControl(Composite parent) {
		browser = new CefBrowser(parent, "www.google.com");
		
		new CefBrowserFunction(browser, "add") {
			@Override
			public Object function(Object[] arguments) {
				if (arguments.length == 2) {
					if (arguments[0] instanceof Integer && arguments[1] instanceof Integer) {
						return (Integer) arguments[0] + (Integer) arguments[1];
					} else if (arguments[0] instanceof Double && arguments[1] instanceof Double) {
						return (Double) arguments[0] + (Double) arguments[1];
					}
				}
				return "Invalid Argument";
			}
		};

		new CefBrowserFunction(browser, "version") {
			@Override
			public Object function(Object[] arguments) {
				if (arguments.length == 0) {
					return CEFBrowserPlugin.getDefault().getBundle().getVersion().toString();
				}
				return null;
			}
		};

		 new UIJob("test") {
			 @Override
			 public IStatus runInUIThread(IProgressMonitor arg0) {
				browser.execute("window.test = [42, [1.01, [null, 'yes'], 'foo'], true]");
				Object result = browser.evaluate("window.test");
				try {
					JSONArray array = new JSONArray(result);
					System.out.println("ChromiumBrowserView.createPartControl() - \n" + array.toString(4));
				} catch (JSONException e) {
				}
				return Status.OK_STATUS;
			 }
		 }.schedule(3000);
	}

	@Override
	public void dispose() {
		browser.dispose();

		super.dispose();
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		if (browser != null)
			browser.setFocus();
	}
}