package buildhelper;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.lang.reflect.Method;
import java.nio.charset.StandardCharsets;
import java.util.Date;
import java.util.function.Consumer;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.compressors.bzip2.BZip2CompressorInputStream;
import org.apache.http.HttpEntity;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.gradle.api.DefaultTask;
import org.gradle.api.GradleException;
import org.gradle.api.tasks.Input;
import org.gradle.api.tasks.OutputFile;
import org.gradle.api.tasks.TaskAction;
import org.gradle.internal.logging.progress.ProgressLogger;
import org.gradle.internal.logging.progress.ProgressLoggerFactory;

import com.diffplug.common.base.Box;
import com.diffplug.common.base.Errors;
import com.diffplug.common.base.Preconditions;
import com.diffplug.common.base.StringPrinter;
import com.diffplug.common.base.Throwing;
import com.diffplug.common.hash.HashCode;
import com.diffplug.common.hash.Hashing;
import com.diffplug.common.io.CharStreams;
import com.diffplug.common.io.Files;
import com.diffplug.gradle.FileMisc;

/**
 * Downloads and extracts a .tar.bz2,
 * while keeping the root folder out,
 * with fast directory caching.
 * 
 * Progress reporting ripped from Michel Kraemer's [gradle-download-task](https://github.com/michel-kraemer/gradle-download-task/blob/master/src/main/java/de/undercouch/gradle/tasks/download/DownloadAction.java), many thanks.
 */
public class DownloadAndExtract extends DefaultTask {
	@Input
	public String getUrl() {
		return url;
	}

	@Input
	public String getShaUrl() {
		return shaUrl;
	}

	@OutputFile
	public File getCompletedToken() {
		return completedToken;
	}

	private String url, shaUrl;
	private File destinationFolder, completedToken;

	void setup(String url, File destinationFolder) {
		setup(url, url + ".sha1", destinationFolder);
	}

	void setup(String url, String shaUrl, File destinationFolder) {
		this.url = url;
		this.shaUrl = shaUrl;
		this.destinationFolder = destinationFolder;
		completedToken = new File(destinationFolder, "README.md");
	}

	private ProgressLogger logger;

	@TaskAction
	public void downloadAndExtract() throws IOException {
		File downloadFile = new File(destinationFolder, url.substring(url.lastIndexOf('/') + 1));
		File extractFolder = new File(destinationFolder, "extracted");

		// find the sha we're expecting
		logHeader("Downloading expected sha1...");
		String sha1 = getSha(shaUrl);

		// find the file that should have been downloaded
		if (downloadFile.exists()) {
			logHeader("File already exists, calculating hash...");
			if (checkSha(downloadFile, sha1)) {
				log("...matches!");
			} else {
				FileMisc.forceDelete(downloadFile);
			}
		}

		// download the file if we don't have it
		if (!downloadFile.exists()) {
			logHeader("Downloading");
			download(url, downloadFile, this::log);
			logHeader("Checking hash...");
			if (checkSha(downloadFile, sha1)) {
				log("...matches!");
			} else {
				throw new GradleException("Downloaded file did not match sha");
			}
		}

		// uncompress the file we downloaded
		logHeader("Extracting");
		FileMisc.cleanDir(extractFolder);
		uncompress(downloadFile, extractFolder);

		// flatten for consistent filenames
		File[] children = extractFolder.listFiles();
		Preconditions.checkArgument(children.length == 1, children);
		FileMisc.flatten(children[0]);

		// write out the completed token
		Files.write(StringPrinter.buildString(printer -> {
			printer.println("Downloaded on " + new Date());
			printer.println("From " + url);
			printer.println("Verified hash " + sha1 + " obtained from " + shaUrl);
		}), completedToken, StandardCharsets.UTF_8);
		endLog();
	}

	static void uncompress(File toUncompress, File destinationDir) throws FileNotFoundException, IOException {
		byte[] buffer = new byte[BUFFER_SIZE];
		// extract the downloaded file
		try (TarArchiveInputStream untarred = new TarArchiveInputStream(
				new BZip2CompressorInputStream(
						new BufferedInputStream(
								new FileInputStream(toUncompress))))) {
			TarArchiveEntry entry;
			while((entry = untarred.getNextTarEntry()) != null) {
				File target = new File(destinationDir, entry.getName());
				if (entry.isDirectory()) {
					FileMisc.mkdirs(target);
				} else {
					long size = entry.getSize();
					long totalRead = 0;
					try (FileOutputStream output = new FileOutputStream(target)) {
						while (totalRead < size) {
							int numRead = untarred.read(buffer);
							output.write(buffer, 0, numRead);
							totalRead += numRead;
						}
					}
				}
			}
		}
	}

	private boolean checkSha(File file, String sha1) throws IOException {
		HashCode calculated = Files.hash(file, Hashing.sha1());
		if (calculated.toString().equals(sha1)) {
			return true;
		} else {
			log("...hash failed, was " + calculated + ", expected " + sha1);
			return false;
		}
	}

	static String getSha(String url) throws IOException {
		Box.Nullable<String> result = Box.Nullable.ofNull();
		httpGet(url, entity -> {
			try (Reader reader = new InputStreamReader(entity.getContent(), StandardCharsets.UTF_8)) {
				result.set(CharStreams.toString(reader));
			}
		});
		return result.get();
	}

	private static final int BUFFER_SIZE = 8192;

	static void download(String url, File destination, Consumer<String> monitor) throws IOException {
		monitor.accept("Connecting");
		httpGet(url, entity -> {
			String toRead = " / " + toKbMbGb(entity.getContentLength());
			monitor.accept("0" + toRead);
			long totalRead = 0;
			FileMisc.mkdirs(destination.getParentFile());
			byte[] buffer = new byte[BUFFER_SIZE];
			try (	InputStream input = new BufferedInputStream(entity.getContent());
					FileOutputStream output = new FileOutputStream(destination)) {
				int numRead;
				while ((numRead = input.read(buffer)) >= 0) {
					output.write(buffer, 0, numRead);
					totalRead += numRead;
					monitor.accept(toKbMbGb(totalRead) + toRead);
				}
			}
		});
	}

	static void httpGet(String url, Throwing.Specific.Consumer<HttpEntity, IOException> consume) throws IOException {
		CloseableHttpClient httpClient = HttpClients.createDefault();
		HttpGet get = new HttpGet(url);
		try (CloseableHttpResponse response = httpClient.execute(get)) {
			HttpEntity entity = response.getEntity();
			consume.accept(entity);
		}
	}

	/////////////
	// logging //
	////////////
	private void logHeader(String header) {
		if (logger != null) {
			endLog();
		}
		ProgressLoggerFactory factory = invoke(invoke(getProject(), "getServices"), "get", ProgressLoggerFactory.class);
		logger = factory.newOperation(header);
		logger.setDescription(header);
		logger.setLoggingHeader(header);
		logger.started();
	}

	private void log(String progress) {
		logger.progress(progress);
	}

	private void endLog() {
		logger.completed();
		logger = null;
	}

	@SuppressWarnings("unchecked")
	private static <T> T invoke(Object obj, String method, Object... args) {
		Class<?>[] argumentTypes = new Class[args.length];
		for (int i = 0; i < args.length; ++i) {
			argumentTypes[i] = args[i].getClass();
		}
		return Errors.rethrow().get(() -> {
			Method m = obj.getClass().getMethod(method, argumentTypes);
			m.setAccessible(true);
			return (T) m.invoke(obj, args);
		});
	}

	static String toKbMbGb(long bytes) {
		if (bytes < 1024) {
			return bytes + " B";
		} else if (bytes < 1024 * 1024) {
			return (bytes / 1024) + " KB";
		} else if (bytes < 1024 * 1024 * 1024) {
			return String.format("%.2f MB", bytes / (1024.0 * 1024.0));
		} else {
			return String.format("%.2f GB", bytes / (1024.0 * 1024.0 * 1024.0));
		}
	}
}
