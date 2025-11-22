#include <freetype2/freetype/fttypes.h>
#define ALLOCATOR_IMPLEMENTATION
#include "../allocator.h"
#define CORE_OS_IMPLEMENTATION
#include "../os.h"
#include <stdint.h>

typedef struct {
  V2 size;
  V2 bearing;
  uint32_t advance;
  uint8_t *buffer;
} OS_Character;

#ifdef __linux__
#include <ft2build.h>
#include <stdio.h>
#include FT_FREETYPE_H
#include <freetype2/freetype/ftimage.h>

const char *freeTypeError(FT_Error e);

#define OS_CHAR_COUNT 128

OS_Character *fontMap = 0;

Bool loadFont(char *fontPath, int32_t fontSize) {
  if (fontMap != 0) {
    for (int i = 0; i < OS_CHAR_COUNT; i++) {
      FreeMemory(fontMap[i].buffer);
    }
  } else {
    fontMap = AllocMemory(OS_CHAR_COUNT * sizeof(OS_Character));
  }
  FT_Error ftError;

  FT_Library ft;
  if ((ftError = FT_Init_FreeType(&ft))) {
    const char *err = freeTypeError(ftError);
    printf("Could not init freetype library: %s\n", err);
    return false;
  }

  FT_Face fontFace;
  if ((ftError = FT_New_Face(ft, fontPath, 0, &fontFace))) {
    const char *err = freeTypeError(ftError);
    printf("Could not init freetype face from file at '%s': %s\n", fontPath,
           err);
    return false;
  }

  if (FT_Set_Pixel_Sizes(fontFace, 0, fontSize)) {
    printf("Could not set pixel sizes\n");
    return false;
  }

  for (uint8_t c = 0; c < OS_CHAR_COUNT; c++) {
    if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER)) {
      printf("Could not load char %c(%d)\n", c, c);
      continue;
    }

    FT_Bitmap charBitmap = fontFace->glyph->bitmap;

    OS_Character osChar = {
        .size = {.x = charBitmap.width, .y = charBitmap.rows},
        .bearing = {.x = fontFace->glyph->bitmap_left,
                    fontFace->glyph->bitmap_top},
        .advance = fontFace->glyph->advance.x,
        .buffer =
            AllocMemory(charBitmap.width * charBitmap.rows * sizeof(uint8_t)),
    };

    for (uint64_t i = 0; i < charBitmap.width * charBitmap.rows; i++) {
      osChar.buffer[i] = charBitmap.buffer[i];
    }

    fontMap[c] = osChar;
  }

  FT_Done_Face(fontFace);
  FT_Done_FreeType(ft);

  return true;
}

int32_t textWidth(char *str, int32_t len) {
  int32_t w = 0;

  for (int i = 0; i < len; i++) {
    OS_Character c = fontMap[str[i]];
    w += (c.advance >> 6);
  }

  return w;
}

const char *freeTypeError(FT_Error e) {
  static const char *errMap[0xBB] = {
      [0x00] = "no error",
      [0x01] = "cannot open resource",
      [0x02] = "unknown file format",
      [0x03] = "broken file",
      [0x04] = "invalid FreeType version",
      [0x05] = "module version is too low",
      [0x06] = "invalid argument",
      [0x07] = "unimplemented feature",
      [0x08] = "broken table",
      [0x09] = "broken offset within table",
      [0x0A] = "array allocation size too large",
      [0x0B] = "missing module",
      [0x0C] = "missing property",

      /* glyph/character errors */

      [0x10] = "invalid glyph index",
      [0x11] = "invalid character code",
      [0x12] = "unsupported glyph image format",
      [0x13] = "cannot render this glyph format",
      [0x14] = "invalid outline",
      [0x15] = "invalid composite glyph",
      [0x16] = "too many hints",
      [0x17] = "invalid pixel size",
      [0x18] = "invalid SVG document",

      /* handle errors */

      [0x20] = "invalid object handle",
      [0x21] = "invalid library handle",
      [0x22] = "invalid module handle",
      [0x23] = "invalid face handle",
      [0x24] = "invalid size handle",
      [0x25] = "invalid glyph slot handle",
      [0x26] = "invalid charmap handle",
      [0x27] = "invalid cache manager handle",
      [0x28] = "invalid stream handle",

      /* driver errors */

      [0x30] = "too many modules",
      [0x31] = "too many extensions",

      /* memory errors */

      [0x40] = "out of memory",
      [0x41] = "unlisted object",

      /* stream errors */

      [0x51] = "cannot open stream",
      [0x52] = "invalid stream seek",
      [0x53] = "invalid stream skip",
      [0x54] = "invalid stream read",
      [0x55] = "invalid stream operation",
      [0x56] = "invalid frame operation",
      [0x57] = "nested frame access",
      [0x58] = "invalid frame read",

      /* raster errors */

      [0x60] = "raster uninitialized",
      [0x61] = "raster corrupted",
      [0x62] = "raster overflow",
      [0x63] = "negative height while rastering",

      /* cache errors */

      [0x70] = "too many registered caches",

      /* TrueType and SFNT errors */

      [0x80] = "invalid opcode",
      [0x81] = "too few arguments",
      [0x82] = "stack overflow",
      [0x83] = "code overflow",
      [0x84] = "bad argument",
      [0x85] = "division by zero",
      [0x86] = "invalid reference",
      [0x87] = "found debug opcode",
      [0x88] = "found ENDF opcode in execution stream",
      [0x89] = "nested DEFS",
      [0x8A] = "invalid code range",
      [0x8B] = "execution context too long",
      [0x8C] = "too many function definitions",
      [0x8D] = "too many instruction definitions",
      [0x8E] = "SFNT font table missing",
      [0x8F] = "horizontal header (hhea) table missing",
      [0x90] = "locations (loca) table missing",
      [0x91] = "name table missing",
      [0x92] = "character map (cmap) table missing",
      [0x93] = "horizontal metrics (hmtx) table missing",
      [0x94] = "PostScript (post) table missing",
      [0x95] = "invalid horizontal metrics",
      [0x96] = "invalid character map (cmap) format",
      [0x97] = "invalid ppem value",
      [0x98] = "invalid vertical metrics",
      [0x99] = "could not find context",
      [0x9A] = "invalid PostScript (post) table format",
      [0x9B] = "invalid PostScript (post) table",
      [0x9C] = "found FDEF or IDEF opcode in glyf bytecode",
      [0x9D] = "missing bitmap in strike",
      [0x9E] = "SVG hooks have not been set",

      /* CFF, CID, and Type 1 errors */

      [0xA0] = "opcode syntax error",
      [0xA1] = "argument stack underflow",
      [0xA2] = "ignore",
      [0xA3] = "no Unicode glyph name found",
      [0xA4] = "glyph too big for hinting",

      /* BDF errors */

      [0xB0] = "`STARTFONT' field missing",
      [0xB1] = "`FONT' field missing",
      [0xB2] = "`SIZE' field missing",
      [0xB3] = "`FONTBOUNDINGBOX' field missing",
      [0xB4] = "`CHARS' field missing",
      [0xB5] = "`STARTCHAR' field missing",
      [0xB6] = "`ENCODING' field missing",
      [0xB7] = "`BBX' field missing",
      [0xB8] = "`BBX' too big",
      [0xB9] = "Font header corrupted or missing fields",
      [0xBA] = "Font glyphs corrupted or missing fields",
  };

  return errMap[e];
}

#endif /* ifdef __linux__ */
