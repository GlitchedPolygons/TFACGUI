/*
 * NAppGUI Cross-platform C SDK
 * 2015-2022 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: tableview.h
 * https://nappgui.com/en/gui/tableview.html
 *
 */

/* TableView */

#include "gui.hxx"

__EXTERN_C

TableView *tableview_create(void);

void tableview_OnNotify(TableView *view, Listener *listener);

void tableview_size(TableView *view, S2Df size);

uint32_t tableview_new_column_text(TableView *view);

void tableview_column_width(TableView *view, const uint32_t column_id, const real32_t width);

void tableview_header_font(TableView *view, const Font *font);

void tableview_header_title(TableView *view, const uint32_t column_id, const char_t *text);

void tableview_header_align(TableView *view, const uint32_t column_id, const align_t align);

void tableview_data_update(TableView *view);

__END_C

