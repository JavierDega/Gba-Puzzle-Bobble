//GAMESCENE COMPONENTS AND UTILITY FUNCTIONS

extern unsigned short* myScreenBase4;

//Handy struct
typedef struct Vector2Str{
	int x;
	int y;
}Vector2;
//Analyze ball's surroundings within the hex grid
typedef struct ClusterBoundsStr{
	bool upCheck;
	bool downCheck;
	bool leftCheck;
	bool rightCheck;
	bool trCheck;
	bool brCheck;
	bool tlCheck;
	bool blCheck;	
}ClusterBounds;
//Ball struct
typedef struct BallStr{
	bool isOccupied;
	Vector2 position;
	Vector2 maxExtent;
	Vector2 velocity;
	//OAM
	unsigned short* myAddress;
	//Ball count right after it was instanced
	unsigned short ID;
	//Obj base id number
	unsigned short colour;
}Ball;
//Static ball when they are attached to the hex grid/2D Array
typedef struct StaticBallStr{
	bool isOccupied;
	//Grid analysis
	bool isHanging;
	ClusterBounds cb;
	unsigned short chain;
	
	Vector2 presetPos;
	Vector2 maxExtent;
	unsigned short* myAddress;
	unsigned short colour;
	//Ball number on the hex grid
	unsigned short ID;
}StaticBall;
//Represents arrow
typedef struct PlayerStr{
	
	Vector2 position;
	bool canFire;
	float rotIndex;
	unsigned short * myAddress;
	//OAM tile number
	unsigned short ID;
}Player;

//Ball deletion routines
void BallDestroy (StaticBall* theStaticBall , bool clearPointer){

	//Clear struct (Memset dangerous?)
	//memset(theStaticBall, 0 ,sizeof(StaticBall));

	theStaticBall->isOccupied = 0;
	theStaticBall->isHanging = 0;
		
	theStaticBall->cb.upCheck = 0;
	theStaticBall->cb.downCheck = 0;
	theStaticBall->cb.leftCheck = 0;
	theStaticBall->cb.rightCheck = 0;
	theStaticBall->cb.trCheck = 0;
	theStaticBall->cb.brCheck = 0;
	theStaticBall->cb.tlCheck = 0;
	theStaticBall->cb.blCheck = 0;

	if (clearPointer){
		theStaticBall->myAddress[0] = 0;
		theStaticBall->myAddress[1] = 0;
		theStaticBall->myAddress[2] = 0;
	}
	
	theStaticBall->myAddress = NULL;

	theStaticBall->colour = 0;
	theStaticBall->ID = 0;
	
	theStaticBall->chain = 0;
}
void BallDestroy2 (Ball* theMovingBall , bool clearPointer){

	//Clear struct (Memset dangerous?)
	//memset(theStaticBall, 0 ,sizeof(StaticBall));
	//We leave presetPos and maxExtent as they are.

	theMovingBall->isOccupied = 0;

	theMovingBall->position.x = 0;
	theMovingBall->position.y = 0;
	theMovingBall->velocity.x = 0;
	theMovingBall->velocity.y = 0;
	theMovingBall->maxExtent.x = 0;
	theMovingBall->maxExtent.y = 0;

	if (clearPointer){
		theMovingBall->myAddress[0] = 0;
		theMovingBall->myAddress[1] = 0;
		theMovingBall->myAddress[2] = 0;
	}
	theMovingBall->myAddress = NULL;

	theMovingBall->colour = 0;
	theMovingBall->ID = 0;

}
//Text functions
inline void DrawWinText(){
	//DRAW 'YOU WON!'
	myScreenBase4[(15*32)+12] = ((35 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+13] = ((25 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+14] = ((31 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+16] = ((33 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+17] = ((25 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+18] = ((24 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+19] = ((37 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
}
inline void DrawLoseText(){
	//DRAW 'YOU LOST'
	myScreenBase4[(15*32)+12] = ((35 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+13] = ((25 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+14] = ((31 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+16] = ((22 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+17] = ((25 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+18] = ((29 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+19] = ((30 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	
}
inline void ClearResultText(){
	//Clear game result text
	myScreenBase4[(15*32)+12] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+13] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+14] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+16] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+17] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+18] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
	myScreenBase4[(15*32)+19] = ((43 << 0) | (0 << 10) | (0 << 11) | (0 << 12) );
}

