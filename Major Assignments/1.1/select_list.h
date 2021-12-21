
void selectList(){
	tft.fillScreen (0);
	tft.setCursor(0, 0); //  where  the  characters  will be  displayed
	tft.setTextWrap(false);selectedRest = 0; //  which  restaurant  is  selected?
	for (int16_t i = 0; i < 21; i++) {
		Restaurant r;
		getRestaurant(restDist[i].index , &r);
			if (i !=  selectedRest) {// not  highlighted
				//  white  characters  on  black  background
				tft.setTextColor (0xFFFF , 0x0000);
				} else { //  highlighted
					//  black  characters  on  white  background
					tft.setTextColor (0x0000 , 0xFFFF);
				}
				tft.print(r.name);
				tft.print("\n");
				}
	tft.print("\n");
	if (digitalRead(53)==HIGH){
		// pressed
		
	}

}