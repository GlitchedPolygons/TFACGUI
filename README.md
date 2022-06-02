# TFAC GUI

## TOTP Generator GUI

TFAC GUI is a 2FA (Two-Factor Authentication) token generator GUI.

It currently only supports 30 seconds for the token interval setting and the `SHA-1`, `SHA-224` and `SHA-256` hashing algorithms, but the standard 2FA setup is 30 seconds `SHA-1` anyway, so yeah..

The GUI is written in plain C, using the awesome [NAppGUI SDK](https://github.com/frang75/nappgui_src). Huge shoutout to [@frang75](https://github.com/frang75) for bringing this awesome piece of software to us!

---

![TFACGUI Screenshot Windows](https://api.files.glitchedpolygons.com/api/v1/files/tfacgui-screenshot-win32.png)

### Usage

Just copy and paste your 2FA secret into the GUI's text field and use the generated two-factor authentication code.

### License

This GUI is licensed under the Apache-2.0 license, but the underlying [NAppGUI](https://github.com/frang75/nappgui_src) library is MIT-licensed. The licenses and readme files are distinguished between this fork and the original [NAppGUI](https://github.com/frang75/nappgui_src) like this:

* [LICENSE.txt](https://github.com/GlitchedPolygons/TFACGUI/blob/main/LICENSE.txt)
* * This GUI's license (Apache-2.0).
* [LICENSE.nappgui.txt](https://github.com/GlitchedPolygons/TFACGUI/blob/main/LICENSE.nappgui.txt)
* * The underlying library's license (MIT)
* [README.md](https://github.com/GlitchedPolygons/TFACGUI/blob/main/README.md)
* * This readme file.
* [README.nappgui.md](https://github.com/GlitchedPolygons/TFACGUI/blob/main/README.nappgui.md)
* * The original [NAppGUI SDK's](https://github.com/frang75/nappgui_src) README.md
