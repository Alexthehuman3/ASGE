#include <map>
#include <Engine/GameSettings.hpp>
#include <Engine/Logger.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include "headers/MapLoader.hpp"
#include "CustomIncludes/FileWatch.hpp"

//Forced Usage of Global variable because Library I'm using is a bit of a pain to store as a std::function to be called in program.
//Further explanation at the function.
bool reload_MapLoader_map = false;

enum GameState
{
  MENU = 0,
  GAME = 1
};
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
      if (key == ASGE::KEYS::KEY_E)
      {
        if (level.getMap())
        {
          if (level.interact(robot->getWorldBounds()))
          {
            interact_countdown = true;
            //position could be based on object's interaction, make an interact function that returns a ASGE::Point2D or something.
            interact_text->setPosition({lh_camera.position().x+100,lh_camera.position().y+100});
          }
        }
      }
      if (key == ASGE::KEYS::KEY_ESCAPE && gs == GameState::MENU)
      {
        //exit.
        signalExit();
      }
      keys[key] = true;
    }

    if (action == ASGE::KEYS::KEY_RELEASED)
    {
      if (key == ASGE::KEYS::KEY_ESCAPE)
      {
        gs = GameState::MENU;
        lh_camera.lookAt({1600/2, 900/2});
      }
      keys[key] = false;
    }
  }

  void update(const ASGE::GameTime& us) override
  {
    double time_delta = us.deltaInSecs()*100;
    if (auto gamepad_data = inputs->getGamePad(0); gamepad_data.is_connected)
    {
      for( int i=0; i<(int)gamepad_data.buttons; ++i)
      {
        if(gamepad_data.buttons[i] == true)
        {
          Logging::ERRORS("button event" + std::to_string(i));
        }
      }
    }

    //Game State system
    switch(gs)
    {
      case MENU:
        if (keys[ASGE::KEYS::KEY_ENTER])
        {
          gs = GameState::GAME;
          //Resets Map regardless of map state - Map is NOT loaded at the start so this also counts as dynamic initialization.
          resetMap();
        }
        break;

      case GAME:
        ASGE::Point2D player_dir {0,0};
        if(keys[ASGE::KEYS::KEY_A])
        {
          player_dir.x = -1;
        }
        if(keys[ASGE::KEYS::KEY_D])
        {
          player_dir.x = 1;
        }
        if(keys[ASGE::KEYS::KEY_W])
        {
          player_dir.y = -1;
        }
        if(keys[ASGE::KEYS::KEY_S])
        {
          player_dir.y = 1;
        }
        ASGE::Point2D oldPos = {robot->xPos(), robot->yPos()};
        robot->xPos(robot->xPos()+(player_dir.x*MOVESPEED*time_delta));
        if (level.AABBCollision(robot->getWorldBounds()))
        {
          robot->xPos(robot->xPos()+(oldPos.x-robot->xPos()));
          robot->yPos(robot->yPos()+(oldPos.y-robot->yPos()));
        }
        oldPos = {robot->xPos(), robot->yPos()};
        robot->yPos(robot->yPos()+(player_dir.y*MOVESPEED*time_delta));
        if (level.AABBCollision(robot->getWorldBounds()))
        {
          robot->xPos(robot->xPos()+(oldPos.x-robot->xPos()));
          robot->yPos(robot->yPos()+(oldPos.y-robot->yPos()));
        }

        if (level.isTouchingGoal(robot->getWorldBounds()))
        {
          gs = GameState::MENU;
          lh_camera.lookAt({1600/2, 900/2});
        }

        lh_camera.lookAt(
          {
            robot->xPos() + robot->width()  * 0.5F,
            robot->yPos() + robot->height() * 0.5F });

        lh_camera.update(us);

        if (reload_MapLoader_map)
        {
          reload_MapLoader_map = false; //bad flag usage, think of something different.
          resetMap();
        }

        if (interact_countdown)
        {
          interact_countdown = false;
          current_time = max_countdown_time;
        }

        if (current_time > 0)
        {
          current_time -= 0.01*time_delta;
        }
        break;
    }
  };

  void render(const ASGE::GameTime& us) override
  {
    switch(gs)
    {
      case MENU:
        renderer->setViewport({1600/8, 900/8, 1600, 900});
        renderer->setProjectionMatrix(lh_camera.getView());
        //Insert stuff to render here
        for (auto text : menu_text)
        {
          renderer->render(*text);
        }
        break;
      case GAME:
        //Camera 1
        renderer->setViewport({1600/8, 900/8, 1600, 900});
        renderer->setProjectionMatrix(lh_camera.getView());
        renderer->render(*robot);
        renderer->render(*zombie);
        level.render(renderer.get());
        if (current_time > 0)
        {
          renderer->render(*interact_text);
        }
        /*
        //Camera 2
            auto view = rh_camera.getView();
            renderer->setViewport({1024/2, 0, 1024/2, 768});
            renderer->setProjectionMatrix(view);
            renderer->render(*bg);
            renderer->render(*robot);
            renderer->render(*zombie);
            */
        /*
        ASGE::Text camera1 = ASGE::Text{ renderer->getDefaultFont(), "CAMERA1" };
        camera1.setPositionX(1024 * 0.25F - (camera1.getWidth() * 0.5F));
        camera1.setPositionY(30);

        ASGE::Text camera2 = ASGE::Text{ renderer->getDefaultFont(), "CAMERA2" };
        camera2.setPositionX(1024 * 0.75F - (camera2.getWidth() * 0.5F));
        camera2.setPositionY(30);
        */
        /*
        renderer->render(camera1);
        renderer->render(camera2);
         */
        break;
    }


  };

  void init(const ASGE::GameSettings& settings)
  {
    robot = renderer->createUniqueSprite();
    robot->loadTexture("/data/images/character_robot_idle.png");
    robot->srcRect()[0] = 10;
    robot->srcRect()[1] = 70;
    robot->srcRect()[2] = robot->width()-10;
    robot->srcRect()[3] = robot->height();
    robot->width(16*6);
    robot->height(16*8);
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
    rh_camera.resize(1600, 900);
    lh_camera.lookAt({ 1600/2, 900/2});
    rh_camera.lookAt({ 1600, 900});
    initMenu(settings);

    interact_text = std::make_shared<ASGE::Text>(renderer->getDefaultFont(),"You broke a barrel");
    interact_text->setScale(0.5F);
    interact_text->setZOrder(2);
  }

  void setPlayerToStart()
  {
    if (robot != nullptr)
    {
      robot->xPos(level.getTileByLayerID(0).x - robot->width()/7);
      robot->yPos(level.getTileByLayerID(0).y - robot->height()/1.5);
      interact_text->setPosition({lh_camera.position().x+100,lh_camera.position().y+100});
    }
  }

  void resetMap()
  {
    level.resetAll();
    if (!level.loadSharedMap("data/tilemap/testTile.tmx"))
    {
      Logging::ERRORS("Failed to Load Map");
      return;
    }
    level.init(renderer.get());
    setPlayerToStart();
  }

  void initMenu(const ASGE::GameSettings& settings)
  {
    int scrn_x = settings.window_width;
    int scrn_y = settings.window_height;
    auto text = menu_text.emplace_back(std::make_shared<ASGE::Text>(renderer->getDefaultFont(),"ASGE Dynamic Map Loader"));
    text->setPositionX(scrn_x/2-(text->getWidth()/2));
    text->setPositionY(scrn_y/2-(text->getHeight()/2));

    auto next_scene = menu_text.emplace_back(std::make_shared<ASGE::Text>(renderer->getDefaultFont(), "Press Enter to go into Editor"));
    next_scene->setPositionX(scrn_x/2-(next_scene->getWidth()/2));
    next_scene->setPositionY(scrn_y*0.75f);
  }

 private:
  int key_callback_id = -1; /**< Key Input Callback ID. */
  GameState gs = GameState::MENU;
  std::shared_ptr<ASGE::Sprite> robot = nullptr;
  std::unique_ptr<ASGE::Sprite> zombie = nullptr;
  std::unique_ptr<ASGE::Sprite> bg = nullptr;
  std::shared_ptr<ASGE::Text> interact_text = nullptr;
  std::vector<std::shared_ptr<ASGE::Text>> menu_text;
  ASGE::Camera lh_camera{};
  ASGE::Camera rh_camera{};
  MapLoader level;
  ASGE::Point2D player_direction = {0,0};
  bool interact_countdown = false;
  float current_time = 0;
  float max_countdown_time = 4;
  const float MOVESPEED = 2;
  ///This below raises an error!
//  std::function<filewatch::FileWatch<std::wstring>()> fw_lbd = (L"D:\\UWE\\CCTP\\ASGE-DynamicTileLoader\\cmake-build-debug\\bin\\data\\tilemap\\testTile.tmx",[](const std::wstring& path, const filewatch::Event change_type){});
  const int SCALE = 3;
};

int main(int /*argc*/, char* /*argv*/[])
{
  ASGE::GameSettings game_settings;
  game_settings.game_title  = "ASGEGame";
  game_settings.window_width  = 1600;
  game_settings.window_height = 900;
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

/**
 * Issue: function outside of class with a lambda expression. I tried putting it in an std::function to contain it to be called inside
 * class but I spent  far too long without any results despite research toward it. So I used a global boolean to control map
 * reloading.
 */
filewatch::FileWatch<std::wstring> watch(
  L"D:\\UWE\\CCTP\\ASGE-DynamicTileLoader\\cmake-build-debug\\bin\\data\\tilemap\\testTile.tmx",
  [](const std::wstring& path, const filewatch::Event change_type) {
    switch (change_type)
    {
      case filewatch::Event::added:
        Logging::INFO("The file was added to the directory.");
        break;
      case filewatch::Event::removed:
        Logging::INFO("The file was removed from directory.");
        break;
      case filewatch::Event::modified:
        Logging::INFO("The file was modified. This can be a change in the time stamp or attributes.");
        break;
      case filewatch::Event::renamed_old:
        Logging::INFO("The file was renamed and this is the old name.");
        break;
      case filewatch::Event::renamed_new:
        reload_MapLoader_map = true;
        break;
    }
  }
);

