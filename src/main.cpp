
/*
 
 Copyright 2012 Fabio Rodella
 
 This file is part of LPC2012Game.
 
 LPC2012Game is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 LPC2012Game is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with LPC2012Game.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include "MainMenuScene.h"
#include "InvestigationScene.h"
#include "DebugCrimeScene.h"

int main(int argc, char **argv) {
    
    Director::getInstance()->initialize();
    
    MainMenuScene *scene = new MainMenuScene();
    //InvestigationScene *scene = new InvestigationScene();  
    //DebugCrimeScene *scene = new DebugCrimeScene();
    
    Director::getInstance()->startWithScene(scene);
    
    return 0;
}

