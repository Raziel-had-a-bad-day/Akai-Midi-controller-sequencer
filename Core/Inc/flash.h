

void flash_page_write(uint8_t page_select,uint8_t* data){    // write single page (256 bytes )

	 	 	 	 	 	 	 	 uint8_t test_data3[270]={0,10,0,0}; // [3]=bytes , [2]=page [1] = 64k block

	 	 	 	 	 	 	 	 memcpy  (test_data3+4 ,data,  256);
	 	 	 	 	 	 	 	 test_data3[2]=page_select;


	 	 	 	 	 	 	 	 test_data3[0]=0x06;

								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
								  HAL_SPI_Transmit(&hspi1, test_data3, 1, 100);
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
								  HAL_Delay(20);


								  test_data3[0]=0x02; //write ,page program
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);   // low
								  HAL_SPI_Transmit(&hspi1, test_data3 ,260, 1000);  //address,then data
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);  // high end
								  HAL_Delay(200);

								  test_data3[0]=0x04; //disable write
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0); // low
								  HAL_SPI_Transmit(&hspi1, test_data3, 1, 100);
								  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);   // high end
								  HAL_Delay(20);


}

void settings_storage(void){   // runs to store setting and read back

			uint8_t *settings[variable_count]={scene_transpose,pot_states,pot_tracking,mute_list,note_accent,midi_channel_list, pitch_list_for_drums,pattern_scale_list,lfo_settings_list,single_settings_list,pitch_change_store,LFO_low_list
			,LFO_high_list,LFO_phase_list};
			uint8_t settings_multi[variable_count]={1,1,4,1,1,1,4,1,2,1,4,4,4,4};   // sets length,  sound_set*x
			uint8_t settings_temp[64];
			uint8_t settings_total=0;  //adds up position
			uint8_t length=0;
			tempo=single_settings_list[1];

			for (i=0;i<variable_count;i++){

				length=(settings_multi[i]*sound_set);
				if(settings_write_flag) {		memcpy(settings_temp,settings[i],length);	// copy to temp
				memcpy(all_settings+settings_total,settings_temp,length);}
				else {

					memcpy(settings_temp,all_settings+settings_total,length);  // copy value
					memcpy(settings[i],settings_temp,length);                }  //

		settings_total=settings_total+length;

			}

			for (i=0;i<1023;i++){if (all_settings[i]>127) all_settings[i]=0;}  // reset values just in case
			settings_write_flag=0;

}

void flash_write(void){					// too much crap needs to simplify , easy mistakes
	  if ((send) & (write_once==0)&&(!shift)          ){

		 		  uint8_t test_data3[270]={0,10,0,0};
		 		  uint8_t patch_mem=(patch_save&15)<<4;    // 16*16 (4kbyte)   start location for series of data stored
		 		  patch_mem=0;
		 		  test_data3[2]=patch_mem;

		 		//  memcpy  (test_data3+4 ,drum_store_one+256,  256);   // maybe drum data


		 		  test_data3[0]=0x06; //enable write each time
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
		 		  HAL_SPI_Transmit(&hspi1, test_data3, 1, 1000);
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
		 		  HAL_Delay(5);

		 		  //----formAT SECTION


		 		  test_data3[0]=0x20; //sector erase 4k (block is 0x52) , this is need , setting FF doesn't seem to work for page erase


		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);         // enable for sector erase   , stays empty when enabled
		 		  HAL_SPI_Transmit(&hspi1,  test_data3, 4, 1000);   //erase sector ,works       4kbytes   (block erase=32kbytes)
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
		 		  HAL_Delay(150);  // S
		 		  // test write


		 		  test_data3[0]=0x04; //disable write

		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0); // low
		 		  HAL_SPI_Transmit(&hspi1, test_data3, 1, 100);
		 		  HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);   // high end
		 		  HAL_Delay(20);


		  //sector erase works

		//  memcpy  (test_data3+4 ,scene_memory,  256);
			  flash_page_write(patch_mem,drum_store_one+256);   // bit mixed up

		  	  single_settings_list[1]=tempo;

		  	  settings_write_flag=1;
			settings_storage();

			//memcpy  (test_data3+4 ,all_settings,  256); // copy

			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,all_settings);   // needs more
			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,drum_store_one);
			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,drum_store_one+512);
			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,drum_store_one+768);
			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,alt_pots);
			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,all_settings+256); // second half ,getting full
			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,all_settings+512);
			patch_mem=patch_mem+1;
			flash_page_write(patch_mem,all_settings+768);



		//  memcpy  (test_data3+4 ,drum_store_one,  256);  // drums


								  HAL_Delay(500);

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
	  	  loop_screen();
}

void flash_read(void){     // 1kbyte for now
	HAL_Delay(100);

	uint8_t test_data2[2056]={0,10,0,0};
	uint8_t test_data3[2048]={0,10,0,0};
	uint8_t patch_mem=(patch_save&15)<<4;    // 16*16 (4kbyte)   start location

	test_data2[0]=0x03; //read ok , get notes
	test_data2[2]=patch_mem;
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);  // when readin low till the end
	HAL_SPI_TransmitReceive (&hspi1,test_data2, test_data3,  2048, 100); // request data , always leave extra room (clock) , works
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);  // high end
	HAL_Delay(100);

	memcpy(drum_store_one+256,test_data3+4,256);    // buttons data

	memcpy(all_settings,test_data3+260,256); // copy back settings block

	settings_storage();

	memcpy(drum_store_one,test_data3+516,256);   // next block (+4)
	memcpy(drum_store_one+512,test_data3+772,512); //
	memcpy(alt_pots,test_data3+1284,256);
	memcpy(all_settings+256,test_data3+1540,256); // second half of settings
	memcpy(program_change_automation,alt_pots+128,32); // program change data


	tempo=all_settings[250];
	if (tempo==255) tempo=120;
	if (!tempo) tempo=120;


	uint8_t d;
	 for (d=0;d<32;d++) {
					 rand_velocities[d]=(rand()&31)+95;
				 } // write random velocites list
	//midi_cue_fill();  // not needed anymore


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






