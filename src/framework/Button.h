
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

#ifndef LPC2012Game_Button_h
#define LPC2012Game_Button_h

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>

#include "Drawable.h"

class Button;

class ButtonHandler {

public:
    virtual void onButtonClicked(Button *sender) = 0;
};

class Button : public Drawable {
    
    std::string labelText;
    
    ALLEGRO_FONT *font;
    
    ALLEGRO_COLOR color;
    
    ALLEGRO_BITMAP *normalImage;
    
    ALLEGRO_BITMAP *pressedImage;
    
    Size size;
    
    bool pressed;
    
    bool enabled;
    
    ButtonHandler *handler;
    
public:
    
    void *data;
    
    Button(const char *text, ALLEGRO_FONT *fnt, ALLEGRO_COLOR col, const char *normalImageFile, const char *pressedImageFile);
    Button(Size sz);
    ~Button();
    
    virtual void draw();
    
    virtual void handleEvent(ALLEGRO_EVENT ev);

    virtual void setVisible(bool vis);
    
    void setLabelText(const char *text);
    const char *getLabelText();
    
    void setEnabled(bool en);
    bool isEnabled();    
    
    void setHandler(ButtonHandler *btnHandler);

};

#endif
