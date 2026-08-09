/**
 * @file font_asset.c
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2026-04-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <assert.h>
#include <string.h>
#include <font/font_asset.h>
#include <library/allocator/allocator.h>
#include <library/core/core.h>
#include <library/type_registry/type_registry.h>


void
font_asset_def(void *ptr)
{
  assert(ptr);

  {
    font_asset_t *font = (font_asset_t *)ptr;
    memset(font, 0, sizeof(font_asset_t));
  }
}

uint32_t
font_asset_is_def(const void *ptr)
{
  assert(ptr);

  {
    const font_asset_t *font = (const font_asset_t *)ptr;
    font_asset_t def;
    font_asset_def(&def);
    return !memcmp(font, &def, sizeof(font_asset_t));
  }
}

void
font_asset_serialize(
  const void *src,
  binary_stream_t *stream)
{
  assert(src && stream);

  {
    const font_asset_t *font = (const font_asset_t *)src;
    asset_ref_serialize(&font->texture_ref, stream);
    binary_stream_write(stream, &font->texture_width, sizeof(uint32_t));
    binary_stream_write(stream, &font->texture_height, sizeof(uint32_t));
    binary_stream_write(stream, &font->cell_width, sizeof(uint32_t));
    binary_stream_write(stream, &font->cell_height, sizeof(uint32_t));
    binary_stream_write(stream, &font->font_width, sizeof(uint32_t));
    binary_stream_write(stream, &font->font_height, sizeof(uint32_t));
    binary_stream_write(stream, &font->start_char, sizeof(uint32_t));
    binary_stream_write(
      stream,
      font->glyphs,
      sizeof(glyph_data_t) * FONT_GLYPH_COUNT);
    binary_stream_write(
      stream,
      font->bounds,
      sizeof(glyph_bounds_t) * FONT_GLYPH_COUNT);
  }
}

void
font_asset_deserialize(
  void *dst,
  const allocator_t *allocator,
  binary_stream_t *stream)
{
  assert(dst && allocator && stream);

  {
    size_t s_32 = sizeof(uint32_t);
    size_t s_gd = sizeof(glyph_data_t) * FONT_GLYPH_COUNT;
    size_t s_gb = sizeof(glyph_bounds_t) * FONT_GLYPH_COUNT;
    font_asset_t *font = (font_asset_t *)dst;
    asset_ref_deserialize(&font->texture_ref, allocator, stream);
    binary_stream_read(stream, (uint8_t *)&font->texture_width, s_32, s_32);
    binary_stream_read(stream, (uint8_t *)&font->texture_height, s_32, s_32);
    binary_stream_read(stream, (uint8_t *)&font->cell_width, s_32, s_32);
    binary_stream_read(stream, (uint8_t *)&font->cell_height, s_32, s_32);
    binary_stream_read(stream, (uint8_t *)&font->font_width, s_32, s_32);
    binary_stream_read(stream, (uint8_t *)&font->font_height, s_32, s_32);
    binary_stream_read(stream, (uint8_t *)&font->start_char, s_32, s_32);
    binary_stream_read(stream, (uint8_t *)font->glyphs, s_gd, s_gd);
    binary_stream_read(stream, (uint8_t *)font->bounds, s_gb, s_gb);
  }
}

size_t
font_asset_type_size(void)
{
  return sizeof(font_asset_t);
}

uint32_t
font_asset_owns_alloc(void)
{
  return 0;
}

const allocator_t *
font_asset_get_alloc(const void *ptr)
{
  return NULL;
}

void
font_asset_cleanup(
  void *ptr,
  const allocator_t *allocator)
{
  assert(ptr && !font_asset_is_def(ptr));
  assert(allocator);

  {
    font_asset_t *font = (font_asset_t *)ptr;
    asset_ref_cleanup(&font->texture_ref, allocator);
    font_asset_def(font);
  }
}

const char *
font_asset_get_dir(void)
{
  static const char *directory = "fonts";
  return directory;
}

static
void
font_asset_loader(
  void **ptr_addr,
  const asset_ref_t *asset_ref,
  const allocator_t *allocator)
{
  assert(ptr_addr && asset_ref && allocator);
  asset_ref_sanity_check(asset_ref, get_type_id(font_asset_t));

  {
    font_asset_t **ptr = (font_asset_t **)ptr_addr;
    font_asset_t *asset_ptr = NULL;
    binary_stream_t *stream = binary_stream_from_file(
      asset_ref->path.str, allocator);
    *ptr = allocator->mem_alloc(sizeof(font_asset_t));
    asset_ptr = *ptr;
    font_asset_def(asset_ptr);
    font_asset_deserialize(asset_ptr, allocator, stream);
    binary_stream_cleanup(stream);
    allocator->mem_free(stream);
  }
}

static
void
font_asset_deloader(
  void **ptr_addr,
  const asset_ref_t *asset_ref,
  const allocator_t *allocator)
{
  assert(ptr_addr && asset_ref && allocator);
  asset_ref_sanity_check(asset_ref, get_type_id(font_asset_t));

  {
    font_asset_t **ptr = (font_asset_t **)ptr_addr;
    font_asset_t *asset_ptr = *ptr;
    font_asset_cleanup(asset_ptr, allocator);
    allocator->mem_free(asset_ptr);
    *ptr = NULL;
  }
}

loader_t
font_asset_get_loader(void)
{
  return font_asset_loader;
}

deloader_t
font_asset_get_deloader(void)
{
  return font_asset_deloader;
}

uint32_t
font_asset_type_asset_count(const void *src)
{
  assert(src);
  return 1;
}

void
font_asset_type_get_assets(const void *src, const asset_ref_t *refs[])
{
  assert(src);

  {
    const font_asset_t *font = src;
    refs[0] = &font->texture_ref;
  }
}

uint32_t
font_asset_is_asset_type(void)
{
  return 1;
}

////////////////////////////////////////////////////////////////////////////////
uint32_t
font_asset_glyph_count(const font_asset_t *font)
{
  return
    (font->texture_width / font->cell_width) *
    (font->texture_height / font->cell_height);
}

uint32_t
font_asset_column_count(const font_asset_t *font)
{
  return font->texture_width / font->cell_width;
}

uint32_t
font_asset_row_count(const font_asset_t *font)
{
  return font->texture_height / font->cell_height;
}

void
query_font_asset_glyph_bounds(
  const font_asset_t *font,
  const char c,
  glyph_bounds_t *out)
{
  assert(font != NULL);

  {
    char glyph = font_asset_has_glyph(font, c) ? c : '$';
    memcpy(out, font->bounds + glyph, sizeof(glyph_bounds_t));
  }
}

uint32_t
font_asset_has_glyph(
  const font_asset_t *font,
  const char c)
{
 uint32_t total = font_asset_glyph_count(font);
 uint32_t cui = (uint32_t)c;
 return cui >= font->start_char && cui < (font->start_char + total);
}

////////////////////////////////////////////////////////////////////////////////
INITIALIZER(register_font_asset_t)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = font_asset_def;
  vtable.fn_is_def = font_asset_is_def;
  vtable.fn_serialize = font_asset_serialize;
  vtable.fn_deserialize = font_asset_deserialize;
  vtable.fn_type_size = font_asset_type_size;
  vtable.fn_owns_alloc = font_asset_owns_alloc;
  vtable.fn_get_alloc = font_asset_get_alloc;
  vtable.fn_cleanup = font_asset_cleanup;
  vtable.fn_get_dir = font_asset_get_dir;
  vtable.fn_get_loader = font_asset_get_loader;
  vtable.fn_get_deloader = font_asset_get_deloader;
  vtable.fn_type_asset_count = font_asset_type_asset_count;
  vtable.fn_type_get_assets = font_asset_type_get_assets;
  vtable.fn_is_asset_type = font_asset_is_asset_type;
  register_type(get_type_id(font_asset_t), &vtable);
}