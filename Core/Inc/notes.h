


UART_HandleTypeDef huart1;
void shift_hold_function(void); // declare first  here than it's happy
void pitch_mode(void);

void bar_map_screen(void){    // draw and modify bar_ map screens, notes as well, works good
	uint8_t note_enabled=0;
	uint8_t scene_select=scene_buttons[0]; // clear last two bits
	uint8_t i=0;
	uint32_t pointer;   // this is the actual address
	uint8_t incoming_message[3];
	uint8_t color=green_button;
	uint8_t accent_color=yellow_button;
		memcpy(incoming_message,cdc_buf2, 3); // works off only receiving buffer , this might be changing
		memset(cdc_buf2,0,3);

	//	clear_rows=1;

		uint8_t incoming_data1 = incoming_message[1]&127;

	switch(bar_map_screen_level){   // grab the pointer for the correct bar map screen
		case 0: pointer=(uint32_t)&bar_map_0;color=green_button;break;
		case 1: pointer=(uint32_t)&bar_map_1;color=red_button;break;
		case 2: pointer=(uint32_t)&bar_map_8;color=red_button;break;
		case 3: pointer=(uint32_t)&bar_map_64;color=yellow_button;break;
		default:break;}


	if ((incoming_data1>7) && (incoming_data1<40) && bar_map_screen_level) {
	// modify button from incoming

			uint8_t alt_list=	 button_states[incoming_data1 ];
			switch(alt_list){    // change state of button
			case 0 :alt_list = color;break;   //
			default:alt_list = 0; ;break; }

			button_states[incoming_data1 ]=alt_list;
			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-31

 // modify data from button state
			if (button_states[square_buttons_list[alt_list]]) {VAR_SET_BIT((pointer+(scene_select&12)+((alt_list>>3))),(alt_list&7));}
			else {VAR_RESET_BIT((pointer+(scene_select&12)+((alt_list>>3))),(alt_list&7));}

	} //end of mod
	if ((incoming_data1>23) && (incoming_data1<40) && (!bar_map_screen_level)  ) {
	// modify button from incoming

			uint8_t alt_list=	 button_states[incoming_data1 ];
			switch(alt_list){    // change state of button
			case 0 :alt_list = color;break;   //
			default:alt_list = 0; ;break; }

			button_states[incoming_data1 ]=alt_list;
			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-31

 // modify data from button state
		if(button_states[square_buttons_list[alt_list]])  {VAR_SET_BIT((pointer+(scene_select*2)),(alt_list&15));}
		else {VAR_RESET_BIT((pointer+(scene_select*2)),(alt_list&15));}


	} //end of mod
	if ((incoming_data1>8) && (incoming_data1<24) && (!bar_map_screen_level)  ) {
	// modify button from incoming
			uint32_t accent_pointer=(uint32_t)&motion_record_buf;

			uint8_t alt_list=	 button_states[incoming_data1 ];
			switch(alt_list){    // change state of button
			case 0 :alt_list = accent_color;break;   //
			default:alt_list = 0; ;break; }

			button_states[incoming_data1 ]=alt_list;
			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-31

 // modify data from button state
		if(button_states[square_buttons_list[alt_list]])  {VAR_SET_BIT((accent_pointer+(scene_select*8)+accent_bit_shift),((accent_bit&24)+(alt_list-16)));}
		else {VAR_RESET_BIT((accent_pointer+(scene_select*8)+accent_bit_shift),((accent_bit&24)+(alt_list-16)));}


	} //end of mod

// draw screen

	if (bar_map_screen_level){       // draw bars
		pointer+=(scene_select&12);
		for(i=0;i<32;i++){
			note_enabled=VAR_GET_BIT((pointer+(i>>3)),(i&7));
			note_enabled*=color;
			button_states[square_buttons_list[i]]=note_enabled;   // set light
		}}

	if (!bar_map_screen_level){   // draw notes also accent on/off
		//uint32_t accent_pointer=(uint32_t)&motion_record_buf;

		for(i=0;i<8;i++){
		//if	(VAR_GET_BIT(accent_pointer+(scene_select*2),(accent_bit))) 	 button_states[16+i]=5;
		//else button_states[16+i]=0;  // adds accent on/off bar on thrid row


		}

		pointer+=(scene_select*2);
			for(i=0;i<16;i++){
				note_enabled=VAR_GET_BIT(pointer,(i&15));
				note_enabled*=color;
				button_states[square_buttons_list[i]]=note_enabled;   // set light
			}
				//memset(button_states+8,0,16);
	}   // clear last two rows

		USB_send();  // do full clear send before new data
	} // end of bar map screen

void bar_map_tracker(void){     // creates sound enable for notes from bar_map_edit
	uint8_t enable[3];
	bar_map_counter=(bar_map_counter+1)&511;
	uint16_t counter=bar_map_counter;
	uint32_t pointer=( uint32_t)&bar_map_1;
	for(i=0;i<16;i++){
	pointer=( uint32_t)&bar_map_1;
	enable[0]=VAR_GET_BIT(pointer+i,(counter&7));
	pointer=( uint32_t)&bar_map_8;
	enable[1]=VAR_GET_BIT(pointer+i,((counter>>3)&7));
	pointer=( uint32_t)&bar_map_64;
	enable[2]=VAR_GET_BIT(pointer+i,((counter>>6)&7));
		if ((enable[1]+enable[2])!=2) enable[0]=0;

	bar_map_sound_enable[i]=enable[0]; // off by one  bar ?

	}

		}



void all_notes_off(void){
	uint8_t data[48];
	for (i=0;i<16;i++){
		data[i*3]=176+i;
		data[(i*3)+1]=123;
		data[(i*3)+2]=0;
	}
	 HAL_UART_Transmit(&huart1,data,48,100); //   send all notes off on serial port
	 HAL_Delay(100);
	 bar_map_counter=511;
	 pause=1;

}

void alt_pots_playing(void){    // shows currently playing related to alt notes selected, for visual feedback, only when clip_stop


	uint8_t note_played=last_pitch_count[scene_buttons[0]]>>3;  // grab last pitch count
	if(note_played<4) note_played+=12;


	memset(button_states+12,0,4);memset(button_states+20,0,4); // clear area
	button_states[8+note_played]=3;



}


void record_overdub(void){ // runs only when triggered by keyboard , wipes old data but only when triggered

	uint16_t drum_byte_select;
	uint16_t clear_select;
		uint8_t drum_byte; // note on time data
		uint8_t selected_scene=scene_buttons[0];  // this wiil change to pattern select 0-15
	uint8_t time=seq_step; // 0-15  , this might need to change to a stored time as it's not accurate


				drum_byte_select= (time>>2)+(selected_scene*4)+(bar_playing*drum_store);  // select byte position from  seq_pos
				clear_select=(selected_scene*4)+(bar_playing*drum_store);
				drum_byte=drum_store_one[drum_byte_select];  // select byt

				if (overdub_enabled==1) {memset(drum_store_one+clear_select,0,4);overdub_enabled=2;} //clear selected bar data , 4 bytes


				drum_byte |=(1<<((time&3)*2));  // write note position , set with or
				 button_states[square_buttons_list[time]]=yellow_button ;  // puts on a light , not ness
				 drum_store_one[drum_byte_select]=drum_byte;

}
void top_bar(uint8_t input){ // red bar function for top row

	if (input>7){
	memset(button_states+32,0,8);loop_screen_disable=0;}
	else button_states[32+input]=3;



}

void step_recording(void)  {    // write keyboard notes to alt_pots  , but only during pause and arm_rec

	top_bar(keyboard_step_record);
	uint8_t selected_scene=scene_buttons[0];
	if (keyboard_step_record>7) { keyboard_step_record=0;  button_states[rec_arm_button]=0;rec_arm=0;
	} //reset rec_arm
	else{		alt_pots[((selected_scene-8)*8)+keyboard_step_record]=(keyboard[0]);
	alt_pots_octave[selected_scene]=(keyboard[0]>>2)/3;  // saves octave
	keyboard_step_record++;}


	}
void midi_cue_fill(void){


	uint8_t d;
	uint8_t n;

	 for (d=0;d<16;d++) {   // load up midi_cue
		//pattern_select=d;

		for (n=0;n<128;n++) {

			midi_send_control();
		}
		}


}


void play_muting(void){    // all muting stuff here , sometimes it loses data , disable


	}
void patch_screen(void)		{     // shows last loaded patch and save patch as well , save=blink
	for (i=0;i<32;i++) {//blank
			button_states[i+7]=0;
		}
		//	button_states[square_buttons_list[load_patch]]=3;   // light currently loaded
			button_states[patch_save+8]=4;   // blink postion to be saved
			//all_update=1;
	}

///  gonna do away with all  this
void loop_screen(void){  // always on now ,produces lights for notes,  16 notes and 8 bars , full redraw  , notes only   might remove this

	}



void note_buttons(void){  // always running only on notes though , might dump it


			for (i = 0; i < 40; i++){   // test for bad data
				if (button_states[i]>10) button_states[i]=0;

			}


	if ((!pan) && (!clip_stop)) {
		{if (down_arrow) fx_map_screen();else bar_map_screen();}

	}
if (clip_stop) pitch_mode();

		}

void buttons_store(void){    // incoming data from controller

	//uint8_t vel=pot_states[0]>>3;  // testing
	uint8_t alt_list;
	uint8_t scene_select=0;

	uint8_t incoming_message[3];
	//uint16_t drum_byte_select;   // selects a trigger 16 + (i*4) 16*64 ... 0-256
	//uint8_t drum_byte;
	// uint16_t pattern=bar_playing;
	 //uint8_t offset_pitch=seq_pos>>3;

	memcpy(incoming_message,cdc_buffer+cdc_start, 3); // works off only receiving buffer

	uint8_t buffer_clear = 0;
	uint8_t incoming_data1 = incoming_message[1]&127;
	uint8_t status=incoming_message[0];
	uint32_t accent_pointer=(uint32_t)&motion_record_buf;
	uint16_t clear[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t current_scene=scene_buttons[0];

//	uint16_t drum_store_select=(current_scene*4)+(bar_playing*drum_store);
	incoming_message[2]=(incoming_message[2]&127);
	//drum_byte_select = (offset_pitch >> 2) + (i * 4) + (pattern * drum_store); //based on time,  selects a trigger 16 + (i*4) 16*64 ... 0-256 (64bytes per pattern, 4 bytes per per )
	//drum_byte = drum_store_one[drum_byte_select]; // this could be smaller



	if (status == MIDI_NOTE_OFF) // note off
	{  note_off_flag[0]=0;


	if (incoming_data1==shift_button)    // shift related functions
	{shift=0;
	shift_hold_function();
	play_list_write=0;
	write_velocity=0;}
	}   // note off buttons

	// skip for now

	if ((incoming_data1 <8)&& (status==MIDI_NOTE_ON)) {   // scene buttons
		scene_select=incoming_data1 +1+second_scene;}  //enable scene_select section


	//if (record && keyboard[0] && (keyboard[0]<128)) record_overdub(); // only on note on , enable overdub
	if (status == MIDI_NOTE_ON){  // Midi Note on  for buttons

		note_off_flag[0]=1;note_off_flag[1]=incoming_data1 ;
		if (incoming_data1==98) shift=1;

		if ((incoming_data1>40)&&(incoming_data1 <99)  ){				// button lights, extras
			alt_list=	 button_states[incoming_data1 ];
			switch(alt_list){    // change state
			case 0 :alt_list = 1;break;   // normal lights , yellow
			case 5 :alt_list= 0;break;		// green
			case 1 :alt_list = 0;  ;break;
			default:alt_list = 0; ;break; }


			button_states[incoming_data1 ]=alt_list;
		}
/*		if ((incoming_data1>23)&&(incoming_data1 <40)  ){				// button lights, notes  , might replace with a separate function
			alt_list=button_states[incoming_data1];

			switch(alt_list){    // change state
			case 0 :alt_list = 5;break;   // normal lights , yellow
			default:alt_list= 0;break;

			//case 5 :alt_list= 0;break;		// green
			//case 3 :alt_list = 0;  ;break;
			//case 5 :button_states[incoming_data1 ] = 3;break; }
			}
			button_states[incoming_data1]=alt_list;

		}*/
		if ((incoming_data1 > 7) && (incoming_data1 < 40))   // do all 4 top rows
						{memcpy(cdc_buf2,incoming_message, 3); // copy current message
						note_buttons(); }  // send to buttons input


		// function select logic  here , change from if-then

		//resets


		if (button_states[incoming_data1] && (incoming_data1>40))  //only test if button is switched on
		{
		switch(incoming_data1){

		case up_arrow_button:   // sends bum notes when pressed
		 if ((scene_buttons[0]<8)) 	{memset(button_states,1,8);{if (down_arrow) fx_map_screen();else bar_map_screen();}

		// scene_buttons[0]=scene_buttons[0]+8;
		 second_scene=8;

		 } else
		 {memset(button_states,1,8);{if (down_arrow) fx_map_screen();else bar_map_screen();} }
		break;

		case solo_button: {scene_solo=1;clip_stop=0;button_states[82]=0; } //solo but with pots now ,turns off clip stop or mute
		 if (shift) memset(mute_list,0,16); // clear all mute info when shift held
		break;
		case right_arrow_button:right_arrow=1; {if (bar_map_screen_level<3) bar_map_screen_level++; }


		{if (down_arrow) fx_map_screen();else bar_map_screen();}
		right_arrow=0;button_states[right_arrow_button]=0;break;//zoom out
		case select_button: {select_bn=1;} ;break; // select enable
		case down_arrow_button: down_arrow=1;fx_map_screen();//
		break;
		case left_arrow_button: {left_arrow=1;  {if (bar_map_screen_level) bar_map_screen_level--; }	   }

		{if (down_arrow) fx_map_screen();else bar_map_screen();}
		left_arrow=0;button_states[left_arrow_button]=0;
		break;// zoom in

		case mute_button:scene_mute=1;break;
		case record_button:record=1;overdub_enabled=1;break;
		case volume_button:volume=1;break;
		case pan_button:pan=1;patch_screen();break;
		case send_button:send=1;break;
		case rec_arm_button:rec_arm=1;break; // set up for step record
		case device_button:{device=1;memset(button_states+24,0,16);button_states[31+(current_midi&7)-((current_midi>>3)<<3)]=yellow_blink_button;}break;// shows midi channel , for now
		case stop_all_clips:  {button_states[play_pause_button]=5;pause=5; seq_step=0;seq_step_long=0;play_position=0;bar_selector=0;button_states[stop_all_clips]=0; all_notes_off();
		memset(LFO_tracking_counter,0,64);memset(last_pitch_count,0,16); memset(last_note_on_channel,0,16);current_playing_bar=0;  }// stop all clips, pause and reset to start
		break;

		case play_pause_button:pause=3;break;
		case clip_stop_button: {clip_stop=1; lcd_downcount=10;lcd_messages_select=3;scene_solo=0;button_states[83]=0; bar_map_screen_level=0; } break;
		default:break;


		}
		if(clip_stop) bar_map_screen_level=0;  // disable function for pitch_mode
		}


		if ((!button_states[incoming_data1]) && (incoming_data1>40))  //only test if button is getting switched off, may not as reliable but quicker
			{
			switch(incoming_data1){

			case up_arrow_button:up_arrow=0;if ((scene_buttons[0]>7)) {
				memset(button_states,1,8);
				//scene_buttons[0]=scene_buttons[0]-8;

				second_scene=0;
				loop_screen();}else{ memset(button_states,1,8);}  ;break;
			case right_arrow_button:right_arrow=0;  break;//zoom out
			case select_button: {select_bn=0;} ;break; // select enable
			case down_arrow_button:down_arrow=0;bar_map_screen(); break;
			case left_arrow_button: left_arrow=0; break;// zoom in
			case mute_button:scene_mute=0;break;
			case record_button:record=0;overdub_enabled=0;break;
			case volume_button:volume=0;break;
			case pan_button:pan=0;break;
			case send_button:send=0;break;
			case rec_arm_button:rec_arm=0;break;
			case device_button:device=0;break;// shows midi channel , for now
			case play_pause_button:{pause=0;button_states[stop_all_clips]=0;}break;
			case clip_stop_button: {clip_stop=0;bar_map_screen();} break;
			default:break;


			}
			}




		} // end of Note on for all buttons


	if ((status == CC_Message) && (clip_stop)&& (current_scene>7)){   // pitch_mode enabled pto functions ,

		// add extra pot functions here

		switch(incoming_data1){

		case pot_3:
			if (current_scene>7) pitch_change_rate[current_scene]=8-(incoming_message[2]>>4);
			 ;break;//
		default:break;
		}
		//uint8_t continue_write=0;
/*
		if(last_pot_used==(incoming_data1-47))    // only if using the same pot as last time

		{			// might disable this function

			if(((old_pitch>(incoming_message[2]+2)) && ((old_pitch<(incoming_message[2]+10))))  ||  (((old_pitch+2)>incoming_message[2]) && (((old_pitch+10)<incoming_message[2])))  )
						button_states[clip_stop_button]=green_blink_button;   // set to green blink  when close

			if 	( (old_pitch==incoming_message[2])) {alt_pots_overwrite_enable[current_scene]=1;}

			if (alt_pots_overwrite_enable[current_scene] )  {alt_pots[(incoming_data1  - 48)+((current_scene-8)*8)] =((incoming_message[2]));button_states[clip_stop_button]=green_button; }




		} else {alt_pots_overwrite_enable[current_scene]=0;button_states[clip_stop_button]=green_blink_button;}


		last_pot_used=(incoming_data1-47); // save last pot used
*/



		status=0; // clear
	}

/*	if ((status == 176) && (scene_solo)&& (incoming_data1<pot_5)){   // solo processing
		uint8_t solo_selector=(incoming_data1-48)*4;
		incoming_message[2]=(incoming_message[2]>>5)&3;

			memset(mute_list+solo_selector,1,4);mute_list[incoming_message[2]+solo_selector]=0;
		status=0; // clear
	}*/


	if (status == CC_Message) {//  controller data , store pot , clip stop off

		if(shift) shift_hold_function();

		pot_states[incoming_data1  - 48] = (incoming_message[2]); // store pot all  // not always ok


		if (pan) {
			patch_save=pot_states[7]>>4;   // set next memory to be save d or loaded
			patch_screen();
		}

		if (!down_arrow){     // down arrow disabled ,default screen
		switch(incoming_data1){   // pots data selector ,default screen

		case pot_1:

		if ((shift)){


		}
		//else 	{LFO_low_list[current_scene]=((pot_states[0]>>4))&7;lcd_downcount=3;lcd_messages_select=6;}
		else  {pitch_change_loop[current_scene]=pot_states[0]>>4;}    // sets number of notes on pitch chnage


		break;

		//if ((select_bn) && (current_scene>3))  //  cc function
		//{midi_cc_cue[0] =midi_channel_list[current_scene]+176; midi_cc_cue[1] =incoming_message[2]; };break; // sets notes playing only on these bars

		case pot_2:LFO_high_list[current_scene]=((pot_states[1]>>4))&7;lcd_downcount=3;lcd_messages_select=6;break; // sets notes playing only on these bars

		case pot_3:	if ((!select_bn) && (current_scene < 8)&& pause )   // only under pause
		{pitch_list_for_drums[(pitch_change_loop_position[current_scene])+(current_scene*8)] =incoming_message[2];//pitch_change_flag=1;
		pitch_selected_drum_value[current_scene]=incoming_message[2];   // this actually sends data for nrpn
		lcd_downcount=3;lcd_messages_select=7;
		pitch_change_flag=1+current_scene; }  // enable nrpn send
		if ((current_scene>7) && (!clip_stop)) pitch_change_rate[current_scene]=8-(incoming_message[2]>>4);
		 ;break;// sets pitch for drums ,only first page

		case pot_4:{pattern_scale_list[current_scene]=(pot_states[3]>>3)&15;lcd_downcount=3;lcd_messages_select=3;};break;
		case pot_5: 	if ((!device)&& (!clip_stop)) {lfo_settings_list[(current_scene*2)]=incoming_message[2];lcd_downcount=3;lcd_messages_select=9;} ;break;  // lfo rate
		case pot_6: if ((!device) && (!clip_stop))   {lfo_settings_list[(current_scene*2)+1]=incoming_message[2] ;lcd_downcount=3;lcd_messages_select=10;} ;break;  // lfo level

		case pot_7:	if ((shift) && (device)) 		 {midi_channel_list[current_scene]=(incoming_message[2]>>3);current_midi=midi_channel_list[current_scene]+1;lcd_downcount=3;lcd_messages_select=4;
		memset(button_states+24,0,16);button_states[31+(current_midi&7)-((current_midi>>3)<<3)]=yellow_blink_button;};break;   // sets midi channel on selected sound

		case pot_8:
			if ((shift) && (device))		{timer_value=bpm_table[incoming_message[2]+64]; tempo=incoming_message[2]+64;} //tempo
			if ((!device)&&(!shift)) {note_accent[current_scene]=incoming_message[2];rand_velocities[current_scene]=incoming_message[2];     // accent input
		lcd_control=1;lcd_downcount=3;lcd_messages_select=8;current_accent=pot_states[7];}  // accent also used for tempo with shift
		if (shift && (!device)){
			if (incoming_message[2]>64) (VAR_SET_BIT(accent_pointer+(current_scene*8)+accent_bit_shift,(accent_bit))); else (VAR_RESET_BIT(accent_pointer+(current_scene*8)+accent_bit_shift,(accent_bit)));  // record motion with shift held

		}

			;break;
		default:break;

		}

		}

		if (down_arrow){      // with down arrow enabled fx menu section ,pot settings

			fx_incoming[1]=incoming_data1;  // selects which pot
			fx_menu(fx_incoming[1]);  // saves pot settings for fx

			}




		//	if ((note_off_flag[0])&& (note_off_flag[1]<32))  scene_velocity[square_buttons_list[note_off_flag[1]]+(scene_buttons[0]*32)]=  pot_states[1];    // set velocity for now for held button , only for notes


/*			if ((incoming_data1==pot_3) && (!keyboard[0]))  // if held down

		{

			scene_transpose[current_scene]=pot_states[2]>>1; // 0-64 transpose from base , only with shift off , should trigger a note or loses track

			if (shift)  play_list_write=1; // enter to play list when enabled

		}*/

		//if ((incoming_data1==pot_2) && (button_states[68]))   scene_velocity[seq_step_pointer]=  (((pot_states[1]>>5)<<5)+31)&112;  // update velocity in realtime if volume button pressed

	} // end of CC (176, pots ) control change message



	if (scene_select)  { // change scene select lite , one at a time though , fully update so need for extra sends
		scene_select=scene_select-1;
		//uint8_t clear_green[8]= {1,1,1,1,1,1,1,1};
		memset(alt_pots_overwrite_enable,0,sound_set );  // clear alt pots edit
		memset(button_states,1,8); ;  // turn green
		for (i=0;i<8;i++){
		if(scene_select<8)	{if (mute_list[i]) button_states[i]=3;}
		if(scene_select>7)	{if (mute_list[i+8]) button_states[i]=3;}

		}
		button_states[scene_select&7]=5;
		if (scene_mute) { // muting control

			{if (mute_list[scene_select]) {mute_list[scene_select]=0; button_states[scene_select&7]=5;} else {mute_list[scene_select]=1; button_states[scene_select&7]=3;}}

		}



			scene_buttons[0]=scene_select;
			 current_midi=midi_channel_list[scene_buttons[0]]+1;

			 //loop_selector=1; // redraw
	
		scene_select=0;
		if (!clip_stop) {if (down_arrow) fx_map_screen();else bar_map_screen();}
		if (clip_stop)  pitch_mode();


	}// end of scene select
	buffer_clear = 1;
	//loop_screen();  // always run except when device on ,except when keyboard step
	if (buffer_clear)
		memcpy(cdc_buffer+cdc_start, clear, 3);


	} // end of button store



void note_replace(uint8_t note_replace) {    // replace first note

	if (note_replace)

	{
		note_bank[0] = 1;
		note_bank[1] = note_temp[1];
		note_bank[2] = note_temp[2];
		note_temp[0] = 0;
		note_temp[1] = 0;
		note_temp[2] = 0;
		note_replace_enable=0;
	}

}





void pattern_settings(void){     // pattern change function , also program change automation, change to 2 for now , using 4bytes *8 per sound (512 bytes) instead
//	uint8_t current_scene=scene_buttons[0];
	//uint8_t pc_set=0;
//	if (seq_step<8)  pc_set=program_change_automation[seq_step_long]&15; else pc_set=(program_change_automation[seq_step_long])>>4;  // modifies program change  twice during 16 notes



///////////bar section


/////////////pitch section

	if ((seq_pos==0) && (!select_bn)&& (!pause)){     // apply pitch to drum notes on first step or when pressed  ,however buggy ,also only send on change ?


		//uint8_t pitch_byte=pitch_change_loop_position[current_scene]; // current pitch selected
	/*	if (clip_stop) memset(button_states+16,0,4);
		else memset(button_states+16,0,8);
		button_states[pitch_byte+16]=yellow_button;*/


		for(i=0;i<8;i++){   // loads up pitch values
		//uint8_t pitch_byte_select=pitch_change_loop_position[i]+(i*8);	// 0-3 + 0-28


		//uint8_t pitch_byte=pitch_change_store[pitch_byte_select]&7; //spits out selected pitch ,first byte seems to be changing ?

		uint8_t pitch_byte=pitch_change_loop_position[i];
	//	if (punch_in[0] && (i==current_scene)) pitch_byte=punch_in[0]-1;
		uint8_t pitch_final=(pitch_list_for_drums[pitch_byte+(i*8)])&127; // there should always some data


		if (pitch_final<10) pitch_final=64; // in case corrupt or missing
		if (pitch_final) pitch_selected_drum_value[i]=pitch_final;  // if missing data keep old value
		if(pitch_change_loop_position[i]<pitch_change_loop[i])         pitch_change_loop_position[i]=pitch_change_loop_position[i]+1; else pitch_change_loop_position[i]=0; // loop pitch change

		}
		pitch_change_flag=1;


	}


	if (seq_step==15){



	} //end of s_temp


	}

void shift_hold_function(void){   // this will autofill tracks with the last setting if shift is held down while a pot is turned

	if(shift_hold_enable){
		memcpy(cdc_buffer,shift_hold_buf,3);
	buttons_store();
	}
	if (shift && (cdc_buffer[0]==CC_Message)&& (!shift_hold_enable)) {shift_hold_enable=1;memcpy(shift_hold_buf,cdc_buffer,3);}

	if(!shift) { memset(shift_hold_buf,0,3); shift_hold_enable=0; } // clears buffer

}

void pitch_mode(void){  // changes display when in pitch mode , maybe diff note input too,entry only in pause

	uint8_t selected_scene=scene_buttons[0];

	uint8_t current_pitch_step=last_pitch_count[selected_scene]>>3;// shows selected pitch step 0-7
	uint8_t current_pitch_playing;
	uint8_t current_octave;
	uint8_t alt_pots_selected= (current_pitch_step)+((selected_scene-8)*8);
	uint8_t incoming_message[3];
	uint8_t incoming_data1=0;


	if(selected_scene>7) current_pitch_playing=alt_pots[(current_pitch_step)+((selected_scene-8)*8)];   // only works on last 8 scenes/keys
	current_octave=(current_pitch_playing>>2)/3;
	current_pitch_playing=current_pitch_playing-(current_octave*12);


	if (pause) { // pitch entry during pause

		//uint8_t accent_color=yellow_button;
		memcpy(incoming_message,cdc_buf2, 3); // works off only receiving buffer , this might be changing
		memset(cdc_buf2,0,3);
		incoming_data1 = incoming_message[1]&127;
		if (keyboard[0] && keyboard[1] ) { // pitch from keyboard

			alt_pots[alt_pots_selected]=keyboard[0];
			current_pitch_playing=keyboard[0];
			current_octave=(current_pitch_playing>>2)/3;
				current_pitch_playing=current_pitch_playing-(current_octave*12);
				if(rec_arm) last_pitch_count[selected_scene]=((current_pitch_step+1)*8)&63; //step recording

		}

		if ((incoming_data1>23) && (incoming_data1<40)) { //select pitch of the currently paying note , only in pause
			// modify button from incoming, no toggle
			uint8_t alt_list=0;
			button_states[incoming_data1 ]=green_button;
			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-15
			if (button_states[square_buttons_list[alt_list]]) alt_pots[alt_pots_selected]=alt_list+(current_octave*12);

		} //end of mod


	if ((incoming_data1>7) && (incoming_data1<16)) { // pitch step selector 0-7 , no toggle

									button_states[incoming_data1 ]=red_button; //sets colour
									last_pitch_count[selected_scene]=(incoming_data1-8)<<3;


	} //end of mod
	} // end of pause

	memset(button_states+8,0,32);
	if(current_pitch_playing<8) button_states[32+current_pitch_playing]=1;  // shows current pitch on first two bars
	else button_states[24+current_pitch_playing-8]=1;

	button_states[8+(current_pitch_step)]=3;  // shows current pitch step on forth bar

	 USB_send();  // do full clear send before new data
}





