//
// Created by Macro on 2020-02-25.
//

#ifndef JJPLAYER_SDL_VOUTOVERLAY_H
#define JJPLAYER_SDL_VOUTOVERLAY_H


#include <cstdint>
#include <libavutil/frame.h>
#include "SDL_Class.h"
#include "SDL_VoutOverlay_Opaque.h"

class SDL_VoutOverlay {

public:
    int w; /**< Read-only */
    int h; /**< Read-only */
    uint32_t format; /**< Read-only */
    int planes; /**< Read-only */
    uint16_t *pitches; /**< in bytes, Read-only */
    uint8_t **pixels; /**< Read-write */

    int is_private;

    int sar_num;
    int sar_den;

    SDL_Class               *opaque_class;
    SDL_VoutOverlay_Opaque  *opaque;

    void    (*free_l)(SDL_VoutOverlay *overlay);
    int     (*lock)(SDL_VoutOverlay *overlay);
    int     (*unlock)(SDL_VoutOverlay *overlay);
    void    (*unref)(SDL_VoutOverlay *overlay);

    int     (*func_fill_frame)(SDL_VoutOverlay *overlay, const AVFrame *frame);
};


#endif //JJPLAYER_SDL_VOUTOVERLAY_H
