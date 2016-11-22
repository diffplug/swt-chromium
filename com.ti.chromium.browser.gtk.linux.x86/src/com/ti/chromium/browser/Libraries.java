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
package com.ti.chromium.browser;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

import com.ti.chromium.browser.cef.Cef;

/**
 * Native libraries loader.
 * 
 * @since 5.4
 */
public class Libraries {
	static {
		try {
			IPath libDir = new Path(Cef.libDir);
			
			/* Load native DLLs */
			System.load(libDir.append("libcef.so").toOSString());
			System.load(libDir.append("libffmpegsumo.so").toOSString());
			System.load(libDir.append("libcef_bridge.so").toOSString());
		} catch (Throwable e) {
			CEFBrowserPlugin.log(e);
		}		
	}
}
