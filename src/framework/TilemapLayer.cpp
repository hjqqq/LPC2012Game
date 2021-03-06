
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

#include "TilemapLayer.h"

std::vector<TilemapLayer *> TilemapLayer::parseTMXFile(const char *file) {
    
    // THIS IS NOT a complete TMX parser! It will only work with:
    // - orthogonal maps
    // - single tileset
    // - all layers with same size
    // - tile data without compression
    // - no flipped tiles
    // - firstGid = 1
    
    std::vector<TilemapLayer *> ret;
    
    xmlDoc *doc = xmlReadFile(file, NULL, 0);
    
    xmlNode *root = xmlDocGetRootElement(doc);
    
	int mapWidth = atoi(xmlGetAttribute(root, "width"));
	int mapHeight = atoi(xmlGetAttribute(root, "height"));
	int tileWidth = atoi(xmlGetAttribute(root, "tilewidth"));
	int tileHeight = atoi(xmlGetAttribute(root, "tileheight"));
    
    xmlNode *tilesetNode = xmlGetFirstChildForName(root, "tileset");
    xmlNode *imageNode = xmlGetFirstChildForName(tilesetNode, "image");
    
    const char *imageFile = xmlGetAttribute(imageNode, "source");
    
    ALLEGRO_PATH *tilesetPath = al_create_path(file);
    al_set_path_filename(tilesetPath, NULL);
    al_join_paths(tilesetPath, al_create_path(imageFile));
    
    std::vector<xmlNode *> layerNodes = xmlGetChildrenForName(root, "layer");
    std::vector<xmlNode *>::iterator layerNodesIt;
    
    for(layerNodesIt = layerNodes.begin(); layerNodesIt < layerNodes.end(); ++layerNodesIt) {
        
        TilemapLayer *layer = new TilemapLayer();
        
        layer->mapSize = sizeMake(mapWidth, mapHeight);
        layer->tileSize = sizeMake(tileWidth, tileHeight);
        
        layer->tileset = new Spritesheet(al_path_cstr(tilesetPath, ALLEGRO_NATIVE_PATH_SEP), tileWidth, tileHeight);
        
        layer->data = new short[mapWidth * mapHeight];
        
        layer->collision = false;
        
        xmlNode *layerNode = (xmlNode *) *layerNodesIt;
        decodeLayerData(xmlGetFirstChildForName(layerNode, "data"), layer);
        
        xmlNode *propertiesNode = xmlGetFirstChildForName(layerNode, "properties");
        
        if (propertiesNode != NULL) {
            
            std::vector<xmlNode *> propertyNodes = xmlGetChildrenForName(propertiesNode, "property");
            std::vector<xmlNode *>::iterator propertyNodesIt;
            
            for(propertyNodesIt = propertyNodes.begin(); propertyNodesIt < propertyNodes.end(); ++propertyNodesIt) {
                
                xmlNode *propertyNode = (xmlNode *) *propertyNodesIt;
                const char *name = xmlGetAttribute(propertyNode, "name");
                
                if (!strcmp(name, LAYER_Z_ORDER_PROPERTY)) {
                    
                    layer->zOrder = atoi(xmlGetAttribute(propertyNode, "value"));
                    
                } else if (!strcmp(name, LAYER_COLLISION_PROPERTY)) {
                    
                    layer->collision = true;
                }
                
            }
        }
        
        ret.push_back(layer);
    }
    
    return ret;
}

TilemapLayer::~TilemapLayer() {
    
    delete data;
    delete tileset;
}

void TilemapLayer::draw() {
    
    //TODO for now, maps can only be positioned at origin (0,0) and
    // the anchor point is disregarded.
    
    float topX = 0;
    float topY = 0;
    
    int tilesX = 0;
    int tilesY = 0;
    
    if (camera != NULL) {
        
        topX = camera->getTop().x;
        topY = camera->getTop().y;
        
        tilesX = camera->getViewportSize().width / tileSize.width;
        tilesY = camera->getViewportSize().height / tileSize.height;
        
    } else {
        
        tilesX = al_get_display_width(al_get_current_display()) / tileSize.width;
        tilesY = al_get_display_height(al_get_current_display()) / tileSize.height;
    }
    
    int tx = topX / tileSize.width;
    int ty = topY / tileSize.height;
    
    int ox = -camera->getTop().x;
    int oy = -camera->getTop().y;
    
    al_hold_bitmap_drawing(true);
    
    for (int j = ty; j < (ty + tilesY + 2); j++) {
        for (int i = tx; i < (tx + tilesX + 2); i++) {
                                    
            if (i >= 0 && i < mapSize.width && j >= 0 && j < mapSize.height && getTileAt(i, j) > 0) {
            
                short tile = getTileAt(i, j) - 1;
                tileset->setFrame(tile);
                tileset->setOpacity(opacity);
                tileset->setPosition(pointMake(i * tileSize.width + ox, j * tileSize.height + oy));
                tileset->draw();
            }
        }
    }
    
    al_hold_bitmap_drawing(false);
}

void TilemapLayer::setTileAt(short tile, int x, int y) {
    
    int mw = mapSize.width;
    data[y * mw + x] = tile;
}

short TilemapLayer::getTileAt(int x, int y) {
    
    int mw = mapSize.width;
    return data[y * mw + x];
}

Size TilemapLayer::getSize() {
    
    return mapSize;
}

Size TilemapLayer::getBoundsSize() {
    
    return sizeMake(mapSize.width * tileSize.width, mapSize.height * tileSize.height);
}

Size TilemapLayer::getTileSize() {
    
    return tileSize;
}

Rect TilemapLayer::getTileRect(int x, int y) {
    return rectMake(x * tileSize.width, y * tileSize.height, tileSize.width, tileSize.height);    
}

short *TilemapLayer::getData() {
    return data;
}

bool TilemapLayer::isCollision() {
    
    return collision;
}

void TilemapLayer::decodeLayerData(xmlNode *dataNode, TilemapLayer *layer) {
    
	char *encoding = xmlGetAttribute(dataNode, "encoding");
	if (!encoding) {
        
		int i = 0;
        int j = 0;
        
        std::vector<xmlNode *> tiles = xmlGetChildrenForName(dataNode, "tile");
        std::vector<xmlNode *>::iterator it;
        
        for(it = tiles.begin(); it < tiles.end(); ++it) {
            
            xmlNode *tileNode = (xmlNode *) *it;
            char *gid = xmlGetAttribute(tileNode, "gid");
            layer->setTileAt(atoi(gid), i, j);
            i++;
            
            if (i >= layer->mapSize.width) {
                i = 0;
                j++;
            }
        }
        
	} else {
        
        fprintf(stderr, "Error: unknown compression format");
		return;
    }
}
