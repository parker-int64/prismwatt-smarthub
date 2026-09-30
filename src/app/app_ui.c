#include "app_ui.h"

#include <string.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app_ui, LOG_LEVEL_INF);

#define UI_WIDTH  128
#define UI_HEIGHT 160

#define FONT_COLS 5
#define FONT_ROWS 7

/* RGB565 colors */
#define UI_BLACK 0x0000U
#define UI_WHITE 0xFFFFU

/*
 * Minimal 5x7 bitmap font. Each glyph is 5 bytes; one byte per column,
 * bit 0 = top row, bit 4 = bottom row (bits 5..7 unused). Only the glyphs
 * needed by the demo string below are provided; extend as required.
 *
 *   'H'  #...#     'W'  #...#     'd'  ....#     'e'  .....
 *        #...#          #...#          ....#          .###.
 *        #...#          #...#          ....#          #...#
 *        #####          #...#          .###.          #####
 *        #...#          #.#.#          #...#          #....
 *        #...#          .#.#.          #...#          .###.
 *        #...#          .....          .###.          .....
 *
 *   'l'  #....     'o'  .....     'r'  .....
 *        #....          .###.          .....
 *        #....          #...#          .####
 *        #....          #...#          #...#
 *        #....          #...#          #....
 *        #....          .###.          #....
 *        #....          .....          #....
 */
enum {
	GLYPH_SPACE = 0,
	GLYPH_H,
	GLYPH_W,
	GLYPH_D,
	GLYPH_E,
	GLYPH_L,
	GLYPH_O,
	GLYPH_R,
	GLYPH_COUNT,
};

static const uint8_t font_5x7[GLYPH_COUNT][FONT_COLS] = {
	[GLYPH_SPACE] = {0x00, 0x00, 0x00, 0x00, 0x00},
	[GLYPH_H]     = {0x7F, 0x08, 0x08, 0x08, 0x7F},
	[GLYPH_W]     = {0x1F, 0x20, 0x30, 0x20, 0x1F},
	[GLYPH_D]     = {0x30, 0x48, 0x48, 0x48, 0x7F},
	[GLYPH_E]     = {0x1C, 0x2A, 0x2A, 0x2A, 0x2E},
	[GLYPH_L]     = {0x7F, 0x00, 0x00, 0x00, 0x00},
	[GLYPH_O]     = {0x1C, 0x22, 0x22, 0x22, 0x3E},
	[GLYPH_R]     = {0x78, 0x04, 0x04, 0x04, 0x0C},
};

static uint8_t glyph_index(char c)
{
	switch (c) {
	case ' ':
		return GLYPH_SPACE;
	case 'H':
		return GLYPH_H;
	case 'W':
		return GLYPH_W;
	case 'd':
		return GLYPH_D;
	case 'e':
		return GLYPH_E;
	case 'l':
		return GLYPH_L;
	case 'o':
		return GLYPH_O;
	case 'r':
		return GLYPH_R;
	default:
		return GLYPH_SPACE;
	}
}

int app_ui_init(void)
{
	const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	struct display_buffer_descriptor desc;
	static uint16_t line[UI_WIDTH];
	const char *text = "Hello World";
	const uint8_t scale = 2;
	const int text_w = (int)strlen(text) * FONT_COLS * scale;
	const int text_h = FONT_ROWS * scale;
	const int x0 = (UI_WIDTH - text_w) / 2;
	const int y0 = (UI_HEIGHT - text_h) / 2;
	int ret;

	if (display == NULL || !device_is_ready(display)) {
		LOG_ERR("Display device not ready");
		return -ENODEV;
	}

	desc.buf_size = UI_WIDTH * sizeof(uint16_t);
	desc.width = UI_WIDTH;
	desc.height = 1;
	desc.pitch = UI_WIDTH;
	desc.frame_incomplete = false;

	/*
	 * Render row by row into a single line buffer: fill white, overlay any
	 * text pixels that land on this row, then push the row to the display.
	 */
	for (int y = 0; y < UI_HEIGHT; y++) {
		for (int x = 0; x < UI_WIDTH; x++) {
			line[x] = UI_WHITE;
		}

		if (y >= y0 && y < y0 + text_h) {
			int glyph_row = (y - y0) / scale;

			for (int i = 0; text[i] != '\0'; i++) {
				const uint8_t *g = font_5x7[glyph_index(text[i])];

				for (int c = 0; c < FONT_COLS; c++) {
					if (g[c] & (1U << glyph_row)) {
						int px = x0 + (i * FONT_COLS + c) * scale;

						for (int sx = 0; sx < scale; sx++) {
							line[px + sx] = UI_BLACK;
						}
					}
				}
			}
		}

		ret = display_write(display, 0, y, &desc, line);
		if (ret < 0) {
			LOG_ERR("display_write row %d failed: %d", y, ret);
			return ret;
		}
	}

	LOG_INF("UI drawn: \"%s\"", text);
	return 0;
}
