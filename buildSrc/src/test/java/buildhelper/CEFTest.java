package buildhelper;

import org.assertj.core.api.Assertions;
import org.junit.Assert;
import org.junit.Test;

import com.diffplug.common.swt.os.OS;
import com.diffplug.common.swt.os.SwtPlatform;

public class CEFTest {
	@Test
	public void testToOs() {
		for (OS os : OS.values()) {
			SwtPlatform platform = SwtPlatform.fromOS(os);
			Assert.assertEquals(os, CEF.toOS(platform));
		}
	}

	private void testParsePlatformsArg(String arg, SwtPlatform... expected) {
		Assertions.assertThat(CEF.parsePlatformsArg(arg)).containsExactlyInAnyOrder(expected);
	}

	@Test
	public void testParsePlatformsArg() {
		testParsePlatformsArg(null, SwtPlatform.getRunning());
		testParsePlatformsArg("win32.win32.x86", SwtPlatform.fromOS(OS.WIN_x86));
		testParsePlatformsArg("win32.win32.x86,win32.win32.x86_64", SwtPlatform.fromOS(OS.WIN_x86), SwtPlatform.fromOS(OS.WIN_x64));
		testParsePlatformsArg("all", SwtPlatform.getAll().toArray(new SwtPlatform[0]));
	}
}
