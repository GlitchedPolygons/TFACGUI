/*
 * NAppGUI Cross-platform C SDK
 * 2015-2022 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: tableview.c
 *
 */

/* TableView */

#include "tableview.h"
#include "drawctrl.inl"
#include "view.h"
#include "view.inl"
#include "gui.inl"

#include "arrst.h"
#include "bmath.h"
#include "cassert.h"
#include "color.h"
#include "draw.h"
#include "event.h"
#include "font.h"
#include "heap.h"
#include "s2d.h"
#include "strings.h"
#include "types.h"

typedef struct _column_t Column;
typedef struct _tdata_t TData;

typedef enum _ctype_t
{
    ekCTYPE_TEXT
} ctype_t;

struct _column_t
{
    ctype_t type;
    ResId head_textid;
    String *head_text;
    Font *font;
    uint32_t width;
    align_t align;
    bool_t editable;
};

struct _tdata_t
{
    Font *head_font;
    ArrSt(Column) *columns;
    uint32_t num_rows;
    uint32_t row_height;
    Listener *OnNotify;
    uint32_t mouse_ypos;
    uint32_t control_width;
    uint32_t control_height;
    uint32_t content_width;
    uint32_t selected;
    bool_t focused;
};

/*---------------------------------------------------------------------------*/

DeclSt(Column);
static const uint32_t i_LEFT_PADDING = 4;
static const uint32_t i_RIGHT_PADDING = 4;
static const uint32_t i_BOTTOM_PADDING = 4;
static const char_t* i_EMPTY_TEXT = "";

/*---------------------------------------------------------------------------*/

static TData *i_create_data(void)
{
    TData *data = heap_new0(TData);
    data->head_font = font_system(font_regular_size(), 0);
    data->columns = arrst_create(Column);
    data->selected = UINT32_MAX;
    return data;
}

/*---------------------------------------------------------------------------*/

static void i_remove_column(Column *column)
{
    str_destroy(&column->head_text);
    font_destroy(&column->font);
}

/*---------------------------------------------------------------------------*/

static void i_destroy_data(TData **data)
{
    cassert_no_null(data);
    cassert_no_null(*data);
    font_destroy(&(*data)->head_font);
    listener_destroy(&(*data)->OnNotify);
    arrst_destroy(&(*data)->columns, i_remove_column, Column);
    heap_delete(data, TData);
}

/*---------------------------------------------------------------------------*/

static void i_cell_data(TableView *view, const TData *data, const uint32_t col_id, const uint32_t row_id, EvTbCell *cell)
{
    cassert_no_null(data);
    cassert_no_null(cell);
    cell->text = i_EMPTY_TEXT;

    if (data->OnNotify != NULL)
    {
        EvTbPos pos;
        pos.col = col_id;
        pos.row = row_id;
        listener_event(data->OnNotify, ekEVTBLCELL, view, &pos, cell, TableView, EvTbPos, EvTbCell); 
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_cell(const EvTbCell *cell, DCtx *ctx, const Column *col, const uint32_t x, const uint32_t y, cstate_t state)
{
    cassert_no_null(col);
    switch (col->type) {
    case ekCTYPE_TEXT:
        draw_font(ctx, col->font);
        drawctrl_text(ctx, cell->text, x, y, state);
        break;

    cassert_default(); 
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnDraw(TableView *view, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    TData *data = view_get_data((View*)view, TData);
    uint32_t nc = 0, nr = 0;
    cassert_no_null(data);

    nc = arrst_size(data->columns, Column);
    nr = data->num_rows;
    drawctrl_clear(p->ctx);

    if (nc > 0 && nr > 0)
    {
        uint32_t stx = (uint32_t)p->x;
        uint32_t sty = (uint32_t)p->y;
        uint32_t strow = sty / data->row_height;
        uint32_t edrow = min_u32(nr, strow + ((uint32_t)p->height / data->row_height) + 2);
        uint32_t mouse_row = data->mouse_ypos != UINT32_MAX ? (data->mouse_ypos / data->row_height) : UINT32_MAX;
        uint32_t y = strow * data->row_height;
        uint32_t i;

        for (i = strow; i < edrow; ++i)
        {               
            cstate_t state = data->focused == TRUE ? ekCSTATE_NORMAL : ekCSTATE_BKNORMAL;
            uint32_t x = i_LEFT_PADDING;
            EvTbCell cell;

            if (data->selected == i)
            {
                state = data->focused == TRUE ? ekCSTATE_PRESSED : ekCSTATE_BKPRESSED;
                drawctrl_fill(p->ctx, 0, y, data->content_width, data->row_height, state);
            }
            else if (i == mouse_row)
            {
                state = data->focused == TRUE ? ekCSTATE_HOT : ekCSTATE_BKHOT;
                drawctrl_fill(p->ctx, 0, y, data->content_width, data->row_height, state);
            }

            arrst_foreach(col, data->columns, Column)
                uint32_t col_st = x;
            
                x += col->width;

                // The column is completely to the left of the visible area
                if (x < stx)
                    continue;

                // The column is completely to the right of the visible area
                if (col_st > stx + data->control_width)
                    continue;

                // The column is visible
                i_cell_data(view, data, col_i, i, &cell);
                i_draw_cell(&cell, p->ctx, col, col_st, y, state);

            arrst_end();

            y += data->row_height;
        }
    }

    // TODO: Draw header

}

/*---------------------------------------------------------------------------*/

static uint32_t i_text_height(const Column *col)
{
    cassert_no_null(col);
    switch(col->type) {
    case ekCTYPE_TEXT:
    {
        uint32_t fheight = (uint32_t)bmath_ceilf(font_size(col->font));
        uint32_t cheight = (uint32_t)bmath_ceilf(font_height(col->font));
        uint32_t height = cheight;

        if ((height - fheight) % 2 == 0)
            height += 1;

        height += drawctrl_row_padding(NULL);
        return height;
    }

    cassert_default();
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

static void i_row_height(TData *data)
{
    data->row_height = 0;
    cassert_no_null(data);
    arrst_foreach(col, data->columns, Column)
        uint32_t height = i_text_height(col);

        // TODO: Calculate height of other cell elements (images, checks, etc)

        if (height > data->row_height)
            data->row_height = height;

    arrst_end();
}

/*---------------------------------------------------------------------------*/

static void i_document_size(TableView *view, TData *data)
{
    uint32_t twidth = 0;
    uint32_t theight = 0;
    real32_t scroll_height = 0;
    uint32_t n = 0;
    cassert_no_null(data);

    n = data->num_rows;

    arrst_foreach(col, data->columns, Column)
        twidth += col->width;
    arrst_end();

    twidth += i_LEFT_PADDING + i_RIGHT_PADDING;
    
	if (twidth < data->control_width)
	{
		twidth = data->control_width;
	}

    theight = data->row_height * n + i_BOTTOM_PADDING;
    view_scroll_size((View*)view, NULL, &scroll_height);
    theight += (uint32_t)scroll_height;

    if (theight < data->control_height)
        theight = data->control_height;
    
    data->content_width = twidth;
    view_content_size((View*)view, s2df((real32_t)twidth, (real32_t)theight), s2df(10, (real32_t)data->row_height));
}

/*---------------------------------------------------------------------------*/

static void i_OnSize(TableView *view, Event *e)
{
    TData *data = view_get_data((View*)view, TData);
    const EvSize *p = event_params(e, EvSize);
    data->control_width = (uint32_t)p->width;
    data->control_height = (uint32_t)p->height;
    i_document_size(view, data);
}

/*---------------------------------------------------------------------------*/

static void i_OnMove(TableView *view, Event *e)
{
    TData *data = view_get_data((View*)view, TData);
    const EvMouse *p = event_params(e, EvMouse);
    uint32_t y = (uint32_t)p->y;
    data->mouse_ypos = y;

    if (data->num_rows > 0)
        view_update((View*)view);
}

/*---------------------------------------------------------------------------*/

static void i_OnExit(TableView *view, Event *e)
{
    TData *data = view_get_data((View*)view, TData);
    data->mouse_ypos = UINT32_MAX;
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnFocus(TableView *view, Event *e)
{
    TData *data = view_get_data((View*)view, TData);
    const bool_t *p = event_params(e, bool_t);
    data->focused = *p;
}

/*---------------------------------------------------------------------------*/

static void i_OnDown(TableView *view, Event *e)
{
    TData *data = view_get_data((View*)view, TData);
    const EvMouse *p = event_params(e, EvMouse);
    uint32_t n = data->num_rows;

    if (n > 0 && p->button == ekMLEFT)
    {
        uint32_t y = (uint32_t)p->y;
        uint32_t sel = y / data->row_height;

        if (sel >= n)
            sel = UINT32_MAX;

        if (sel != data->selected)
        {
            data->selected = sel;
            view_update((View*)view);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnUp(TableView *view, Event *e)
{
    unref(view);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_update_sel_top(TableView *view, TData *data, const uint32_t scroll_y)
{
    if (scroll_y > 0)
    {
        uint32_t ypos = data->selected * data->row_height;
        if (scroll_y > ypos)
            view_scroll_y((View*)view, (real32_t)ypos);
    }

    view_update((View*)view);
}

/*---------------------------------------------------------------------------*/

static void i_update_sel_bottom(TableView *view, TData *data, const uint32_t scroll_y)
{
    uint32_t ypos = (data->selected + 1) * data->row_height + i_BOTTOM_PADDING;
    real32_t scroll_height = 0;

    view_scroll_size((View*)view, NULL, &scroll_height);
    if (scroll_y + data->control_height - scroll_height < ypos)
        view_scroll_y((View*)view, (real32_t)ypos - (real32_t)data->control_height + scroll_height);

    view_update((View*)view);
}

/*---------------------------------------------------------------------------*/

static void i_OnKeyDown(TableView *view, Event *e)
{
    TData *data = view_get_data((View*)view, TData);
    const EvKey *p = event_params(e, EvKey);
    uint32_t n = data->num_rows;

    if (n > 0)
    {
        uint32_t scroll_x, scroll_y;

        {
            V2Df pos;
            view_viewport((View*)view, &pos, NULL);
            scroll_x = (uint32_t)pos.x;
            scroll_y = (uint32_t)pos.y;
        }

        if (p->key == ekKEY_UP)
        {
            bool_t update = FALSE;
            if (data->selected == UINT32_MAX)
            {
                data->selected = 0;
                update = TRUE;
            }
            else if (data->selected > 0)
            {
                data->selected -= 1;
                update = TRUE;
            }

            if (update == TRUE)
                i_update_sel_top(view, data, scroll_y);
        }
        else if (p->key == ekKEY_DOWN)
        {
            bool_t update = FALSE;
            if (data->selected == UINT32_MAX)
            {
                data->selected = n - 1;
                update = TRUE;
            }
            else if (data->selected < n - 1)
            {
                data->selected += 1;
                update = TRUE;
            }

            if (update == TRUE)
                i_update_sel_bottom(view, data, scroll_y);
        }
        else if (p->key == ekKEY_HOME)
        {
            if (data->selected != 0 || scroll_y > 0)
            {
                data->selected = 0;
                i_update_sel_top(view, data, scroll_y);
            }
        }
        else if (p->key == ekKEY_END)
        {
            if (data->selected != n - 1)
            {
                data->selected = n - 1;
                i_update_sel_bottom(view, data, scroll_y);
            }
        }
        else if (p->key == ekKEY_PAGEUP)
        {
            if (data->selected != 0)
            {
                if (data->selected == UINT32_MAX)
                {
                    data->selected = 0;
                }
                else
                {
                    uint32_t psize = data->control_height / data->row_height;
                    if (data->selected > psize)
                        data->selected -= psize;
                    else
                        data->selected = 0;
                }

                i_update_sel_top(view, data, scroll_y);
            }
        }
        else if (p->key == ekKEY_PAGEDOWN)
        {
            if (data->selected != n - 1)
            {
                if (data->selected == UINT32_MAX)
                {
                    data->selected = n - 1;
                }
                else
                {
                    uint32_t psize = data->control_height / data->row_height;
                    if (data->selected + psize < n - 1)
                        data->selected += psize;
                    else
                        data->selected = n - 1;
                }

                i_update_sel_bottom(view, data, scroll_y);
            }
        }
        else if (p->key == ekKEY_LEFT)
        {
            if (data->content_width > data->control_width)
            {
                if (scroll_x > 0)
                {
                    view_scroll_x((View*)view, (real32_t)(scroll_x > 10 ? scroll_x - 10 : 0));
                    view_update((View*)view);                    
                }
            }
        }
        else if (p->key == ekKEY_RIGHT)
        {
            real32_t scroll_width = 0;
            view_scroll_size((View*)view, &scroll_width, NULL);
            if (data->content_width > data->control_width - scroll_width)
            {
                if (scroll_x < data->content_width - data->control_width + scroll_width)
                {
                    view_scroll_x((View*)view, (real32_t)(scroll_x + 10));
                    view_update((View*)view);                    
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnKeyUp(TableView *view, Event *e)
{
    unref(view);
    unref(e);
}

/*---------------------------------------------------------------------------*/

TableView *tableview_create(void)
{
    View *view = _view_create(ekHSCROLL | ekVSCROLL | ekBORDER | ekCONTROL | ekNOERASE);
    TData *data = i_create_data();
    view_data(view, &data, i_destroy_data, TData);
    view_OnDraw(view, listener((TableView*)view, i_OnDraw, TableView));
    view_OnSize(view, listener((TableView*)view, i_OnSize, TableView));
    view_OnMove(view, listener((TableView*)view, i_OnMove, TableView));
    view_OnExit(view, listener((TableView*)view, i_OnExit, TableView));
    view_OnFocus(view, listener((TableView*)view, i_OnFocus, TableView));
    view_OnDown(view, listener((TableView*)view, i_OnDown, TableView));
    view_OnUp(view, listener((TableView*)view, i_OnUp, TableView));
    view_OnKeyDown(view, listener((TableView*)view, i_OnKeyDown, TableView));
    view_OnKeyUp(view, listener((TableView*)view, i_OnKeyUp, TableView));
    _view_set_subtype(view, "TableView");
    view_size(view, s2df(256, 128));
    i_document_size((TableView*)view, view_get_data(view, TData));
    return (TableView*)view;
}

/*---------------------------------------------------------------------------*/

void tableview_OnNotify(TableView *view, Listener *listener)
{
    TData *data = view_get_data((View*)view, TData);
    cassert_no_null(data);
    listener_update(&data->OnNotify, listener);
}

/*---------------------------------------------------------------------------*/

void tableview_size(TableView *view, S2Df size)
{
    view_size((View*)view, size);
}

/*---------------------------------------------------------------------------*/

uint32_t tableview_new_column_text(TableView *view)
{
    TData *data = view_get_data((View*)view, TData);
    uint32_t n = 0;
    Column *column = NULL;
    cassert_no_null(data);
    n = arrst_size(data->columns, Column);
    column = arrst_new0(data->columns, Column);
    column->type = ekCTYPE_TEXT;
    column->head_text = str_printf("Column %d", n);
    column->font = font_system(font_regular_size(), 0);
    column->width = 150;
    column->align = ekCENTER;
    column->editable = FALSE;
    i_row_height(data);
    i_document_size(view, data);
    view_update((View*)view);
    return n;
}

/*---------------------------------------------------------------------------*/

void tableview_column_width(TableView *view, const uint32_t column_id, const real32_t width)
{
    TData *data = view_get_data((View*)view, TData);
    Column *column = NULL;
    cassert_no_null(data);
    column = arrst_get(data->columns, column_id, Column);
    column->width = (uint32_t)width;
    i_document_size(view, data);
    view_update((View*)view);
}

/*---------------------------------------------------------------------------*/

void tableview_header_font(TableView *view, const Font *font)
{
    TData *data = view_get_data((View*)view, TData);
    cassert_no_null(data);
    _gui_update_font(&data->head_font, NULL, font);
    view_update((View*)view);
}

/*---------------------------------------------------------------------------*/

void tableview_header_title(TableView *view, const uint32_t column_id, const char_t *text)
{
    TData *data = view_get_data((View*)view, TData);
    Column *column = NULL;
    const char_t *ltext = NULL;
    cassert_no_null(data);
    column = arrst_get(data->columns, column_id, Column);
    ltext = _gui_respack_text(text, &column->head_textid);
    str_upd(&column->head_text, ltext);
    view_update((View*)view);
}

/*---------------------------------------------------------------------------*/

void tableview_header_align(TableView *view, const uint32_t column_id, const align_t align)
{
    TData *data = view_get_data((View*)view, TData);
    Column *column = NULL;
    cassert_no_null(data);
    column = arrst_get(data->columns, column_id, Column);
    column->align = align;
    view_update((View*)view);
}

/*---------------------------------------------------------------------------*/

static void i_num_rows(TableView *view, TData *data)
{
    cassert_no_null(data);
    data->num_rows = 0;
    if (data->OnNotify != NULL)
    {
        listener_event(data->OnNotify, ekEVTBLNROWS, view, NULL, &data->num_rows, TableView, void, uint32_t);
    }
}

/*---------------------------------------------------------------------------*/

void tableview_data_update(TableView *view)
{
    TData *data = view_get_data((View*)view, TData);
    cassert_no_null(data);
    i_num_rows(view, data);
    view_update((View*)view);
}
