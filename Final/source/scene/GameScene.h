//GAMESCENE STRUCT, LOGIC AND ALGORITHMS
#include <math.h>


#define N 256 //SCALE FACTOR;FIXED POINT MATH USED EXTENSIVELY ON UPDATE
#define X_MIN 46 //16 Balls Horizontally , 15 vertically. 16*15 = 240. We have 126 addresses so we won't place all of them.
#define X_MAX 170
#define Y_MAX 120
#define START_POSX 115
#define START_POSY 138
#define VEL_MULT 2
#define BALL_SIZE 8
#define BALL_MAX 126
#define SCORE_MAX 75
#define TIMER_FREQ 4
#define GRID_COLUMNS 16
#define GRID_ROWS 15

extern int isMenu;
//First Object Attribute Memory address
extern unsigned short* myOAM;
//Game screens which we might modify at runtime
extern unsigned short* myScreenBase12;
extern unsigned short* myScreenBase13;
//Assembly functions (ARM THUMB2)
extern void addAndLoopFunc(unsigned short * number, unsigned short begin, unsigned short end);
extern void updateFunc(int * px, int * py, int vx, int vy);
extern void updateExtentFunc(int * maxExt , int pos, int ballSize);
extern void edgeBounceFunc(int px, int * vx, int xmin, int xmax);
extern void clampFunc(int * val, int min, int max);
//All gameScene related stuff
typedef struct GameSceneStr{
	Ball movingBall;
	StaticBall staticBalls[GRID_COLUMNS][GRID_ROWS];
	unsigned short yTimer;
	unsigned short yOffset;
	unsigned short staticBallCount;
	unsigned short score;
	unsigned short nextColour;
	Player myPlayer;
}GameScene;

//Called in BallChainRoutine
void DeleteClustersRecursive(GameScene* myGs , int col , int row){
	//All balls this function is run on , will be deleted at the end of the routine
	StaticBall * theStaticBall = &(myGs->staticBalls[col][row]);
	//Up 
	int up = row+1;
	//Down
	int down = row-1;
	//Left
	int left = col-1;
	//Right
	int right = col+1;
	
	if (theStaticBall->cb.upCheck){
		theStaticBall->cb.upCheck = false;
		myGs->staticBalls[col][up].cb.downCheck = false;
		DeleteClustersRecursive(myGs , col ,up);
	}
	if (theStaticBall->cb.downCheck){
		theStaticBall->cb.downCheck = false;
		myGs->staticBalls[col][down].cb.upCheck = false;
		DeleteClustersRecursive(myGs , col ,down);
	}
	if (theStaticBall->cb.leftCheck){
		theStaticBall->cb.leftCheck = false;
		myGs->staticBalls[left][row].cb.rightCheck = false;
		DeleteClustersRecursive(myGs , left ,row);
	}
	if (theStaticBall->cb.rightCheck){
		theStaticBall->cb.rightCheck = false;
		myGs->staticBalls[right][row].cb.leftCheck = false;
		DeleteClustersRecursive(myGs , right ,row);
	}
	if (row % 2){
		//Its odd = offset = extra neighbours are top,bottom right
		if (theStaticBall->cb.trCheck){
			theStaticBall->cb.trCheck = false;
			myGs->staticBalls[right][up].cb.blCheck = false;
			DeleteClustersRecursive(myGs , right , up);
		}
		if (theStaticBall->cb.brCheck){
			theStaticBall->cb.brCheck = false;
			myGs->staticBalls[right][down].cb.tlCheck = false;
			DeleteClustersRecursive(myGs , right , down);
		}
	}
	else {
		//Its even = no offset = extra neighbours are top , bottom left
		if (theStaticBall->cb.tlCheck){
			theStaticBall->cb.tlCheck = false;
			myGs->staticBalls[left][up].cb.brCheck = false;
			DeleteClustersRecursive(myGs , left , up);
		}
		if (theStaticBall->cb.blCheck){
			theStaticBall->cb.blCheck = false;
			myGs->staticBalls[left][down].cb.trCheck = false;
			DeleteClustersRecursive(myGs, left , down);
		}
	}
	//Delete data if it isn't already (See DeleteClustersFlaws.png)
	if (theStaticBall->isOccupied){
		BallDestroy(theStaticBall , true);
	}
}
//Called in BallChainRoutine
void FindClustersRecursive (GameScene * myGs , int col , int row){
	StaticBall * theStaticBall = &(myGs->staticBalls[col][row]);
	//Up 
	int up = row+1;
	//Down
	int down = row-1;
	//Left
	int left = col-1;
	//Right
	int right = col+1;
	
	if (up < 15 && theStaticBall->cb.upCheck == false){
		//Up index is valid
		StaticBall * upStaticBall  = &(myGs->staticBalls[col][up]);
		if (upStaticBall->isOccupied && theStaticBall->colour == upStaticBall->colour){
			//They share colour
			theStaticBall->chain++;
			upStaticBall->chain++;	
			theStaticBall->cb.upCheck = true;
			upStaticBall->cb.downCheck = true;
			FindClustersRecursive(myGs , col , up);
		}
	}
	if (down >=0 && theStaticBall->cb.downCheck == false){
		//Down index is valid
		StaticBall * downStaticBall  = &(myGs->staticBalls[col][down]);
		if (downStaticBall->isOccupied && theStaticBall->colour == downStaticBall->colour){
			//They share colour
			theStaticBall->chain++;
			downStaticBall->chain++;	
			theStaticBall->cb.downCheck = true;
			downStaticBall->cb.upCheck = true;
			FindClustersRecursive(myGs , col , down);
		}
	}
	if (left >=0 && theStaticBall->cb.leftCheck == false){
		//Left index is valid
		StaticBall * leftStaticBall = &(myGs->staticBalls[left][row]);
		if (leftStaticBall->isOccupied && theStaticBall->colour == leftStaticBall->colour){
			//They share colour
			theStaticBall->chain++;
			leftStaticBall->chain++;
			theStaticBall->cb.leftCheck = true;
			leftStaticBall->cb.rightCheck = true;
			FindClustersRecursive(myGs, left , row);
		}
	}
	if (right < 16 && theStaticBall->cb.rightCheck == false){
		//Right index is valid
		StaticBall* rightStaticBall = &(myGs->staticBalls[right][row]);
		if (rightStaticBall->isOccupied && theStaticBall->colour == rightStaticBall->colour){
			//They share colour
			theStaticBall->chain++;
			rightStaticBall->chain++;
			theStaticBall->cb.rightCheck = true;
			rightStaticBall->cb.leftCheck = true;
			FindClustersRecursive(myGs , right , row);
		}
	}
	if (row % 2) {
		//Odd row , they are offset
		//Meaning their extra neighbours are top , bottom right diagonals
		//Top right
		if (right < 16 && up < 15 && theStaticBall->cb.trCheck == false){
			//Top right index is valid
			StaticBall* topRightStaticBall = &(myGs->staticBalls[right][up]);
			if (topRightStaticBall->isOccupied && theStaticBall->colour == topRightStaticBall->colour){
				//They share colour
				theStaticBall->chain++;
				topRightStaticBall->chain++;
				theStaticBall->cb.trCheck = true;
				topRightStaticBall->cb.blCheck = true;
				FindClustersRecursive(myGs, right , up);
			}
		}
		//Bottom right
		if (right < 16 && down >= 0 && theStaticBall->cb.brCheck == false){
			//Bottom right index is valid
			StaticBall * bottomRightStaticBall = &(myGs->staticBalls[right][down]);
			if (bottomRightStaticBall->isOccupied && theStaticBall->colour == bottomRightStaticBall->colour){
				//They share colour
				theStaticBall->chain++;
				bottomRightStaticBall->chain++;
				theStaticBall->cb.brCheck = true;
				bottomRightStaticBall->cb.tlCheck = true;
				FindClustersRecursive(myGs , right , down);
			}
		}
	}
	else {
		//Even row , no offset
		//Meaning their extra neighbours are top , bottom left diagonals
		//Top left
		if (left >= 0 && up < 15 && theStaticBall->cb.tlCheck == false){
			//Top right index is valid
			StaticBall* topLeftStaticBall = &(myGs->staticBalls[left][up]);
			if (topLeftStaticBall->isOccupied && theStaticBall->colour == topLeftStaticBall->colour){
				//They share colour
				theStaticBall->chain++;
				topLeftStaticBall->chain++;
				theStaticBall->cb.tlCheck = true;
				topLeftStaticBall->cb.brCheck = true;
				FindClustersRecursive(myGs, left , up);
			}
		}
		//Bottom left
		if (left >= 0 && down >= 0 && theStaticBall->cb.blCheck == false){
			//Bottom right index is valid
			StaticBall * bottomLeftStaticBall = &(myGs->staticBalls[left][down]);
			if (bottomLeftStaticBall->isOccupied && theStaticBall->colour == bottomLeftStaticBall->colour){
				//They share colour
				theStaticBall->chain++;
				bottomLeftStaticBall->chain++;
				theStaticBall->cb.blCheck = true;
				bottomLeftStaticBall->cb.trCheck = true;
				FindClustersRecursive(myGs , left , down);
			}
		}
	}
}
//Identifies ball chains. Called in NextTurn()
void BallChainRoutine (GameScene* myGs , int newBallCol ,int newBallRow){
	//Find clusters by identifying number of adjacent balls with same colour
	FindClustersRecursive(myGs , newBallCol , newBallRow);
	//So if there's a new chain , we will have a ball of chain value >=2;
	//Therefore we try to find it
	bool breakOuterLoop = false;
	for (int row = 0 ; row < GRID_ROWS ; row++){
		for (int col = 0 ; col < GRID_COLUMNS ; col++){
			if (myGs->staticBalls[col][row].chain >= 2){
				DeleteClustersRecursive(myGs , col , row);
				//No need to remove more chains , since we can only create one
				//Per turn
				breakOuterLoop = true;
				break;
			}
		}
		if (breakOuterLoop)break;
	}
}
//Called in HangingBallsRoutine
bool FindSupport ( GameScene * myGs, int col , int row){
	//Check for all colliding edges , and if found a supported ball ,
	//Get isHanging = true value from it
	StaticBall * theStaticBall = &(myGs->staticBalls[col][row]);
	//Skip if already analyzed
	if (theStaticBall->isHanging){return false;}
	//Up 
	int up = row+1;
	//Down
	int down = row-1;
	//Left
	int left = col-1;
	//Right
	int right = col+1;

	//Then direct neighbours are : up , down , top right , bottom right.
	if (up < 15){
		//Up index is valid
		StaticBall * upStaticBall  = &(myGs->staticBalls[col][up]);
		if (upStaticBall->isOccupied && upStaticBall->isHanging){
			//This one hangs from it
			theStaticBall->isHanging = true;
			return true;
		}
	}
	if (down >=0 ){
		//Down index is valid
		StaticBall * downStaticBall  = &(myGs->staticBalls[col][down]);
		if (downStaticBall->isOccupied && downStaticBall->isHanging){
			//This one hangs from it
			theStaticBall->isHanging = true;
			return true;
		}
	}
	if (left >=0 ){
		//Left index is valid
		StaticBall * leftStaticBall = &(myGs->staticBalls[left][row]);
		if (leftStaticBall->isOccupied && leftStaticBall->isHanging){
			//This one hangs from it
			theStaticBall->isHanging = true;
			return true;
		}
	}
	if (right < 16){
		//Right index is valid
		StaticBall* rightStaticBall = &(myGs->staticBalls[right][row]);
		if (rightStaticBall->isOccupied && rightStaticBall->isHanging){
			//This one hangs from it
			theStaticBall->isHanging = true;
			return true;
		}
	}
	if (row % 2) {
		//Odd row , they are offset
		//Meaning their extra neighbours are top , bottom right diagonals
		//Top right
		if (right < 16 && up < 15){
			//Top right index is valid
			StaticBall* topRightStaticBall = &(myGs->staticBalls[right][up]);
			if (topRightStaticBall->isOccupied && topRightStaticBall->isHanging){
				//This one hangs from it
				theStaticBall->isHanging = true;
				return true;
			}
		}
		//Bottom right
		if (right < 16 && down >= 0){
			//Bottom right index is valid
			StaticBall * bottomRightStaticBall = &(myGs->staticBalls[right][down]);
			if (bottomRightStaticBall->isOccupied && bottomRightStaticBall->isHanging){
				//This one hangs from it
				theStaticBall->isHanging = true;
				return true;
			}
		}
	}
	else {
		//Even row , no offset
		//Meaning their extra neighbours are top , bottom left diagonals
		//Top left
		if (left >= 0 && up < 15){
			//Top right index is valid
			StaticBall* topLeftStaticBall = &(myGs->staticBalls[left][up]);
			if (topLeftStaticBall->isOccupied && topLeftStaticBall->isHanging){
				//This one hangs from it
				theStaticBall->isHanging = true;
				return true;
			}
		}
		//Bottom left
		if (left >= 0 && down >= 0){
			//Bottom right index is valid
			StaticBall * bottomLeftStaticBall = &(myGs->staticBalls[left][down]);
			if (bottomLeftStaticBall->isOccupied && bottomLeftStaticBall->isHanging){
				//This one hangs from it
				theStaticBall->isHanging = true;
				return true;
			}
		}
	}
	//If it hasn't returned yet
	return false;
}
//Called in HangingBallsRoutine
void SpreadSupportRecursive ( GameScene * myGs, int col , int row){
	//Check for all colliding edges
	//If they are unsupported , give them support and rerun this function on them
	//The order in which we analyze each border might matter?**
	//Up 
	int up = row+1;
	//Down
	int down = row-1;
	//Left
	int left = col-1;
	//Right
	int right = col+1;
	
	if (left >=0 ){
		//Left index is valid
		StaticBall * leftStaticBall = &(myGs->staticBalls[left][row]);
		if (leftStaticBall->isOccupied && !leftStaticBall->isHanging){
			//Give hanging property , rerun on it
			leftStaticBall->isHanging = true;
			SpreadSupportRecursive(myGs , left , row);
		}
	}
	if (down >=0 ){
		//Down index is valid
		StaticBall * downStaticBall  = &(myGs->staticBalls[col][down]);
		if (downStaticBall->isOccupied && !downStaticBall->isHanging){
			//Give hanging property , rerun on it
			downStaticBall->isHanging = true;
			SpreadSupportRecursive(myGs, col , down);
		}
	}
	if (up < 15){
		//Up index is valid
		StaticBall * upStaticBall  = &(myGs->staticBalls[col][up]);
		if (upStaticBall->isOccupied && !upStaticBall->isHanging){
			//Give hanging property , rerun on it
			upStaticBall->isHanging = true;
			SpreadSupportRecursive(myGs , col , up);
		}
	}
	if (right < 16){
		//Right index is valid
		StaticBall* rightStaticBall = &(myGs->staticBalls[right][row]);
		if (rightStaticBall->isOccupied && !rightStaticBall->isHanging){
			//Give hanging property , rerun on it
			rightStaticBall->isHanging = true;
			SpreadSupportRecursive(myGs , right , row);
		}
	}
	if (!(row % 2)) {
		//Even row , no offset
		//Meaning their extra neighbours are top , bottom left diagonals
		//Top left
		if (left >= 0 && up < 15){
			//Top right index is valid
			StaticBall* topLeftStaticBall = &(myGs->staticBalls[left][up]);
			if (topLeftStaticBall->isOccupied && !topLeftStaticBall->isHanging){
				//Give hanging property , rerun on it
				topLeftStaticBall->isHanging = true;
				SpreadSupportRecursive(myGs , left , up);
			}
		}
		//Bottom left
		if (left >= 0 && down >= 0){
			//Bottom right index is valid
			StaticBall * bottomLeftStaticBall = &(myGs->staticBalls[left][down]);
			if (bottomLeftStaticBall->isOccupied && !bottomLeftStaticBall->isHanging){
				//Give hanging property , rerun on it
				bottomLeftStaticBall->isHanging = true;
				SpreadSupportRecursive(myGs , left , down);
			}
		}
	}
	else {
		//Odd row , they are offset
		//Meaning their extra neighbours are top , bottom right diagonals
		//Top right
		if (right < 16 && up < 15){
			//Top right index is valid
			StaticBall* topRightStaticBall = &(myGs->staticBalls[right][up]);
			if (topRightStaticBall->isOccupied && !topRightStaticBall->isHanging){
				//Give hanging property , rerun on it
				topRightStaticBall->isHanging = true;
				SpreadSupportRecursive(myGs, right , up);
			}
		}
		//Bottom right
		if (right < 16 && down >= 0){
			//Bottom right index is valid
			StaticBall * bottomRightStaticBall = &(myGs->staticBalls[right][down]);
			if (bottomRightStaticBall->isOccupied && !bottomRightStaticBall->isHanging){
				//Give hanging property , rerun on it
				bottomRightStaticBall->isHanging = true;
				SpreadSupportRecursive(myGs , right , down);
			}
		}
	}
}
//Identifies unsupported balls. Called in NextTurn()
void HangingBallsRoutine(GameScene * myGs){
	///DELETE FLOATING BALLS
	//We give the ceiling balls isHanging=true and the rest isHanging=false
	for (int row = 0 ; row < GRID_ROWS ; row++){
		for ( int col = 0 ; col < GRID_COLUMNS; col++){
			StaticBall * staticBall = &(myGs->staticBalls[col][row]);
			if (staticBall->isOccupied && row == 0){
				staticBall->isHanging = true;
			}
			else if (staticBall->isOccupied && row != 0){
				staticBall->isHanging = false;
			}
			//We don't touch the nonOccupied ones.
		}
	}
	//After that, we run the FindSupport() function on all balls, excluding the first row
	/*Array Loop Approach (REPLACED**)
	//Down-to-Up
	for (int row = 1 ; row < GRID_ROWS ; row++){
		//Left-to-right
		for ( int col = 0 ; col < GRID_COLUMNS; col++){
			if (myGs->staticBalls[col][row].isOccupied){
				FindSupport(myGs , col , row);
			}

		}
		//Right-to-left
			for ( int col = GRID_COLUMNS-1 ; col >= 0; col--){
			if (myGs->staticBalls[col][row].isOccupied){
				FindSupport(myGs , col , row);
			}
		}
	}
	//Backwards , Up-To-Down (See HangingBallsAlgorithmAnalysis.png
	for (int row = GRID_ROWS-1 ; row >= 1 ; row--){
			//Left-to-right
		for ( int col = 0 ; col < GRID_COLUMNS; col++){
			if (myGs->staticBalls[col][row].isOccupied){
				FindSupport(myGs , col , row);
			}

		}
		//Right-to-left
			for ( int col = GRID_COLUMNS-1 ; col >= 0; col--){
			if (myGs->staticBalls[col][row].isOccupied){
				FindSupport(myGs , col , row);
			}
		}
	}
	*/
	
	//Recursive approach:
	for (int row = 1 ; row < GRID_ROWS ; row++){
		for (int col = 0 ; col < GRID_COLUMNS ; col++){
			StaticBall * currentBall = &(myGs->staticBalls[col][row]);
			if (currentBall->isOccupied){
				if(FindSupport(myGs , col , row)){
					//It has been detected hanging by means of FindSupport() function 
					//Not by means of the recursive function, therefore , we run a new recursive function call
					SpreadSupportRecursive(myGs , col ,row);	
				}
			}
		}
	}
	//Therefore now we need to delete all balls whose isHanging==false;
	for (int row = 1 ; row < GRID_ROWS ; row++){
		for ( int col = 0 ; col < GRID_COLUMNS; col++){
			StaticBall * currentBall = &(myGs->staticBalls[col][row]);
			if (currentBall->isOccupied && currentBall->isHanging == false){
				BallDestroy(currentBall , true);
			}
		}
	}
}	
//Count and draw ceiling. Called in UpdateGrid()
void TimerTick(GameScene* myGs){
	///Tick
	myGs->yTimer--;
	if (myGs->yTimer == 0) {
		//If reach zero , increase yOffset/difficulty
		myGs->yTimer = TIMER_FREQ;
		myGs->yOffset += BALL_SIZE*N;
		//Draw bricks up to new offset
		unsigned short screenBaseOffset = myGs->yOffset/(BALL_SIZE*N);
		for (int y = 0 ; y < screenBaseOffset ; y++){
			for (int x = 5 ; x < 23 ; x++){
				//Foreground				//ID
				myScreenBase13[(y*32)+ x] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
				if (x==5 || x == 22) {
					//Draw skulls
					myScreenBase12[(y*32)+x] = ((46 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
				}
			}
		}	
	}
	//Drop timer display			 //ID        //Flip h    //Flip v    //Palette
	myScreenBase12[(18*32) + 3] = ( ((myGs->yTimer+1) << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
}
//Recount balls and score. Called in UpdateGrid()
void MemoryReassign(GameScene* myGs){
	//Recount balls
	int newBallCount = 0;
	for (int row = 0 ; row < GRID_ROWS ; row++){
		for (int col = 0 ; col < GRID_COLUMNS ; col++){
			StaticBall* currentStaticBall = &(myGs->staticBalls[col][row]);
			if (currentStaticBall->isOccupied){
				newBallCount++;
				currentStaticBall->ID = newBallCount;
				//**Preset balls dont need to have their address be cleared
				if(currentStaticBall->myAddress != NULL){
				//Clear current address before reassigning
				currentStaticBall->myAddress[0] = 0;
				currentStaticBall->myAddress[1] = 0;
				currentStaticBall->myAddress[2] = 0;
				}
				//Reassign address
				currentStaticBall->myAddress = &(myOAM[newBallCount*4]);
			}
		}
	}
	//Update Score
	myGs->score += myGs->staticBallCount - newBallCount;
	myGs->staticBallCount = newBallCount;
	//Score divide into two variables , for their respective SBB
	unsigned short scoreDig1 = (unsigned short)myGs->score/10;
	//Lets say score is 15: 15/10 = 1.5 ->(unsigned short)1.5 = 1 = scoreDig1
	//scoreDig2 = 15 - 1*10 = 5
	unsigned short scoreDig2 = myGs->score - scoreDig1*10;
	//Score display in screen base block
	//Offset by tile viewer index of 0 (Transparency tile)
	myScreenBase12[(15*32) + 26] = ( ((1+scoreDig1) << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase12[(15*32) + 27] = ( ((1+scoreDig2) << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	//Win
	if(myGs->score >= SCORE_MAX) {
		isMenu = 0;
		//DRAW 'YOU WON!'
		DrawWinText();
	}
}
//Update hex grid state. Called in NextTurn() and outside.
void UpdateGrid(GameScene * myGs, bool tick){
	//Tick (Ceiling drop)
	if (tick)TimerTick(myGs);
	//Recount balls
	MemoryReassign(myGs);
	//Update position (Ceiling might have dropped) , Re-Draw
	for (int row = 0 ; row < GRID_ROWS ; row++){
		for (int col = 0 ; col < GRID_COLUMNS ; col++){
			StaticBall* theStaticBall = &(myGs->staticBalls[col][row]);
			//Set position of the grid
			theStaticBall->presetPos.x = (X_MIN +  col*BALL_SIZE)*N;//48 , 56 , 64
			theStaticBall->presetPos.y = myGs->yOffset + row*BALL_SIZE*N;//0 , 8 , 16
			theStaticBall->maxExtent.x = theStaticBall->presetPos.x + BALL_SIZE*N;
			theStaticBall->maxExtent.y = theStaticBall->presetPos.y + BALL_SIZE*N;
			if (row % 2){
				//If row is odd (hexGrid) we offset by halfWidth
				theStaticBall->presetPos.x += (BALL_SIZE/2)*N;
				theStaticBall->maxExtent.x += (BALL_SIZE/2)*N;
			}
			//If occupied , redraw
			if (theStaticBall->isOccupied){
										//y		    	  			    				//RotOn     //ODisable //Shape
				theStaticBall->myAddress[0] = ( (theStaticBall->presetPos.y/N << 0) | (0 << 8) | (0 << 9) | (0 << 14) );
										//x												//Hflip		//VFlip		//Size
				theStaticBall->myAddress[1] = ( (theStaticBall->presetPos.x/N << 0) | (0 << 12) | (0 << 13) | ( 0 << 14));
										//ID									//Prior		//Palette
				theStaticBall->myAddress[2] = ( (theStaticBall->colour << 0) | (0 << 10) | (0 << 12));
				if (theStaticBall->presetPos.y >= Y_MAX*N){
					//Back to menu
					isMenu = 0;
					//DRAW 'YOU LOST'
					DrawLoseText();
				}
			}
		}
	}
}
//Updates the grid after collision. Called in Collision() and SceneUpdate().
void NextTurn( GameScene* myGs){
	myGs->myPlayer.canFire = true;
	///SNAP TO GRID
	//Convert screen coordinate to  gridPosition
	//X: Dist from row beginning
	float offSetX = myGs->movingBall.position.x/N - X_MIN;//0-128
	//Y: Dist from roof
	float offSetY = (myGs->movingBall.position.y - myGs->yOffset)/N;
	int gridX;
	int gridY = round(offSetY/BALL_SIZE);
	if ((gridY % 2)){
		//If its odd row
		offSetX -= BALL_SIZE/2;
	}
	gridX = round(offSetX/BALL_SIZE);
	//Move onto staticBall struct in the ball grid
	clampFunc(&gridX, 0, GRID_COLUMNS-1);
	clampFunc(&gridY, 0, GRID_ROWS-1);
	StaticBall* newStaticBall = &(myGs->staticBalls[gridX][gridY]);
	newStaticBall->colour = myGs->movingBall.colour;
	newStaticBall->isOccupied = true;
	//Delete movingBall
	BallDestroy2(&(myGs->movingBall) , true);
	///BALL CHAINS
	BallChainRoutine(myGs , gridX , gridY);
	///UNSUPPORTED BALLS
	HangingBallsRoutine(myGs);
	///UPDATE HEX GRID
	UpdateGrid(myGs, true);	
}
//Collision. Called in SceneUpdate().
void Collision(GameScene* myGs){
	Ball* movingBall = &(myGs->movingBall);
	bool hasCollided = false;
	//Loop through the entire hex grid
	for (int row = 0 ; row < GRID_ROWS ; row++){
		for (int col = 0 ; col < GRID_COLUMNS ; col++){
			StaticBall* otherBall = &(myGs->staticBalls[col][row]);
			if (otherBall->isOccupied){				
				//Can only collide with static balls
				int thisLeft = movingBall->position.x;
				int thisRight = movingBall->maxExtent.x;
				//Flipped y, remember, screen coordinates = positive y axis is down
				int thisTop = movingBall->maxExtent.y;
				int thisBottom = movingBall->position.y;
				int otherLeft = otherBall->presetPos.x;
				int otherRight = otherBall->maxExtent.x;
				int otherTop = otherBall->maxExtent.y;
				int otherBottom = otherBall->presetPos.y;
				//Collision calc. We only accept ONE collision, the first one detected.
				if (!(otherLeft > thisRight
				|| otherRight < thisLeft
				|| otherTop < thisBottom
				|| otherBottom > thisTop)){
					if (!hasCollided) {
						NextTurn(myGs);
						hasCollided = true;
					}
				}
			}
		}
	}	
}
//Used to set up level
void SetUpStaticBall(GameScene * myGs, unsigned short gridX, unsigned short gridY, unsigned short colour){
	
	myGs->staticBalls[gridX][gridY].isOccupied = true;
	myGs->staticBalls[gridX][gridY].colour = colour;
	//Just in case
	myGs->staticBalls[gridX][gridY].myAddress = NULL;
	myGs->staticBallCount++;
}
//Creates new ball.
void InstanceBall (GameScene* myGs){
	myGs->myPlayer.canFire = false;
	//Set address to point on ball obj, pos,vel,...
	myGs->movingBall.isOccupied = true;
	myGs->movingBall.ID = myGs->staticBallCount;
	myGs->movingBall.myAddress = &(myOAM[myGs->movingBall.ID * 4]);
	myGs->movingBall.position.x = START_POSX*N;
	myGs->movingBall.position.y = START_POSY*N;
	//Angles go from 10-170 degrees split on 32 indices with 5 degree angle difference
	float angle = 12.5 +5*myGs->myPlayer.rotIndex;//CounterClockwise
	//Deduce x and y components of velocity
	Vector2 vel;
	//Angles are counterClockwise
	vel.x = -cos(angle*M_PI/180)*N*VEL_MULT;
	//y coords flipped (screenCoords)
	vel.y = -sin(angle*M_PI/180)*N*VEL_MULT;
	myGs->movingBall.velocity = vel;
	myGs->movingBall.colour = myGs->nextColour;
	//Update next ball's colour in assembly , display on screen
	addAndLoopFunc(&myGs->nextColour, 13, 17);
						    //ID      				  //Priority  //Palette
	myOAM[(127*4) + 2] = ( (myGs->nextColour << 0) | (1 << 10) | (0 << 12) );
}
//Update moving ball
void SceneUpdate(GameScene* myGs){
	Ball* movingBall = &(myGs->movingBall);
	if (movingBall->isOccupied){
		//Update
		//ASSEMBLY
		updateFunc(&movingBall->position.x, &movingBall->position.y, movingBall->velocity.x, movingBall->velocity.y);
		//After update calc , update collision data
		updateExtentFunc(&movingBall->maxExtent.x, movingBall->position.x, BALL_SIZE*N);
		updateExtentFunc(&movingBall->maxExtent.y, movingBall->position.y, BALL_SIZE*N);
		//Bounce off edges
		edgeBounceFunc(movingBall->position.x, &movingBall->velocity.x, X_MIN*N, X_MAX*N);
		//Draw
								//y		    	  			    //RotOn    //ODisable //Shape
		movingBall->myAddress[0] = ( (movingBall->position.y/N << 0) | (0 << 8) | (0 << 9) | (0 << 14) );
								//x		    			        //HFlip	    //VFlip	    //Size        
		movingBall->myAddress[1] = ( (movingBall->position.x/N << 0) | (0 << 12) | (0 << 13) | (0 << 14) );
								//ID      		       //Priority  //Palette
		movingBall->myAddress[2] = ( (movingBall->colour << 0) | (0 << 10) | (0 << 12) );	
		//Collide with ceiling
		if (movingBall->position.y < myGs->yOffset) {
			NextTurn(myGs);
		}
		else Collision(myGs);	
	}
	//Clamp rotationIndex
	if (myGs->myPlayer.rotIndex > 31) myGs->myPlayer.rotIndex = 31;
	if (myGs->myPlayer.rotIndex < 0) myGs->myPlayer.rotIndex = 0;
	//Player update (RotScaled - https://www.coranac.com/tonc/text/affine.htm)
									//y								        //RotOn    //ODisable //Shape
	myGs->myPlayer.myAddress[0] = ( (myGs->myPlayer.position.y << 0) | (1 << 8) | (1 << 9) | (1 << 14) );
									//x								    	//RotIndex?  						   //HFlip	   //VFlip	     //Size        
	myGs->myPlayer.myAddress[1] = ( (myGs->myPlayer.position.x << 0) | ((int)myGs->myPlayer.rotIndex << 9) /*| (0 << 12) | (0 << 13)*/ | (2 << 14) );
									//ID     				   //Priority  //Palette
	myGs->myPlayer.myAddress[2]= ( (myGs->myPlayer.ID << 0) | (0 << 10) | (0 << 12) );
}
