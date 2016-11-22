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

import java.io.IOException;
import java.net.URL;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class CEFBrowserPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "com.ti.chromium.browser"; //$NON-NLS-1$

	// The shared instance
	private static CEFBrowserPlugin plugin;
	
	/**
	 * The constructor
	 */
	public CEFBrowserPlugin() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
	}
	
	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static CEFBrowserPlugin getDefault() {
		return plugin;
	}

	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return imageDescriptorFromPlugin(PLUGIN_ID, path);
	}
	
	/**
	 * Convert the relative path of a bundle resource to fullpath
	 * 
	 * @param relativePath the relative path
	 * @return fullpath
	 */
	public static IPath getResourceFullpath(IPath relativePath) {
		Bundle bundle = getDefault().getBundle();
		URL entry = bundle.getEntry(relativePath.toPortableString());
		if (entry != null) {
			try {
				entry = FileLocator.toFileURL(entry);
				return new Path(entry.getPath());
			} catch (IOException e) {
				log(e);
			}
		}
		return null;
	}
	
	/**
	 * Log an exception
	 * 
	 * @param e the exception
	 */
	public static void log(Throwable e) {
		Status status = new Status(Status.ERROR, PLUGIN_ID, Status.OK, e.getMessage(), e);
		getDefault().getLog().log(status);
	}

	/**
	 * Log an exception with a message
	 * 
	 * @param message additional message
	 * @param statusId exception status id
	 * @param e the exception
	 */
	public static void log(String message, int statusId, Throwable e) {
		Status status = new Status(statusId, PLUGIN_ID, Status.OK, message, e);
		getDefault().getLog().log(status);
	}
	/**
	 * Log a message
	 * 
	 * @param message the message
	 */
	public static void log(String message) {
		Status status = new Status(Status.ERROR, PLUGIN_ID, Status.OK, message, null);
		getDefault().getLog().log(status);
	}
}
