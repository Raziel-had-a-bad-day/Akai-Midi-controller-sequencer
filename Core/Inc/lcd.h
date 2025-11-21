




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

		//	HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , &lcd_tx, 1, 100);  // enable high
			HAL_I2C_Master_Transmit_IT( &hi2c1, (uint16_t) 0x4E , &lcd_tx, 1);
			HAL_Delay(lcd_delay[i]);
			lcd_tx=8; // turn off enable always !!!
		//	HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , &lcd_tx, 1, 100);  // enable low
			HAL_I2C_Master_Transmit_IT ( &hi2c1, (uint16_t) 0x4E , &lcd_tx, 1);  // enable low

			HAL_Delay(1);
			memset(lcd_buffer,48,32);
			memset(lcd_buffer_mem,32,32);


		}
	}

	void  lcd_print (uint8_t pos, char print){      // print character to a position 0-15 64-81

		uint8_t lcd_send[8];

		if (pos>31) pos=31;


		if (pos>15) pos=pos+48;  // position
		pos=pos&127;
		//pos=80-pos;
		lcd_send[0]=(((pos&240)+128))+12;   // rs is P0 rs off
		lcd_send[1]=8;
		lcd_send[2]=((pos&15)<<4)+12;
		lcd_send[3]=8;
		//HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , lcd_send, 4, 100);

//
		//HAL_Delay(1);


		if (print<32) print=95;   // character, filtered
	//	print=print+48;   // B

		lcd_send[4]=((print)&240)+13;  // rs is p0   rs on
		lcd_send[5]=8;
		lcd_send[6]=((print&15)<<4)+13;
		lcd_send[7]=8;
		memcpy(I2C_buffer+lcd_buffer_counter,lcd_send,8);
		lcd_buffer_counter=lcd_buffer_counter+8;
	//	HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , lcd_send, 4, 100);

	}


void lcd_mem(void){   // updates lcd buffer , v
	lcd_buffer_counter=0;

	for (lcd_pos=0;lcd_pos<32;lcd_pos++){   // one character at a time
		if ((lcd_buffer[lcd_pos]!=lcd_buffer_mem[lcd_pos]) && (!lcd_buffer_full))             {

			lcd_print(lcd_pos,lcd_buffer[lcd_pos]);
			lcd_buffer_mem[lcd_pos]=lcd_buffer[lcd_pos];

		//	HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , I2C_buffer, lcd_buffer_counter, 100);lcd_buffer_counter=0;`
		}


	}
	lcd_buffer_full=1;

//	{HAL_I2C_Master_Transmit ( &hi2c1, (uint16_t) 0x4E , I2C_buffer, lcd_buffer_counter, 100); lcd_buffer_full=0;}// keep it short or eventually it breaks
	  if (HAL_I2C_GetState(&hi2c1))		{HAL_I2C_Master_Transmit_IT ( &hi2c1, (uint16_t) 0x4E , I2C_buffer, lcd_buffer_counter); lcd_buffer_full=0;}     // works ok with normal cdc running


	  // can randomly kill usb
}


void lcd_menu_vars(uint8_t selected_var ,uint8_t var_position){     // print vars to selected position on lcd

//	if (lcd_downcount) return; // exit if downcount is enabled

	uint8_t scene=scene_buttons[0];
	//uint8_t *lcd_page1 []={ // need to enable more pages,this needs to be here,  also letters etc
		if (selected_var>10) selected_var=10;
		if (var_position>29) var_position=29;

		// works , menu data section , variables need to be here  // can be somewhere else but it needs to run sommetime

		switch(selected_var){   //select only needed vars


		case 0:lcd_item[0].var=	&LFO_phase_list[scene];  lcd_item[0].name="LFO_phase";break;
		case 1:lcd_item[1].var=&seq_step_long;lcd_item[1].name="Bar_count";break;

		case 2:lcd_item[2].var=&loop_lfo_out[scene+32];lcd_item[2].name="LFO_out";break;
		case 3:lcd_item[3].var=&pattern_scale_list[scene];lcd_item[3].name="Scale_select";break;
		case 4:lcd_item[4].var=&midi_channel_list[scene];lcd_item[4].name="Midi channel";break;

		case 5:lcd_item[5].var=&tempo;lcd_item[5].name="Tempo";break;
		case 6:lcd_item[6].var=&note_enable_list_selected;lcd_item[6].name="LFO_low_high";break;
		case 7:lcd_item[7].var=&pitch_list_for_drums[pitch_selected_for_drums[scene]+(scene*8)];lcd_item[7].name="Pitch_adjust";break;
		case 8:lcd_item[8].var=&note_accent[scene];lcd_item[8].name="Note_accent";break;
		case 9:lcd_item[9].var=&lfo_settings_list[(scene*2)];lcd_item[9].name="filter_rate";break;
		case 10:lcd_item[10].var=&lfo_settings_list[(scene*2)+1];lcd_item[10].name="filter_gain";break;
		default:break;}

		char lcd_char[5];    // string   to print

		int dec_hold;

			dec_hold=*lcd_item[selected_var].var;   //  hold numbers only for now, migth do 16 bit or strings

			if (dec_hold<10)  {sprintf (lcd_char,"%d", dec_hold) ;var_size=1;}  else if (dec_hold<100)  {sprintf (lcd_char,"%d", dec_hold) ;var_size=2;}  //sort digits for number
			else if (dec_hold>99)  {sprintf (lcd_char,"%d", dec_hold) ;var_size=3;}   // check length and add space

			if (var_size==1)  {lcd_buffer[var_position+2]=lcd_char[0];lcd_buffer[var_position]=32;lcd_buffer[var_position+1]=32;}   // 32=space 48=0 65=A
			if (var_size==2)  {lcd_buffer[var_position+1]=lcd_char[0]; lcd_buffer[var_position+2]=lcd_char[1]; lcd_buffer[var_position]=32;}
			if (var_size==3)  {lcd_buffer[var_position]=lcd_char[0];lcd_buffer[var_position+1]=lcd_char[1]; lcd_buffer[var_position+2]=lcd_char[2];  }
			var_size=1;

			}



void lcd_message(void){    // prints pot used




	uint8_t lengthofstring=strlen(lcd_item[lcd_messages_select].name)+16;
	memset(lcd_buffer,32,32); //clear

	if(lengthofstring>29) lengthofstring=16; // check for o/f
	char  *select;
	char select_char;

	for (lcd_pos=16;lcd_pos<lengthofstring;lcd_pos++){

		select=(lcd_item[lcd_messages_select].name);
		select_char=*(select+(lcd_pos-16));

		if (lcd_downcount==1) lcd_buffer[lcd_pos]=32;     else      lcd_buffer[lcd_pos]=select_char;

	}  //writes bottom line text
	lcd_menu_vars(lcd_messages_select,9);
	//lcd_buffer[3]=lcd_item[lcd_messages_select].var; //still needs more here or separate var handling
	lcd_control=lcd_messages_select;
	lcd_pos=0;



}

void lcd_menu_pages(uint8_t page){  // various setups for different pages  ,0-3 for now ,position needs to be around every 3 digits or it might miss some of the digits


	switch (page){
	case 1:lcd_menu_vars(1,3);lcd_menu_vars(4,12); break;
	case 2:break;
	case 3:break;
	default:break;

	}

}







void led_full_clear(void){   // ok ,delayed initial screen clear , won't work if not connected
if(led_clear) {


	led_clear++;
	if (led_clear==30)  memset (button_states,10,99);	// after an elapsed time
if (led_clear==32)  memset (button_states,0,99);
if (led_clear>40) led_clear=0;
}


}








