/*****************************************************************
 * Copyright (c) 2013 Texas Instruments and others
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Patrick Chuong (Texas Instruments) - Initial API and implementation
 *****************************************************************/
package com.ti.chromium.browser.internal.cef;

import java.lang.reflect.Method;

import org.eclipse.core.runtime.Platform;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Widget;

/**
 * JNI keyboard listener for native callback.
 * 
 * @since 6.0
 * @noextend This interface is not intended to be extended by clients.
 * @noinstantiate This class is not intended to be instantiated by clients.
 */
public class N_KeyboardListener {
	private Widget widget;
	private int handle;

	public N_KeyboardListener(Widget widget) {
		this.widget = widget;
		if (widget instanceof Control) {
			handle = ((Control)widget).handle;
		}
	}
	
	boolean onKeyEvent(int arg0, int arg1, int arg2) {
		try {
			if (Platform.WS_WIN32.equals(Platform.getWS())) {				
				Method m = Control.class.getDeclaredMethod("windowProc", new Class[] {int.class/*hwnd*/, int.class/*msg*/, int.class/*wParm*/, int.class/*lParm*/});
				if (m != null) {
					m.setAccessible(true);
					Object result = m.invoke(this.widget, handle, arg0, arg1, arg2);
					if (result.equals(1)) {
						return true;
					}
				}
			} else if (Platform.WS_GTK.equals(Platform.getWS())) {
				Method m = Control.class.getDeclaredMethod("windowProc", new Class[] {int.class/*hwnd*/, int.class/*arg0*/, int.class/*user_data*/});
				if (m != null) {
					m.setAccessible(true);
					Object result = m.invoke(this.widget, handle, arg1, arg0);
					if (result.equals(1)) {
						return true;
					}
				}
			}
		} catch (Throwable t) {}
		
		return false;
	}
}
