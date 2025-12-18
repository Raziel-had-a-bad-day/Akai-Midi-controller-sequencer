
UART_HandleTypeDef huart1;

void waterfall(void){
	uint8_t current_scene=scene_buttons[0];
	if (current_scene<8 )  current_scene=0; else current_scene=8;
	for (i=0;i<32;i++){

		//buttons_bank_1[i]=

	}

}
void bar_map_screen(void){    // draw and modify bar_ map screens, notes as well, works good
	uint8_t note_enabled=0;
	uint8_t scene_select=scene_buttons[0]; // clear last two bits
	uint8_t i=0;
	uint32_t pointer;   // this is the actual address
	uint8_t incoming_message[3];
	uint8_t color=green_button;
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


// draw screen

	if (bar_map_screen_level){       // draw bars
		pointer+=(scene_select&12);
		for(i=0;i<32;i++){
			note_enabled=VAR_GET_BIT((pointer+(i>>3)),(i&7));
			note_enabled*=color;
			button_states[square_buttons_list[i]]=note_enabled;   // set light
		}}

	if (!bar_map_screen_level){   // draw notes
		pointer+=(scene_select*2);
			for(i=0;i<16;i++){
				note_enabled=VAR_GET_BIT(pointer,(i&15));
				note_enabled*=color;
				button_states[square_buttons_list[i]]=note_enabled;   // set light
			}
				memset(button_states+8,0,16);}   // clear last two rows

		USB_send();  // do full clear send before new data
	} // end of bar map screen

void bar_map_tracker(void){     // creates sound enable for notes from bar_map_edit
	uint8_t enable[3];
	uint16_t counter=bar_map_counter;
	uint32_t pointer=( uint32_t)&bar_map_1;
	for(i=0;i<16;i++){
	pointer=( uint32_t)&bar_map_1;
	enable[0]=VAR_GET_BIT(pointer+i,(counter&7));
	pointer=( uint32_t)&bar_map_8;
	enable[1]=VAR_GET_BIT(pointer+i,((counter>>3)&7));
	pointer=( uint32_t)&bar_map_64;
	enable[2]=VAR_GET_BIT(pointer+i,((counter>>6)&7));
		if ((enable[0]+enable[1]+enable[2])!=3) enable[0]=0; else enable[0]=1;

	bar_map_sound_enable[i]=enable[0];

	}


		bar_map_counter=(bar_map_counter+1)&511;
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

/*
	//uint16_t current_scene=scene_buttons[0]*256;  // pattern select

	//uint8_t current_scene_drums=((scene_buttons[0]&3)*4) + (pattern_select*16);  // pattern location
	uint8_t selected_scene=scene_buttons[0];  // this wiil change to pattern select 0-15
	uint16_t data_temp2;
//	uint8_t accent_temp;
	//uint16_t fast_bit;
	uint8_t note_counter=0;
	uint8_t drums=0;
	//uint32_t drum_pattern;   // holds 4 bytes
	//pattern_select=0;
	uint16_t drum_byte_select;
	uint8_t drum_byte;
	//memset(button_states+24,0,16);
	drums=0;    //always

	if (loop_screen_disable) drums=0; //disable for rec arm
*/

/*	if(device) drums=0;  // disables loop screen

		if (drums){  // for all sounds now

			for (i=0;i<16;i++) {// drums fill  i=actual step 0-15

				drum_byte_select= (i>>2)+(selected_scene*4)+(drum_store);  // select byte position
				drum_byte=drum_store_one[drum_byte_select];  // get data

				if (drum_byte & (1<<((i&3)*2))) data_temp2=1; else data_temp2=0;     // note test ok
				if (drum_byte & (1<<(((i&3)*2)+1))) accent_temp=1; else accent_temp=0;  // accent test

				if (data_temp2)    // read back on first loop select   , change button state colour
				{
					//if (accent_temp) button_states[square_buttons_list[i]]=red_button ;  else
						button_states[square_buttons_list[i]]=yellow_button ;}
				else button_states[square_buttons_list[i]]=0;

				note_counter++;

			} // end of loop
		}*/ // end of drums

		//loop_note_count[selected_scene]=note_counter;
	}



void note_buttons(void){  // always running only on notes though , might dump it


/*
	uint8_t selected_scene=scene_buttons[0];
	uint8_t incoming_message[3];
		memcpy(incoming_message,cdc_buf2, 3); // works off only receiving buffer , this might be changing

		uint8_t incoming_data1 = incoming_message[1]&127;

	last_button = square_buttons_list[incoming_data1];   // memory location  of note pressed 0-256(0-3 (16notes)   + sounds*4+(pattern_select*16)

	uint8_t last_press = square_buttons_list[incoming_data1];  //0-32  ,uses the modified map from top to bottom

	uint16_t drum_byte_select= (last_press>>2)+((selected_scene)*4)+(bar_playing*drum_store);  //
	uint8_t drum_byte=drum_store_one[drum_byte_select];  // get data
*/


			for (i = 0; i < 40; i++){   // test for bad data
				if (button_states[i]>10) button_states[i]=0;

			}



/*
	if ((!pan) && (last_press<16) && (!bar_map_screen_level))      //  normal screen  , draws notes on main screen

			{ //  , only if enabled though ,



				switch(button_states[incoming_data1]){    // change state
							case 0 : drum_byte=drum_byte &~ (1<<((last_press&3)*2));  //turn off note
							drum_byte=drum_byte &~ (1<<(((last_press&3)*2)+1));  // turn off accent
							// ok on/off ,clear note and accent too
							break;   // clear note and accent ,works
							//	case 3 :drum_byte=drum_byte | (1<<((((last_press-(drum_byte_select*4))*2))+1));  break;		// add accent
							case 5 :drum_byte=drum_byte + (1<<((last_press&3)*2));
							if (shift) { drum_byte=drum_byte + (1<<(((last_press&3)*2)+1));}  // adds an accented note
							break;		// note on ok

				}

				drum_store_one[drum_byte_select]=drum_byte; //write back info

			}
*/

	if ((!pan) ) {
		bar_map_screen();







	}





/*				if((last_press>15)&& (last_press<24) && (!select_bn) && (selected_scene<8)){    // pitch  change on first page  only
											//uint8_t pattern=pattern_select;
								memset(button_states+16, 0, 8);

								pitch_selected_for_drums[selected_scene]=last_press-16; button_states[last_press]=yellow_button; // controls lights
							//	if(rec_arm | pause) {   // might just drop this
									//uint8_t pitch_byte_select=(bar_playing)+((selected_scene&7)*8);	// 0-3 + 0-28
								//	uint8_t pitch_select=(bar_playing&7)+((selected_scene&7)*8); // 0-63


								//	pitch_change_store[pitch_select]=last_press-16;  // 0-7 , stores in on the timeline
									pitch_change_loop_position[selected_scene]=last_press-16;
									//pitch_byte=(pitch_byte & (~pitch_mask)) | ((last_press-15)&7);   //clears bits then or in the new ones
									//pitch_change_store[pitch_byte_select]=pitch_byte ;
							//	}


								pitch_change_flag=1+selected_scene; //enable pitch nrpn send , just count from current one maybe
							//	punch_in[0]=last_press-15; // +1   doesnt do much

								if(pause){

								uint8_t pitch_byte=last_press-16;
								uint8_t pitch_final=(pitch_list_for_drums[pitch_byte+(selected_scene*8)])&127; // there should always some data
									pitch_selected_drum_value[selected_scene]=pitch_final;  // if missing data keep old value
								}

								//	button_states[punch_in[0]+16]=yellow_button;

							} */  // pitch change end
//////////////////////////////////////////////

/*				if((last_press>23)){    // row 4 , changed to bar selection for now
				//uint8_t pattern=pattern_select;
				uint8_t new_bar=(last_press-24);
				bar_selector=new_bar;
				button_states[square_buttons_list[new_bar+24]]=green_blink_button; //enable blink
					//if (shift) { button_states[square_buttons_list[new_bar+24]]=red_blink_button;  shift=0;}
					//else button_states[square_buttons_list[new_bar+24]]=green_button; //enable blink

					// new_pattern_select=new_pattern;  //add only to new pattern select,   disable pattern select for now !!!


				}*/


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

	uint16_t clear[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t current_scene=scene_buttons[0];

//	uint16_t drum_store_select=(current_scene*4)+(bar_playing*drum_store);
	incoming_message[2]=(incoming_message[2]&127);
	//drum_byte_select = (offset_pitch >> 2) + (i * 4) + (pattern * drum_store); //based on time,  selects a trigger 16 + (i*4) 16*64 ... 0-256 (64bytes per pattern, 4 bytes per per )
	//drum_byte = drum_store_one[drum_byte_select]; // this could be smaller



	if (status == MIDI_NOTE_OFF) // note off
	{  note_off_flag[0]=0;


	if (incoming_data1==98)    // shift related functions
	{shift=0;
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

		case up_arrow_button:
		 if ((scene_buttons[0]<8)) 	{memset(button_states,1,8);bar_map_screen();

		 scene_buttons[0]=scene_buttons[0]+8;second_scene=8;

		 } else
		 {memset(button_states,1,8);bar_map_screen(); }
		break;

		case solo_button: {scene_solo=1;clip_stop=0;button_states[82]=0; } //solo but with pots now ,turns off clip stop or mute
		 if (shift) memset(mute_list,0,16); // clear all mute info when shift held
		break;
		case right_arrow_button:right_arrow=1; {if (bar_map_screen_level<3) bar_map_screen_level++; } bar_map_screen();right_arrow=0;button_states[right_arrow_button]=0;break;//zoom out
		case select_button: {select_bn=1;} ;break; // select enable
		case down_arrow_button: down_arrow=1;//
		break;
		case left_arrow_button: {left_arrow=1;  {if (bar_map_screen_level) bar_map_screen_level--; }	   }bar_map_screen();left_arrow=0;button_states[left_arrow_button]=0;
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
		case clip_stop_button: {clip_stop=1; lcd_downcount=10;lcd_messages_select=3;scene_solo=0;button_states[83]=0;  } break;
		default:break;


		}
		}


		if ((!button_states[incoming_data1]) && (incoming_data1>40))  //only test if button is getting switched off, may not as reliable but quicker
			{
			switch(incoming_data1){

			case up_arrow_button:up_arrow=0;if ((scene_buttons[0]>7)) {
				memset(button_states,1,8);
				scene_buttons[0]=scene_buttons[0]-8;second_scene=0;
				loop_screen();}else{ memset(button_states,1,8);}  ;break;
			case right_arrow_button:right_arrow=0;  break;//zoom out
			case select_button: {select_bn=0;} ;break; // select enable
			case down_arrow_button:down_arrow=0; break;
			case left_arrow_button: left_arrow=0; break;// zoom in
			case mute_button:scene_mute=0;break;
			case record_button:record=0;overdub_enabled=0;break;
			case volume_button:volume=0;break;
			case pan_button:pan=0;break;
			case send_button:send=0;break;
			case rec_arm_button:rec_arm=0;break;
			case device_button:device=0;break;// shows midi channel , for now
			case play_pause_button:{pause=0;button_states[stop_all_clips]=0;}break;
			case clip_stop_button: {clip_stop=0;} break;
			default:break;


			}
			}

		if (shift && pause && clip_stop) { 			// clear all program change info on drums , needs to be saved though ,disabled
			//memset(program_change_automation,0,32);
			//memcpy(alt_pots+128,program_change_automation,32);clip_stop=0;button_states[82]=0;
		}



		} // end of Note on for all buttons


		    // not very useful

	//if ((status == 177) && (incoming_data1==64) && (incoming_message[2]==127) && (sustain)) {sustain=0;button_states[square_buttons_list[pattern_select+16]]=yellow_button;status=0;}
	//if ((status == 177) && (incoming_data1==64) && (incoming_message[2]==127) && (!sustain)) {sustain=1;button_states[square_buttons_list[pattern_select+16]]=red_blink_button;status=0;}	 // toggle sustain also reset pattern repeat

	if ((status == 176) && (clip_stop)){   // with clip stop on

		if (current_scene>7)    // program alt pots based on keys selected  on second page

	{alt_pots[(incoming_data1  - 48)+((current_scene-8)*8)] =((incoming_message[2]));}   // at zero setting velocity is off  , set for 4 banks for the last for 4 scenes
	// maybe 16 values or about 2 octaves in scales

		status=0; // clear
	}

	if ((status == 176) && (scene_solo)&& (incoming_data1<pot_5)){   // solo processing
		uint8_t solo_selector=(incoming_data1-48)*4;
		incoming_message[2]=(incoming_message[2]>>5)&3;

			memset(mute_list+solo_selector,1,4);mute_list[incoming_message[2]+solo_selector]=0;
		status=0; // clear
	}


	if (status == 176) {//  controller data , store pot , clip stop off



		pot_states[incoming_data1  - 48] = (incoming_message[2]); // store pot all  // not always ok


		if (pan) {
			patch_save=pot_states[7]>>4;   // set next memory to be save d or loaded
			patch_screen();
		}

		if (!down_arrow){     // down arrow disabled ,default screen
		switch(incoming_data1){   // pots data selector ,default screen

		case pot_1:

		if ((shift)){
		// {if (midi_channel_list[current_scene]!=9)  {program_change[1]=incoming_message[2]; program_change[2]=1;}}}   // program change for channels other than drums  pot 1 , not owrking atm


			//uint8_t pot_temp_in=((pot_states[0]>>4))&7;
			//uint8_t pot_temp_old=LFO_delay_list[current_scene];
		//	uint8_t current_lfo_pos=LFO_tracking_counter[current_scene];

			//if(pot_temp_old>pot_temp_in){current_lfo_pos=(current_lfo_pos&48)+((current_lfo_pos&7)-(pot_temp_old-pot_temp_in));}   // modify lfo counter position

			// else {current_lfo_pos=(current_lfo_pos&48)+((current_lfo_pos&7)+(pot_temp_in-pot_temp_old));}

			//LFO_tracking_counter[current_scene]= current_lfo_pos;
			LFO_phase_list[current_scene]=((pot_states[0]>>5))&3;	 // lfo phase setting
			lcd_downcount=3;lcd_messages_select=0;

		}
		else 	{LFO_low_list[current_scene]=((pot_states[0]>>4))&7;lcd_downcount=3;lcd_messages_select=6;}
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
		;break;
		default:break;

		}

		}

		if (down_arrow){      // with down arrow enabled , change pot function
			switch(incoming_data1){   // pots data selector ,default screen

			case pot_1:pitch_change_loop[current_scene]=pot_states[0]>>4; break;

			//if ((select_bn) && (current_scene>3))  //  cc function
			//{midi_cc_cue[0] =midi_channel_list[current_scene]+176; midi_cc_cue[1] =incoming_message[2]; };break; // sets notes playing only on these bars

			case pot_2:program_change[current_scene]=pot_states[1];program_change_flag=current_scene+1; break; // send program change value

			case pot_3: control_change[current_scene]=pot_states[2];control_change_flag=current_scene+1;control_change_value=91; break;// cc reverb

			case pot_4:control_change[current_scene]=pot_states[3];control_change_flag=current_scene+1;control_change_value=72; break;// cc release
			case pot_5: control_change[current_scene]=pot_states[4];control_change_flag=current_scene+1;control_change_value=5; break; // cc portamento time
			case pot_6:control_change[current_scene]=pot_states[5];control_change_flag=current_scene+1;control_change_value=75; break;  //

			case pot_7:	control_change[current_scene]=pot_states[6];control_change_flag=current_scene+1;control_change_value=76; break;   //

			case pot_8:control_change[current_scene]=pot_states[7];control_change_flag=current_scene+1;control_change_value=77; break;
			default:break;

			}

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

		memset(button_states,1,8); ;  // turn green
		button_states[scene_select&7]=5;
		if (scene_mute) { // muting control

			{if (mute_list[scene_select]) {mute_list[scene_select]=0; button_states[scene_select&7]=5;} else {mute_list[scene_select]=1; button_states[scene_select&7]=3;}}

		}



			scene_buttons[0]=scene_select;
			 current_midi=midi_channel_list[scene_buttons[0]]+1;

			 //loop_selector=1; // redraw
	
		scene_select=0;
		bar_map_screen();
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



/*
		if ((!seq_step) || (seq_step==8)) { // program change automation ,modified to change twice on note 0 and 8
		if (pc_set)     program_change[0] = (pc_set - 1) & 7; //change pc but only on fresh data

	if (select_bn)	{memset(button_states + 16, 0, 8);   // sets lights for program change but only if select is on

		if (current_scene<12) button_states[program_change[0] + 16] = red_button;
		else button_states[alt_pots_selector + 16] = red_button;  //sets selected program change light




	}
	else {memset(button_states + 16, 0, 8);
	button_states[pitch_selected_for_drums[scene_buttons[0]&7] + 16] = yellow_button;

	}

	} // end of program change
*/


///////////bar section
/*	if ((seq_pos==0) && (!pause)){memset(button_states+8,0,8);

	//{ if (bar_playing!=bar_selector) bar_playing=bar_selector;  else bar_playing=(bar_playing+1)&7;   }
	{ if (bar_playing!=bar_selector) bar_playing=bar_selector;   }  // disable bar advance for now

	button_states[bar_playing+8]=green_button;
	loop_screen();

	bar_selector=bar_playing;

	}// also changes bar postion


	if ( (pause)){ memset(button_states+8,0,8);bar_playing=bar_selector;
	button_states[bar_playing+8]=green_button;
	loop_screen();

	} */ // change current bar when in pause as well

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
		//uint8_t pitch_byte=pitch_change_store[bar_playing+((current_scene&7)*8)];


	/*	if (punch_in[0]) { punch_in[0]--; pitch_selected_drum_value[current_scene&7]=pitch_list_for_drums[((current_scene&7)*8)+punch_in[0]];
		button_states[punch_in[0]+16]=red_button;

		}// replace value with current selected only temp though*/

		//punch_in[0]=0;

	}

/*	  	  	  	  uint8_t scene_high=0;   // scene buttons off if no data in bar
				  uint8_t d;
				 // memset(button_states,1,8); //scene lights

				  if(button_states[64]) scene_high=8; // if up arrow
				  for (d = 0; d < 8; d++) {   // clears scene light if no data in bar , seems to kill blinky lights ?

				//	if (bar_note_register[bar_selector]&(1<<(d+scene_high))) button_states[d]=1; else button_states[d]=1;

				}*/

//	if (pause && (!pc_set)) memset(button_states+8,2,8);  // green blink in pause if no data in current bar

	if (seq_step==15){



	} //end of s_temp


	}








