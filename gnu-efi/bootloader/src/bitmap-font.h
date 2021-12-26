//https://www.win.tue.nl/~aeb/linux/kbd/font-formats-1.html
#define PSF1_MAGIC0     0x36
#define PSF1_MAGIC1     0x04

#define PSF1_MAGIC 0x0436

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR  0xFFFF
#define PSF1_STARTSEQ   0xFFFE

typedef struct {
        unsigned char magic[2];     /* Magic number */
        unsigned char mode;         /* PSF font mode */
        unsigned char charsize;     /* Character size */
}psf1_header;


#define PSF2_MAGIC0     0x72
#define PSF2_MAGIC1     0xb5
#define PSF2_MAGIC2     0x4a
#define PSF2_MAGIC3     0x86
#define PSF2_MAGIC 0x864ab572


/* bits used in flags */
#define PSF2_HAS_UNICODE_TABLE 0x01

/* max version recognized so far */
#define PSF2_MAXVERSION 0

/* UTF8 separators */
#define PSF2_SEPARATOR  0xFF
#define PSF2_STARTSEQ   0xFE

typedef struct{
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
}psf2_header;

typedef struct {
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
    CHAR8* buffer;
}bitmap_font;

//create a bitmap font from a loaded file
bitmap_font load_font(CHAR8* buffer){
    bitmap_font font;
    if (((CHAR16*)buffer)[0]==PSF1_MAGIC){
        Print(L"PSF1 font found");
        psf1_header* header=(psf1_header*)buffer;

        font.numglyph=256*(1+(header->mode&PSF1_MODE512));
        font.bytesperglyph=header->charsize;

        font.height=header->charsize;
        font.width=8;
        font.buffer=buffer+4;


    }
    else if(((uint32_t*)buffer)[0]==PSF2_MAGIC){
        Print(L"PSF2 font found");
        psf2_header* header=(psf2_header*)buffer;
        if(header->flags&PSF2_HAS_UNICODE_TABLE){
            Print(L"Unicode table found, todo\n");
            //This wont be necesary until/if I implement unicode text, seems i can just ignore the tables entirely for now.
        }

        font.numglyph=header->numglyph;
        font.bytesperglyph=header->bytesperglyph;

        font.height=header->height;
        font.width=header->width;
        font.buffer=buffer+32;
        Print(L"%u\n",font.width);

    }
    return font;

}