#include "ui_font.h"

#include <string.h>

const ui_font_glyph_t *ui_font_find_glyph(const ui_font_t *font, char c)
{
	uint8_t index = ((unsigned char)c < 128)
			? font->lookup[(unsigned char)c] : UI_FONT_MISSING;

	if (index == UI_FONT_MISSING) {
		index = font->lookup[' '];
	}

	return &font->glyphs[index];
}

int ui_font_text_width(const ui_font_t *font, const char *str, int len)
{
	uint32_t width = 0;

	if (len < 0) {
		len = (int)strlen(str);
	}

	for (int i = 0; i < len; i++) {
		width += ui_font_find_glyph(font, str[i])->advance;
	}

	return (int)((width + 8) >> 4);
}

int ui_font_text_height(const ui_font_t *font)
{
	return font->line_height;
}
