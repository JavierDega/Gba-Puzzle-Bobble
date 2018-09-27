#include "Setup.h"
#include "Menu.h"
#include "GameLoop.h"

//Setup
void SetRotDataI(float angle , int index){
	SetRotData(angle , index);
};
void SetUpDataI(){
	SetUpData();
};
GameScene SetUpGameI(){
	return SetUpGame();
};
//Menu
void MenuLoopI(){
	MenuLoop();
};
void Menu2LoopI(){
	Menu2Loop();
}
//GameLoop
void OAMCleanUpI(unsigned short index){
	OAMCleanUp(index);
}
void GameLoopI(){
	GameLoop();
}







