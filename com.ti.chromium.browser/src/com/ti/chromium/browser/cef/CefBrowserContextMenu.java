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

/**
 * Instances of this class implement the CEF context menu user interface.
 * 
 * @since 5.4
 */
public abstract class CefBrowserContextMenu {
	final public int EVENTFLAG_NONE                = 0;
	final public int EVENTFLAG_CAPS_LOCK_DOWN      = 1 << 0;
	final public int EVENTFLAG_SHIFT_DOWN          = 1 << 1;
	final public int EVENTFLAG_CONTROL_DOWN        = 1 << 2;
	final public int EVENTFLAG_ALT_DOWN            = 1 << 3;
	final public int EVENTFLAG_LEFT_MOUSE_BUTTON   = 1 << 4;
	final public int EVENTFLAG_MIDDLE_MOUSE_BUTTON = 1 << 5;
	final public int EVENTFLAG_RIGHT_MOUSE_BUTTON  = 1 << 6;
	// Mac OS-X command key.
	final public int  EVENTFLAG_COMMAND_DOWN       = 1 << 7;
	// Windows extended key (see WM_KEYDOWN doc).
	final public int EVENTFLAG_EXTENDED            = 1 << 8; 
	
	private final CefBrowser browser;
	private final int id;
	
	public CefBrowserContextMenu(CefBrowser browser, int id, String label) {
		this.browser = browser;
		this.id = id;
		
		browser.addContextMenu(id, label, this);
	}
	
	public void dispose() {
		browser.removeContextMenu(id);
	}
	
	/**
	 * Call when the menu is selected.
	 * 
	 * @param detail contains the {@code EVENTFLAG} values
	 */
	public abstract void widgetSelected(int detail);
}
