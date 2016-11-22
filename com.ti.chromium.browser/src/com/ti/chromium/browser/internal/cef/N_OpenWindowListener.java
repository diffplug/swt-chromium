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
package com.ti.chromium.browser.internal.cef;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.browser.OpenWindowListener;
import org.eclipse.swt.browser.WindowEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Widget;

/** 
 * JNI open window listener for native callback.
 * 
 * @since 5.5
 * @noextend This interface is not intended to be extended by clients.
 * @noinstantiate This class is not intended to be instantiated by clients.
 */
public class N_OpenWindowListener {
	private Widget widget;
	private List<OpenWindowListener> listeners = new ArrayList<OpenWindowListener>();
	
	public N_OpenWindowListener(Widget widget) {
		this.widget = widget;
	}
	
	public void add(OpenWindowListener listener) {
		listeners.add(listener);
	}
	
	public void remove(OpenWindowListener listener) {
		listeners.remove(listener);
	}
	
	public boolean onBeforePopup(int x, int y, int width, int height, boolean addressBar, boolean menuBar, boolean statusBar, boolean toolBar) {
		WindowEvent event = new WindowEvent(widget);
		event.location = new Point(x, y);
		event.size = new Point(width, height);
		event.addressBar = addressBar;
		event.menuBar = menuBar;
		event.statusBar = statusBar;
		event.required = false;
		
		for (OpenWindowListener listener : listeners) {
			listener.open(event);
		}
		
		return event.required;
	}
}
