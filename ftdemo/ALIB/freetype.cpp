#include <swilib.h>
#include <de/freetype.h>
#include "freetype.h"

ft_font *_ft_open(const char *font, uint16_t size, int load)
{


    ft_font *ftf = (ft_font *)malloc(sizeof(ft_font));
    if( !ftf ) return 0;

    ftf->fte = fte_open(font);
    if(!ftf->fte){
        goto release;
    }

    if (load) fte_set_flags (ftf->fte, load);
    ftf->fti = fte_open_cache_by_metrics(ftf->fte, size);
    if(!ftf->fti)
    {
        fte_close(ftf->fte, 0);
        goto release;
    }

    return ftf;
release:
    free(ftf);

    return 0;
}


void _ft_close(ft_font *f)
{
    if(!f) return;
    if(!f->fte) return;
    if(!f->fti) return;

    fte_close_cache_metrics(f->fti);
    fte_close(f->fte, 0);
    free(f);
}


