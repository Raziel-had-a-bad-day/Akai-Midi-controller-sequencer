
#include "main.h"


extern InputSig sig;



void cc_control_page_1(const InputSig *sig){
	uint8_t current_scene=voice_list[scene_buttons[0]];  // this now only selects the voice
	fx_incoming[1]=sig->source;  // selects which pot


	if (!shift){

		lcd_menu_print("CC send         ",16);
		uint8_t shift_fx=current_scene*8; // voice based
		uint8_t cc_selected=fx_pot_settings[((sig->source-48))+shift_fx]; //gets cc number for the pot
		lcd_number(cc_selected,26);
	//	cc_lut(cc_selected);  //disable for now
		cc_extra_send[0]=cc_selected;
		cc_extra_send[1]=sig->value;
		memcpy(lcd_buffer+16,cc_string,10); // copy name of cc
		lcd_number(sig->value,29);
		fx_menu(fx_incoming[1]);  // saves pot settings for fx
	}
	else { // assign cc to pots

		lcd_menu_print("CC re-assign     ",16);
		uint8_t shift_fx=current_scene*8; // voice based


		fx_pot_settings[((sig->source-48))+shift_fx]=sig->value; //gets cc number for the pot
		uint8_t cc_selected=fx_pot_settings[((sig->source-48))+shift_fx]; //gets cc nc number for the pot
		lcd_number(cc_selected,26);
		//cc_lut(cc_selected); //disable for now
		memcpy(lcd_buffer+16,cc_string,10); // copy name of cc
		lcd_number(sig->value,29);

	}

}
void cc_control_page_2(const InputSig *sig){

	lcd_menu_print("CC extra        ",16);
	// control_change_flag=current_scene+96;// extra cc send , not stored , this is enabled elsewhere

	cc_extra_send[0]=(sig->source-48)+90;// select pot  cc 90-97
	lcd_number(cc_extra_send[0],24);
	cc_extra_send[1]=sig->value;// cc value 0-127
	lcd_number(cc_extra_send[1],28);

}

void cc_control_page_3(const InputSig *sig){ //default cc control

	//if(shift) shift_hold_function();
	uint8_t current_scene=voice_list[scene_buttons[0]];  // this now only selects the voice
		pot_states[sig->source  - 48] = (sig->value); // store pot all  // not always ok


		if (pan) {
			patch_save=pot_states[7]>>4;   // set next memory to be save d or loaded
			//patch_screen();
		}

		switch(sig->source){   // pots data selector ,default screen

		case pot_1:
		{pitch_pot=pot_states[0]>>3;
		 lcd_menu_print("Pitch enter     ",16);lcd_number((pitch_pot),29);
		}    // sets pitch for entering notes
		break;

		//if ((select_bn) && (current_scene>3))  //  cc function
		//{midi_cc_cue[0] =midi_channel_list[current_scene]+176; midi_cc_cue[1] =sig->value; };break; // sets notes playing only on these bars

		case pot_2:seq_pos_set[current_scene]=0.3+((pot_states[1]>>3)*0.1);break; // sets notes playing only on these bars

		case pot_3://
		pitch_change_rate[current_scene]=1<<(sig->value/24); //1-32  // pitch hcange rate
		 lcd_menu_print("Pitch change rate    ",16);
				;break;// sets pitch for drums ,only first page

		case pot_4:seq_step_modify=(pot_states[3]&127)+1; lcd_menu_print("Jump to bar     ",16);lcd_number((seq_step_modify-1),29);
		 if(pause && (seq_step_modify)) seq_step_long=seq_step_modify-1;// just force
		;break; // scrub bars
		case pot_5: 	if ((!device)) {lfo_settings_list[(current_scene*2)]=sig->value;} ; lcd_menu_print("LFO rate        ",16);;lcd_number(sig->value,29); break;  // lfo rate
		case pot_6: if ((!device) && (!clip_stop))   {lfo_settings_list[(current_scene*2)+1]=sig->value;} ; lcd_menu_print("LFO depth       ",16);lcd_number(sig->value,29); break;  // lfo level

		case pot_7:	if ((pause) && (device)) 		 {  // enter mid channel
			uint8_t midi_selected=(sig->value>>3)&15;
			midi_channel_list[current_scene]=midi_selected;
			lcd_number(midi_selected,29);
			 lcd_menu_print("Edit MIDI       ",16);

		} else { lcd_menu_print("LFO dest       ",16);lcd_number(sig->value,29);


		}
		//memset(button_states+24,0,16);button_states[31+(current_midi&7)-((current_midi>>3)<<3)]=yellow_blink_button;}

		;break;   // sets midi channel on selected sound

		case pot_8:
			if ((shift) && (device))		{timer_value=bpm_table[sig->value+64]; tempo=sig->value+64;  lcd_menu_print("Edit Tempo     ",16);lcd_number(sig->value,29);} //tempo
			if ((!device)&&(!shift)) {note_accent[current_scene]=sig->value;rand_velocities[current_scene]=sig->value;  lcd_menu_print("Velocity / Accent    ",16);lcd_number(sig->value,29);    // accent input
		current_accent=pot_states[7];}  // accent also used for tempo with shift

			;break;
		default:break;
		}


} // end of cc control page 3
