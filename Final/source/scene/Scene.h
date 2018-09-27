#include "SceneComponents.h"
#include "GameScene.h"

//Scene Components
void BallDestroyI(StaticBall* theStaticBall , bool clearPointer){
	BallDestroy(theStaticBall , clearPointer);
}
void BallDestroy2I(Ball* theMovingBall , bool clearPointer){
	BallDestroy2(theMovingBall , clearPointer);
}
inline void DrawWinTextI(){
	DrawWinText();
}
inline void DrawLoseTextI(){
	DrawLoseText();
}
inline void ClearResultTextI(){
	ClearResultText();
}

//GameScene
void SetUpStaticBallI(GameScene * myGs, unsigned short gridX, unsigned short gridY, unsigned short colour){
	SetUpStaticBall(myGs, gridX, gridY, colour);
}
void UpdateGridI(GameScene * myGs, bool tick){
	UpdateGrid(myGs, tick);
}
void InstanceBallI (GameScene* myGs){
	InstanceBall(myGs);
}
void SceneUpdateI(GameScene* myGs){
	SceneUpdate(myGs);
}

