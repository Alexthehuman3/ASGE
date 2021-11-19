//
// Created by aw987 on 19/11/2021.
//

#include "../headers/MapLoader.hpp"

bool MapLoader::loadSharedMap(const std::string& path)
{
  map = std::make_shared<tmx::Map>();
  return map->load(path);
}
bool MapLoader::loadRawMap(const std::string& path)
{
  return raw_map.load(path);
}
std::shared_ptr<tmx::TileLayer> MapLoader::getLayer()
{
  return tileLayer;
}
void MapLoader::init()
{
  for (const auto& layer : map->getLayers())
  {
    if (layer->getType() == tmx::Layer::Type::Tile)
    {
      tileLayer = std::make_shared<tmx::TileLayer>(layer->getLayerAs<tmx::TileLayer>());
      for (unsigned int row = 0; row < layer->getSize().y; ++row)
      {
        for (unsigned int col = 0; col < layer->getSize().x; ++col)
        {
          auto tile_info = tileLayer->getTiles()[row*tileLayer->getSize().x + col];
          auto tile = map->getTilesets()[0].getTile(tile_info.ID);
          if (tile != nullptr)
          {
            tiles.push_back(*tile);
          }
        }
      }
    }
  }
}
std::shared_ptr<tmx::Map> MapLoader::getMap()
{
  return map;
}
std::vector<tmx::Tileset::Tile> MapLoader::getLoadedTiles()
{
  return tiles;
}
