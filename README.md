# TFAC GUI

## TOTP Generator GUI

TFAC GUI is a 2FA (Two-Factor Authentication) token generator GUI.

It currently only supports 30 seconds for the token interval setting and the `SHA-1`, `SHA-224` and `SHA-256` hashing algorithms, but the standard 2FA setup is 30 seconds `SHA-1` anyway, so yeah..

The GUI is written in plain C, using the awesome [NAppGUI SDK](https://github.com/frang75/nappgui_src). Huge shoutout to [@frang75](https://github.com/frang75) for bringing this awesome piece of software to us!

---

### Usage

Just copy and paste your 2FA secret into the GUI's text field and use the generated two-factor authentication code.
