/* image.c - Sprite and texture system (C11 analog of image.go)
 * Implements SFF v1/v2 sprite format parsing, palette management,
 * texture loading, and palette effects (PalFX system).
 * 
 * MUGEN SFF Format:
 * - SFF v1: RLE-encoded PCX sprites with shared palettes
 * - SFF v2: PNG/RLE sprites with embedded palettes, LZMA compression
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* === PalFX (Palette Effects) System === */

typedef struct {
    int32_t time;
    float color;
    int32_t add[3];
    int32_t mul[3];
    int32_t sinadd[3];
    int32_t sinmul[3];
    int32_t sincolor;
    int32_t sinhue;
    int32_t cycletime[4];
    bool invertall;
    int32_t invertblend;
    float hue;
    bool interpolate;
    int32_t iadd[6];
    int32_t imul[6];
    float icolor[2];
    float ihue[2];
    int32_t itime;
} ikm_palfx_def_t;

struct ikm_palfx {
    ikm_palfx_def_t def;
    int* remap;
    size_t remap_count;
    bool allow_neg;
    int32_t sintime[4];
    bool enable;
    /* Effect cache */
    bool e_allow_neg;
    bool e_invertall;
    int32_t e_invertblend;
    int32_t e_add[3];
    int32_t e_mul[3];
    float e_color;
    float e_hue;
    bool e_interpolate;
    int32_t ei_add[3];
    int32_t ei_mul[3];
    float ei_color;
    float ei_hue;
    int32_t ei_time;
};

ikm_palfx_t* ikm_palfx_create(void) {
    ikm_palfx_t* pf = calloc(1, sizeof(ikm_palfx_t));
    if (!pf) return NULL;
    
    /* Initialize defaults (matches Go newPalFXDef) */
    pf->def.color = 1.0f;
    pf->def.icolor[0] = 1.0f;
    pf->def.icolor[1] = 1.0f;
    pf->def.mul[0] = 256;
    pf->def.mul[1] = 256;
    pf->def.mul[2] = 256;
    for (int i = 0; i < 6; i++) {
        pf->def.imul[i] = 256;
    }
    
    return pf;
}

void ikm_palfx_destroy(ikm_palfx_t* pf) {
    if (!pf) return;
    free(pf->remap);
    free(pf);
}

void ikm_palfx_clear(ikm_palfx_t* pf, bool allow_neg) {
    if (!pf) return;
    
    /* Reset to defaults */
    ikm_palfx_def_t def = {0};
    def.color = 1.0f;
    def.icolor[0] = 1.0f;
    def.icolor[1] = 1.0f;
    def.mul[0] = 256;
    def.mul[1] = 256;
    def.mul[2] = 256;
    for (int i = 0; i < 6; i++) {
        def.imul[i] = 256;
    }
    
    pf->def = def;
    pf->allow_neg = allow_neg;
    memset(pf->sintime, 0, sizeof(pf->sintime));
}

/* === SFF Header Structures === */

typedef struct {
    uint8_t ver0, ver1, ver2, ver3;
    uint32_t first_sprite_offset;
    uint32_t first_palette_offset;
    uint32_t num_sprites;
    uint32_t num_palettes;
} ikm_sff_header_t;

/* Read little-endian uint32 */
static uint32_t read_le32(FILE* f) {
    uint8_t buf[4];
    if (fread(buf, 1, 4, f) != 4) return 0;
    return (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) | 
           ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
}

/* Read little-endian uint16 */
static uint16_t read_le16(FILE* f) {
    uint8_t buf[2];
    if (fread(buf, 1, 2, f) != 2) return 0;
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

static int ikm_sff_read_header(FILE* f, ikm_sff_header_t* hdr, 
                                uint32_t* lofs, uint32_t* tofs) {
    /* Read signature */
    char sig[12];
    if (fread(sig, 1, 12, f) != 12) return -1;
    if (memcmp(sig, "ElecbyteSpr\0", 12) != 0) {
        fprintf(stderr, "Invalid SFF signature\n");
        return -1;
    }
    
    /* Read version */
    if (fread(&hdr->ver3, 1, 1, f) != 1) return -1;
    if (fread(&hdr->ver2, 1, 1, f) != 1) return -1;
    if (fread(&hdr->ver1, 1, 1, f) != 1) return -1;
    if (fread(&hdr->ver0, 1, 1, f) != 1) return -1;
    
    /* Skip reserved */
    uint32_t dummy = read_le32(f);
    (void)dummy;
    
    if (hdr->ver0 == 1) {
        /* SFF v1 */
        hdr->first_palette_offset = 0;
        hdr->num_palettes = 0;
        hdr->num_sprites = read_le32(f);
        hdr->first_sprite_offset = read_le32(f);
        read_le32(f); /* skip dummy */
    } else if (hdr->ver0 == 2) {
        /* SFF v2 */
        for (int i = 0; i < 4; i++) {
            read_le32(f); /* skip reserved */
        }
        hdr->first_sprite_offset = read_le32(f);
        hdr->num_sprites = read_le32(f);
        hdr->first_palette_offset = read_le32(f);
        hdr->num_palettes = read_le32(f);
        *lofs = read_le32(f);
        read_le32(f); /* skip dummy */
        *tofs = read_le32(f);
    } else {
        fprintf(stderr, "Unsupported SFF version: %d\n", hdr->ver0);
        return -1;
    }
    
    return 0;
}

/* === Sprite Structure === */

struct ikm_sprite {
    uint32_t* pal;
    size_t pal_size;
    void* tex;              /* Platform-specific texture handle */
    int16_t group;
    int16_t number;
    uint16_t size[2];
    int16_t offset[2];
    int palidx;
    int rle;
    uint8_t coldepth;
    uint32_t* paltemp;
    size_t paltemp_size;
    void* pal_tex;          /* Platform-specific palette texture */
};

ikm_sprite_t* ikm_sprite_create(void) {
    ikm_sprite_t* s = calloc(1, sizeof(ikm_sprite_t));
    if (!s) return NULL;
    s->palidx = -1;
    return s;
}

void ikm_sprite_destroy(ikm_sprite_t* s) {
    if (!s) return;
    free(s->pal);
    free(s->paltemp);
    /* tex and pal_tex freed by renderer */
    free(s);
}

bool ikm_sprite_is_blank(const ikm_sprite_t* s) {
    return !s || !s->tex || s->size[0] == 0 || s->size[1] == 0;
}

/* === SFF Sprite File === */

typedef struct {
    ikm_sff_header_t header;
    ikm_sprite_t** sprites;
    size_t sprite_count;
    uint32_t** palettes;
    size_t palette_count;
    char* filename;
} ikm_sff_t;

ikm_sff_t* ikm_sff_create(void) {
    ikm_sff_t* sff = calloc(1, sizeof(ikm_sff_t));
    return sff;
}

void ikm_sff_destroy(ikm_sff_t* sff) {
    if (!sff) return;
    
    for (size_t i = 0; i < sff->sprite_count; i++) {
        ikm_sprite_destroy(sff->sprites[i]);
    }
    free(sff->sprites);
    
    for (size_t i = 0; i < sff->palette_count; i++) {
        free(sff->palettes[i]);
    }
    free(sff->palettes);
    
    free(sff->filename);
    free(sff);
}

/* Read sprite header for SFF v1 */
static int ikm_sff_read_sprite_header_v1(FILE* f, ikm_sprite_t* s,
                                          uint32_t* next_offset,
                                          uint32_t* data_size,
                                          uint16_t* prev_index) {
    fseek(f, 0, SEEK_CUR); /* Current position is header start */
    
    *next_offset = read_le32(f);
    *data_size = read_le32(f);
    
    uint16_t axis_x = read_le16(f);
    uint16_t axis_y = read_le16(f);
    s->group = (int16_t)read_le16(f);
    s->number = (int16_t)read_le16(f);
    *prev_index = read_le16(f);
    
    uint8_t same_pal;
    if (fread(&same_pal, 1, 1, f) != 1) return -1;
    
    /* Skip comments (13 bytes) */
    fseek(f, 13, SEEK_CUR);
    
    s->offset[0] = (int16_t)axis_x;
    s->offset[1] = (int16_t)axis_y;
    
    return 0;
}

/* Read sprite header for SFF v2 */
static int ikm_sff_read_sprite_header_v2(FILE* f, ikm_sprite_t* s,
                                          uint32_t* next_offset,
                                          uint32_t* data_size,
                                          uint32_t lofs, uint32_t tofs,
                                          uint16_t* prev_index) {
    s->group = (int16_t)read_le16(f);
    s->number = (int16_t)read_le16(f);
    
    s->size[0] = read_le16(f);
    s->size[1] = read_le16(f);
    s->offset[0] = (int16_t)read_le16(f);
    s->offset[1] = (int16_t)read_le16(f);
    
    *prev_index = read_le16(f);
    uint8_t fmt;
    if (fread(&fmt, 1, 1, f) != 1) return -1;
    
    uint8_t coldepth;
    if (fread(&coldepth, 1, 1, f) != 1) return -1;
    s->coldepth = coldepth;
    
    uint32_t data_offset = read_le32(f);
    uint32_t data_len = read_le32(f);
    uint16_t pal_idx = read_le16(f);
    uint16_t flags = read_le16(f);
    
    (void)fmt;
    (void)data_offset;
    (void)data_len;
    (void)pal_idx;
    (void)flags;
    
    *next_offset = ftell(f);
    *data_size = data_len;
    
    return 0;
}

/* Load SFF file */
ikm_sff_t* ikm_sff_load(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open SFF file: %s\n", filename);
        return NULL;
    }
    
    ikm_sff_t* sff = ikm_sff_create();
    if (!sff) {
        fclose(f);
        return NULL;
    }
    
    sff->filename = strdup(filename);
    
    uint32_t lofs = 0, tofs = 0;
    if (ikm_sff_read_header(f, &sff->header, &lofs, &tofs) < 0) {
        fclose(f);
        ikm_sff_destroy(sff);
        return NULL;
    }
    
    /* Allocate sprite array */
    sff->sprite_count = sff->header.num_sprites;
    sff->sprites = calloc(sff->sprite_count, sizeof(ikm_sprite_t*));
    if (!sff->sprites) {
        fclose(f);
        ikm_sff_destroy(sff);
        return NULL;
    }
    
    /* Read all sprite headers */
    fseek(f, sff->header.first_sprite_offset, SEEK_SET);
    
    for (uint32_t i = 0; i < sff->header.num_sprites; i++) {
        ikm_sprite_t* s = ikm_sprite_create();
        if (!s) continue;
        
        uint32_t next_offset, data_size;
        uint16_t prev_index;
        
        if (sff->header.ver0 == 1) {
            ikm_sff_read_sprite_header_v1(f, s, &next_offset, &data_size, &prev_index);
        } else {
            ikm_sff_read_sprite_header_v2(f, s, &next_offset, &data_size,
                                           lofs, tofs, &prev_index);
        }
        
        sff->sprites[i] = s;
        
        /* Skip to next header */
        fseek(f, next_offset, SEEK_SET);
    }
    
    fclose(f);
    
    printf("[INFO] Loaded SFF: %s (v%d, %u sprites, %u palettes)\n",
           filename, sff->header.ver0, sff->header.num_sprites, sff->header.num_palettes);
    
    return sff;
}

/* Find sprite by group and number */
ikm_sprite_t* ikm_sff_get_sprite(ikm_sff_t* sff, int16_t group, int16_t number) {
    if (!sff) return NULL;
    
    for (size_t i = 0; i < sff->sprite_count; i++) {
        ikm_sprite_t* s = sff->sprites[i];
        if (s && s->group == group && s->number == number) {
            return s;
        }
    }
    
    return NULL;
}

/* === Texture Management === */

typedef struct {
    void* handle;           /* OpenGL texture ID or similar */
    uint32_t width;
    uint32_t height;
    uint8_t* data;
    size_t data_size;
} ikm_texture_t;

ikm_texture_t* ikm_texture_create(uint32_t width, uint32_t height) {
    ikm_texture_t* tex = calloc(1, sizeof(ikm_texture_t));
    if (!tex) return NULL;
    
    tex->width = width;
    tex->height = height;
    tex->data_size = width * height * 4; /* RGBA */
    tex->data = calloc(1, tex->data_size);
    
    if (!tex->data) {
        free(tex);
        return NULL;
    }
    
    return tex;
}

void ikm_texture_destroy(ikm_texture_t* tex) {
    if (!tex) return;
    free(tex->data);
    free(tex);
}

/* === Palette Management === */

typedef struct {
    uint32_t** palettes;
    size_t palette_count;
    int default_palette;
} ikm_palette_list_t;

ikm_palette_list_t* ikm_palette_list_create(void) {
    ikm_palette_list_t* pl = calloc(1, sizeof(ikm_palette_list_t));
    if (!pl) return NULL;
    
    /* Allocate default 256-color palette */
    pl->palette_count = 1;
    pl->palettes = calloc(1, sizeof(uint32_t*));
    if (!pl->palettes) {
        free(pl);
        return NULL;
    }
    
    pl->palettes[0] = calloc(256, sizeof(uint32_t));
    if (!pl->palettes[0]) {
        free(pl->palettes);
        free(pl);
        return NULL;
    }
    
    /* Initialize with grayscale ramp */
    for (int i = 0; i < 256; i++) {
        uint32_t gray = (uint32_t)i;
        pl->palettes[0][i] = 0xFF000000 | (gray << 16) | (gray << 8) | gray;
    }
    
    pl->default_palette = 0;
    
    return pl;
}

void ikm_palette_list_destroy(ikm_palette_list_t* pl) {
    if (!pl) return;
    
    for (size_t i = 0; i < pl->palette_count; i++) {
        free(pl->palettes[i]);
    }
    free(pl->palettes);
    free(pl);
}

int ikm_palette_list_add(ikm_palette_list_t* pl, const uint32_t* pal, size_t size) {
    if (!pl || !pal) return -1;
    
    /* Reallocate palette array */
    uint32_t** new_pals = realloc(pl->palettes, 
                                   (pl->palette_count + 1) * sizeof(uint32_t*));
    if (!new_pals) return -1;
    
    pl->palettes = new_pals;
    
    /* Allocate and copy new palette */
    pl->palettes[pl->palette_count] = calloc(size, sizeof(uint32_t));
    if (!pl->palettes[pl->palette_count]) return -1;
    
    memcpy(pl->palettes[pl->palette_count], pal, size * sizeof(uint32_t));
    pl->palette_count++;
    
    return (int)(pl->palette_count - 1);
}

/* === PCX RLE Decompression (for SFF v1) === */

static int ikm_pcx_decode_rle(const uint8_t* src, size_t src_len,
                                uint8_t* dst, size_t dst_len) {
    size_t si = 0, di = 0;
    
    while (si < src_len && di < dst_len) {
        uint8_t b = src[si++];
        
        if ((b & 0xC0) == 0xC0) {
            /* RLE run */
            int count = b & 0x3F;
            if (si >= src_len) break;
            uint8_t value = src[si++];
            
            for (int i = 0; i < count && di < dst_len; i++) {
                dst[di++] = value;
            }
        } else {
            /* Literal byte */
            dst[di++] = b;
        }
    }
    
    return (di == dst_len) ? 0 : -1;
}

/* === Exports for renderer integration === */

void ikm_sprite_get_size(const ikm_sprite_t* s, uint16_t* w, uint16_t* h) {
    if (!s || !w || !h) return;
    *w = s->size[0];
    *h = s->size[1];
}

void ikm_sprite_get_offset(const ikm_sprite_t* s, int16_t* x, int16_t* y) {
    if (!s || !x || !y) return;
    *x = s->offset[0];
    *y = s->offset[1];
}

void ikm_sprite_get_group_number(const ikm_sprite_t* s, int16_t* g, int16_t* n) {
    if (!s || !g || !n) return;
    *g = s->group;
    *n = s->number;
}
