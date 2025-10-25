




uint8_t lcd_init[20]={3,3,3,2,2,8,0,8,0,12,0,6,0,1,0,2 };  // 4 bit initialize code ,starting at DB4  ,0-3 send straight  ignore finish  + add delay ,   then send double and wait for finish no delay , no cursor or blinking (12)
uint8_t lcd_delay[20]={100,100,100,10,10,10,10,10,10,100,10,10,10,10,10};  //top then bottom

uint8_t lcd_tx;   // send 2 bytes , RS(0) = data/instruction  ,R/W(1)=read/write , CS(2) =enable  as well ,bit 3= backlight ,  bits 4-7=data
uint8_t lcd_rx;  // check for
uint8_t lcd_pos;

I2C_HandleTypeDef hi2c1;


uint8_t lcd_page1_ref[]={1,3,4,2,8,2,11,2,30,2,24,2,27,2,16,3,21,2,14,2,0,0,0,0};   // start and length of *lcd_page1 references ,last entry can be trouble,  keep 0 at the end to finish
uint8_t page_up_counter; // tracks menu search
int8_t var_size; // track size of variables for lcd






	void lcd_start(void){


		uint8_t  state_check= HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(0x4E), 3, 5);   // Always left shift  0x27 or it  will fail

		HAL_Delay(10);

		if (	 (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(0x4E), 3, 10))==HAL_OK	)    printf("I2C is ok ");   else printf("I2C fail=");
		printf(" %d \n ",state_check);   // check if the slave i2c is responding
		HAL_Delay(1000);
		for (i=0;i<16;i++){

			lcd_tx=(lcd_init[i]<<4)&255; // always also no  RS
			lcd_tx=lcd_tx+12;  //4 enable  + 8 backlight  ?
			if (i==14)  lcd_tx=lcd_tx-8;
		//	if (i>11)  lcd_tx=lcd_tx+1;  // rs data

			HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , &lcd_tx, 1, 100);  // enable high

			HAL_Delay(lcd_delay[i]);
			lcd_tx=8; // turn off enable always !!!
			HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , &lcd_tx, 1, 100);  // enable low
			HAL_Delay(1);


		}
	}

	void  lcd_print (uint8_t pos, char print){      // print character to a position 0-15 64-81

		uint8_t lcd_send[4];
		if (pos>31) pos=31;

		if (pos>15) pos=pos+48;  //
		pos=pos&127;
		//pos=80-pos;
		lcd_send[0]=(((pos&240)+128))+12;   // rs is P0 rs off
		lcd_send[1]=8;
		lcd_send[2]=((pos&15)<<4)+12;
		lcd_send[3]=8;
		HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , lcd_send, 4, 100);


		HAL_Delay(1);


		if (print<32) print=95;
	//	print=print+48;   // B

		lcd_send[0]=((print)&240)+13;  // rs is p0   rs on
		lcd_send[1]=8;
		lcd_send[2]=((print&15)<<4)+13;
		lcd_send[3]=8;
		HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , lcd_send, 4, 100);

	}


void lcd_mem(void){   // updates lcd buffer

	for (lcd_pos=0;lcd_pos<32;lcd_pos++){
		if (lcd_buffer[lcd_pos]!=lcd_buffer_mem[lcd_pos]){

			lcd_print(lcd_pos,lcd_buffer[lcd_pos]);
			lcd_buffer_mem[lcd_pos]=lcd_buffer[lcd_pos];

		}


	}


}


void lcd_menu_vars(void){     // grab vars for menu


	uint8_t lcd_page_extra=0;
	uint8_t extra_value1=LFO_phase_list[scene_buttons[0]];
	uint8_t current_scale=pattern_scale_list[pattern_select]; // scale select variable
	uint8_t *lcd_message_list[]={&current_scale,&current_accent,NULL,NULL

	};   // list of variables for lcd messages
	lcd_page_extra=*lcd_message_list[lcd_messages_select]; // grab data from variables

	uint8_t *lcd_page1 []={ // need to enable more pages, also letters etc

			&extra_value1,
			&seq_step_long,
			&seq_current_step,
			&loop_current_speed,
			&current_midi,
			&tempo,
			&note_enable_list_selected,
			&current_scale,
			&current_accent,
			&lcd_page_extra

			};			// lcd pointers menu page 1 list  , start position + variable+ length

	uint8_t lcd_page1_ref[]={
			0,3,7,10,13,16,19,22,25,28,0,0,0
	};


	page_up_counter=0;
	char lcd_char[5];    // string   to print

	int dec_hold;

	for (lcd_pos=0;lcd_pos<32;lcd_pos++){   // check for menu values

		if ((lcd_pos>13)&&(lcd_downcount)) {page_up_counter=9;}  // exit if downcount is enabled

		if (lcd_pos==lcd_page1_ref[page_up_counter]) {


			//var_size=lcd_page1_ref[page_up_counter+1]; // set length

			dec_hold=*lcd_page1[page_up_counter];   //  hold numbers only for now

			if (dec_hold<10)  {sprintf (lcd_char,"%d", dec_hold) ;var_size=1;}  else if (dec_hold<100)  {sprintf (lcd_char,"%d", dec_hold) ;var_size=2;}
			else if (dec_hold>99)  {sprintf (lcd_char,"%d", dec_hold) ;var_size=3;}   // check length and add space

			if (var_size==1)  {lcd_buffer[lcd_pos+2]=lcd_char[0];lcd_buffer[lcd_pos]=32;lcd_buffer[lcd_pos+1]=32;}
			if (var_size==2)  {lcd_buffer[lcd_pos+1]=lcd_char[0]; lcd_buffer[lcd_pos+2]=lcd_char[1]; lcd_buffer[lcd_pos]=32;}
			if (var_size==3)  {lcd_buffer[lcd_pos]=lcd_char[0];lcd_buffer[lcd_pos+1]=lcd_char[1]; lcd_buffer[lcd_pos+2]=lcd_char[2];  }
			var_size=1;

			page_up_counter++;
			if (lcd_page1_ref[page_up_counter]==0)   {page_up_counter=0;lcd_pos=0;return;} // reset if empty
		}

	}


}



void lcd_message(void){    // prints controls being used while being used, can have memory error ,redo later

	char *lcd_messages[] = {"Note_scale  ","Note_accent  ","Second_bank  ","Pitch_sets   ","MIDI_channel ","do_not_use   ","LFO_level    ","Drum_pitch   ","LFO_rate     "

	};  // set to 13 char long, no 5 cause to crash it,rest ok   ?


	uint8_t lengthofstring=strlen(lcd_messages[lcd_messages_select])+16;
	if(lengthofstring>29) lengthofstring=16; // check for o/f
	char  *select;
	char select_char;

	for (lcd_pos=16;lcd_pos<lengthofstring;lcd_pos++){

		select=(lcd_messages[lcd_messages_select]);
		select_char=*(select+(lcd_pos-16));

		if (lcd_downcount==1) lcd_buffer[lcd_pos]=32;     else      lcd_buffer[lcd_pos]=select_char;



	}
	lcd_control=lcd_messages_select;
	lcd_pos=0;



}

void led_full_clear(void){   // ok ,delayed initial screen clear , won't work if not connected
if(led_clear) {


	led_clear++;
	if (led_clear==30)  memset (button_states,10,99);	// after an elapsed time
if (led_clear==32)  memset (button_states,0,99);
if (led_clear>40) led_clear=0;
}


}








