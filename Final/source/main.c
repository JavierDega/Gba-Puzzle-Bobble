#include <gba_interrupt.h>
#include <maxmod.h>

#include "soundbank.h"		// created by building project
#include "soundbank_bin.h"	// created by building project

#include "scene/Scene.h"
#include "game/Game.h"

int isMenu = 0;
//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	irqInit();
	irqSet( IRQ_VBLANK, mmVBlank );
	irqEnable(IRQ_VBLANK);
	//Maxmod sound
	mmInitDefault( (mm_addr)soundbank_bin, 8 ); 
	mmSetModuleVolume(512);
	SetUpData();
	//Main loop
	while (1) 
	{
		if (isMenu == 0){
			MenuLoop();
		}
		else if (isMenu == 1) {
			Menu2Loop();
		}
		else if (isMenu == 2){
			GameLoop();
		}
	}
}