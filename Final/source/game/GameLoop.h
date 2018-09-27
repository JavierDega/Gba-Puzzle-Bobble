//GAME LOOP

#define ARROW_SENS 0.25

//Easy way of cleaning up balls in OAM
void OAMCleanUp(unsigned short index){
	for (int i = 0 ; i < index ; i++){
		
		myOAM[(i*4) + 0] = 0;
		myOAM[(i*4) + 1] = 0;
		myOAM[(i*4) + 2] = 0;
	}
	//CleanUp ballColour indicator
		myOAM[(127*4) + 0] = 0;
		myOAM[(127*4) + 1] = 0;
		myOAM[(127*4) + 2] = 0;
}
//Main game loop
void GameLoop(){
	//Setup
	GameScene myGs = SetUpGame();
	//Sound
	mmStart( MOD_FINALSONG, MM_PLAY_LOOP );
	while (isMenu == 2){
		//At the start of your main game loop , call scanKeys()
		scanKeys();
		//Function in gba_input that checks which buttons are being held down
		u16 myKeysHeld = keysHeld();
		//Movement input
		if (myKeysHeld & KEY_LEFT){
			//Left pressed		
			myGs.myPlayer.rotIndex-= 1*ARROW_SENS;
		}
		if (myKeysHeld & KEY_RIGHT){
			//Right pressed
			myGs.myPlayer.rotIndex+= 1*ARROW_SENS;
		}
		//Fire Input
		if (myKeysHeld & KEY_A ){
			
			//Does bool allow to fire?
			if (myGs.myPlayer.canFire){

				//Arrow if first OAM address so we offset one
				if(myGs.staticBallCount < BALL_MAX){
					
					myGs.staticBallCount++;
					InstanceBall(&myGs);
				}	
			}
		}
		//Some exit condition
		if (myKeysHeld & KEY_SELECT){
			isMenu = 0;
			ClearResultText();
		}
		//Update balls
		SceneUpdate(&myGs);		
		//Draw Interrupt
		VBlankIntrWait();
		//Update Sound
		mmFrame();
	}
	OAMCleanUp(myGs.staticBallCount + 1);
	//Stop sound
	mmStop();	
}
