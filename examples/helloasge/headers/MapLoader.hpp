//
// Created by aw987 on 19/11/2021.
//

#ifndef ASGE_MAPLOADER_HPP
#define ASGE_MAPLOADER_HPP

#include <Engine/Renderer.hpp>
#include <Engine/Sprite.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/TileLayer.hpp>
#include <Engine/Logger.hpp>

class MapLoader
{
 public:
  MapLoader() = default;
  ~MapLoader() = default;

  bool loadSharedMap(const std::string& path);
  bool loadRawMap(const std::string& path);
  void init(ASGE::Renderer* renderer);
  void render(ASGE::Renderer* renderer);
  std::shared_ptr<tmx::Map> getMap();
  std::shared_ptr<tmx::TileLayer> getLayer();
  std::vector<std::shared_ptr<tmx::TileLayer>> getTileLayerGroup();
  ASGE::Point2D getTileByLayerID(unsigned int id);
  bool AABBCollision(ASGE::SpriteBounds bound1, ASGE::Point2D player);

 private:
    std::shared_ptr<tmx::Map> map = nullptr;
    tmx::Map raw_map;
    std::shared_ptr<tmx::TileLayer> tileLayer = nullptr;
    std::vector<std::shared_ptr<tmx::TileLayer>> tileLayerGroup;
    std::vector<tmx::Tileset::Tile> tiles;
    std::vector<std::vector<std::unique_ptr<ASGE::Sprite>>> Level1Tiles;
    const float SCALE = 5;
};

#endif // ASGE_MAPLOADER_HPP
