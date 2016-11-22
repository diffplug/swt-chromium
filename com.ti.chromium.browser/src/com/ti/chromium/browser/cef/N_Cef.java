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

import com.ti.chromium.browser.internal.cef.N_JavaLogger;

/**
 * JNI Cef Interface.
 * 
 * @since 5.5
 * @noextend This interface is not intended to be extended by clients.
 * @noinstantiate This class is not intended to be instantiated by clients.
 */
public class N_Cef {
	/*
	 * JNI Java logger field
	 */
	public final N_JavaLogger N_JavaLogger = new N_JavaLogger();
	
	/*
	 * Cef runtime methods
	 */
	public final native void N_EnableLogging(boolean enable);
	public final native boolean N_Initialize(String settings, String libDir);
	public final native void N_Shutdown();
	public final native void N_DoMessageLoopWork();
}
