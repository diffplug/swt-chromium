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

import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.widgets.Widget;

import com.ti.chromium.browser.CEFBrowserPlugin;

/** 
 * JNI location listener for native callback.
 * 
 * @since 5.5
 * @noextend This interface is not intended to be extended by clients.
 * @noinstantiate This class is not intended to be instantiated by clients.
 */
public class N_LocationListener {
	private Widget widget;
	private List<LocationListener> listeners = new ArrayList<LocationListener>();
	
	// TODO: workaround, since there is no progress in CEF browser that I can find
	private final int maxProgress = 42;
	N_ProgressListener progressListeners;
	
	public N_LocationListener(Widget widget, N_ProgressListener progressListeners) {
		this.widget = widget;
		this.progressListeners = progressListeners;
	}

	public void add(LocationListener listener) {
		listeners.add(listener);
	}
	
	public void remove(LocationListener listener) {
		listeners.remove(listener);
	}
	
	public boolean changing(String location, boolean isMain) {
		LocationEvent event = new LocationEvent(widget);
		event.location = location;
		event.top = isMain;
		event.doit = true;
		
		for (LocationListener listener : listeners) {
			try {
				listener.changing(event);
				if (!event.doit) {
					break;
				}
			} catch (Throwable t) {
				CEFBrowserPlugin.log(t);
			}
		}
		
		// Workaround progress
		progressListeners.changed(maxProgress, (int) Math.random()*maxProgress);
		
		return event.doit;
	}

	public void changed(String location, boolean isMain) {
		LocationEvent event = new LocationEvent(widget);
		event.location = location;
		event.top = isMain;
		
		for (LocationListener listener : listeners) {
			try {
				listener.changed(event);
			} catch (Throwable t) {
				CEFBrowserPlugin.log(t);
			}
		}
		
		// Workaround progress
		progressListeners.changed(maxProgress, maxProgress);
		progressListeners.completed(maxProgress, maxProgress);
	}
}
