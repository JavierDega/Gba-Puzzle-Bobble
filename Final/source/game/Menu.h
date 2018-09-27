//MENU
#include <gba_input.h>
#include <gba_systemcalls.h>

//Point to background control registers and change settings
unsigned short * myBg0 = (unsigned short*) 0x04000008;
unsigned short * myBg1 = (unsigned short*) 0x0400000A;
unsigned short * myBg2 = (unsigned short*) 0x0400000C;
unsigned short * myBg3 = (unsigned short*) 0x0400000E;
//Main menu
void MenuLoop(){
	//Menu screen Bases
				    //Prior   //CBB	     //COLMODE  //SBB(MENU)//SIZE(256*256)
	myBg0[0] = ( (0 << 0) | (0 << 2) | (0 << 7) | (4 << 8) | (0 << 14) );
	myBg1[0] = ( (1 << 0) | (0 << 2) | (0 << 7) | (5 << 8) | (0 << 14) );
	myBg2[0] = ( (2 << 0) | (0 << 2) | (0 << 7) | (6 << 8) | (0 << 14) );
	myBg3[0] = ( (3 << 0) | (0 << 2) | (0 << 7) | (7 << 8) | (0 << 14) ); 
	//OAM Pointer
	int x = 60;
						//y		   //		  //	     //
	myOAM[(0*4) + 0] = ( (75 << 0) | (0 << 8) | (0 << 9) | (0 << 14) );
						//x		   //         
	myOAM[(0*4) + 1] = ( (x << 0) | (0 << 12) | (0 << 13) | (1 << 14) );
	
	myOAM[(0*4) + 2] = ( (1 << 0) | (0 << 10) | (0 << 12) );
	//Sound
	mmStart( MOD_MENUSONG, MM_PLAY_LOOP );
	while (isMenu == 0){
		
		//At the start of your main game loop , call scanKeys()
		scanKeys();
		//Function in gba_input that checks which buttons are being held down
		u16 myKeysHeld = keysHeld();
		
		if (myKeysHeld & KEY_LEFT){
			//Left pressed		
			x = 60;
		}
	
		if (myKeysHeld & KEY_RIGHT){
			//Right pressed
			x = 115;
		}
		//Bitwise and to identify menu state when pressing enter
		if (myKeysHeld & KEY_START){
			
			if (x == 60){
				//Is at x = 60 meaning Left position (GO!) selected currently
				isMenu = 2;
			}
			else {
				//Credits section?
				isMenu = 1;
			}
		}
		//Update pointer OAM 
		myOAM[(0*4) + 1] = ((x << 0 ) | (0 << 12) | (0 << 13) | ( 1 << 14) );
		//Frame
		VBlankIntrWait();
		mmFrame();
	}
	//Stop sound
	mmStop();
}
//Credits menu
void Menu2Loop(){
	//Credits menu screen Bases
				    //Prior   //CBB	     //COLMODE  //SBB(MENU)//SIZE(256*256)
	myBg0[0] = ( (0 << 0) | (0 << 2) | (0 << 7) | (8 << 8) | (0 << 14) );
	myBg1[0] = ( (1 << 0) | (0 << 2) | (0 << 7) | (9 << 8) | (0 << 14) );
	myBg2[0] = ( (2 << 0) | (0 << 2) | (0 << 7) | (10 << 8) | (0 << 14) );
	myBg3[0] = ( (3 << 0) | (0 << 2) | (0 << 7) | (11 << 8) | (0 << 14) ); 
	//OAM Pointer
	int y = 35;
						//y		   //		  //	     //
	myOAM[(0*4) + 0] = ( (y << 0) | (0 << 8) | (0 << 9) | (0 << 14) );
						//x		   //         
	myOAM[(0*4) + 1] = ( (22 << 0) | (0 << 12) | (0 << 13) | (1 << 14) );
	
	myOAM[(0*4) + 2] = ( (1 << 0) | (0 << 10) | (0 << 12) );
	//Sound
	mmStart(MOD_MENU2SONG, MM_PLAY_LOOP);
	while (isMenu == 1){
		
		//At the start of your main game loop , call scanKeys()
		scanKeys();
		//Function in gba_input that checks which buttons are being held down
		u16 myKeysHeld = keysHeld();
		
		if (myKeysHeld & KEY_UP){
			//Left pressed		
			y = 35;
		}
	
		if (myKeysHeld & KEY_DOWN){
			//Right pressed
			y= 67;
		}
		//Bitwise and to identify menu state when pressing enter
		if (myKeysHeld & KEY_SELECT){
			isMenu = 0;
			ClearResultText();
		}
		//Update pointer OAM 
		myOAM[(0*4) + 0] = ((y << 0 ) | (0 << 8) | (0 << 9) | (0 << 14) );
		//Frame
		VBlankIntrWait();
		mmFrame();
	}
	//Stop sound
	mmStop();
}
