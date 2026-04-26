

#include "main.h"
#define sound_set 8
#define record_per_channel 8
//#include "main.h"
#define cpu_clock    96000000
#define prescaler 192   // TIM10 values
#define ppq_set 192   // TIM10 values
#define drum_store 64  // scene_select  *4

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


#define seq_play_note_count 16   // note on + note off


//1 is default or green, 2= default or green blink, 5=is yellow, 6 =yellow blink , 4 =red blink, 3=red,
uint16_t bpm_table[256];  // lut values for temp TIM10 ARR
uint8_t tempo; // tempo value
uint8_t ppq_count; // ppq_count for seq_pos

volatile uint8_t ppq_send; // ppq_counter
volatile uint8_t skip_enable; // allow ppq toggle skip
volatile uint8_t skip_setting; // slow or faster 3,1,0
// SWING - even numbered notes are shifted left or right
uint8_t countSetBits(uint8_t number) { // count bits in byte
	 uint8_t count  =   0;
    while (number) {
        count +=    number & 1; // Increment count if the last bit is 1
        number >>=   1;        // Right shift n by 1
    }
    return count;}


//buttons
uint8_t volume; // volume button
uint8_t pan;
uint8_t shift; // track shift button
uint8_t pause; // enable pause mode
uint8_t select_bn; //select button
uint8_t right_arrow;
uint8_t left_arrow;
uint8_t up_arrow=1; // on by default
uint8_t down_arrow;
uint8_t solo;

uint8_t device;
uint8_t clip_stop;
uint8_t send;
uint8_t record;
uint8_t sustain;
uint8_t pause_delay=0;




uint8_t serial1_hold[10];
uint8_t serial1_hold2[10];

uint16_t i;  // always 16 bit or higher
uint16_t n;
uint8_t led_blink;
uint8_t note_bank[18];   //contains note on and off info


uint8_t led_clear=1; // track claearing leds

uint8_t note_temp[3]={0,0,0};   // holds current note being filled
uint8_t note_replace_enable=0;


uint8_t patch_save;  //  patch save  0-15



volatile uint16_t seq_pos;  // 16 bit  , 24/quater or 8/step  on 1/16th , sequencer clock, rarely used might change
uint16_t s_temp;
uint16_t seq_pos_mem=1;
uint16_t mtc_tick=0;  // incoming realtime clock counter

uint8_t seq_step; // 0-15 steps,paused
uint8_t realtime;
uint8_t program_change_flag=0;
uint8_t seq_pos_swing[sound_set]={2,1,1,1,3,3,3}; // swing timing for seq_pos note triggering
volatile uint16_t timer_value=512; // sets timer ccr  def is 1100 for now
uint8_t cdc_buffer[64];  // receive buffer on sub , check usbd_cdc_if.c and h for more
uint32_t Len;

//  pot byte  are 176 , 48-55, 0-127
// square buttons byte   144 /128 ,   0-39,127,   top row is 32-39
//  round vertical  144/128,81-85,127
// play 91 . stop all clip =81 ,record 93 ,shift button 98 ,82 clip stop
// round horizontal 64-71
// velocity  															1 is default or green, 2= default or green blink, 5=is yellow, 6 =yellow blink , 4 =red blink, 3=red,
const uint8_t square_buttons_list [100]= {32,33,34,35,36,37,38,39,24,25,26,27,28,29,30,31,16,17,18,19,20,21,22,23,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99}; // just reads buttons in top.down order
const uint8_t button_convert[41]=		  {32,33,34,35,36,37,38,39,24,25,26,27,28,29,30,31,16,17,18,19,20,21,22,23,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7};
uint8_t scene_buttons[10];  // scene select buttons , bottom square row , also [0] is last button pressed , somehow Palpatine returned
uint8_t button_states[100]={1,1,1,1,1,1,1,1} ; // storage for incoming data on button presses , 8-40 (0-32)  is out of order to make it easier on operations ,bad idea -reversed
uint8_t pot_states[sound_set]={64,64,64,64,64,64,64,64}; // stores pots 1-8 current state
uint8_t note_off_flag[3]; // use this to detect held buttons 0 is on off ,1 is last button detected
uint8_t all_update=1; // update all buttons from button_states , 40 for now
uint8_t keyboard[3];  // store keys
//const uint8_t drum_list[sound_set] ={20,21,22,23,24,25,26,27,28,29,30,30,30,30,30,30}; // notes played for drum scenes
const uint8_t drum_list[sound_set*2] ={48,50,52,41,43,60,61,42,46,47,45,30,30,30,30,30}; // notes played for drum scenes
uint8_t note_off_enable;
uint8_t settings_write_flag=0;
uint8_t cdc_start=0;  // checks for extra messages
//bar section
uint8_t bar_selector; // select bar for editing
uint8_t bar_playing; // currently playing bar , 16 notes per bar

uint8_t flash_settings_count; // count of elements in settings array
uint16_t flash_settings_size;  // size of all settings
uint8_t counter_a;
// SPI stuff
uint8_t status_reg[2];
uint8_t first_message=0; // flag to clear once a button is pressed
uint8_t rec_arm=0;

uint8_t ppq_temp;

uint8_t flash_flag=0;
uint8_t flash_read_block2[260] ={1,1,1,1,1,1,1,0}; // this should clearif flash read works
uint8_t send_spi2[260];
uint8_t write_once; // allow only a single flash write for now
uint8_t test_data[32]={0,0,0,0,1,0,5,1,1,0,1,5,1,1,0,1,1,3,0,1,1,0,1,0,1,0,1,0,1,0,1};

uint8_t all_settings[2048];  // store all extra settings:  transpose , pots
uint8_t other_buttons; // update control button lights
uint8_t other_buttons_hold[100]; // keeps track of buttons
uint8_t send_buffer_sent;
uint8_t button_states_save[100]; // reference for button changes for controller , not in order
uint8_t loop_current_speed;
uint8_t loop_lfo_out[sound_set*3];  // used for some level of lfo using pot7 for now 0-255
// uint8_t lfo_settings[sound_set*3];  // lfo 0-8   rate , gain,offset, target
uint8_t alt_pots[sound_set*16]; // stores a set of alt pot settings , 8 bytes per bank , 8 banks , also holds extra data > 128

uint8_t alt_pots_octave[sound_set]; // tracks octave from alt pots
uint8_t seq_step_mem;  // mem for looper

uint8_t pot_tracking[sound_set] ; // record pot movements , maybe after 1 bar ,only transpose for now
uint8_t mute_list[sound_set]; //track scene mutes
//notes
uint8_t scene_transpose[sound_set];
uint8_t scene_mute; // muting
uint8_t stop_toggle=0; // use it for pause

uint8_t seq_current_step; // current position on selected from seq_step_list
uint8_t seq_step_long; // 16*32

uint8_t note_enable_list_selected; // just current note neable
uint8_t LFO_low_list[sound_set*4]; // sets period low ,LFO bar count, *4 sets for various things, 1=note on/off  2=change pitch maybe
uint8_t LFO_high_list[sound_set*4]; // sets high period
uint8_t LFO_phase_list[sound_set*4];// 0-1 , might put it elsewhere
uint8_t LFO_tracking_counter[sound_set*4]; //keeps track of LFO ,counts 0-7 then flips to 15-23 depending on settings
uint8_t LFO_tracking_out[sound_set*4]; // LFO output

uint8_t serial_out[128]; // holds midi out data
uint8_t serial_len;
uint8_t midi_channel_list[sound_set]={9,4,2,3,5};   //holds midi channel settings for voices
uint8_t nrpn_cue[80]={186,99,5,186,98,16,186,6,32};  // stores message for nrpn on es1 only needs 1 initial c99=5  then only  2 bytes repeating  CC 98 =NRPN LSB and CC 6 =value , for now 9 bytes though  , initial normal 3 bytes then convrted to 9
const uint8_t pitch_lut[127] ={0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,12,14,16,18,20,22,24,27,30,33,36,39,42,45,48,51,54,57,64,0,2,4,6,8,10,12,14,16,18,20,22,24,27,30,33,36,39,42,45,48,51,54,57,64,70,73,76,79,82,85,88,91,94,97,100,103,105,107,109,111,113,115,117,119,121,123,125,127,64,70,73,76,79,82,85,88,91,94,97,100,103,105,107,109,111,113,115,117,119,121,123,125,127};   // es1 pitch table  4 octaves

uint8_t midi_extra_cue[30] ; // extra stuff to be sent that ins"t regular [28] is length
uint8_t note_accent[sound_set]={127,127,127,127,127,127,127,127};  // stores accent data for tracks on/off

uint8_t drum_store_one[1024]; // holds drum notes 2 bit , note one and accent , 4bytes per scene ,16 scenes in series (64bytes) , then bar 2
uint8_t second_scene=0;  // select second set of sounds
uint8_t program_change[sound_set]; // hold current prograam change for 2 channels
uint8_t green_position[20];
uint8_t cdc_buf2[12];
//uint8_t pattern_scales[]={"major","minor","maj penta","minor penta","blues","dorian","half diminshed","lydian", "asc melodic minor","lyidian aug"," half dimished"," octatonic","persian" };  // 8 per scale
uint8_t pattern_scale_data[]={
		0,0,0,0,0,0,0,0,
		0,2,4,5,7,9,11,0,
		0,2,3,5,7,8,10,0,
		0,2,4,7,9,0,0,0,
		0,3,5,7,10,0,0,0,
		0,3,5,6,7,10,0,0,
		0,2,3,5,7,9,10,0,
		0,2,3,5,6,8,10,0,
		0,2,4,6,7,9,11,0,
		0,2,3,5,7,9,11,0,
		0,2,4,6,8,9,11,0,
		0,2,3,5,6,8,10,0,
		0,2,3,5,6,8,9,11,
		0,1,4,5,6,8,11,0, // repeat here
		0,2,3,5,6,8,10,0,
		0,2,3,5,6,8,9,11,
		0,1,4,5,6,8,11,0



 	 	 	 	 	 	 }; // 8 per scale , 0 terminate

uint8_t pattern_scale_list [sound_set]={0,0,0,0,0,0,0,0}; //  keeps track of scale used in pattern, one set for now
//uint8_t midi_cue_main[1024] ; //holds all midi note data for now , all notes all patterns
uint16_t midi_send_time=0; // holds seq_pos for midi_send 0-2048
// pattern related

uint8_t rand_velocities[sound_set*2]; //holds a list of modified velocities
uint8_t last_note_on_channel[sound_set];  // stores last played note on non drum channels ,second page

uint8_t program_change_automation[sound_set*4];   // record a single pc per bar 4 bit for position from seq_pos and 3 bit for actual pc selected ,can be overwritten ,only 1 per bar
uint8_t lcd_control=0;  // controls type of lcd menu , used for downcount as well
uint8_t lcd_downcount=0;
uint8_t lcd_messages_select=0;//select from lcd_meesages , do not use [5] or it"ll crash
uint8_t current_accent;// current selected sound accent value
uint8_t lcd_buffer[32]; // holds outgoing characters
uint8_t lcd_buffer_mem[32]; // holds outgoing characters

uint8_t I2C_buffer[256]; // 4*32 , full page
uint8_t I2C_transmit=1; // transmit flag
uint8_t lcd_buffer_counter;// keeps track of lcd_buffer

uint8_t lcd_buffer_full; // check flag for being sent

uint8_t pitch_list_for_drums[64];  // holds 8 pitches per (first page ) 8 notes for drums for automation , used with nrpn data ,separate from pitch data for now
uint8_t pitch_selected_for_drums[8];  // current selected pitch on first page of drums 1-7 , not actual pitch but just selection, useless
uint8_t pitch_change_flag; //enabled when pitch data needs to be sent
uint8_t pitch_change_store[64];  //stores pitch changes 0-7 per bar ,per sound , 8bytes(per bar) *8 sounds ,holds presets for pitch_list_for drums for now
uint8_t pitch_selected_drum_value[8]; // holds outgoing drum value
uint8_t pitch_change_loop[8]={0,0,0,0,0,0,0,0}; //sets the looping length of pitch changes may default to pressed button as the first in step
uint8_t pitch_change_loop_position[8]={0,0,0,0,0,0,0,0};  // keeps track of pitch change loop
uint8_t pitch_change_rate[16]; // sets alt_pots playback rate , 2 bars ,4 bars , 8 bars ,16bars,32 bars

uint8_t lfo_settings_list[sound_set*2]; //holds lfo settings 1 rate , 2 level , testing for now
uint8_t lfo_full_send_enable=0; // lfo transmit flag
uint8_t single_settings_list[sound_set]; // holds single variables ie tempo for storage
uint8_t last_pitch_count[sound_set];
uint8_t overdub_enabled; // flag to keep track of overdub
uint8_t keyboard_buffer[32]; // store keyboard presses  with recorded time , maybe clear on bar if pressed again  or mute for a bar on playback

uint16_t bar_note_register[8];  //scans for data bars if none it keeps scene select light off    1 bit , 16bit-8bars
uint8_t blink_light_list[8]; // just add blink light to scene buttons

uint8_t nrpn_gating_enable;  // for gating audio effects on drum
uint8_t nrpn_gating_switch[8]; //just the levels for gating or volume
uint8_t nrpn_gating_sequence[16]={127,0,127,0,120,0,100,0,127,0,127,0,120,0,100,0};   // just a gating sequence

uint8_t control_change_flag=0;  // selects from fx list , not sounds or midi

uint8_t keyboard_step_record;   // keeps count of keys pressed once rec_arm and pause is on
uint8_t loop_screen_disable=0;
uint8_t control_change[16];   // set control change data using fx menu list
uint8_t control_change_value=71;

uint8_t current_playing_bar;   // keep record of the current playing bar , lenght to be set
uint8_t LFO_tracking_bank[64]; //stores 4 steps when run
uint8_t bar_map_0[sound_set*2];  // bit map for notes 16*16bit
uint8_t bar_map_1[sound_set]={255,255,255,255,255,255,255,255};  // bit mapped enable 1 bar
uint8_t bar_map_8[sound_set]={255,255,255,255,255,255,255,255};// bit mapped enable 8 bar
uint8_t bar_map_64[sound_set]={255,255,255,255,255,255,255,255};// bit mapped enable 8*8 bar   , for now might change to diff values
uint8_t bar_map_screen_level=0;  //selects which zoom is displayed 0-3   , 0=none 1=8 2=32
uint8_t bar_end_enable=0;  // if enabled runs everything needed on seq_pos=0
uint8_t bar_start_enable=1; // if enabled runs everything for seq_pos=255

uint8_t clear_rows=0;   // clear_top 4 rows only on USB send
uint8_t bar_map_sound_enable[sound_set]; // output from bar map editor
uint16_t bar_map_counter=0;  // keeps total bar count , getting corrupted
uint8_t alt_pots_overwrite_enable[sound_set];  // allows to overwrite alt pots value after reaching oroginal value ,then reset after a while ,maybe count down

uint8_t temp_midi_var=0;

uint8_t fx_incoming[3];
uint8_t fx_pot_settings[48]={3,71,3,74,3,5,3,76,3,77,3,78,3,79,3,80,4,72,4,5,4,76,4,77,4,74,4,94,4,71,4,15,2,71,2,72,2,73,2,74,2,75,2,76,2,77,2,78 };      //  holds midi_channel and cc assignment for 8 pots  0-15 and 0-127 , fx list cha 3 ,4,2
uint8_t fx_pot_values[48]={64,64,64,64,
							64,64,64,64,
							23,76,64,64,
							23,76,64,64,
							0,84,64,64,
							0,84,84,84,
							23,76,64,64,
							23,76,64,64
							//23,76,64,64,
							//23,76,64,64,





}; // holds on and off values as well as transition speed for both , 8 tracks for now

uint8_t fx_map_sound_enable[sound_set]={1,1,1,1,1,1,1,1}; //tracks if bar is enabled
uint8_t control_change_buf[32];  // stores control change values
uint8_t fx_counter=0;

uint8_t motion_record_buf[sound_set*8];  // holds 1 bit motion record when pressing shift and changing knob levels set elsewhere
uint8_t motion_record_limits[sound_set*2]={64,127,64,127,64,127,64,127,64,127,64,127,64,127,64,127
		}; // using for accent only atm so only using the low values , high is note_accent
uint8_t accent_bit; // keeps track of bit for motion play 0-32
uint8_t accent_bit_shift; // moves up one word

uint8_t shift_hold_enable;  // this is enabled when shift is held while a pot is turned
uint8_t shift_hold_buf[4]; // this holds the last message , for repeating at regular intervals until shift is released
uint8_t note_on_tracking_buf[8];  // keeps tracking note-ons for 40 track buttons , for multitouch etc , 1 bit
uint8_t bar_map_looping[2]={0,7}; //sets base looping length for 8 bars , can be changed
uint8_t bar_map_lights[8];  // holds bar map light values for looping display
uint8_t bar_loop_current;
uint8_t seq_play_buf[seq_play_note_count*sound_set*3*record_per_channel]; // holds  *16 note+velocity+time   , recorder with arm_rec , one bar now, maybe more than one bar later ,48bytes all with time
uint16_t seq_record_timer;
uint8_t seq_record_enable;
uint8_t transpose_octave_modifier[sound_set];   // it changes base octave and transpose notes for cdc_send2 , calculated from alt_pots  ,octave shift, make sure always above 0
int8_t transpose_pitch_modifier[sound_set];  // it changes base octave and transpose notes for cdc_send2 , calculated from alt_pots  ,-6-+6 notes , pitch shift ,
uint16_t seq_pos_out[sound_set]; // output from seq_pos_set
float seq_pos_set[sound_set]={1,1,1,1,1,1,1,1}; // seq_pos multiplier
uint8_t last_note_time[sound_set];
uint8_t seq_play_buf_time[seq_play_note_count*sound_set*record_per_channel]; // holds time values of seq_play_buf , for modification
uint16_t seq_play_buf_end[sound_set]; // records the last midi message time
uint8_t seq_pos_flag[sound_set]; // enables longer than 255 count on seq_pos
uint8_t seq_reset_flag[sound_set]; // allows reset
const char *microkorg_cc_list[]={"Portamento", "Wave", "OSC1 Control1",
		"OSC1 Control2", "OSC1 Wave", "OSC2 Mod"," ","HPF Level ","Tune ",
		"Audioin Threshold", "OSC 1 Level", "OSC 2 Level", "Noise Level", "Filter Type", "Filter Cutoff", "Filter Resonance", "Filter EG Int", "Filter KBD Track",
		"FEG Attack", "FEG Decay", "FEG Sustain", "FEG Release", "Amp Level", "Amp Panpot", "Amp Distortion", "AEG Attack", "AEG Decay", "AEG Sustain", "AEG Release", "Wave",
		"Frequency", "Wave", "Frequency", "Patch1 Intensity", "Patch2 Intensity",
		"Patch3 Intensity", "Patch4 Intensity", "Mod FX LFO Speed", "Mod FX Depth", "Delay Time", "Delay Depth", "Timbre Select", "Sync Ctrl"

};
const uint8_t microkorg_cc_numbers[43]={05, 77, 14, 15, 78, 82, 82, 18, 19, 20, 21, 22,
		22, 63, 74, 71, 79, 85, 23, 24, 25, 26, 07, 10, 92, 73, 75,
		70, 87, 87, 28, 88, 26, 28, 29, 30, 31, 12, 13, 13, 94, 95, 90};
uint8_t voice_list[sound_set]={3,1,2,0,0,0,0,0}; // tracks assigned to voice channels , related to midi channel



char lcd_char[4];

char cc_string[16];  // holds outgoing string for lcd

uint8_t note_recording_set_current[sound_set]={0,0,0,0,0,0,0,0};  // currently playing recording set , setting to 8
uint8_t short_repeat_counter[sound_set];  // counts triggered playback //1-8 bars
uint8_t short_repeat_buf[seq_play_note_count*sound_set*3*record_per_channel];// this holds temp notes when played during run
uint8_t short_repeat_time[seq_play_note_count*sound_set*record_per_channel]; // holds time values for short playback , 8 sets
uint8_t short_play_buf_end[sound_set]; // end of
uint8_t short_track_disable=0; // disables playback on track for one bar while recording to avoid notes hanging
uint8_t button_states_clear[128];
uint8_t cdc_echo[3]; // deleteuint
uint8_t cdc_to_notes[3];
uint8_t note_accent_modulate[sound_set]={127,127,127,127,127,127,127,127}; // control velocity 0-127










uint16_t sine_wave[128]={
		512, 537, 562, 587, 611, 636, 660, 684, 707, 730, 753, 774, 796, 816, 836, 855,
		873, 890, 907, 922, 937, 950, 963, 974, 984, 993, 1001, 1008, 1013, 1017, 1021, 1022,
		1023, 1022, 1021, 1017, 1013, 1008, 1001, 993, 984, 974, 963, 950, 937, 922, 907, 890,
		873, 855, 836, 816, 796, 774, 753, 730, 707, 684, 660, 636, 611, 587, 562, 537,
		512, 486, 461, 436, 412, 387, 363, 339, 316, 293, 270, 249, 227, 207, 187, 168,
		150, 133, 116, 101, 86, 73, 60, 49, 39, 30, 22, 15, 10, 6, 2, 1,
		0, 1, 2, 6, 10, 15, 22, 30, 39, 49, 60, 73, 86, 101, 116, 133,
		150, 168, 187, 207, 227, 249, 270, 293, 316, 339, 363, 387, 412, 436, 461, 486,

};
