//#include "main.h"
#define cpu_clock    96000000
#define prescaler 192   // TIM10 values
#define ppq_set 192   // TIM10 values
#define drum_store 64  // scene_select  *4
#define sound_set 16   // define selectable sounds for scene_buttons[0]  , important
#define SRAM_BitB_BASE         0x22000000 //sram bit band  base for calculations
#define RAM_BASE                         0x20000000   // ram address start
#define VAR_RESET_BIT(variables_addr, bit_number)    \
        (*( uint32_t *)(SRAM_BitB_BASE + ((variables_addr - RAM_BASE) * 32) + ((bit_number) * 4))  =   0)

#define VAR_SET_BIT(variables_addr, bit_number)       \
        (*( uint32_t *)(SRAM_BitB_BASE + ((variables_addr - RAM_BASE) * 32) + ((bit_number) * 4))  =   1)

#define VAR_GET_BIT(variables_addr, bit_number)       \
        (*(uint32_t *)(SRAM_BitB_BASE + ((variables_addr - RAM_BASE) * 32) + ((bit_number) * 4)))

#include"string.h"

// access individual bits as byte addresses


#define MIDI_NOTE_OFF 128
#define MIDI_NOTE_ON 144
#define CC_Message 176   // for this particular unit
#define up_arrow_button 64
#define down_arrow_button 65
#define left_arrow_button 66
#define right_arrow_button 67
#define volume_button 68
#define pan_button 69
#define send_button 70
#define device_button 71
#define play_pause_button 91
#define clip_stop_button 82
#define solo_button 83
#define rec_arm_button 84
#define mute_button 85
#define select_button 86
#define stop_all_clips 81
#define record_button 93
#define shift_button 98

#define green_button 1
#define green_blink_button 2
#define red_button 3
#define red_blink_button 4
#define yellow_button 5
#define yellow_blink_button 6
#define variable_count 20   // for flash mem

#define pot_1 48
#define pot_2 49
#define pot_3 50
#define pot_4 51
#define pot_5 52
#define pot_6 53
#define pot_7 54
#define pot_8 55

	//MIDI CC
	//	0 	Bank Select (MSB)  for patch banks , 7-volume, 5-portamento time
	// 73 -Attack ,   72 -Release ,71 -VCF REsonance , 74 -VCF cutoff freq,  91 -Reverb , 84-portamento, 94-detune, 95-phaser, 70-sound variation,
	//92 -tremolo, 75-79 generic fx settings , may use it for delay unit



//1 is default or green, 2 ;    default or green blink, 5 ;   is yellow, 6  ;   yellow blink , 4  ;   red blink, 3 ;   red,
extern uint16_t bpm_table[256];  // lut values for temp TIM10 ARR
extern uint8_t tempo; // tempo value
extern uint8_t ppq_count; // ppq_count for seq_pos
extern uint16_t skip_counter; // keeps track of ppq skip
volatile extern uint8_t ppq_send; // ppq_counter
volatile extern uint8_t skip_enable; // allow ppq toggle skip
volatile extern uint8_t skip_setting; // slow or faster 3,1,0
// SWING - even numbered notes are shifted left or right


//buttons
extern uint8_t volume; // volume button
extern uint8_t pan;
extern uint8_t shift; // track shift button
extern uint8_t pause; // enable pause mode
extern uint8_t select_bn; //select button
extern uint8_t right_arrow;
extern uint8_t left_arrow;
extern uint8_t up_arrow; // on by default
extern uint8_t down_arrow;
extern uint8_t solo;

extern uint8_t device;
extern uint8_t clip_stop;
extern uint8_t send;
extern uint8_t record;
extern uint8_t sustain;
extern uint8_t pause_delay;

extern uint8_t serial1_temp;
extern uint8_t temp;

extern uint8_t serial1_hold[10];
extern uint8_t serial1_hold2[10];
extern uint8_t serial1_counter;
extern uint16_t i;  // always 16 bit or higher
extern uint16_t n;
extern uint8_t led_blink;
extern uint8_t note_bank[18];   //contains note on and off info
extern uint8_t last_note ;  // 1-3  last note played on note bank
extern uint8_t last_key ;    // last actual key played
extern uint8_t clear[3] ;
extern uint8_t led_clear ;   // track claearing leds

extern uint8_t note_temp[3] ;      // holds current note being filled
extern uint8_t note_replace_enable ;
extern uint8_t note_length_list[10];   // keeps note count for note off
extern uint8_t program; // current loaded program pointer
extern uint8_t patch_load; // patch to be loaded 0-15
extern uint8_t patch_save;  //  patch save  0-15
extern char print_out[10][3];
extern uint8_t send_all[128]; //scene sends
extern uint8_t send_buffer[34] ;
volatile extern uint16_t seq_pos;  // 16 bit  , 24/quater or 8/step  on 1/16th , sequencer clock, rarely used might change
extern uint16_t s_temp;
extern uint16_t seq_pos_mem;
extern uint16_t mtc_tick ;    // incoming realtime clock counter
extern uint8_t seq_enable ;
extern uint8_t seq_step;
extern uint8_t realtime;
extern uint8_t program_change_flag ;
extern uint8_t seq_pos_swing[sound_set]; // swing timing for seq_pos note triggering


volatile extern uint16_t timer_value; // sets timer ccr  def is 1100 for now
extern uint16_t seq_tmp;
extern uint8_t cdc_buffer[64];  // receive buffer on sub , check usbd_cdc_if.c and h for more
extern uint8_t cdc_send_cue[260];   //hold from cdc
extern uint8_t cdc_len;
extern uint32_t Len;
volatile extern uint8_t button_send_trigger;  // send button midi repeat
extern uint8_t trigger_mem;
extern uint8_t lfo; // temp storage
//  pot byte  are 176 , 48-55, 0-127
// square buttons byte   144 /128 ,   0-39,127,   top row is 32-39
//  round vertical  144/128,81-85,127
// play 91 . stop all clip  ;   81 ,record 93 ,shift button 98 ,82 clip stop
// round horizontal 64-71
// velocity  															1 is default or green, 2 ;    default or green blink, 5 ;   is yellow, 6  ;   yellow blink , 4  ;   red blink, 3 ;   red,
extern const  uint8_t square_buttons_list [100] ;
const extern uint8_t button_convert[41] ;
extern uint8_t scene_buttons[10];  // scene select buttons , bottom square row , also [0] is last button pressed , somehow Palpatine returned

extern uint8_t button_states[100] ;  // storage for incoming data on button presses , 8-40 (0-32)  is out of order to make it easier on operations ,bad idea -reversed


extern uint8_t pot_states[sound_set] ;   // stores pots 1-8 current state
extern uint8_t note_off_flag[3]; // use this to detect held buttons 0 is on off ,1 is last button detected
extern uint8_t all_update ;   // update all buttons from button_states , 40 for now
extern uint8_t keyboard[3];  // store keys
//const extern uint8_t drum_list[sound_set]  ;   {20,21,22,23,24,25,26,27,28,29,30,30,30,30,30,30}; // notes played for drum scenes
extern const  uint8_t drum_list[sound_set]  ;    // notes played for drum scenes

extern uint32_t sys_cnt[3];
extern uint8_t note_off_enable;
extern uint8_t settings_write_flag ;
extern uint8_t cdc_start ;    // checks for extra messages
//bar section
extern uint8_t bar_selector; // select bar for editing
extern uint8_t bar_playing; // currently playing bar , 16 notes per bar
extern uint8_t punch_in[8]; // allow instant response for certain buttons
extern uint8_t flash_settings_count; // count of elements in settings array
extern uint16_t flash_settings_size;  // size of all settings


extern uint8_t counter_a;
// SPI stuff
extern uint8_t spi_send[10];
extern uint8_t status_reg[2];
extern uint8_t first_message ;  // flag to clear once a button is pressed
extern uint8_t rec_arm ;
extern uint8_t last_note_played[16]; // stores note for note off
extern uint8_t ppq_temp;

extern uint8_t flash_flag ;
extern uint8_t flash_read_block2[260]  ;  //this should clearif flash read works
extern uint8_t send_spi2[260];
extern uint8_t write_once; // allow only a single flash write for now
extern uint8_t test_data[32] ;
extern uint8_t spi_hold[260] ;
extern uint8_t all_settings[2048];  // store all extra settings:  transpose , pots
extern uint8_t other_buttons; // update control button lights
extern uint8_t other_buttons_hold[100]; // keeps track of buttons
extern uint8_t send_buffer_sent;
extern uint8_t button_states_save[100]; // reference for button changes for controller , not in order



//extern uint16_t loop_note_list[sound_set]; //tracks currently playing note position

extern uint8_t loop_note_count[sound_set];  // keeps track of number of notes in a loop
extern uint8_t loop_current_speed;
extern uint8_t loop_lfo_out[sound_set*3];  // used for some level of lfo using pot7 for now 0-255
// extern uint8_t lfo_settings[sound_set*3];  // lfo 0-8   rate , gain,offset, target
extern uint8_t alt_pots[sound_set*16]; // stores a set of alt pot settings , 8 bytes per bank , 8 banks , also holds extra data > 128
extern uint8_t alt_pots_selector ;  // changes alt_pots banks (8bytes each )
extern uint8_t alt_pots_selector_list[16]; // changes alt_pots banks (8bytes each )
extern uint8_t alt_pots_octave[sound_set]; // tracks octave from alt pots
extern uint8_t seq_step_mem;  // mem for looper


extern uint8_t pot_tracking[33] ; // record pot movements , maybe after 1 bar ,only transpose for now
extern uint8_t mute_list[sound_set]; //track scene mutes
extern uint8_t noteoff_list[25]; //track note offs sorted 0-7 , empty when not in use
//notes
extern uint8_t scene_transpose[sound_set];

extern uint8_t last_button;

extern uint8_t scene_mute; // muting
extern uint8_t last_incoming;
extern uint8_t scene_solo; //enable solo mode
extern uint8_t stop_toggle ;    // use it for pause

extern uint8_t seq_step_list[20]; //store seq_step position per part  .for now just notes 4-8
extern uint16_t seq_step_fine[sound_set];  // holds high count for seq_step_list   *8 res
extern uint8_t seq_current_step; // current position on selected from seq_step_list
extern uint8_t seq_step_reset[sound_set];  // tracks when seq_step_list reset to start
extern uint8_t seq_step_long; // 16*32
extern uint8_t seq_step_enable[sound_set]; // step change tracking

extern uint8_t note_enable_list[sound_set]; //  plays note one every X number of bar

extern uint8_t note_enable_list_counter[sound_set] ;    // keeps count for the note enable list
extern uint8_t note_enable_list_selected; // just current note neable
extern uint8_t LFO_low_list[sound_set*4]; // sets period low ,LFO bar count, *4 sets for various things, 1 ;   note on/off  2 ;   change pitch maybe
extern uint8_t LFO_high_list[sound_set*4]; // sets high period
extern uint8_t LFO_phase_list[sound_set*4];// 0-1 , might put it elsewhere
extern uint8_t LFO_tracking_counter[sound_set*4]; //keeps track of LFO ,counts 0-7 then flips to 15-23 depending on settings
extern uint8_t LFO_tracking_out[sound_set*4]; // LFO output




extern uint8_t loop_length;

extern uint8_t serial_out[128]; // holds midi out data
extern uint8_t serial_len;
extern uint8_t midi_channel_list[21] ;    //holds midi channel settings 0 ;   1 (midi channels 1-16)
extern uint8_t nrpn_cue[80] ;    // stores message for nrpn on es1 only needs 1 initial c99 ;   5  then only  2 bytes repeating  CC 98  ;   NRPN LSB and CC 6  ;   value , for now 9 bytes though  , initial normal 3 bytes then convrted to 9
const extern uint8_t pitch_lut[127]  ; // es1 pitch table  4 octaves
extern uint8_t es_filter[9]; // track es1 filters  old and new values  say 4+4
extern uint8_t es_filter_cue[20];   // hold filter data for nrpn
extern uint8_t midi_cc[sound_set]; // enabled if sending midi cc
extern uint8_t midi_cc_cue[20];  //  0 ;   part+1 ,1 is value
extern uint8_t midi_extra_cue[30] ; // extra stuff to be sent that ins't regular [28] is length
extern uint8_t current_midi; // holds selected channel 1-16
extern uint8_t note_accent[sound_set] ;    // stores accent data for tracks on/off

extern uint8_t play_list[257] ;     // holds playback muting order 0-7,8-15,16-23,24-31  for 8x 4 bars  on all parts maybe the 4 bit LSB ,only needs 8 bytes per part, change to hold transpose
extern uint8_t play_screen ;     // enable for secondary screen for muting setup
extern uint8_t play_position;  // track muting list 8*4     each 8 steps  +1
extern uint8_t play_list_write ;  // keeps writing while shift is held down
extern uint8_t write_velocity;  // keeps writing velocity while enable , holds velocity value as well
extern uint8_t looper_list[sound_set*4] ;    // holds looper settings , 0 ;   start offset 2 ;   vel offset 3 ;   lfo level  4 ;   speed  ,obsolete for now
extern uint8_t looper_list_mem[20];  // keeps track of previous values for ppq skip
extern uint8_t loop_current_offset;
extern uint8_t loop_current_length;
extern uint8_t step_record; // works in stop mode
extern uint8_t drum_store_one[1024]; // holds drum notes 2 bit , note one and accent , 4bytes per scene ,16 scenes in series (64bytes) , then bar 2

extern uint8_t test_byte[64];
extern uint8_t test_byteB[64];
extern uint8_t second_scene ;    // select second set of sounds
extern uint8_t cdc_len_temp;
extern uint8_t program_change[16]; // hold current prograam change for 2 channels
extern uint8_t green_position[20];
extern uint8_t cdc_buf2[12];
//extern uint8_t pattern_scales[] ;   {"major","minor","maj penta","minor penta","blues","dorian","half diminshed","lydian", "asc melodic minor","lyidian aug"," half dimished"," octatonic","persian" };  // 8 per scale
extern uint8_t pattern_scale_data[] ;

extern uint8_t pattern_scale_list [16] ;  //  keeps track of scale used in pattern, one set for now
extern uint8_t pattern_copy_full ;
extern uint8_t midi_cue[8192];  // data cue for one entire pattern now 2*16*16*16 in total (assuming evey note is on )

extern uint16_t midi_cue_time[2048];  // holds actual time for  midi note messages in midi_cue_main 0-2047 for now
extern uint8_t midi_cue_size[2048]; // holds message size (usually 4 for now )
extern uint16_t midi_cue_loc[2048]; // holds data location ( *4  )

//extern uint8_t midi_cue_main[1024] ; //holds all midi note data for now , all notes all patterns
extern uint16_t midi_send_time ;
extern uint16_t midi_send_current ;
extern uint16_t midi_cue_count ;
// pattern related
extern uint8_t last_pattern_select;
extern uint8_t pattern_rewind;  // +1
extern uint8_t new_pattern_select;
extern uint8_t pattern_select ;   // disabled for now , will have 2
extern uint8_t pattern_offset_list[16] ;     // list of offset for a single part (keys only ) on each pattern , handy
extern uint8_t pattern_offset; // for lcd
extern uint8_t pattern_copy[32];  // buf for copypasta
extern uint8_t pattern_loop;  // pattern select counter from pattern count

extern uint8_t pattern_repeat[16] ;    // sets the number of times to repeat before stepping
extern uint8_t pattern_count ;  // sets the pattern total repeating size
extern uint8_t pattern_start ;   //sets start position of pattern playback
extern uint8_t rand_velocities[32]; //holds a list of modified velocities
extern uint8_t last_note_on_channel[16];  // stores last played note on non drum channels ,second page
extern uint8_t last_note_end_count[16] ;    // counts down for note off , second page
extern uint8_t program_change_automation[32];   // record a single pc per bar 4 bit for position from seq_pos and 3 bit for actual pc selected ,can be overwritten ,only 1 per bar
extern uint8_t lcd_control ;   // controls type of lcd menu , used for downcount as well
extern uint8_t lcd_downcount ;
extern uint8_t lcd_messages_select ;  //select from lcd_meesages , do not use [5] or it'll crash
extern uint8_t current_accent;// current selected sound accent value
extern uint8_t lcd_buffer[32]; // holds outgoing characters
extern uint8_t lcd_buffer_mem[32]; // holds outgoing characters
extern uint8_t lcd_delay_counter; // counts to 32 for sending
extern uint8_t I2C_buffer[256]; // 4*32 , full page
extern uint8_t I2C_transmit ;  // transmit flag
extern uint8_t lcd_buffer_counter;// keeps track of lcd_buffer
extern uint8_t last_solo_selected ;    // saves last selected solo button
extern uint8_t lcd_buffer_full; // check flag for being sent

extern uint8_t pitch_list_for_drums[64];  // holds 8 pitches per (first page ) 8 notes for drums for automation , used with nrpn data ,separate from pitch data for now
extern uint8_t pitch_selected_for_drums[8];  // current selected pitch on first page of drums 1-7 , not actual pitch but just selection, useless
extern uint8_t pitch_change_flag; //enabled when pitch data needs to be sent
extern uint8_t pitch_change_store[64];  //stores pitch changes 0-7 per bar ,per sound , 8bytes(per bar) *8 sounds ,holds presets for pitch_list_for drums for now
extern uint8_t pitch_selected_drum_value[8]; // holds outgoing drum value
extern uint8_t pitch_change_loop[8] ;  //sets the looping length of pitch changes may default to pressed button as the first in step
extern uint8_t pitch_change_loop_position[8] ;   // keeps track of pitch change loop
extern uint8_t pitch_change_rate[16]; // sets alt_pots playback rate , 2 bars ,4 bars , 8 bars ,16bars,32 bars


extern uint8_t lfo_settings_list[32]; //holds lfo settings 1 rate , 2 level , testing for now
extern uint8_t lfo_full_send_enable ;  // lfo transmit flag
extern uint8_t single_settings_list[16]; // holds single variables ie tempo for storage
extern uint8_t last_pitch_count[16];
extern uint8_t overdub_enabled; // flag to keep track of overdub
extern uint8_t keyboard_buffer[32]; // store keyboard presses  with recorded time , maybe clear on bar if pressed again  or mute for a bar on playback
//typedef struct { uint8_t *var; char *name;} LCD_item;    // creates lcd menu items to make it a bit easier

extern uint16_t bar_note_register[8];  //scans for data bars if none it keeps scene select light off    1 bit , 16bit-8bars
extern uint8_t blink_light_list[8]; // just add blink light to scene buttons

extern uint8_t nrpn_gating_enable;  // for gating audio effects on drum
extern uint8_t nrpn_gating_switch[8]; //just the levels for gating or volume
extern uint8_t nrpn_gating_sequence[16] ;      // just a gating sequence
extern uint8_t controller_fx_enable; // controller data flag
extern uint8_t controller_fx_select[2];  // selects which controller data
extern uint8_t controller_fx_value[2]; // controller level
extern uint8_t control_change_flag ;    // selects from fx list , not sounds or midi

extern uint8_t keyboard_step_record;   // keeps count of keys pressed once rec_arm and pause is on
extern uint8_t loop_screen_disable ;
extern uint8_t control_change[16];   // set control change data using fx menu list
extern uint8_t control_change_value ;
extern uint8_t buttons_bank_1[32];  // alternative buttons_states for different displays
extern uint8_t current_playing_bar;   // keep record of the current playing bar , lenght to be set
extern uint8_t LFO_tracking_bank[64]; //stores 4 steps when run
extern uint8_t bar_map_0[sound_set*2];  // bit map for notes 16*16bit
extern uint8_t bar_map_1[sound_set] ;   // bit mapped enable 1 bar
extern uint8_t bar_map_8[sound_set] ;  // bit mapped enable 8 bar
extern uint8_t bar_map_64[sound_set] ;  // bit mapped enable 8*8 bar   , for now might change to diff values
extern uint8_t bar_map_screen_level ;    //selects which zoom is displayed 0-3   , 0 ;   none 1 ;   8 2 ;   32
extern uint8_t bar_end_enable ;    // if enabled runs everything needed on seq_pos ;   0
extern uint8_t bar_start_enable ;  // if enabled runs everything for seq_pos ;   255

extern uint8_t clear_rows ;    // clear_top 4 rows only on USB send
extern uint8_t bar_map_sound_enable[sound_set]; // output from bar map editor
extern uint16_t bar_map_counter ;   // keeps total bar count
extern uint8_t alt_pots_overwrite_enable[sound_set];  // allows to overwrite alt pots value after reaching oroginal value ,then reset after a while ,maybe count down
extern uint8_t last_pot_used ;
extern uint8_t temp_midi_var ;

extern uint8_t fx_incoming[3];
extern uint8_t fx_pot_settings[48] ;      //  holds midi_channel and cc assignment for 8 pots  0-15 and 0-127 , fx list cha 3 ,4,2
extern uint8_t fx_pot_values[48] ;

extern uint8_t fx_map_sound_enable[16] ;   ; //tracks if bar is enabled
extern uint8_t control_change_buf[32];  // stores control change values
extern uint8_t fx_counter ;

extern uint8_t motion_record_buf[sound_set*8];  // holds 1 bit motion record when pressing shift and changing knob levels set elsewhere
extern uint8_t motion_record_limits[sound_set*2] ;// using for accent only atm so only using the low values , high is note_accent
extern uint8_t accent_bit; // keeps track of bit for motion play 0-32
extern uint8_t accent_bit_shift; // moves up one word

extern uint8_t shift_hold_enable;  // this is enabled when shift is held while a pot is turned
extern uint8_t shift_hold_buf[4]; // this holds the last message , for repeating at regular intervals until shift is released
extern uint8_t note_on_tracking_buf[8];  // keeps tracking note-ons for 40 track buttons , for multitouch etc , 1 bit
extern uint8_t bar_map_looping[2] ;  //sets base looping length for 8 bars , can be changed
extern uint8_t bar_map_lights[8];  // holds bar map light values for looping display
extern uint8_t bar_loop_current;
extern uint8_t seq_play_buf[2048]; // holds  *16 note+velocity+time   , recorder with arm_rec , one bar now, maybe more than one bar later
extern uint16_t seq_record_timer;
extern uint8_t seq_record_enable;
extern uint8_t transpose_octave_modifier[sound_set];   // it changes base octave and transpose notes for cdc_send2 , calculated from alt_pots  ,octave shift, make sure always above 0
extern int8_t transpose_pitch_modifier[sound_set];  // it changes base octave and transpose notes for cdc_send2 , calculated from alt_pots  ,-6-+6 notes , pitch shift ,
extern uint16_t seq_pos_out[sound_set]; // output from seq_pos_set
extern float seq_pos_set[sound_set] ;   // seq_pos multiplier
extern uint8_t last_note_time[sound_set];
extern uint8_t seq_play_buf_time[512]; // holds time values of seq_play_buf , for modification
extern uint16_t seq_play_buf_end[sound_set]; // records the last midi message time
extern uint8_t seq_pos_flag[sound_set]; // enables longer than 255 count on seq_pos
extern uint8_t seq_reset_flag[sound_set]; // allows reset

uint8_t short_repeat_counter[sound_set];  // counts triggered playback //1-8 bars
uint8_t short_repeat_buf[2048]; // this holds temp notes when played during run
uint8_t short_repeat_time[512]; // holds time values for short playback
uint8_t short_play_buf_end[sound_set]; // end of
uint8_t short_track_disable=0; // disables playback on track for one bar while recording to avoid notes hanging
char lcd_char[4];
extern uint8_t microkorg_cc_numbers[43];
extern const char *microkorg_cc_list[];
char cc_string[16];  // holds outgoing string for lcd

uint8_t note_recording_set_current[8];  // currently playing recording set ,only 4 for now
extern uint8_t voice_list[sound_set]; // tracks assigned to voice channels , related to midi channel














