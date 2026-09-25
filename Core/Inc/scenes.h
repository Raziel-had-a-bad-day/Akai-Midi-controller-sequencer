// this hosts the storage function for cc based scenes


#include <stdint.h>
#include <string.h>



/* ------------------------------------------------------------------ */
/*  Normal button press: load scene                                    */
/* ------------------------------------------------------------------ */
void select_and_load(uint8_t button)
{
    if (button >= MAX_SCENES) return;

    live.count = scenes[button].count;
    memcpy(live.pairs, scenes[button].pairs, sizeof(live.pairs));
}

/* ------------------------------------------------------------------ */
/*  Incoming CC – records any CC (max 8 unique)                        */
/* ------------------------------------------------------------------ */
void update_live_cc(uint8_t cc, uint8_t value)
{
    uint8_t i;

    /* Replace if already present */
    for (i = 0; i < live.count; i++) {
        if (live.pairs[i].cc == cc) {
            live.pairs[i].value = value;
            return;
        }
    }

    /* Add new */
    if (live.count < MAX_CCS_PER_SCENE) {
        live.pairs[live.count].cc    = cc;
        live.pairs[live.count].value = value;
        live.count++;
    }
}

/* ------------------------------------------------------------------ */
/*  Shift + button → save current live data permanently                */
/* ------------------------------------------------------------------ */
void save_scene(uint8_t button)
{
    if (button >= MAX_SCENES) return;
    scenes[button] = live;
}

/* ------------------------------------------------------------------ */
/*  Compact flash / file helpers                                       */
/* ------------------------------------------------------------------ */
size_t save_all_to_buffer(uint8_t *buf, size_t bufsize)
{
    uint8_t *p = buf;
    uint8_t used = 0;
    uint8_t s, i;

    if (bufsize < 2) return 0;

    *p++ = MAGIC;
    p++;                                /* leave space for used count */

    for (s = 0; s < MAX_SCENES; s++) {
        if (scenes[s].count == 0) continue;

        if ((size_t)(p - buf) + 2 + scenes[s].count * 2 > bufsize)
            break;

        *p++ = s;
        *p++ = scenes[s].count;
        for (i = 0; i < scenes[s].count; i++) {
            *p++ = scenes[s].pairs[i].cc;
            *p++ = scenes[s].pairs[i].value;
        }
        used++;
    }

    buf[1] = used;
    return (size_t)(p - buf);
}

int load_all_from_buffer(const uint8_t *buf, size_t len)
{
    const uint8_t *p = buf;
    uint8_t used, n, s, count, i;

    if (len < 2 || *p++ != MAGIC) return 0;
    used = *p++;

    memset(scenes, 0, sizeof(scenes));
    memset(&live, 0, sizeof(live));

    for (n = 0; n < used; n++) {
        if (p + 2 > buf + len) return 0;
        s     = *p++;
        count = *p++;
        if (s >= MAX_SCENES || count > MAX_CCS_PER_SCENE) return 0;
        if (p + count * 2 > buf + len) return 0;

        scenes[s].count = count;
        for (i = 0; i < count; i++) {
            scenes[s].pairs[i].cc    = *p++;
            scenes[s].pairs[i].value = *p++;
        }
    }
    return 1;
}


void cc_scene_controls (uint8_t button ){   // when cc scenes are selected

if(!shift) select_and_load(button); else save_scene(button);   // save current changes if shift

//memcpy(live.pairs, scenes[button].pairs, sizeof(live.pairs));
uint8_t list=sizeof(live.pairs)/2;
uint8_t extras=midi_extra_cue[28];
for (int var = 0; var < list; ++var) {  // send stored cc values

		if(live.pairs[var].cc==0) continue;
		 uint8_t voice_select=voice_list[scene_buttons[0]]; //
		  midi_extra_cue[extras]=176+midi_channel_list[voice_select];   // select midi for cc
		  		  midi_extra_cue[extras+1]=  live.pairs[var].cc ; // setting for reverb atm
		  		  midi_extra_cue[extras+2]=  live.pairs[var].value ;
		  			 extras+=3;

}
midi_extra_cue[28]=extras;

}
#include <stdio.h>      /* for FILE, fopen, fwrite, fread, etc. */

/* ------------------------------------------------------------------ */
/*  Backup all scenes to a single file                                 */
/*  Returns 1 on success, 0 on failure                                 */
/* ------------------------------------------------------------------ */
int backup_scenes(const char *filename)
{
    uint8_t buf[600];               /* plenty for 32 scenes */
    size_t  len;
    FILE   *f;
    size_t  written;

    len = save_all_to_buffer(buf, sizeof(buf));
    if (len == 0) return 0;

    f = fopen(filename, "wb");
    if (f == NULL) return 0;

    written = fwrite(buf, 1, len, f);
    fclose(f);

    return (written == len) ? 1 : 0;
}

/* ------------------------------------------------------------------ */
/*  Restore all scenes from a single file                              */
/*  Returns 1 on success, 0 on failure                                 */
/* ------------------------------------------------------------------ */
int restore_scenes(const char *filename)
{
    uint8_t buf[600];
    size_t  len;
    FILE   *f;

    f = fopen(filename, "rb");
    if (f == NULL) return 0;

    len = fread(buf, 1, sizeof(buf), f);
    fclose(f);

    if (len == 0) return 0;

    return load_all_from_buffer(buf, len);
}
/*


 Normal button press
select_and_load(button);

 Shift + button
save_scene(button);

 On every MIDI CC
update_live_cc(cc, value);      // now accepts any CC

*/






