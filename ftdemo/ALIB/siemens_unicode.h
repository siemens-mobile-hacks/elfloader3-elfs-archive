#ifndef _SIEMENS_UNICODE_H_
#define _SIEMENS_UNICODE_H_

// FONT
#define UTF16_DIS_UNDERLINE (0xE002)
#define UTF16_ENA_UNDERLINE (0xE001)
//#define UTF16_DIS_UNK (0xE002)
//#define UTF16_ENA_UNK (0xE003)
#define UTF16_DIS_INVERT (0xE004)
#define UTF16_ENA_INVERT (0xE005)

#define UTF16_INK_RGBA (0xE006)
#define UTF16_PAPER_RGBA (0xE007)
#define UTF16_INK_INDEX (0xE008)
#define UTF16_PAPER_INDEX (0xE009)

#define UTF16_FONT_SMALL (0xE012)
#define UTF16_FONT_SMALL_BOLD (0xE013)
#define UTF16_FONT_MEDIUM (0xE014)
#define UTF16_FONT_LARGE (0xE016)

#define UTF16_FONT_MEDIUM_BOLD (0xE015)
#define UTF16_FONT_LARGE_BOLD (0xE017)

#define UTF16_ALIGN_LEFT (0xE01C)
#define UTF16_ALIGN_RIGHT (0xE01D)
#define UTF16_ENA_CENTER (0xE01E)
#define UTF16_DIS_CENTER (0xE01F)

// UI
#ifdef NEWSGOLD
#define CBOX_CHECKED 0xE116
#define CBOX_UNCHECKED 0xE117
#else
#define CBOX_CHECKED 0xE10B
#define CBOX_UNCHECKED 0xE10C
#endif

#define RADIOB_CHECKED 0xE116
#define RADIOB_UNCHECKED 0xE117

//
#define UTF16_NEWLINE 0x000A
#define UTF16_SPACE 0x0020

#endif /* _SIEMENS_UNICODE_H_ */
