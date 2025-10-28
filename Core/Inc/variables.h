
#define cpu_clock    96000000
#define prescaler 192   // TIM10 values
#define ppq_set 192   // TIM10 values
#define drum_store 64  // scene_select  *4
#define sound_set 16   // define selectable sounds for scene_buttons[0]  , important
#define MIDI_NOTE_OFF 128
#define MIDI_NOTE_ON 144
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
#define variable_count 14   // for flash mem

#define pot_1 48
#define pot_2 49
#define pot_3 50
#define pot_4 51
#define pot_5 52
#define pot_6 53
#define pot_7 54
#define pot_8 55






//1 is default or green, 2= default or green blink, 5=is yellow, 6 =yellow blink , 4 =red blink, 3=red,
uint16_t bpm_table[256];  // lut values for temp TIM10 ARR
uint8_t tempo; // tempo value
uint8_t ppq_count; // ppq_count for seq_pos
uint16_t skip_counter; // keeps track of ppq skip
volatile uint8_t ppq_send; // ppq_counter
volatile uint8_t skip_enable; // allow ppq toggle skip
volatile uint8_t skip_setting; // slow or faster 3,1,0



//buttons
uint8_t volume; // volume button
uint8_t pan;
uint8_t shift; // track shift button
uint8_t pause; // enable pause mode
uint8_t select_bn; //select button
uint8_t right_arrow;
uint8_t left_arrow;
uint8_t up_arrow;
uint8_t down_arrow;

uint8_t device;
uint8_t clip_stop;
uint8_t send;
uint8_t record;
uint8_t sustain;

uint8_t serial1_temp;
uint8_t temp;

uint8_t serial1_hold[10];
uint8_t serial1_hold2[10];
uint8_t serial1_counter;
uint16_t i;  // always 16 bit or higher
uint16_t n;
uint8_t led_blink;
uint8_t note_bank[18];   //contains note on and off info
uint8_t last_note=0; // 1-3  last note played on note bank
uint8_t last_key=0;  // last actual key played
uint8_t clear[3]={0,0,0};
uint8_t led_clear=1; // track claearing leds

uint8_t note_temp[3]={0,0,0};   // holds current note being filled
uint8_t note_replace_enable=0;
uint8_t note_length_list[10];   // keeps note count for note off
uint8_t program; // current loaded program pointer
uint8_t patch_load; // patch to be loaded 0-15
uint8_t patch_save;  //  patch save  0-15
char print_out[10][3];
uint8_t send_all[128]; //scene sends
uint8_t send_buffer[34]={144,5,3,144,5,3,144,0,0}; // light off, light on , scene light off ,only for controller,then midi
volatile uint16_t seq_pos;  // 16 bit  , 24/quater or 8/step  on 1/16th , sequencer clock, rarely used might change
uint16_t s_temp;
uint16_t seq_pos_mem=1;
uint16_t mtc_tick=0;  // incoming realtime clock counter
uint8_t seq_enable=1;  // start stop sequencer
uint8_t seq_step; // 0-15 steps,paused
uint8_t realtime;


volatile uint16_t timer_value=512; // sets timer ccr  def is 1100 for now
uint16_t seq_tmp;
uint8_t cdc_buffer[12];  // receive buffer on sub , check usbd_cdc_if.c and h for more
uint8_t cdc_send_cue[260];   //hold from cdc
uint8_t cdc_len=1;
uint32_t Len;
volatile uint8_t button_send_trigger;  // send button midi repeat
uint8_t trigger_mem;
uint8_t lfo; // temp storage
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
const uint8_t drum_list[sound_set] ={48,50,52,41,43,60,61,42,46,47,45,30,30,30,30,30}; // notes played for drum scenes

uint32_t sys_cnt[3];
uint8_t note_off_enable;
uint8_t settings_write_flag=0;
uint8_t cdc_start=0;  // checks for extra messages
//bar section
uint8_t bar_selector; // select bar for editing
uint8_t bar_playing; // currently playing bar , 16 notes per bar
uint8_t punch_in[8]; // allow instant response for certain buttons



uint8_t counter_a;
// SPI stuff
uint8_t spi_send[10];
uint8_t status_reg[2];
uint8_t first_message=0; // flag to clear once a button is pressed
uint8_t rec_arm=0;
uint8_t last_note_played[16]; // stores note for note off

uint8_t flash_flag=0;
uint8_t flash_read_block2[260] ={1,1,1,1,1,1,1,0}; // this should clearif flash read works
uint8_t send_spi2[260];
uint8_t write_once; // allow only a single flash write for now
uint8_t test_data[32]={0,0,0,0,1,0,5,1,1,0,1,5,1,1,0,1,1,3,0,1,1,0,1,0,1,0,1,0,1,0,1};
uint8_t spi_hold[260]={0,10,0,0};
uint8_t all_settings[1024];  // store all extra settings:  transpose , pots
uint8_t other_buttons; // update control button lights
uint8_t other_buttons_hold[100]; // keeps track of buttons
uint8_t send_buffer_sent;
uint8_t button_states_save[100]; // reference for button changes for controller , not in order



//uint16_t loop_note_list[sound_set]; //tracks currently playing note position

uint8_t loop_note_count[sound_set];  // keeps track of number of notes in a loop
uint8_t loop_current_speed;
uint8_t loop_lfo_out[sound_set*3];  // used for some level of lfo using pot7 for now 0-255
// uint8_t lfo_settings[sound_set*3];  // lfo 0-8   rate , gain,offset, target
uint8_t alt_pots[sound_set*16]; // stores a set of alt pot settings , 8 bytes per bank , 8 banks , also holds extra data > 128
uint8_t alt_pots_selector=0; // changes alt_pots banks (8bytes each )


uint8_t seq_step_mem;  // mem for looper


uint8_t pot_tracking[33] ; // record pot movements , maybe after 1 bar ,only transpose for now
uint8_t mute_list[sound_set]; //track scene mutes
uint8_t noteoff_list[25]; //track note offs sorted 0-7 , empty when not in use
//notes
uint8_t scene_transpose[sound_set];

uint8_t last_button;

uint8_t scene_mute; // muting
uint8_t last_incoming;
uint8_t scene_solo; //enable solo mode
uint8_t stop_toggle=0; // use it for pause

uint8_t seq_step_list[20]; //store seq_step position per part  .for now just notes 4-8
uint16_t seq_step_fine[sound_set];  // holds high count for seq_step_list   *8 res
uint8_t seq_current_step; // current position on selected from seq_step_list
uint8_t seq_step_reset[sound_set];  // tracks when seq_step_list reset to start
uint8_t seq_step_long; // 16*32
uint8_t seq_step_enable[sound_set]; // step change tracking

uint8_t note_enable_list[sound_set]; //  plays note one every X number of bar

uint8_t note_enable_list_counter[sound_set]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // keeps count for the note enable list
uint8_t note_enable_list_selected; // just current note neable
uint8_t LFO_low_list[sound_set*4]; // sets period low ,LFO bar count, *4 sets for various things, 1=note on/off  2=change pitch maybe
uint8_t LFO_high_list[sound_set*4]; // sets high period
uint8_t LFO_phase_list[sound_set*4];// 0-1 , might put it elsewhere
uint8_t LFO_tracking_counter[sound_set*4]; //keeps track of LFO ,counts 0-7 then flips to 15-23 depending on settings
uint8_t LFO_tracking_out[sound_set*4]; // LFO output




uint8_t loop_length;

uint8_t serial_out[96]; // holds midi out data
uint8_t serial_len;
uint8_t midi_channel_list[21]={9,9,9,9,9,9,9,9,9,9,9,9,3,3,3,3,3,3 };   //holds midi channel settings 0=1 (midi channels 1-16)
uint8_t nrpn_cue[80]={186,99,5,186,98,16,186,6,32};  // stores message for nrpn on es1 only needs 1 initial c99=5  then only  2 bytes repeating  CC 98 =NRPN LSB and CC 6 =value , for now 9 bytes though  , initial normal 3 bytes then convrted to 9
uint8_t pitch_lut[127] ={0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,12,14,16,18,20,22,24,27,30,33,36,39,42,45,48,51,54,57,64,0,2,4,6,8,10,12,14,16,18,20,22,24,27,30,33,36,39,42,45,48,51,54,57,64,70,73,76,79,82,85,88,91,94,97,100,103,105,107,109,111,113,115,117,119,121,123,125,127,64,70,73,76,79,82,85,88,91,94,97,100,103,105,107,109,111,113,115,117,119,121,123,125,127};   // es1 pitch table  4 octaves
uint8_t es_filter[9]; // track es1 filters  old and new values  say 4+4
uint8_t es_filter_cue[20];   // hold filter data for nrpn
uint8_t midi_cc[sound_set]; // enabled if sending midi cc
uint8_t midi_cc_cue[20];  //  0=part+1 ,1 is value
uint8_t midi_extra_cue[30] ; // extra stuff to be sent that ins't regular [28] is length
uint8_t current_midi; // holds selected channel 1-16
uint8_t note_accent[sound_set]={127,127,127,127,127,127,127,127,127};  // stores accent data for tracks on/off

uint8_t play_list[257]={15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};  // holds playback muting order 0-7,8-15,16-23,24-31  for 8x 4 bars  on all parts maybe the 4 bit LSB ,only needs 8 bytes per part, change to hold transpose
uint8_t play_screen=0;  // enable for secondary screen for muting setup
uint8_t play_position;  // track muting list 8*4     each 8 steps  +1
uint8_t play_list_write=0; // keeps writing while shift is held down
uint8_t write_velocity;  // keeps writing velocity while enable , holds velocity value as well
uint8_t looper_list[sound_set*4]={0,31,0,8,0,31,0,8,0,32,0,8,0,32,0,8,0,32,0,8,0,32,0,8,0,32,0,8,0,32,0,8,0};  // holds looper settings , 0=start offset 2=vel offset 3=lfo level  4=speed  ,obsolete for now
uint8_t looper_list_mem[20];  // keeps track of previous values for ppq skip
uint8_t loop_current_offset;
uint8_t loop_current_length;
uint8_t step_record; // works in stop mode
uint8_t drum_store_one[1024]; // holds drum notes 2 bit , note one and accent , 4bytes per scene ,16 scenes in series (64bytes) , then bar 2

uint8_t test_byte[64];
uint8_t test_byteB[64];
uint8_t second_scene=0;  // select second set of sounds
uint8_t cdc_len_temp;
uint8_t program_change[2]; // hold current prograam change for 2 channels
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

uint8_t pattern_scale_list [16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //  keeps track of scale used in pattern, one set for now
uint8_t pattern_copy_full=0;
uint8_t midi_cue[8192];  // data cue for one entire pattern now 2*16*16*16 in total (assuming evey note is on )

uint16_t midi_cue_time[2048];  // holds actual time for  midi note messages in midi_cue_main 0-2047 for now
uint8_t midi_cue_size[2048]; // holds message size (usually 4 for now )
uint16_t midi_cue_loc[2048]; // holds data location ( *4  )

//uint8_t midi_cue_main[1024] ; //holds all midi note data for now , all notes all patterns
uint16_t midi_send_time=0; // holds seq_pos for midi_send 0-2048
uint16_t midi_send_current=0; // tracks data entered not cue_counter   ,reset on pattern change ?
uint16_t midi_cue_count=0; //tracks the number of notes recorded
// pattern related
uint8_t last_pattern_select;
uint8_t pattern_rewind;  // +1
uint8_t new_pattern_select;
uint8_t pattern_select=0; // disabled for now , will have 2
uint8_t pattern_offset_list[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};   // list of offset for a single part (keys only ) on each pattern , handy
uint8_t pattern_offset; // for lcd
uint8_t pattern_copy[32];  // buf for copypasta
uint8_t pattern_loop;  // pattern select counter from pattern count
uint8_t pattern_loop_repeat; // counts only currently playing pattern
uint8_t pattern_repeat[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // sets the number of times to repeat before stepping
uint8_t pattern_count=0; // sets the pattern total repeating size
uint8_t pattern_start=0; //sets start position of pattern playback
uint8_t rand_velocities[32]; //holds a list of modified velocities
uint8_t last_note_on_channel[16];  // stores last played note on non drum channels ,second page
uint8_t last_note_end_count[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; // counts down for note off , second page
uint8_t program_change_automation[32];   // record a single pc per bar 4 bit for position from seq_pos and 3 bit for actual pc selected ,can be overwritten ,only 1 per bar
uint8_t lcd_control=0;  // controls type of lcd menu , used for downcount as well
uint8_t lcd_downcount=0;
uint8_t lcd_messages_select=0;//select from lcd_meesages , do not use [5] or it'll crash
uint8_t current_accent;// current selected sound accent value
uint8_t lcd_buffer[32]; // holds outgoing characters
uint8_t lcd_buffer_mem[32]; // holds outgoing characters
uint8_t lcd_delay_counter; // counts to 32 for sending
uint8_t I2C_buffer[256]; // 4*32 , full page
uint8_t I2C_transmit=1; // transmit flag
uint8_t lcd_buffer_counter;// keeps track of lcd_buffer
uint8_t last_solo_selected=0;  // saves last selected solo button

uint8_t pitch_list_for_drums[64];  // holds 8 pitches per (first page ) 8 notes for drums for automation , used with nrpn data ,separate from pitch data for now
uint8_t pitch_selected_for_drums[8];  // current selected pitch on first page of drums 1-7 , not actual pitch but just selection, useless
uint8_t pitch_change_flag; //enabled when pitch data needs to be sent
uint8_t pitch_change_store[64];  //stores pitch changes 0-7 per bar ,per sound , 8bytes(per bar) *8 sounds ,holds presets for pitch_list_for drums for now
uint8_t pitch_selected_drum_value[8]; // holds outgoing drum value

uint8_t lfo_settings_list[32]; //holds lfo settings 1 rate , 2 level , testing for now
uint8_t lfo_full_send_enable=0; // lfo transmit flag
uint8_t single_settings_list[16]; // holds single variables ie tempo for storage
uint8_t last_pitch_count[16];
uint8_t overdub_enabled; // flag to keep track of overdub
uint8_t keyboard_buffer[32]; // store keyboard presses  with recorded time , maybe clear on bar if pressed again  or mute for a bar on playback

typedef struct { uint8_t *var; char *name;} LCD_item;    // creates lcd menu items to make it a bit easier
LCD_item lcd_item[12];
uint16_t bar_note_register[8];  //scans for data bars if none it keeps scene select light off    1 bit , 16bit-8bars
uint8_t blink_light_list[8]; // just add blink light to scene buttons
uint8_t nrpn_gating_enable;  // for gating audio effects on drum
uint8_t nrpn_gating_switch[8]; //just the levels for gating or volume
