# swt-chromium

- CEF3 is the embedding API to Chromium, and is used by Electron.  It sees new releases every 2 months, but is relatively stable.
- CEF3 is *also* the embedding API to the upcoming [Servo browser from Firefox](https://github.com/servo/servo/tree/master/ports/cef)

Electron has shown the development velocity you can have with a guaranteed browser implementation.  SWT could have this too, but we would need to move to CEF3 and ship the browser binaries, perhaps through a fragment like `swt-chromium.win32.win32.x86_64`, etc.

The SWT team is currently too busy to take this on, so this repository is an attempt to coordinate the effort and host the binaries in an easy-to-consume way.

For now, the action is all in [the issues section](https://github.com/diffplug/swt-chromium/issues).
