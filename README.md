> [!IMPORTANT]
>
> # **NOTICE:**
>
> ## Development of this project has been migrated to: https://codeberg.org/GlitchedPolygons/TFACGUI
>
> ## All pull requests and issues on GitHub will be ignored from the 16th of August, 2025 onwards.

# TFAC GUI

## TOTP Generator GUI

TFAC GUI is a 2FA (Two-Factor Authentication) token generator GUI.

It currently only supports 30 seconds for the token interval setting and the `SHA-1`, `SHA-224` and `SHA-256` hashing algorithms, but the standard 2FA setup is 30 seconds `SHA-1` anyway, so yeah..

It's based on [TFAC](https://codeberg.org/GlitchedPolygons/TFAC), an open-source TOTP library I wrote in C some time ago.

The GUI is written in plain C, using the awesome [NAppGUI SDK](https://github.com/frang75/nappgui_src). Huge shoutout to [@frang75](https://github.com/frang75) for bringing this awesome piece of software to us!

---

![TFACGUI Screenshot Windows](https://api-files.glitchedpolygons.com/api/v1/files/tfacgui-screenshot-win32.png)

### Usage

Just copy and paste your 2FA secret into the GUI's text field and use the generated two-factor authentication code.

### License

This GUI is licensed under the Apache-2.0 license, but the underlying [NAppGUI](https://github.com/frang75/nappgui_src) library is MIT-licensed. The licenses and readme files are distinguished between this fork and the original [NAppGUI](https://github.com/frang75/nappgui_src) like this:

* [LICENSE.txt](https://codeberg.org/GlitchedPolygons/TFACGUI/src/branch/main/LICENSE.txt)
* * This GUI's license (Apache-2.0).
* [LICENSE.nappgui.txt](https://codeberg.org/GlitchedPolygons/TFACGUI/src/branch/main/LICENSE.nappgui.txt)
* * The underlying library's license (MIT)
* [README.md](https://codeberg.org/GlitchedPolygons/TFACGUI/src/branch/main/README.md)
* * This readme file.
* [README.nappgui.md](https://codeberg.org/GlitchedPolygons/TFACGUI/src/branch/main/README.nappgui.md)
* * The original [NAppGUI SDK's](https://github.com/frang75/nappgui_src) README.md

It's also available for purchase on [glitchedpolygons.com](https://glitchedpolygons.com) (ships with some cool, additional stuff, PDF files/manual, etc...). 

Please consider buying it from there to support the development of nifty tools like this :)
