// input_sig.h
// action_dispatch.h
// -------------------------------------------------
// One-header input → action dispatcher for STM32
// -------------------------------------------------
#ifndef ACTION_DISPATCH_H
#define ACTION_DISPATCH_H

#include <stdint.h>
#include <stddef.h>
#include "cc.h"

// ------------------------------------------------------------------
// Modifier bits (14)
// ------------------------------------------------------------------
enum {
    MOD_CLIP_STOP      = (1u <<  0),
    MOD_SOLO           = (1u <<  1),
    MOD_REC_ARM        = (1u <<  2),
    MOD_MUTE           = (1u <<  3),
    MOD_SELECTING      = (1u <<  4),
    MOD_STOP_ALL_CLIPS = (1u <<  5),
    MOD_VOLUME         = (1u <<  6),
    MOD_PAN            = (1u <<  7),
    MOD_SEND           = (1u <<  8),
    MOD_DEVICE         = (1u <<  9),
    MOD_LEFT           = (1u << 10),
    MOD_RIGHT          = (1u << 11),
    MOD_UP             = (1u << 12),
    MOD_DOWN           = (1u << 13),
};

// ------------------------------------------------------------------
// Flags
// ------------------------------------------------------------------
#define FLAG_SHIFT           (1u << 0)
#define FLAG_PAUSE           (1u << 1)
#define FLAG_CC           	(1u << 2) // always check 176 (4)
#define FLAG_PC          	 (1u << 3)

// Convenience masks for the table ,,still always checking  for cc
#define FLAGS_EXACT          0x07u   // must match both shift + pause
#define FLAGS_IGNORE_ALL     0x04u   // ignore shift and pause, not message
#define FLAGS_IGNORE_SHIFT   0x06u   // ignore shift, care about pause
#define FLAGS_IGNORE_PAUSE   0x05u   // ignore pause, care about shift

// ------------------------------------------------------------------
// Core types
// ------------------------------------------------------------------
/*typedef struct {  // add more for extra incoming data ,
    uint8_t  source;     // pitch or cc 0-127
    uint16_t mods;       // bitfield of the 14 modifiers
    uint8_t  flags;      // FLAG_SHIFT | FLAG_PAUSE. FLAG_CC
    uint8_t value; // second value sent , velocity or cc value 0-127
} InputSig;

typedef void (*ActionFn)(const InputSig *sig);*/

typedef struct {
    uint8_t   src_min;
    uint8_t   src_max;
    uint16_t  mods;
    uint8_t   flags;
    uint8_t   flags_mask;
    ActionFn  fn;
} ActionEntry;

// ------------------------------------------------------------------
// Readable table macros
// ------------------------------------------------------------------
// ------------------------------------------------------------------
// Readable table macros  (editor-friendly version)
// ------------------------------------------------------------------
#define ON_RANGE(min, max, mods, flags, mask, fn) \
    { (min), (max), (mods), (flags), (mask), (fn) }

#define ON(src, mods, flags, mask, fn) \
    ON_RANGE((src), (src), (mods), (flags), (mask), (fn))
// ------------------------------------------------------------------
// Lookup (inline for convenience)
// ------------------------------------------------------------------
static inline ActionFn lookup_action(const InputSig *sig,
                                     const ActionEntry *table,
                                     size_t table_size)
{
    for (size_t i = 0; i < table_size; i++) {
        const ActionEntry *e = &table[i];

        if (sig->source >= e->src_min &&
            sig->source <= e->src_max &&
            sig->mods   == e->mods &&
            (sig->flags & e->flags_mask) == (e->flags & e->flags_mask))
        {
            return e->fn;
        }
    }
    return NULL;
}

// ------------------------------------------------------------------
// Helper to build the bitfield from your existing variables
// (call this in your sampling code)
// ------------------------------------------------------------------
static inline uint16_t build_mods(int clip_stop, int solo, int rec_arm,int mute,

								  int selecting,
								  int stop_all_clips,
                                  int volume, int pan, int send, int device,
                                  int left, int right, int up, int down)
{
    uint16_t m = 0;
    if (clip_stop)      m |= MOD_CLIP_STOP;
    if (solo)           m |= MOD_SOLO;
    if (rec_arm)        m |= MOD_REC_ARM;
    if (mute)           m |= MOD_MUTE;
    if (selecting)         m |= MOD_SELECTING;
    if (stop_all_clips) m |= MOD_STOP_ALL_CLIPS;
    if (volume)         m |= MOD_VOLUME;
    if (pan)            m |= MOD_PAN;
    if (send)           m |= MOD_SEND;
    if (device)         m |= MOD_DEVICE;
    if (left)           m |= MOD_LEFT;
    if (right)          m |= MOD_RIGHT;
    if (up)             m |= MOD_UP;
    if (down)           m |= MOD_DOWN;
    return m;
}

////////////// Functions and entries into the table


void do_mute_track(const InputSig *sig) { /* ... */ };
void do_mute_solo(const InputSig *sig)  { /* ... */ };
static const ActionEntry action_table[] = {
    ON_RANGE(0, 127, MOD_DOWN,            4, FLAGS_IGNORE_ALL, cc_control_page_1),
	ON_RANGE(0, 127, MOD_CLIP_STOP,       4, FLAGS_IGNORE_ALL, cc_control_page_2),
	ON_RANGE(0, 127, 0,       4, FLAGS_IGNORE_ALL, cc_control_page_3),

    // … all your entries
};

#define ACTION_TABLE_SIZE (sizeof(action_table)/sizeof(action_table[0]))



#endif // ACTION_DISPATCH_H
