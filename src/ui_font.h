#ifndef UI_FONT_H_
#define UI_FONT_H_

#include <stdint.h>

typedef struct ui_font_glyph {
	uint16_t code;
	int8_t xoff;
	int8_t yoff;
	uint8_t w;
	uint8_t h;
	uint16_t advance; /* 1/16 pixel */
	uint16_t offset;
} ui_font_glyph_t;

typedef struct ui_font {
	uint8_t line_height;
	int8_t ascent;
	uint8_t bpp;
	uint16_t glyph_count;
	const ui_font_glyph_t *glyphs;
	const uint8_t *lookup;
	const uint8_t *bitmap;
} ui_font_t;

#define UI_FONT_MISSING 0xFF

const ui_font_glyph_t *ui_font_find_glyph(const ui_font_t *font, char c);
int ui_font_text_width(const ui_font_t *font, const char *str, int len);
int ui_font_text_height(const ui_font_t *font);

static inline uint8_t ui_font_glyph_pixel(const ui_font_glyph_t *glyph,
					  const uint8_t *bitmap,
					  int x, int y, uint8_t bpp)
{
	const uint8_t *row = bitmap + glyph->offset +
			     y * ((glyph->w * bpp + 7) / 8);

	switch (bpp) {
	case 1:
		return (row[x / 8] >> (7 - (x & 7))) & 1;
	case 2:
		return (row[x / 4] >> (6 - 2 * (x & 3))) & 3;
	case 4: {
		uint8_t value = row[x / 2];

		return (x & 1) ? (value & 0x0F) : (value >> 4);
	}
	case 8:
		return row[x];
	default:
		return 0;
	}
}

#endif /* UI_FONT_H_ */
