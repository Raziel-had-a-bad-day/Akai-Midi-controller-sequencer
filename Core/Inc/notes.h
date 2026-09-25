


extern UART_HandleTypeDef huart1;
void shift_hold_function(void); // declare first  here than it's happy
void pitch_mode(void);
void bar_map_tracker(void);
uint8_t countSetBits(uint8_t number);








void enter_note(uint8_t step){ // enters a midi note on for a selected voice at a certain step in the current pattern 0-255, mainly for one shot samples

	uint8_t voice =voice_list[scene_buttons[0]]; // selects voice
	uint8_t start=((note_recording_set_current[voice]&7)*seq_play_note_count)+(voice*(seq_play_note_count*sound_set)); // start time byte  in seq_play_buf_time
	uint8_t pitch=pitch_pot;
	//if (!step) step=1; // always on

	for(i=0;i<16;i++){
		if ((seq_play_buf_time[start+i]==step)&&(seq_play_buf[(start+i)*3])) {
			seq_play_buf[(start*3)+(i*3)]=pitch;
			seq_play_buf[(start*3)+(i*3)+1]=127; //vel
			return;} // replace exit if found an existing(first) record
		}
	for(i=0;i<16;i++){
		if (!seq_play_buf_time[start+i]){
			seq_play_buf_time[start+i]=step;
			seq_play_buf[(start*3)+(i*3)]=pitch;
			seq_play_buf[(start*3)+(i*3)+1]=127; //vel
			seq_play_buf[(start*3)+(i*3)+2]=step; //time
			return; // find empty slot then enter new pitch/time , exit
		}

	}// no fallback for now if pattern is full , needs manual delete, might use the last recorded not pointer ?

	} // end of enter note

void delete_note(uint8_t step){ // delete a midi note on for a selected voice at a certain step in the current pattern 0-255, mainly for one shot samples

	uint8_t voice =voice_list[scene_buttons[0]];
	uint8_t start=((note_recording_set_current[voice]&7)*seq_play_note_count)+(voice*(seq_play_note_count*sound_set)); // start time byte  in seq_play_buf_time

	//if (!step) step=1; // always on

	for(i=0;i<16;i++){
		if (seq_play_buf_time[start+i]==step){
			seq_play_buf_time[(start)+(i)]=0;
			seq_play_buf[(start*3)+(i*3)]=0;
			seq_play_buf[(start*3)+(i*3)+1]=0; //vel
			seq_play_buf[(start*3)+(i*3)+2]=0;
			return;} // replace exit if found an existing(first) record
		}

	} //end of delete note




void bar_map_screen(void){    // draw and modify bar_ map screens, notes as well, works good
	//uint8_t note_enabled=0;
	uint8_t scene_select=scene_buttons[0]; // clear last two bits
	uint8_t i=0;
	//uint32_t pointer;   // this is the actual address
	uint8_t incoming_message[3];
	//uint8_t color=green_button;
	uint8_t accent_color=yellow_button;
	uint32_t note_pointer=(uint32_t)&note_on_tracking_buf;
		memcpy(incoming_message,cdc_buf2, 3); // works off only receiving buffer , this might be changing
		memset(cdc_buf2,0,3);

	//	clear_rows=1;

		uint8_t incoming_data1 = incoming_message[1]&127;

/*	switch(bar_map_screen_level){   // not needed
		case 0: pointer=(uint32_t)&bar_map_0;color=green_button;break;
		case 1: pointer=(uint32_t)&bar_map_1;color=red_button;break;
		case 2: pointer=(uint32_t)&bar_map_8;color=red_button;break;
		case 3: pointer=(uint32_t)&bar_map_64;color=yellow_button;break;
		default:break;}*/


/*
	if ((incoming_data1>7) && (incoming_data1<40) && bar_map_screen_level) { //when changing
	// modify button from incoming

			uint8_t alt_list=	 button_states[incoming_data1 ];
			switch(alt_list){    // change state of button
			case 0 :alt_list = 0;break;   //
			default:alt_list = color; ;break; }

			button_states[incoming_data1 ]=alt_list;
			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-31

 // modify data from button state
			if (button_states[square_buttons_list[alt_list]]) {VAR_SET_BIT((pointer+(scene_select&12)+((alt_list>>3))),(alt_list&7));}
			else {VAR_RESET_BIT((pointer+(scene_select&12)+((alt_list>>3))),(alt_list&7));}

	} //end of mod
*/

	if ((incoming_data1>23) && (incoming_data1<40)   ) {  // top 2 bars
	// modify button from incoming

			//uint8_t alt_list=	 button_states[incoming_data1 ]; // might use a diff system
			uint8_t alt_list=blink_light_list[square_buttons_list[incoming_data1]];
			switch(alt_list){    // change state of button
			case 0 :alt_list = red_button;break;   //not very effective
			default:alt_list = 0; ;break; }

			//button_states[incoming_data1 ]=alt_list;
			blink_light_list[square_buttons_list[incoming_data1]]=alt_list;

			// enter notes here
			//	uint8_t step=(incoming_data1-32)*(32>>zoom_level); // change multiplier with zoom later
				uint8_t step=(square_buttons_list[incoming_data1])*16; // need a dif way
			//memset(blink_light_list,0,256); //clear store lights
				if (alt_list) {enter_note(step);

			} else delete_note(step);

			memcpy(lcd_buffer+16,"Step            ",16);lcd_number((step>>4),25);lcd_number((step&15),29);


			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-31
 // modify data from button state
	//	if(button_states[square_buttons_list[alt_list]])  {VAR_SET_BIT((pointer+(scene_select*2)),(alt_list&15));}
	//	else {VAR_RESET_BIT((pointer+(scene_select*2)),(alt_list&15));}

// use this for tracking note recorded ?
	} //end of mod

	if ((incoming_data1>7) && (incoming_data1<24)  ) {   // cc control scene  select
	// modify button from incoming
		// add shift copy to buf
			memset(button_states+8,0,16);   // clear row
			uint8_t alt_list=	 button_states[incoming_data1 ];
			switch(alt_list){    // change state of button
			case 0 :alt_list = accent_color;break;   //
			default:alt_list = 0; ;break; }

			button_states[incoming_data1 ]=alt_list;
			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-31
			cc_scene_controls (alt_list-16 ); //call functions for cc , might need modifiers
			lcd_menu_print("CC scene        ",16);lcd_number((alt_list-16),29);




/*
			if(button_states[square_buttons_list[alt_list]]) {
			save_current_pattern(alt_list&7);
			uint8_t	set_scene=voice_list[scene_select]; // midi channel based , might convert to a list ie 0=2 3=1 4=2 for now stay with 0-4
				set_scene&=3;
				note_recording_set_current[0]=alt_list&7;  // for now set all the sounds to the same
				note_recording_set_current[1]=alt_list&7;  //
				note_recording_set_current[2]=alt_list&7;  //
				note_recording_set_current[3]=alt_list&7;  //
				note_recording_set_current[4]=alt_list&7;  //

			}
			if (shift) seq_play_copy();  // copy if shift pressed , needs to be the same selection though
 // modify data from button state
		//if(button_states[square_buttons_list[alt_list]])  {VAR_SET_BIT((accent_pointer+(scene_select*8)+accent_bit_shift),((accent_bit&24)+(alt_list-16)));}
	//	else {VAR_RESET_BIT((accent_pointer+(scene_select*8)+accent_bit_shift),((accent_bit&24)+(alt_list-16)));}  // controls accent bit
*/


	} //end of mod

// draw screen


	//if (!bar_map_screen_level){   // normal screen default , runs always

		uint8_t multi=countSetBits(note_on_tracking_buf[1]);// get number of bits on bar select


/*		if ((incoming_data1>8) && (incoming_data1<16) && (multi>1)) { // multitouch bar_looping, only while held
			uint8_t looping_count=0;
			for(i=0;i<8;i++){
				if (looping_count>1) note_on_tracking_buf[1]=0; // clear
				if (VAR_GET_BIT((note_pointer+1),(i))) {bar_map_looping[looping_count]=i;  looping_count++;}

				}  // in case more than 2 buttons
			i=bar_map_looping[0];
			memset(bar_map_lights,0,8);
			while(!(i>bar_map_looping[1])){
				bar_map_lights[i]=1;
				i++;

			}

		} */// end of bar map looping ,works as long as one button held after another


/*		if ((incoming_data1>8) && (incoming_data1<16) && (multi<=1)) {
			memset(bar_map_lights,0,8);
			bar_map_lights[incoming_data1-8]=red_button;// enable bar selected light
			//bar_map_counter=(bar_map_counter&504)+(incoming_data1-8);  // move bar map counter

			//
		}*/



		//pointer+=(scene_select*2);

				//memset(button_states+8,0,16);
	//}   // end of !bar_map_level
		USB_send();  // do full clear send before new data
	} // end of bar map screen

void bar_map_tracker(void){     // creates sound enable for notes from bar_map_edit
	uint8_t enable[3];
	//bar_map_counter=(bar_map_counter+1)&511;



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
	 //bar_map_counter=0;
	 pause=1;
	 memset(bar_map_sound_enable,1,sound_set); // enable all sounds during off
	 memset(fx_map_sound_enable,1,(sound_set*1));

}

void alt_pots_playing(void){    // shows currently playing related to alt notes selected, for visual feedback, only when clip_stop
		//not used now

	uint8_t note_played=last_pitch_count[scene_buttons[0]]>>3;  // grab last pitch count
	if(note_played<4) note_played+=12;


	memset(button_states+12,0,4);memset(button_states+20,0,4); // clear area
	button_states[8+note_played]=3;
}



void top_bar(uint8_t input){ // red bar function for top row

	if (input>7){
	clear_row(0);loop_screen_disable=0;}
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
// not terribly useful

			for (i = 0; i < 40; i++){   // test for bad data
				if (button_states[i]>10) button_states[i]=0;

			}

			bar_map_screen();
/*	if ((!pan) && (!clip_stop)) {
		{bar_map_screen();}

	}*/
//if (clip_stop) pitch_mode();

		}

void buttons_store(void){    // incoming data from controller, will only hold lcd messages and enable modifier buttons from now on , all functions elsewhere
				// might just rewrite the lot , might also skip button lights here completely

	//uint8_t vel=pot_states[0]>>3;  // testing
	uint8_t alt_list;
	uint8_t scene_select=0;

	uint8_t incoming_message[3];

	lcd_downcount=4;  // start on ay button presses as lots of messages
	memcpy(incoming_message,cdc_to_notes,3); // works off only receiving buffer
	memcpy(cdc_to_input,cdc_to_notes,3); // works off only receiving buffer
	if (incoming_message[0]==176){  // test for repeat cc

		if ((incoming_message[1]==cdc_repeat_check[1] )&&	 (incoming_message[2]==cdc_repeat_check[2] )){
						memset(cdc_to_notes,0,3);return; // exit if repeated cc
		}
		}
	if (incoming_message[0]==176) memcpy(cdc_repeat_check,cdc_to_notes,3); // this should help




	uint8_t incoming_data1 = incoming_message[1]&127;
	uint8_t status=incoming_message[0];

	//uint16_t clear[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t current_scene=voice_list[scene_buttons[0]];  // this now only selects the voice
	uint32_t note_pointer=(uint32_t)&note_on_tracking_buf;

	incoming_message[2]=(incoming_message[2]&127);
	if (status == MIDI_NOTE_OFF) // note off
	{  note_off_flag[0]=0;
	if ((incoming_data1 <40)  )
		{VAR_RESET_BIT((note_pointer+(incoming_data1>>3)),(incoming_data1&7));} // resets note_on in buffer for later

	if (incoming_data1==shift_button)    // shift related functions
	{shift=0;
	shift_hold_function();
}
	}   // note off buttons

	// skip for now

	if ((incoming_data1 <8)&& (status==MIDI_NOTE_ON)) {   // scene buttons
		//scene_select=incoming_data1 +1+second_scene;}  //enable scene_select section
	scene_select=incoming_data1 +1;}  //back single page for now

	//if (record && keyboard[0] && (keyboard[0]<128)) record_overdub(); // only on note on , enable overdub
	if (status == MIDI_NOTE_ON){  // Midi Note on  for buttons


		if ((incoming_data1 <40)  )
		{VAR_SET_BIT((note_pointer+(incoming_data1>>3)),(incoming_data1&7));} // sets note_on in buffer for later

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


		if (button_states[incoming_data1] && incoming_data1 > 40)  // function buttons , messages for buttons
		{
		    switch (incoming_data1)
		    {
		        case up_arrow_button:           // bum notes
		        	 lcd_menu_print("Keys edit       ",16);
		        	if (scene_buttons[0] < 8)
		            {
		                memset(button_states, 1, 8);
		                second_scene = 8;
		            }
		            else
		            {
		                memset(button_states, 1, 8);
		            }
		            if (down_arrow) bar_map_screen();
		            break;

		        case solo_button:
		            solo = 1;
		            //memcpy(lcd_buffer+16,"Solo mode      ",16);
		            lcd_menu_print("Solo mode      ", 16);
		            if (shift) {memset(mute_list, 0, sound_set);   lcd_menu_print("Clear all muting    ",16);}       // clear mutes on shift
		            break;

		        case right_arrow_button:        // zoom in
		            if (zoom_level<3) zoom_level++;
		            lcd_menu_print("Zoom in         ",16);lcd_number((zoom_level),29);
		            right_arrow = 0;
		            button_states[right_arrow_button] = 0;
		            clear_row(0);
		            break;

		        case left_arrow_button:         // zoom out
		        	if (zoom_level) zoom_level--;
		        	 lcd_menu_print("Zoom out        ",16);lcd_number((zoom_level),29);
		        	left_arrow = 0;
		            button_states[left_arrow_button] = 0;
		            clear_row(0);
		            break;

		        case select_button:
		            select_bn = 1;
		            break;

		        case down_arrow_button:
		        	 lcd_menu_print(" FX_edit_mode  ",16);
		        	 button_states[clip_stop_button] = 0;
		            clip_stop=0;//disable pitch mode
		        	down_arrow = 1;down=1;
		            break;

		        case mute_button:
		            scene_mute = 1;
		            break;

		        case record_button:
		            record = 1;
		            overdub_enabled = 1;
		            break;

		        case volume_button:
		            volume = 1;
		            break;

		        case pan_button:
		            pan = 1;
		            patch_screen();
		            break;

		        case send_button:
		            send = 1;
		            break;

		        case rec_arm_button:
		            rec_arm = 1;
		            lcd_menu_print("Recording mode  ",16);
		            if (shift && !clip_stop)
		            {
		            	memset(blink_light_list,0,32); //clear store lights
		            uint16_t set_scenes=current_scene;
		            set_scenes&=7;
		            set_scenes=((note_recording_set_current[set_scenes]&7)*seq_play_note_count)+(set_scenes*(16*sound_set));// 8*16 atm

		            memset(seq_play_buf + (set_scenes * 3), 0, 48);// clear recording when shift held
		            memset(seq_play_buf_time + (set_scenes ), 0, 16);// clear recording when shift held
		            lcd_menu_print("Clearing notes  ",16);

		            }

		            break;

		        case device_button:
		            device = 1;
		            break;

		        case stop_all_clips_button:
		            button_states[play_pause_button] = 5;
		            lcd_menu_print("Stop, jump to start.  ",16);
		           // memset(note_recording_set_counter,0,sound_set);
		            pause = 5;
		            seq_step = seq_step_long = 0;
		            bar_selector = 0;
		            button_states[stop_all_clips] = 0;
		            all_notes_off();

		            memset(LFO_tracking_counter, 0, (sound_set*4));
		            memset(last_pitch_count, 0, sound_set);
		            memset(last_note_on_channel, 0, sound_set);
		            current_playing_bar = 0;
		            memset(seq_pos_out, 0,(sound_set*2));
		            break;

		        case play_pause_button:
		            if (shift)
		                bar_loop_current = 1;
		            else
		                {pause_delay = 1; lcd_menu_print("Pause           ",16);}
		            break;

		        case clip_stop_button:
		            clip_stop = 1;
		            down_arrow=0; // disable cc mode
		            down=0;
		            button_states[down_arrow_button] = 0;
		            lcd_menu_print("Enter pitch mode  ",16);

		            bar_map_screen_level = 0;
		            break;

		        default:
		            break;
		    }
		}  // function buttons

		if ((!button_states[incoming_data1]) && (incoming_data1>40))  //only test if button is getting switched off, may not as reliable but quicker
			{
			switch(incoming_data1){

			case up_arrow_button:up_arrow=0;if ((scene_buttons[0]>7)) { lcd_menu_print("Select track    ",16);
				memset(button_states,1,8);
				//scene_buttons[0]=scene_buttons[0]-8;

				second_scene=0;
				loop_screen();}//else{ memset(button_states,1,8);memcpy(lcd_buffer,"Select drum track    ",16);}
				;break;
			case right_arrow_button:right_arrow=0;  break;//zoom out
			case select_button: {select_bn=0;} ;break; // select enable
			case down_arrow_button:down_arrow=0;down=0;bar_map_screen() ; lcd_menu_print("Normal mode     ",16);; break;
			case left_arrow_button: left_arrow=0; break;// zoom in
			case mute_button:scene_mute=0;break;
			case record_button:record=0;overdub_enabled=0;break;
			case volume_button:volume=0;break;
			case pan_button:pan=0;break;
			case send_button:send=0;break;
			case rec_arm_button:rec_arm=0;seq_record_enable=0;seq_record_timer=0; lcd_menu_print("Normal mode     ",16);break;
			case device_button:device=0;break;// shows midi channel , for now
			case play_pause_button:{pause=0;button_states[stop_all_clips]=0;bar_loop_current=0;   lcd_menu_print("Play            ",16);}break;
			case clip_stop_button: {clip_stop=0;bar_map_screen();} lcd_menu_print("Normal mode     ",16); break;
			case solo_button:solo=0; lcd_menu_print("Normal mode     ",16);break;
			default:break;
			}
			}

		} // end of Note on for all buttons






	if (scene_select)  { // change scene select lite , one at a time though , fully update so need for extra sends
		scene_select=scene_select-1;
		lcd_menu_print("Select keys,drums    ",16);

		//uint8_t clear_green[8]= {1,1,1,1,1,1,1,1};
		memset(alt_pots_overwrite_enable,0,sound_set );  // clear alt pots edit
		memset(button_states,1,8); ;  // turn green
		for (i=0;i<8;i++){
		if(scene_select<8)	{if (mute_list[i]) button_states[i]=3;}
	//	if(scene_select>7)	{if (mute_list[i+8]) button_states[i]=3;}

		}
		button_states[scene_select&7]=5;
		if (scene_mute) { // muting control

			{if (mute_list[scene_select]) {mute_list[scene_select]=0; button_states[scene_select&7]=5;} else {mute_list[scene_select]=1; button_states[scene_select&7]=3;}}

		}

			scene_buttons[0]=scene_select;
			// current_midi=midi_channel_list[scene_buttons[0]]+1;

			 //loop_selector=1; // redraw
	
		scene_select=0;
		//if (!clip_stop)
			bar_map_screen(); // might just keep this only no extra changes
		//if (clip_stop)  pitch_mode();


	}// end of scene select
	//buffer_clear = 1;
	//loop_screen();  // always run except when device on ,except when keyboard step
	//if (buffer_clear)
	//	memcpy(cdc_buffer+cdc_start, clear, 3);
	memset(cdc_to_notes,0,3); // should clear

	} // end of button store


void shift_hold_function(void){   // this will autofill tracks with the last setting if shift is held down while a pot is turned

	if(shift_hold_enable){
		//memcpy(cdc_buffer,shift_hold_buf,3);
	buttons_store();
	}
	//if (shift && (cdc_buffer[0]==CC_Message)&& (!shift_hold_enable)) {shift_hold_enable=1;memcpy(shift_hold_buf,cdc_buffer,3);}

	if(!shift) { memset(shift_hold_buf,0,3); shift_hold_enable=0; } // clears buffer

}

void pitch_mode(void){  // changes display when in pitch mode , maybe diff note input too,entry only in pause , always runs  on second page only

	uint8_t selected_scene=scene_buttons[0]&7;

	uint8_t current_pitch_step=(last_pitch_count[selected_scene])&7;// shows selected pitch step 0-7
//	if(!pause) current_pitch_step=((bar_map_counter)/pitch_change_rate[selected_scene+8])&7;  // div 0

	uint8_t current_pitch_playing;
	uint8_t current_octave;
	uint8_t alt_pots_selected= (current_pitch_step)+((selected_scene)*8);
	uint8_t incoming_message[3];
	uint8_t incoming_data1=0;


	current_pitch_playing=alt_pots[(current_pitch_step)+((selected_scene)*8)];   // only works on last 8 scenes/keys
	current_octave=(current_pitch_playing>>2)/3;
	current_pitch_playing=current_pitch_playing-(current_octave*12);


	//if (pause) { // pitch entry during pause

		//uint8_t accent_color=yellow_button;
		memcpy(incoming_message,cdc_buf2, 3); // works off only receiving buffer , this might be changing
		memset(cdc_buf2,0,3);
		incoming_data1 = incoming_message[1]&127;
		if (keyboard[0] && keyboard[1] ) { // pitch from keyboard

			alt_pots[alt_pots_selected]=keyboard[0];
			current_pitch_playing=keyboard[0];
			current_octave=(current_pitch_playing>>2)/3;
				current_pitch_playing=current_pitch_playing-(current_octave*12);
				if(rec_arm) last_pitch_count[selected_scene]=(current_pitch_step+1)&7; //step recording

	//	}

		if ((incoming_data1>23) && (incoming_data1<40)) { //select pitch of the currently paying note
			// modify button from incoming, no toggle
			uint8_t alt_list=0;
			button_states[incoming_data1 ]=green_button;
			alt_list=square_buttons_list[incoming_data1]; // chnage to 0-15
			if (button_states[square_buttons_list[alt_list]]) alt_pots[alt_pots_selected]=alt_list+(current_octave*12);

		} //end of mod


	if ((incoming_data1>7) && (incoming_data1<16)) { // pitch step selector 0-7 , no toggle

									button_states[incoming_data1 ]=red_button; //sets colour
									last_pitch_count[selected_scene]=(incoming_data1-8);


	} //end of mod
	} // end of pause

	clear_row(0);clear_row(1);clear_row(2);clear_row(3);
	if(current_pitch_playing<8) button_states[32+current_pitch_playing]=1;  // shows current pitch on first two bars
	else button_states[24+current_pitch_playing-8]=1;

	button_states[8+(current_pitch_step)]=3;  // shows current pitch step on forth bar
	transpose_tracker();
	 USB_send();  // do full clear send before new data
}





