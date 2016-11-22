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
package com.ti.chromium.browser.internal.cef;

/**
 * JNI Java logger for native callback.
 * 
 * @noextend This interface is not intended to be extended by clients.
 * @noinstantiate This class is not intended to be instantiated by clients.
 */
public class N_JavaLogger {
	void log(String msg) {
		System.out.println(msg);
	}
	
	void log(String msg, int errorCode) {
		System.out.println(msg + " [errorCode = " + errorCode + "]");
	}
	
	void log(String msg, int errorCode, String stacktrace) {
		System.out.println(msg + " [errorCode = " + errorCode + "]");
		System.out.println(stacktrace);
	}
}
