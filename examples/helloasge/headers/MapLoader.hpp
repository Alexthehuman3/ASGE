//
// Created by aw987 on 19/11/2021.
//

#ifndef ASGE_MAPLOADER_HPP
#define ASGE_MAPLOADER_HPP

#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/ObjectGroup.hpp>

class MapLoader
{
 public:
  MapLoader() = default;
  ~MapLoader() = default;

  bool loadSharedMap(const std::string& path);
  bool loadRawMap(const std::string& path);
  void init();
  std::shared_ptr<tmx::Map> getMap();
  std::shared_ptr<tmx::TileLayer> getLayer();
  std::vector<tmx::Tileset::Tile> getLoadedTiles();

 private:
    std::shared_ptr<tmx::Map> map = nullptr;
    tmx::Map raw_map;
    std::shared_ptr<tmx::TileLayer> tileLayer;
    std::vector<tmx::Tileset::Tile> tiles;
};

#endif // ASGE_MAPLOADER_HPP
