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
void MapLoader::init(ASGE::Renderer* renderer)
{
  float starting_z_order = 0;
  int layers = 0;
  for (const auto& layer : map->getLayers())
  {
    Level1Tiles.emplace_back();
    if (layer->getType() == tmx::Layer::Type::Tile)
    {
      tileLayer = std::make_shared<tmx::TileLayer>(layer->getLayerAs<tmx::TileLayer>());
      for (unsigned int row = 0; row < layer->getSize().y; ++row)
      {
        for (unsigned int col = 0; col < layer->getSize().x; ++col)
        {
          auto tile_info = tileLayer->getTiles()[row*tileLayer->getSize().x + col];
          if (layers == 0 || layers == 2 || layers == 3)
          {
            auto tile = map->getTilesets()[0].getTile(tile_info.ID);
            if (tile != nullptr)
            {

              auto& sprite = Level1Tiles[layers].emplace_back(renderer->createUniqueSprite());
              if (sprite->loadTexture(tile->imagePath))
              {
                sprite->srcRect()[0] = static_cast<float>(tile->imagePosition.x);
                sprite->srcRect()[1] = static_cast<float>(tile->imagePosition.y);
                sprite->srcRect()[2] = static_cast<float>(tile->imageSize.x);
                sprite->srcRect()[3] = static_cast<float>(tile->imageSize.y);
                sprite->setGlobalZOrder(starting_z_order);

                sprite->width(static_cast<float>(tile->imageSize.x));
                sprite->height(static_cast<float>(tile->imageSize.y));

                sprite->scale(SCALE);
                sprite->setMagFilter(ASGE::Texture2D::MagFilter::NEAREST);

                sprite->yPos(static_cast<float>(tile->imageSize.y * SCALE * row));
                sprite->xPos(static_cast<float>(tile->imageSize.x * SCALE * col));
              }
              else
              {
                Logging::ERRORS("Loading Tile Texture failed");
              }
            }
          }
          else
          {
            auto tile = map->getTilesets()[1].getTile(tile_info.ID);
            if (tile != nullptr)
            {

              auto& sprite = Level1Tiles[layers].emplace_back(renderer->createUniqueSprite());
              if (sprite->loadTexture(tile->imagePath))
              {
                sprite->srcRect()[0] = static_cast<float>(tile->imagePosition.x);
                sprite->srcRect()[1] = static_cast<float>(tile->imagePosition.y);
                sprite->srcRect()[2] = static_cast<float>(tile->imageSize.x);
                sprite->srcRect()[3] = static_cast<float>(tile->imageSize.y);
                sprite->setGlobalZOrder(starting_z_order);

                sprite->width(static_cast<float>(tile->imageSize.x));
                sprite->height(static_cast<float>(tile->imageSize.y));

                sprite->scale(SCALE);
                sprite->setMagFilter(ASGE::Texture2D::MagFilter::NEAREST);

                sprite->yPos(static_cast<float>(tile->imageSize.y * SCALE * row));
                sprite->xPos(static_cast<float>(tile->imageSize.x * SCALE * col));
              }
              else
              {
                Logging::ERRORS("Loading Tile Texture failed");
              }
            }
          }
        }
      }
      tileLayerGroup.push_back(tileLayer);
    }
    //if it's other layer types, come here
    starting_z_order += 0.1F;
    layers++;
  }
}
std::shared_ptr<tmx::Map> MapLoader::getMap()
{
  return map;
}
void MapLoader::render(ASGE::Renderer* renderer)
{
  for (auto& layer : Level1Tiles)
  {
    for (auto& sprite : layer)
    {
      renderer->renderSprite(*sprite);
    }
  }

}
bool MapLoader::AABBCollision(ASGE::SpriteBounds bound1, ASGE::Point2D player)
{
  bool colliding_something = false;
  for (auto& sprite : Level1Tiles[1])
  {
    auto bound2 = sprite->getWorldBounds();
    colliding_something = (bound1.v3.y > bound2.v1.y && bound1.v1.y < bound2.v3.y &&
                           bound1.v1.x < bound2.v2.x && bound1.v2.x > bound2.v1.x);
  }
  if (colliding_something)
  {
    return true;
  }
  for (auto& sprite : Level1Tiles[2])
  {
    auto bound2 = sprite->getWorldBounds();
    colliding_something = (bound1.v3.y > bound2.v1.y && bound1.v1.y < bound2.v3.y &&
                           bound1.v1.x < bound2.v2.x && bound1.v2.x > bound2.v1.x);
  }
  return colliding_something;
}
std::vector<std::shared_ptr<tmx::TileLayer>> MapLoader::getTileLayerGroup()
{
  return tileLayerGroup;
}
ASGE::Point2D MapLoader::getTileByLayerID(unsigned int id)
{
  return {Level1Tiles[id][4]->xPos(), Level1Tiles[id][4]->yPos()};
}
