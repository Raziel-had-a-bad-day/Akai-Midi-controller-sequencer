void read_busy(void){
	uint8_t temp[2]={0x05};
	uint8_t  temp2[2]={0,0};
	uint8_t volatile flag=1;  // this is needed as it can stuck here , something with gpio toggling
//	HAL_Delay(20);
	 while (flag) {
		HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);  // when readin low till the end
		HAL_SPI_TransmitReceive (&hspi1,temp,temp2,2, 10); // request data , always leave extra room (clock) , works
		HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);  // high end
		flag=temp2[1]&1;

	 }

}

void flash_page_write(uint8_t page_select,uint8_t* data){    // write single page (256 bytes )

	 	 	 	 	 	 	 	 uint8_t send_data3[270]={0,10,0,0}; // [3]=bytes , [2]=page [1] = 64k block

	 	 	 	 	 	 	 	 memcpy  (send_data3+4 ,data,  256);
	 	 	 	 	 	 	 	 send_data3[2]=page_select;


	 	 	 	 	 	 	 	 send_data3[0]=0x06;
	 	 	 	 	 	 	 	 read_busy();
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
								  HAL_SPI_Transmit(&hspi1, send_data3, 1, 100);
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
									read_busy();


								  send_data3[0]=0x02; //write ,page program
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);   // low
								  HAL_SPI_Transmit(&hspi1, send_data3 ,260, 1000);  //address,then data
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);  // high end
									read_busy();

								  send_data3[0]=0x04; //disable write
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0); // low
								  HAL_SPI_Transmit(&hspi1, send_data3, 1, 100);
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);   // high end
									read_busy();


}

void settings_storage(void)
{
    // runs to store setting and read back

    uint8_t *settings[] = {
        scene_transpose, pot_states, pot_tracking, mute_list, note_accent, midi_channel_list,
         pattern_scale_list,
        lfo_settings_list, single_settings_list, pitch_change_loop,
          pitch_change_rate, bar_map_1, bar_map_8,
        bar_map_64, bar_map_0, fx_pot_settings, fx_map_1, note_recording_set_current,
        voice_list, fx_pot_values
    }; // try to add to the end

    uint8_t settings_temp[256];
    uint16_t settings_total = 0;
    uint16_t length = 0;
    uint8_t array_length[]={1,1,1,1,1,1,1,2,1,1,2,1,1,1,2,8,1,1,1,8


    };  //  this is needed no simple way around it
    tempo = single_settings_list[1];

    uint8_t flash_settings_count = sizeof(settings) / sizeof(settings[0]);

    for (uint8_t i = 0; i < flash_settings_count; i++) {

        // Automatic size calculation
       // length = (uint16_t)sizeof(*(&settings+1)-settings);  // this is wrong
        length = array_length[i]*8;
        //*(&array + 1) - array   , this just give 4 always (32bit)

        if (settings_total + length > sizeof(all_settings)) {
            // Emergency stop - something went wrong with size calculation
            break;
        }

        if (settings_write_flag) {
            memcpy(settings_temp, settings[i], length);
            memcpy(all_settings + settings_total, settings_temp, length);
        }
        else {
            memcpy(settings_temp, all_settings + settings_total, length);
            memcpy(settings[i], settings_temp, length);
        }

        settings_total += length;
    }

    flash_settings_size = settings_total;
    settings_write_flag = 0;
}
void flash_write(void){					// too much crap needs to simplify , easy mistakes
	  if ((send) && (!shift)          ){

		 		  uint8_t test_data3[270]={0,10,0,0};
		 		  uint8_t patch_mem=(patch_save&15)<<4;    // 16*16 (4kbyte)   start location for series of data stored
		 		  patch_mem=16;  // it will add 1 on writer
		 		  test_data3[2]=patch_mem;  // page counter 64kb total
		 		 read_busy();

		 		  // needs more than 4k cleared by now

		 		  test_data3[0]=0x06; //enable write each time
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
		 		  HAL_SPI_Transmit(&hspi1, test_data3, 1, 1000);
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
		 			read_busy();
		 		  //----formAT SECTION


		 		  test_data3[0]=0x20; //sector erase 4k (block is 0x52) , this is need , setting FF doesn't seem to work for page erase


		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);         // enable for sector erase   , stays empty when enabled
		 		  HAL_SPI_Transmit(&hspi1,  test_data3, 4, 1000);   //erase sector ,works       4kbytes   (block erase=32kbytes)
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
		 			read_busy();
		 		  // test write


		 		  test_data3[0]=0x04; //disable write

		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0); // low
		 		  HAL_SPI_Transmit(&hspi1, test_data3, 1, 100);
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);   // high end
		 			read_busy();

		 			  test_data3[2]=patch_mem+16;  //second sector clear


		 			test_data3[0]=0x06; //enable write each time
			 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
			 		  HAL_SPI_Transmit(&hspi1, test_data3, 1, 1000);
			 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
			 			read_busy();
			 		  //----formAT SECTION


			 		  test_data3[0]=0x20; //sector erase 4k (block is 0x52) , this is need , setting FF doesn't seem to work for page erase


			 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);         // enable for sector erase   , stays empty when enabled
			 		  HAL_SPI_Transmit(&hspi1,  test_data3, 4, 1000);   //erase sector ,works       4kbytes   (block erase=32kbytes)
			 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
			 			read_busy();
			 		  // test write


			 		  test_data3[0]=0x04; //disable write

			 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0); // low
			 		  HAL_SPI_Transmit(&hspi1, test_data3, 1, 100);
			 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);   // high end
			 			read_busy();

			 			single_settings_list[1]=tempo;

			 			settings_write_flag=1;
			 			settings_storage(); // all settings

			 			uint16_t up_counter=0;
			 			uint16_t play_buf_len = (uint16_t)sizeof(all_settings);     // plenty big for settings

			 			while (play_buf_len>up_counter){

			 				flash_page_write(patch_mem,all_settings+up_counter);
			 				patch_mem=patch_mem+1;
			 				up_counter+=256;

			 			}


			 			flash_page_write(patch_mem,alt_pots); // stores pots
			 			patch_mem=patch_mem+1;
			 			up_counter=0;
			 			play_buf_len = (uint16_t)sizeof(seq_play_buf);     // notes

			 			while (play_buf_len>up_counter){

			 				flash_page_write(patch_mem,seq_play_buf+up_counter);
			 				patch_mem=patch_mem+1;
			 				up_counter+=256;

			 			}

			read_busy();

		  //  write_once=1;
		  send=0;
		  button_states[send_button]=0;
		  button_states[pan_button]=0;
		  pan=0;
		 //scene_buttons[0]=0;
		 all_update=1;
		  other_buttons=1;
		  button_states[square_buttons_list[patch_save]]=0;
		 first_message=2;
	  }
	 // 	  loop_screen();
}

void flash_read(void){     //can hang here
	read_busy();

	uint16_t settings_len = (uint16_t)sizeof(all_settings);     // plenty big for settings
	uint16_t play_buf_len = (uint16_t)sizeof(seq_play_buf);   // this will change
	uint16_t total_len=0;
	total_len=settings_len+play_buf_len+256+4; // should be a bit more consistent

	uint8_t test_data2[total_len];
	uint8_t test_data3[total_len];
	uint8_t patch_mem=(patch_save&15)<<4;    // 16*16 (4kbyte)   start location
	  patch_mem=16; // off by 256 bytes

	memset (test_data2,0,4);memset (test_data3,0,4);

	test_data3[1]=10;  // starts here
	test_data2[1]=10;
	test_data2[0]=0x03; //read ok , get notes
	test_data2[2]=patch_mem;
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);  // when readin low till the end
	HAL_SPI_TransmitReceive (&hspi1,test_data2, test_data3,  total_len, 100); // request data , always leave extra room (clock) , works
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);  // high end

	memcpy(all_settings,test_data3+4,settings_len); // all settings
	memcpy(alt_pots,test_data3+(settings_len+4),256); // alt pots
	memcpy(seq_play_buf,test_data3+(settings_len+260),play_buf_len); //play buf

	uint16_t d;
	for (d=0;d<2048;d++) {  if(all_settings[d]==255) all_settings[d]=0;
	 }
	settings_storage(); // all settings read out

	tempo=single_settings_list[1];
	if (tempo==255) tempo=120;
	if (tempo<40) tempo=120;



	 for (d=0;d<(sound_set*2);d++) {
					 rand_velocities[d]=(rand()&31)+95;
				 } // write random velocites list





/*	 uint8_t selected_scene=0;
	 uint16_t drum_byte_select=0;
	 	uint8_t drum_byte=0; // get data

	 uint8_t selected_bar=0;
	 uint8_t note_register=0;*/

/*
while (selected_bar<8){				// this tests for data in bars ,if none leaves scene light off

	 	for (d=0;d<16;d++) {  // notes test

		 drum_byte_select= (d>>2)+(selected_scene*4)+(selected_bar*drum_store);
	 	 drum_byte=drum_store_one[drum_byte_select];
	 	if (drum_byte &(1<<((d&3)*2)))       note_register=1;


	 }// end of notes test

	 	if (note_register)    bar_note_register[selected_bar]|=1<<selected_scene;  // ok
	 	//if (note_register)    bar_note_register[selected_bar]+=1<<selected_scene;


	 	note_register=0;
	 	if (selected_scene<15) selected_scene++ ; else {selected_scene=0;selected_bar++;}

} // end of while loop
*/

	 float tempo_hold=1;  // calculate tempo look up

	 float period= cpu_clock/prescaler; //  24584 hz 1/120   /128

	 for (d=1;d<250;d++) {     // calculate ARR vales for TIM10

		 if (d<40) tempo_hold=23437;  // 40bpm

		 else tempo_hold=(period/(0.0166667*d*ppq_set));   // 500khz or 191 prescaler   , 32 ppq maybe do 96ppq  count

		 bpm_table[d]=tempo_hold;
	 }
	 timer_value=bpm_table[tempo]-1;  // starting bpm -1
	 //	timer_value=511;

	 TIM2->PSC=prescaler-1;
	 TIM2->ARR=timer_value;
	 //	TIM2->CCR1=(timer_value/2) ;

	 /*  	 for (d=1;d<8;d++)
		  	{pitch_selected_for_drums[d]=pitch_list_for_drums[pitch_selected_for_drums[d]*8]&127;}  // pitch nrpn data*/

	 send=0;
	 button_states[send_button]=0;
	 button_states[pan_button]=0;
	 pan=0;
	 // scene_buttons[0]=0;
	 all_update=1;
	 memset(button_states_save,10,32);
	 button_states[patch_save+8]=0;
	 first_message=2;

}

// note bank search and replace






