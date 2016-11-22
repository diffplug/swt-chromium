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

import org.eclipse.swt.browser.ProgressEvent;
import org.eclipse.swt.browser.ProgressListener;
import org.eclipse.swt.widgets.Widget;

import com.ti.chromium.browser.CEFBrowserPlugin;

/** 
 * JNI progress listener for native callback.
 * 
 * @since 5.5
 * @noextend This interface is not intended to be extended by clients.
 * @noinstantiate This class is not intended to be instantiated by clients.
 */
public class N_ProgressListener {
	private Widget widget;
	List<ProgressListener> listeners = new ArrayList<ProgressListener>();
	
	public N_ProgressListener(Widget widget) {
		this.widget = widget;
	}

	public void add(ProgressListener listener) {
		listeners.add(listener);
	}
	
	public void remove(ProgressListener listener) {
		listeners.remove(listener);
	}

	
	public void changed(int total, int current) {
		ProgressEvent event = new ProgressEvent(widget);
		event.current = current;
		event.total = total;
		event.time = (int) (0xFFFFFFFFL & System.currentTimeMillis());
	
		for (ProgressListener listener : listeners) {
			try {
				listener.changed(event);
			} catch (Throwable t) {
				CEFBrowserPlugin.log(t);
			}
		}
	}

	public void completed(int total, int current) {
		ProgressEvent event = new ProgressEvent(widget);
		event.current = current;
		event.total = total;
		event.time = (int) (0xFFFFFFFFL & System.currentTimeMillis());
		
		for (ProgressListener listener : listeners) {
			try {
				listener.completed(event);
			} catch (Throwable t) {
				CEFBrowserPlugin.log(t);
			}
		}
	}
}
