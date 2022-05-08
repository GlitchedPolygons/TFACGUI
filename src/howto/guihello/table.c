/*
 * NAppGUI Cross-platform C SDK
 * 2015-2022 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: table.c
 *
 */

/* Use of tables */

#include "table.h"
#include "guiall.h"
#include "res.h"

typedef struct _appdata_t AppData;
static char_t TEMP_STRING[256];

/*---------------------------------------------------------------------------*/

// AppData should contain the real data access (array, stream, etc)
static void i_OnTableNotify(AppData *data, Event *e)
{
    uint32_t etype = event_type(e);
    unref(data);

    switch(etype) {
    case ekEVTBLNROWS:
    {
        uint32_t *n = event_result(e, uint32_t);
        *n = 100;
        break;
    }

    case ekEVTBLCELL:
    {
        const EvTbPos *pos = event_params(e, EvTbPos);
        EvTbCell *cell = event_result(e, EvTbCell);

        switch(pos->col) {
        case 0:
            bstd_sprintf(TEMP_STRING, sizeof(TEMP_STRING), "Name %d", pos->row);
            break;

        case 1:
            bstd_sprintf(TEMP_STRING, sizeof(TEMP_STRING), "Adress %d", pos->row);
            break;

        case 2:
            bstd_sprintf(TEMP_STRING, sizeof(TEMP_STRING), "City %d", pos->row);
            break;

        case 3:
            bstd_sprintf(TEMP_STRING, sizeof(TEMP_STRING), "Age %d", pos->row);
            break;

        case 4:
            bstd_sprintf(TEMP_STRING, sizeof(TEMP_STRING), "Position %d", pos->row);
            break;

        cassert_default();
        }

        cell->text = TEMP_STRING;
        break;
    }

    cassert_default();
    }
}

/*---------------------------------------------------------------------------*/

Panel *table_view(void)
{
    Panel *panel = panel_create();
    Layout *layout = layout_create(1, 1);
    TableView *table = tableview_create();
    tableview_size(table, s2df(500, 300));
    tableview_OnNotify(table, listener(NULL, i_OnTableNotify, AppData));
    tableview_new_column_text(table);
    tableview_new_column_text(table);
    tableview_new_column_text(table);
    tableview_new_column_text(table);
    tableview_new_column_text(table);
    tableview_header_title(table, 0, "Name");
    tableview_header_title(table, 1, "Address");
    tableview_header_title(table, 2, "City");
    tableview_header_title(table, 3, "Age");
    tableview_header_title(table, 4, "Position");
    tableview_column_width(table, 0, 100);
    tableview_column_width(table, 1, 175);
    tableview_column_width(table, 2, 50);
    tableview_column_width(table, 3, 50);
    tableview_column_width(table, 4, 170);
    tableview_data_update(table);
    layout_tableview(layout, table, 0, 0);
    panel_layout(panel, layout);
    return panel;
}
