package buildhelper;

import java.io.File;
import java.util.HashSet;
import java.util.Set;

import org.gradle.api.Nullable;
import org.gradle.api.Project;
import org.gradle.api.Task;
import org.gradle.api.tasks.Delete;

import com.diffplug.common.swt.os.OS;
import com.diffplug.common.swt.os.SwtPlatform;

/** Miscellany for downloading and extracting the CEF. */
public class CEF {
	/**
	 * null -> the running platform
	 * 'all' -> all platforms
	 * 'win32.win32.x86,win32.win32.x86_64' -> those platforms
	 */
	public static Set<SwtPlatform> parsePlatformsArg(@Nullable String platforms) {
		Set<SwtPlatform> result = new HashSet<>();
		if (platforms == null) {
			result.add(SwtPlatform.getRunning());
		} else if (platforms.equals("all")) {
			result.addAll(SwtPlatform.getAll());
		} else {
			for (String platform : platforms.split(",")) {
				result.add(SwtPlatform.parseWsOsArch(platform));
			}
			
		}
		return result;
	}

	/** Converts an SwtPlatform back to an OS. */
	static OS toOS(SwtPlatform platform) {
		for (OS os : OS.values()) {
			if (SwtPlatform.fromOS(os).equals(platform)) {
				return os;
			}
		}
		throw new IllegalArgumentException("No known OS matches this platform: " + platform);
	}

	/** Converts an SwtPlatform back to an OS. */
	public static String cefUrl(String cefVersion, SwtPlatform platform, boolean minimal) {
		OS os = toOS(platform);
		String plat = os.winMacLinux("windows", "macosx", "linux") + os.getArch().x86x64("32", "64");
		String min = minimal ? "_minimal" : "";
		return "http://opensource.spotify.com/cefbuilds/cef_binary_" + cefVersion + "_" + plat + min + ".tar.bz2";
	}

	private static String prop(Project proj, String prop) {
		return (String) proj.property(prop);
	}

	/** Returns a task which will download and extract the CEF binaries for the given platform into the given folder. */
	public static Task downloadTask(Project project, SwtPlatform platform, Object destFolder) {
		String cefVersion = prop(project, "VER_CEF");
		boolean isMinimal = Boolean.parseBoolean(prop(project, "CEF_MINIMAL"));
		File folder = project.file(destFolder);

		project.getTasks().create("cleanCEF", Delete.class, task -> {
			task.delete(folder);
		});

		return project.getTasks().create("downloadCEF", DownloadAndExtract.class, task -> {
			task.setup(cefUrl(cefVersion, platform, isMinimal), folder);
		});
	}
}
