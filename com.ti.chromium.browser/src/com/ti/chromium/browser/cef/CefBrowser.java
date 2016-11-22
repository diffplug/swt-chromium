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

import org.eclipse.core.runtime.Platform;
import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.browser.OpenWindowListener;
import org.eclipse.swt.browser.ProgressListener;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.json.JSONException;
import org.json.JSONObject;

import com.ti.chromium.browser.CEFBrowserPlugin;

/**
 * Instances of this class implement the CEF browser user interface.
 * It allows the user to visualize and navigate through HTML documents.
 */
public class CefBrowser extends Composite {
	/* Expression evaluation time out */
	private static final int EVAL_TIME_OUT = 5000;
	
	private N_CefBrowser N_CefBrowser;
		
	/**
	 * Create a new instance of the CEF browser and begin processing
	 * CEF message loop on the application main thread. Client is required
	 * to call {@code dispose} to deallocate CEF resources. 
	 * 
	 * @param parent parent window handle
	 */
	public CefBrowser(final Composite parent) {
		this(parent, null);
	}
	
	/**
	 * Create a new instance of the CEF browser and begin processing
	 * CEF message loop on the application main thread. Client is required
	 * to call {@code dispose} to deallocate CEF resources. 
	 * 
	 * @param parent parent window handle
	 * @param url the initial browser url
	 */
	public CefBrowser(Composite parent, String url) {
		super(parent, Platform.OS_WIN32.equals(Platform.getWS()) ? SWT.EMBEDDED : SWT.NONE);
		Cef.INSTANCE.init();
		
		
		N_CefBrowser = new N_CefBrowser(this);
		
		/*
		 * Add a control listener to resize the CEF browser
		 */
		addControlListener(new ControlAdapter() {
			@Override
			public void controlResized(ControlEvent e) {
				try {
					Rectangle area = CefBrowser.this.getClientArea();
					N_CefBrowser.N_SetWindowSize(0, 0, area.width, area.height);
				} catch (Throwable t) {
					CEFBrowserPlugin.log(t);
				}
			}
		});
		
		/*
		 * Create the native browser and initialize the message loop
		 */
		try {
			N_CefBrowser.N_CreateBrowser((int)this.handle, url);		
			Cef.INSTANCE.initMessageThread(parent.getDisplay());
			Cef.INSTANCE.addMessageThreadRef();
			
			/*
			 * This is a workaround GTK - the browser doesn't always update itself, can be reproduce with TiRex.
			 */
			if (Platform.WS_GTK.equals(Platform.getWS())) {
				addLocationListener(new LocationListener() {
					@Override
					public void changing(LocationEvent arg0) {
					}
					
					@Override
					public void changed(LocationEvent arg0) {
						Rectangle area = CefBrowser.this.getClientArea();
						N_CefBrowser.N_SetWindowSize(0, 0, area.width, area.height);
					}
				});
			}
			
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}	
	
	/**
	 * Dispose allocated resources.
	 */
	public void dispose() {
		try {
			CefBrowserContextMenu[] menus = N_CefBrowser.contextMenus.values().toArray(new CefBrowserContextMenu[0]);
			for (CefBrowserContextMenu menu : menus) {
				try {
					menu.dispose();
				} catch (Throwable t) {
					CEFBrowserPlugin.log(t);
				}
			}
			
			N_CefBrowser.N_Destroy();
			N_CefBrowser.N_CefHandle = 0;
			Cef.INSTANCE.releaseMessageThread();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	/**
	 * Returns the zoom level. 
	 * 
	 * @return zoom level
	 */
	public double getZoomLevel() {
		try {
			return N_CefBrowser.N_GetZoomLevel();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return 1;
	}
	
	/***
	 * Sets the zoom level.
	 * 
	 * @param level the zoom level
	 */
	public void setZoomLevel(double level) {
		try {
			N_CefBrowser.N_SetZoomLevel(level);
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	/**
	 * Set the browser url.
	 * 
	 * @param url url string
	 * @return {@code true} if the operation succeeded, otherwise {@code false}
	 */
	public boolean setUrl(String url) {
		try {
			N_CefBrowser.N_SetUrl(url);
			return true;
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return false;
	}

	public String getUrl() {
		try {
			return N_CefBrowser.N_GetUrl();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return null;
	}
	
	public boolean isBackEnabled() {
		try {
			return N_CefBrowser.N_IsBackEnabled();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return false;
	}
	
	public boolean back() {
		try {
			N_CefBrowser.N_Back();
			return true;
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return false;
	}
	
	public boolean isForwardEnabled() {
		try {
			return N_CefBrowser.N_IsForwardEnabled();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return false;
	}
	
	public boolean forward() {
		try {
			N_CefBrowser.N_Forward();
			return true;
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return false;
	}
	
	public void refresh() {
		try {
			N_CefBrowser.N_Refresh();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	public void stop() {
		try {
			N_CefBrowser.N_Stop();
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	public boolean execute(String script) {
		try {
			return N_CefBrowser.N_Execute(script);
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return false;
	}
	
	public Object evaluate(String script) {
		try {
			if (script != null && script.trim().length() > 0) {
				N_CefBrowser.N_Evaluate(script);
				
				/* 
				 * JS evaluation in the browser is async ->
				 * 		messages are passed between browser process and child render process. 
				 */
				N_CefBrowser.N_EvalRespond = null;
				long start = System.currentTimeMillis();
				while (N_CefBrowser.N_EvalRespond == null && System.currentTimeMillis() - start < EVAL_TIME_OUT) {
					Cef.N_Cef.N_DoMessageLoopWork();
					Thread.sleep(10);
				}
				if (N_CefBrowser.N_EvalRespond == null) { 
					return "JavaScript evaluation timeout after " + EVAL_TIME_OUT + "ms";
				}
				
				try {
					JSONObject root = new JSONObject(N_CefBrowser.N_EvalRespond);
					Object object = root.get("result"); // this is the root node of the respond JSON object
					Object nativeObject = N_CefBrowser.createNativeJavaObject(object);
					return nativeObject;
				} catch (JSONException j) {
					// ignore parsing exception
				}
			}
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
		return null;
	}	
	
	
	public void createBrowserFunction(String name, CefBrowserFunction function) {
		try {
			N_CefBrowser.N_CreateBrowserFunction(name, function);
			N_CefBrowser.functions.put(name, function);
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	public void destroyBrowserFunction(String name) {
		try {
			N_CefBrowser.functions.remove(name);
			N_CefBrowser.N_DestroyBrowserFunction(name);
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	public void addContextMenu(int id, String label, CefBrowserContextMenu menu) {
		try {
			N_CefBrowser.N_AddContextMenu(id, label, menu);
			N_CefBrowser.contextMenus.put(id, menu);
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	public void removeContextMenu(int id) {
		try {
			N_CefBrowser.contextMenus.remove(id);
			N_CefBrowser.N_RemoveContextMenu(id);
		} catch (Throwable t) {
			CEFBrowserPlugin.log(t);
		}
	}
	
	public void addLocationListener(LocationListener listener) {
		N_CefBrowser.N_LocationListener.add(listener);
	}
	
	public void removeLocationListener(LocationListener listener) {
		N_CefBrowser.N_LocationListener.remove(listener);
	}
	
	public void addProgressListener(ProgressListener listener) {
		N_CefBrowser.N_ProgressListener.add(listener);
	}

	public void removeProgressListener(ProgressListener listener) {
		N_CefBrowser.N_ProgressListener.remove(listener);
	}
	
	public void addOpenWindowListener(OpenWindowListener listener) {
		N_CefBrowser.N_OpenWindowListener.add(listener);
	}
	
	public void removeOpenWindowListener(OpenWindowListener listener) {
		N_CefBrowser.N_OpenWindowListener.remove(listener);
	}
}
