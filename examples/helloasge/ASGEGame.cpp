#include <map>
#include <Engine/GameSettings.hpp>
#include <Engine/Logger.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include "headers/MapLoader.hpp"

class ASGENetGame : public ASGE::OGLGame
{
 public:
  explicit ASGENetGame(const ASGE::GameSettings& settings) :
    OGLGame(settings)
  {
    inputs->use_threads = false;
    std::string id = inputs->addCallbackFnc(ASGE::EventType::E_KEY, &ASGENetGame::keyHandler, this);
    init(settings);
  }

  ~ASGENetGame() override = default;
  ASGENetGame(const ASGENetGame&) = delete;
  ASGENetGame& operator=(const ASGENetGame&) = delete;

 private:
  std::map<int, bool> keys;
  std::map<int, bool> buttons;

  void keyHandler(ASGE::SharedEventData data)
  {
    const auto* key_event = dynamic_cast<const ASGE::KeyEvent*>(data.get());
    auto key              = key_event->key;
    auto action           = key_event->action;

    if (action == ASGE::KEYS::KEY_PRESSED)
    {
      if (key == ASGE::KEYS::KEY_8)
      {
        renderer->setWindowedMode(ASGE::GameSettings::WindowMode::BORDERLESS_FULLSCREEN);
      }
      if (key == ASGE::KEYS::KEY_9)
      {
        renderer->setWindowedMode(ASGE::GameSettings::WindowMode::WINDOWED);
      }

      keys[key] = true;
    }

    if (action == ASGE::KEYS::KEY_RELEASED)
    {
      keys[key] = false;
    }
  }

  void update(const ASGE::GameTime& us) override
  {
    double time_delta = us.deltaInSecs()*100;
    if (auto gamepad_data = inputs->getGamePad(0); gamepad_data.is_connected)
    {
      for( int i=0; i<gamepad_data.no_of_buttons; ++i)
      {
        if(gamepad_data.buttons[i] == true)
        {
          Logging::ERRORS("button event" + std::to_string(i));
        }
      }
    }

    float oldxPos = robot->xPos();
    float oldyPos = robot->yPos();
    if(keys[ASGE::KEYS::KEY_A])
    {
      robot->xPos(robot->xPos() -MOVESPEED*time_delta);
      // rh_camera.translateX(-5);
    }
    if(keys[ASGE::KEYS::KEY_D])
    {
      robot->xPos(robot->xPos() +MOVESPEED*time_delta);
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_W])
    {
      robot->yPos(robot->yPos() -MOVESPEED*time_delta);
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_S])
    {
      robot->yPos(robot->yPos() +MOVESPEED*time_delta);
      // rh_camera.translateX(+5);
    }

    if(keys[ASGE::KEYS::KEY_LEFT])
    {
      zombie->xPos(zombie->xPos() -MOVESPEED*time_delta);
      // rh_camera.translateX(-5);
    }
    if(keys[ASGE::KEYS::KEY_RIGHT])
    {
      zombie->xPos(zombie->xPos() +MOVESPEED*time_delta);
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_UP])
    {
      zombie->yPos(zombie->yPos() -MOVESPEED*time_delta);
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_DOWN])
    {
      zombie->yPos(zombie->yPos() +MOVESPEED*time_delta);
      // rh_camera.translateX(+5);
    }
    if (level.AABBCollision(robot->getWorldBounds(),{robot->xPos(),robot->yPos()}))
    {
      Logging::INFO("Colliding! Returning to old position!");
      robot->xPos(robot->xPos()+(oldxPos-robot->xPos()));
      robot->yPos(robot->yPos()+(oldyPos-robot->yPos()));
    }

    lh_camera.lookAt(
      {
        robot->xPos() + robot->width()  * 0.5F,
        robot->yPos() + robot->height() * 0.5F });


    rh_camera.lookAt(
      {
        zombie->xPos() + zombie->width()  * 0.5F,
        zombie->yPos() + zombie->height() * 0.5F });

    lh_camera.update(us);
    rh_camera.update(us);
  };

  void render(const ASGE::GameTime& us) override
  {
    renderer->setViewport({0, 0, 1024, 768});
    renderer->setProjectionMatrix(lh_camera.getView());
//    renderer->render(*bg);
    renderer->render(*robot);
    renderer->render(*zombie);
    level.render(renderer.get());
/*
    auto view = rh_camera.getView();
    renderer->setViewport({1024/2, 0, 1024/2, 768});
    renderer->setProjectionMatrix(view);
    renderer->render(*bg);
    renderer->render(*robot);
    renderer->render(*zombie);
    */
    ASGE::Text camera1 = ASGE::Text{ renderer->getDefaultFont(), "CAMERA1" };
    camera1.setPositionX(1024 * 0.25F - (camera1.getWidth() * 0.5F));
    camera1.setPositionY(30);

    ASGE::Text camera2 = ASGE::Text{ renderer->getDefaultFont(), "CAMERA2" };
    camera2.setPositionX(1024 * 0.75F - (camera2.getWidth() * 0.5F));
    camera2.setPositionY(30);

    renderer->setViewport({0, 0, 1024, 768});
    renderer->setProjectionMatrix(0, 1024, 0, 768);
    renderer->render(camera1);
    renderer->render(camera2);
  };

  void init(const ASGE::GameSettings& settings)
  {
    robot = renderer->createUniqueSprite();
    robot->loadTexture("/data/images/character_robot_idle.png");
    robot->width(16*7.5);
    robot->height(16*10);
    robot->setGlobalZOrder(1);

    zombie = renderer->createUniqueSprite();
    zombie->loadTexture("/data/images/character_zombie_idle.png");
    zombie->xPos(settings.window_width / 2);
    robot->setGlobalZOrder(1);

    bg = renderer->createUniqueSprite();
    bg->loadTexture("/data/images/FHD.png");
    bg->setGlobalZOrder(-1);
    renderer->setClearColour(ASGE::COLOURS::BLACK);
    lh_camera.resize(settings.window_width, settings.window_height);
    rh_camera.resize(1024 / 2.0F, 768);
    lh_camera.lookAt({ 1024 * 0.25F, 768 / 2.0F });
    rh_camera.lookAt({ 1024 * 0.50F, 768 / 2.0F });

    if (!level.loadSharedMap("data/tilemap/testTile.tmx"))
    {
      Logging::ERRORS("Tilemap cannot be loaded");
    }
    level.init(renderer.get());

    robot->xPos(level.getTileByLayerID(3).x);
    robot->yPos(level.getTileByLayerID(3).y);
  }

  bool AABBCollision(std::shared_ptr<ASGE::Sprite> target1, std::shared_ptr<ASGE::Sprite> target2)
  {
    auto bound1 = target1->getWorldBounds();
    auto bound2 = target2->getWorldBounds();
    return (bound1.v1.x <= bound2.v2.x || bound1.v1.y <= bound2.v3.y ||
            bound1.v2.x >= bound2.v1.x || bound1.v3.y >= bound2.v1.y);
  }
 private:
  int key_callback_id = -1; /**< Key Input Callback ID. */
  std::shared_ptr<ASGE::Sprite> robot = nullptr;
  std::unique_ptr<ASGE::Sprite> zombie = nullptr;
  std::unique_ptr<ASGE::Sprite> bg = nullptr;
  ASGE::Camera lh_camera{};
  ASGE::Camera rh_camera{};
  MapLoader level;
  ASGE::Point2D player_direction = {0,0};
  const float MOVESPEED = 2;

  const int SCALE = 3;
};

int main(int /*argc*/, char* /*argv*/[])
{
  ASGE::GameSettings game_settings;
  game_settings.window_title  = "ASGEGame";
  game_settings.window_width  = 1024;
  game_settings.window_height = 768;
  game_settings.mode          = ASGE::GameSettings::WindowMode::WINDOWED;
  game_settings.fixed_ts      = 60;
  game_settings.fps_limit     = 60;
  game_settings.msaa_level    = 1;
  game_settings.vsync         = ASGE::GameSettings::Vsync::DISABLED;

  Logging::INFO("Launching Game!");
  ASGENetGame game(game_settings);
  game.run();
  return 0;
}
