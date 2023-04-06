/*******************************************************************************
 * GIFDEC Wrapper Class
 * 
 * Rewrite from: https://github.com/BasementCat/arduino-tft-gif
 ******************************************************************************/
#ifndef _GIFCLASS_H_
#define _GIFCLASS_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif

#ifndef MAX
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#define GIF_BUF_SIZE 1024

struct GifRGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    inline GifRGB() __attribute__((always_inline))
    :r(0),g(0),b(0)
    {

    }

    inline GifRGB( uint32_t colorcode)  __attribute__((always_inline))
    : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b((colorcode >> 0) & 0xFF)
    {

    }
    
    inline uint32_t getColor () const
    {
        uint32_t color = 0;
        color = (color + r)<<8;
        color = (color + g)<<8;
        color = (color + b);
        return color;
    }

    inline GifRGB& operator= (const uint32_t colorcode) __attribute__((always_inline))
    {
        r = (colorcode >> 16) & 0xFF;
        g = (colorcode >>  8) & 0xFF;
        b = (colorcode >>  0) & 0xFF;
        return *this;
    }
};

typedef struct gd_Palette
{
    uint8_t size;
    GifRGB colors[256];
} gd_Palette;

typedef struct gd_GCE
{
    uint16_t delay;
    uint8_t tindex;
    uint8_t disposal;
    uint8_t input;
    uint8_t transparency;
} gd_GCE;

typedef struct gd_Entry
{
    int32_t length;
    uint16_t prefix;
    uint8_t suffix;
} gd_Entry;

typedef struct gd_Table
{
    int16_t bulk;
    int16_t nentries;
    gd_Entry *entries;
} gd_Table;

typedef struct gd_GIF
{
    const uint8_t *fd;
    uint32_t size;
    off_t anim_start;
    uint16_t width, height;
    uint16_t depth;
    uint16_t loop_count;
    gd_GCE gce;
    gd_Palette *palette;
    gd_Palette lct, gct;
    void (*plain_text)(
        struct gd_GIF *gif, uint16_t tx, uint16_t ty,
        uint16_t tw, uint16_t th, uint8_t cw, uint8_t ch,
        uint8_t fg, uint8_t bg);
    void (*comment)(struct gd_GIF *gif);
    void (*application)(struct gd_GIF *gif, char id[8], char auth[3]);
    uint16_t fx, fy, fw, fh;
    uint8_t bgindex;
    gd_Table *table;
} gd_GIF;

class GifClass
{
public:
    
    bool decode(const uint8_t *fd,uint32_t size);
    
    GifClass();

    virtual ~GifClass();
    /* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
    int32_t gd_get_frame(gd_GIF *gif, uint8_t *frame);

    void gd_rewind(gd_GIF *gif);

    void gd_close_gif(gd_GIF *gif);

    uint32_t getColor(uint8_t x,uint8_t y) const;

    uint16_t getDuration() const;

    bool renderFrame(bool loop);

    uint8_t gifwidth();

    uint8_t gifheight();

private:
    void gif_freeDecoder();

    bool gif_buf_seek(const uint8_t *fd, int16_t len);

    int16_t gif_buf_read(const uint8_t *fd, uint8_t *dest, int16_t len);

    uint8_t gif_buf_read(const uint8_t *fd);

    uint16_t gif_buf_read16(const uint8_t *fd);

    void read_palette(const uint8_t *fd, gd_Palette *dest, int32_t num_colors);

    void discard_sub_blocks(gd_GIF *gif);

    void read_plain_text_ext(gd_GIF *gif);

    void read_graphic_control_ext(gd_GIF *gif);

    void read_comment_ext(gd_GIF *gif);

    void read_application_ext(gd_GIF *gif);

    void read_ext(gd_GIF *gif);

    gd_Table *new_table();

    void reset_table(gd_Table *table, uint16_t key_size);

    /* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
    int32_t add_entry(gd_Table *table, int32_t length, uint16_t prefix, uint8_t suffix);

    uint16_t get_key(gd_GIF *gif, uint16_t key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte);

    /* Compute output index of y-th input line, in frame of height h. */
    int16_t interlaced_line_index(int16_t h, int16_t y);

    /* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
    int8_t read_image_data(gd_GIF *gif, int16_t interlace, uint8_t *frame);

    /* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
    int8_t read_image(gd_GIF *gif, uint8_t *frame);


    int16_t gif_buf_last_idx, gif_buf_idx, file_pos;
    uint8_t* _screenBuffer = nullptr;
    gd_GIF *_gif = nullptr;
};

#endif /* _GIFCLASS_H_ */
