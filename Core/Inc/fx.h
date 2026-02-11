


uint8_t fx_map_1[sound_set]={255,255,255,255,255,255,255,255};  // these are set by midi and cc , not sound select
uint8_t fx_map_8[sound_set]={255,255,255,255,255,255,255,255};
uint8_t fx_map_64[sound_set]={255,255,255,255,255,255,255,255};
void transpose_tracker(void);




void fx_menu(uint8_t incoming){ // pot 1-8 control fx list 1-8 cc value ,, cc settings is stored in fx_pot _settings, shift+pot might set transition speed ,
	//maybe pot1 is high level and pot4 is low
	// for now just triggers a  cc send
	uint8_t current_scene=incoming&7;   // this just selects which pot 0-3 high level 4-7 low level

	//uint8_t current_channel=0;
	//uint8_t current_cc=0;

	if (current_scene<4){
		fx_pot_values[current_scene*4]=pot_states[current_scene];}
	else fx_pot_values[((current_scene-4)*4)+1]=pot_states[current_scene];   // sets on /off values  , first 4 tracks





	//current_channel=fx_pot_settings[current_scene*2];



/*	switch(fx_incoming[1]){   // pots data selector ,fx functions



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

		}*/


}
void fx_cc_send(uint8_t send){   // sends cc for particular fx track

	uint8_t current_scene=send&7;
	control_change_value=fx_pot_settings[(current_scene*2)+1]; // selects cc

	if (fx_map_sound_enable[send]) control_change[current_scene]=fx_pot_values[current_scene*4];
	else control_change[current_scene]=fx_pot_values[(current_scene*4)+1];


	if (control_change_buf[current_scene]!=control_change[current_scene])
		{control_change_flag=current_scene+1;control_change_buf[current_scene]=control_change[current_scene];}
			//only sends cc on change of value

	//control_change[current_scene]=pot_states[current_scene];




}
void fx_map_screen(void){    // fx screen functions
	uint8_t note_enabled=0;
	uint8_t scene_select=0; // this needs to change for fx , only once scene for now, 4 tracks with diff channel settings and cc
	uint8_t i=0;
	uint32_t pointer;   // this is the actual address
	uint8_t incoming_message[3];
	uint8_t color=green_button;
		memcpy(incoming_message,cdc_buf2, 3); // works off only receiving buffer , this might be changing
		memset(cdc_buf2,0,3);

	//	clear_rows=1;

		uint8_t incoming_data1 = incoming_message[1]&127;
		if (!bar_map_screen_level) bar_map_screen_level=1;  // no zero
		switch(bar_map_screen_level){   // grab the pointer for the correct bar map screen

		case 1: pointer=(uint32_t)&fx_map_1;color=red_button;break;
		case 2: pointer=(uint32_t)&fx_map_8;color=red_button;break;
		case 3:pointer=(uint32_t)&fx_map_64;color=red_button;break;
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

void fx_map_tracker(void){     // creates sound enable for notes from fx_map_screen
	uint8_t enable[3];
	uint16_t counter=bar_map_counter;
	uint32_t pointer=( uint32_t)&fx_map_1;
	for(i=0;i<4;i++){
	pointer=( uint32_t)&fx_map_1;
	enable[0]=VAR_GET_BIT(pointer+i,(counter&7));
	pointer=( uint32_t)&fx_map_8;
	enable[1]=VAR_GET_BIT(pointer+i,((counter>>3)&7));
	pointer=( uint32_t)&fx_map_64;
	enable[2]=VAR_GET_BIT(pointer+i,((counter>>6)&7));
		if ((enable[0]+enable[1]+enable[2])!=3) enable[0]=0; else enable[0]=1;

	fx_map_sound_enable[i]=enable[0];

	}


		//bar_map_counter=(bar_map_counter+1)&511;
		}
void transpose_tracker(void){ // this runs every 2 bars , should finish after the last note off , hopefluffy
	uint8_t pitch_counter=0;
	uint8_t random_list[64];
	 memcpy (random_list,alt_pots,64);  // 8*8  for now
	 int8_t alt_pots_value=0;

	for (i=0;i<8;i++){
		//pitch_counter=last_pitch_count[i];


		//pitch_counter=(pitch_counter+pitch_change_rate[i+8])&255;   // pitch change rate =  +1 +2 +4 +8 +16 +32
		pitch_counter=((bar_map_counter/2)/pitch_change_rate[i+8])&7; // modify to fixed position

		alt_pots_value=(random_list[(i*8)+(pitch_counter)])&127; // grab alt pots
		transpose_octave_modifier[i]=(alt_pots_value/12)*12;  // holds base octave value
		alt_pots_value= alt_pots_value-(transpose_octave_modifier[i]);
		if (alt_pots_value<7) transpose_pitch_modifier[i]=0-(6-alt_pots_value);  else transpose_pitch_modifier[i]=alt_pots_value-6;
		if (!transpose_octave_modifier[i]) transpose_octave_modifier[i]=1; // fix zero values




		last_pitch_count[i]=pitch_counter;


	}
			}

