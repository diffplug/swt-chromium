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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.net.URL;
import java.util.Map.Entry;
import java.util.Set;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.progress.WorkbenchJob;
import org.json.JSONObject;
import org.osgi.framework.Bundle;

import com.ti.chromium.browser.CEFBrowserPlugin;

/**
 * Chromium Embedded Framework (CEF) factory. 
 */
public class Cef {
	public final static Cef INSTANCE = Initializer();
	
	/**
	 * The native CEF instance (readonly).
	 */
	public static N_Cef N_Cef;
	/**
	 * The library directory (readonly),
	 */
	public static String libDir;
	
	/**
	 * System property prefix
	 */
	private static String PROP_Prefix = "cef_";
	
//	private Runnable messageJob;
	private Job messageJob;
	private int msgThreadCount = 0;
	private int messageJobTimer = 0;
	
	static Cef Initializer() {
		try {
			/*
			 * Retrieves the libraries folder.
			 */
			libDir = System.getProperty("ChromeDir");
			if (libDir == null) {
				String fragmentBundleName = CEFBrowserPlugin.getDefault().getBundle().getSymbolicName() + "." + 
											Platform.getWS() + "." + 
											Platform.getOS() + "." + 
											Platform.getOSArch();
				Bundle bundle = Platform.getBundle(fragmentBundleName);
				URL url = bundle.getEntry("libs");
				url = FileLocator.toFileURL(url);
				
				libDir = new Path(url.getPath()).toOSString();
			} else {
				if (!libDir.endsWith("" + Path.SEPARATOR)) {
					libDir += Path.SEPARATOR;
				}
			}
			
			N_Cef = new N_Cef();
			return new Cef();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
			return null;
		} 
	}
	
	/*
	 * Private constructor. 
	 */
	private Cef() {
		
		/*
		 * Platform specific fragment is required to provide a Library class
		 * and a static constructor. All native libraries are required to be 
		 * loaded from the static constructor.
		 */
		try {
			Class.forName("com.ti.chromium.browser.Libraries");
		} catch (ClassNotFoundException e) {
			CEFBrowserPlugin.log(e);
		}
	}
	
	/**
	 * Initialize the CEF browser runtime. Client is required to call {@code dispose()} before terminating the process.
	 * 
	 * @see #dispose()
	 */
	public void init() {
		String os = Platform.getOS();
		
		if (Platform.OS_WIN32.equals(os)) {
			messageJobTimer = 10;
			
		} else if (Platform.OS_LINUX.equals(os)) {
			messageJobTimer = 10;
		
		} else {
			// Set a default for all other OS - just in case.
			messageJobTimer = 20;
		}
		
		String settings = "";
		IPath path = CEFBrowserPlugin.getResourceFullpath(new Path(".default_settings"));
		if (path != null) {
			File file = path.toFile();
			try {
				BufferedReader reader = new BufferedReader(new FileReader(file));
				String input;
				while ((input = reader.readLine()) != null) {
					settings += input + "\n";
				}
				reader.close();
			} catch (Exception e) {
				CEFBrowserPlugin.log(e);
			}
		}
		
		try {
			/*
			 * Enable options to be passed in from system property.
			 */
			JSONObject jsonObj = null;
			Set<Entry<Object, Object>> entrySet = System.getProperties().entrySet();
			for (Entry<Object, Object> entry : entrySet) {
				try {
					String key = entry.getKey().toString();
					if (key.startsWith(PROP_Prefix)) {
						if (jsonObj == null) {
							jsonObj = new JSONObject(removeComments(settings));
						}
											
						key = key.replaceFirst(PROP_Prefix, "");
						String value = entry.getValue().toString();
						if (isNumber(value)) {
							jsonObj.put(key, Integer.parseInt(value));
						} else {
							jsonObj.put(key, value);
						}
					}
				} catch (Throwable t) {
					CEFBrowserPlugin.log(t);
					jsonObj = null;
					break;
				}
			}
			if (jsonObj != null) {
				settings = jsonObj.toString(4);
			}
			
			boolean debug = "true".equalsIgnoreCase(Platform.getDebugOption(CEFBrowserPlugin.PLUGIN_ID + "/JNITrace"));			
			N_Cef.N_EnableLogging(debug);
			N_Cef.N_Initialize(settings, libDir);			
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	/**
	 * De-initialize the CEF browser runtime.
	 */
	public void dispose() {
		messageJob = null;
		
		try {
			N_Cef.N_Shutdown();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	/**
	 * Initialize the CEF message polling thread. This method should only be called if {@code init()} is called. 
	 * 
	 * @param display
	 */
	void initMessageThread(final Display display) {
		if (messageJob != null)
			return;

//		messageJob = new Runnable() {
//			@Override
//			public void run() {
//				try {
//					N_Cef.N_DoMessageLoopWork();				
//					if (messageJob != null) {
//						display.timerExec(messageJobTimer, messageJob);
//					}
//					
//				} catch (Throwable t) {
//					t.printStackTrace();
//				}
//			}
//		};
//		display.timerExec(messageJobTimer, messageJob);
		
		messageJob = new WorkbenchJob(display, "CEF Polling") { //$NON-NLS-1$
			{setSystem(true);}
			@Override
			public IStatus runInUIThread(IProgressMonitor arg0) {
				N_Cef.N_DoMessageLoopWork();				
				if (messageJob != null) {
					messageJob.schedule(messageJobTimer);
				}
				return Status.OK_STATUS;
			}
		};
		messageJob.schedule(messageJobTimer);
	}

	/**
	 * Add a reference count to the message thread.
	 */
	void addMessageThreadRef() {
		msgThreadCount++;
	}
	
	/**
	 * Decrement the message thread reference count.
	 */
	void releaseMessageThread() {
		msgThreadCount--;
		if (msgThreadCount <= 0) {
			messageJob = null;
		}
	}

	/**
	 * Returns whether value is a 10 base number or not.
	 * 
	 * @param value
	 */
	private boolean isNumber(String value) {
		for (char ch : value.toCharArray()) {
			if (!Character.isDigit(ch)) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * Remove comment blocks.
	 * 
	 * @param 
	 * @return
	 */
	private String removeComments(String json) {
		int start = 0;
		while ((start = json.indexOf("/*")) > 0) { //$NON-NLS-1$
			if (start > 0) {
				int end = json.indexOf("*/", start); //$NON-NLS-1$
				if (end > start) {
					json = json.substring(0, start) + json.substring(end+2);
				}
			}
		}
		
		return json;
	}
}
