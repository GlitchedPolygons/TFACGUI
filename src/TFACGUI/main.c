/*
   Copyright 2022 Raphael Beck

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "nappgui.h"
#include <time.h>

#include "../lib/TFAC/src/tfac.c" // This includes windows.h
#include "../lib/TFAC/src/base32.c"

struct app_t
{
	Window* window;
	Panel* panel;
	Layout* layout;
	Edit* edit_totp_secret;
	Label* label_totp_secret;
	Label* label_totp_algo;
	TextView* text_view_totp;
	Button* button_copy_totp_to_clipboard;
	Progress* progress;
	PopUp* popup_algo;
	Label* label_footer;

	uint32_t clicks;
	struct tfac_token totp;
};

static void update(struct app_t* app, const real64_t prtime, const real64_t ctime)
{
	const time_t utc = time(NULL);
	const float progress = 1.0f - ((utc % 30) / 30.f);
	const char* totp_secret = edit_get_text(app->edit_totp_secret);
	const enum tfac_hash_algo totp_algo = (enum tfac_hash_algo)popup_get_selected(app->popup_algo);

	app->totp = tfac_totp(totp_secret, TFAC_DEFAULT_DIGITS, TFAC_DEFAULT_STEPS, totp_algo);

	progress_value(app->progress, progress);

	textview_clear(app->text_view_totp);
	textview_writef(app->text_view_totp, app->totp.string);
}

static void on_change_totp_secret(struct app_t* app, Event* e)
{
	const EvText* params = event_params(e, EvText);

	int i = 0;
	char* c = params->text;
	char new_totp_secret[64] = { 0x00 };

	for (; c != NULL && i < 64 - 1; ++c, ++i)
	{
		if (*c == '\0')
		{
			break;
		}

		const char append = toupper(*c);

		switch (append)
		{
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '=':
			{
				new_totp_secret[i] = append;
				break;
			}
		}
	}

	edit_text(app->edit_totp_secret, new_totp_secret);

	update(app, 0, 0);
}

static void on_change_totp_algo(struct app_t* app, Event* e)
{
	update(app, 0, 0);
}

static void on_click_copy_totp(struct app_t* app, Event* e)
{
	if (app == NULL)
	{
		return;
	}

	if (app->totp.string[0] == '\0')
	{
		return;
	}

#ifdef _WIN32

	if (!OpenClipboard(NULL))
	{
		return;
	}

	EmptyClipboard();

	const size_t totp_string_length = strlen(app->totp.string);

	HGLOBAL temp = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, totp_string_length + 1);

	if (temp == NULL)
	{
		return;
	}

	LPSTR templock = (LPSTR)GlobalLock(temp);

	if (templock == NULL)
	{
		GlobalFree(temp);
		return;
	}

	memcpy(templock, app->totp.string, totp_string_length);
	templock[totp_string_length] = '\0';

	GlobalUnlock(temp);

	if (!SetClipboardData(CF_TEXT, temp))
	{
		GlobalFree(temp);
		return;
	}

	CloseClipboard();

#elif defined(__linux__)

	char cmd[64] = { 0x00 };
	snprintf(cmd, sizeof(cmd), "echo '%s' | xclip -sel clip", app->totp.string);

	system(cmd);

#elif defined(__APPLE__)

	char cmd[64] = { 0x00 };
	snprintf(cmd, sizeof(cmd), "echo '%s' | pbcopy", app->totp.string);

	system(cmd);

#endif

	++app->clicks;
	unref(e);
}

static void on_click_label_footer(struct app_t* app, Event* e)
{
#ifdef _WIN32
	ShellExecute(0, 0, L"https://glitchedpolygons.com", 0, 0, SW_SHOW);
#else
	system("open https://glitchedpolygons.com");
#endif
}

static void on_close(struct app_t* app, Event* e)
{
	osapp_finish();
	unref(app);
	unref(e);
}

static Panel* create_main_panel(struct app_t* app)
{
	app->panel = panel_create();
	app->layout = layout_create(1, 8);
	app->edit_totp_secret = edit_create();
	app->label_totp_secret = label_create();
	app->label_totp_algo = label_create();
	app->text_view_totp = textview_create();
	app->button_copy_totp_to_clipboard = button_push();
	app->progress = progress_create();
	app->popup_algo = popup_create();
	app->label_footer = label_create();
	popup_add_elem(app->popup_algo, "SHA1", NULL);
	popup_add_elem(app->popup_algo, "SHA224", NULL);
	popup_add_elem(app->popup_algo, "SHA256", NULL);
	popup_OnSelect(app->popup_algo, listener(app, on_change_totp_algo, struct app_t));

	label_text(app->label_totp_secret, "2FA Secret");
	label_text(app->label_totp_algo, "Algorithm");
	textview_fstyle(app->text_view_totp, ekFBOLD);
	textview_units(app->text_view_totp, ekFPOINTS);
	textview_fsize(app->text_view_totp, 20);
	textview_family(app->text_view_totp, "Courier New");
	textview_size(app->text_view_totp, s2df(0, 32));
	textview_halign(app->text_view_totp, ekCENTER);
	textview_editable(app->text_view_totp, 0);
	textview_writef(app->text_view_totp, "123 456");

	edit_tooltip(app->edit_totp_secret, "Enter your 2FA secret here. This is typically an alphanumeric string of random characters (the one that is also contained inside the QR code you'd scan with e.g. Google Authenticator, Authy, etc...)");
	edit_text(app->edit_totp_secret, "");
	edit_passmode(app->edit_totp_secret, 1);
	edit_autoselect(app->edit_totp_secret, 1);
	edit_OnFilter(app->edit_totp_secret, listener(app, on_change_totp_secret, struct app_t));

	button_text(app->button_copy_totp_to_clipboard, "Copy to clipboard");
	button_OnClick(app->button_copy_totp_to_clipboard, listener(app, on_click_copy_totp, struct app_t));

	label_align(app->label_footer, ekCENTER);
	label_text(app->label_footer, "Copyright (C) 2022, Raphael Beck | Glitched Polygons");
	label_color(app->label_footer, color_rgb(19, 146, 156));
	label_color_over(app->label_footer, color_rgb(16, 167, 179));
	label_font(app->label_footer, font_system(10, ekFNORMAL | ekFUNDERLINE));
	label_OnClick(app->label_footer, listener(app, on_click_label_footer, struct app_t));

	layout_label(app->layout, app->label_totp_secret, 0, 0);
	layout_edit(app->layout, app->edit_totp_secret, 0, 1);
	layout_label(app->layout, app->label_totp_algo, 0, 2);
	layout_popup(app->layout, app->popup_algo, 0, 3);
	layout_progress(app->layout, app->progress, 0, 4);
	layout_textview(app->layout, app->text_view_totp, 0, 5);
	layout_button(app->layout, app->button_copy_totp_to_clipboard, 0, 6);
	layout_label(app->layout, app->label_footer, 0, 7);
	layout_hsize(app->layout, 0, 250);
	layout_vsize(app->layout, 3, 100);
	layout_margin(app->layout, 5);
	layout_vmargin(app->layout, 0, 5);
	layout_vmargin(app->layout, 1, 5);
	layout_vmargin(app->layout, 2, 5);
	layout_vmargin(app->layout, 3, 12);
	layout_vmargin(app->layout, 4, 6);
	layout_vmargin(app->layout, 5, 5);
	layout_vmargin(app->layout, 6, 7);
	panel_layout(app->panel, app->layout);

	return app->panel;
}

static struct app_t* create(void)
{
	struct app_t* app = heap_new0(struct app_t);
	Panel* panel = create_main_panel(app);

	app->window = window_create(ekWNSTD);

	window_panel(app->window, panel);
	window_title(app->window, "TFAC");
	window_origin(app->window, v2df(500, 200));
	window_OnClose(app->window, listener(app, on_close, struct app_t));
	window_show(app->window);

	update(app, 0, 0);

	return app;
}

static void destroy(struct app_t** app)
{
	window_destroy(&(*app)->window);

	heap_delete(app, struct app_t);
}

/*---------------------------------------------------------------------------*/

#include "osmain.h"
osmain_sync(1.0f, create, destroy, update, "", struct app_t)
