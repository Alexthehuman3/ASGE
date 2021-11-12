#include <Engine/GameSettings.hpp>
#include <Engine/Logger.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>

class ASGENetGame : public ASGE::OGLGame
{
 public:
  explicit ASGENetGame(const ASGE::GameSettings& settings) :
    OGLGame(settings)
  {
    inputs->use_threads = false;
    inputs->addCallbackFnc(ASGE::EventType::E_KEY, &ASGENetGame::keyHandler, this);

    renderer->setClearColour(ASGE::COLOURS::BLACK);
    lh_camera.resize(1024 / 2.0F, 768);
    rh_camera.resize(1024 / 2.0F, 768);
    lh_camera.lookAt({1024 * 0.25F, 768 / 2.0F});
    rh_camera.lookAt({1024 * 0.50F, 768 / 2.0F});
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
    if(keys[ASGE::KEYS::KEY_A])
    {
      // rh_camera.translateX(-5);
    }
    if(keys[ASGE::KEYS::KEY_D])
    {
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_W])
    {
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_S])
    {
      // rh_camera.translateX(+5);
    }

    if(keys[ASGE::KEYS::KEY_LEFT])
    {
      // rh_camera.translateX(-5);
    }
    if(keys[ASGE::KEYS::KEY_RIGHT])
    {
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_UP])
    {
      // rh_camera.translateX(+5);
    }
    if(keys[ASGE::KEYS::KEY_DOWN])
    {
      // rh_camera.translateX(+5);
    }

/*
    lh_camera.lookAt(
      {
        robot->xPos() + robot->width()  * 0.5F,
        robot->yPos() + robot->height() * 0.5F });

    rh_camera.lookAt(
      {
        zombie->xPos() + zombie->width()  * 0.5F,
        zombie->yPos() + zombie->height() * 0.5F });
*/
    lh_camera.update(us);
    rh_camera.update(us);
  };

  void render(const ASGE::GameTime& us) override
  {
    renderer->setViewport({0, 0, 1024/2, 768});
    renderer->setProjectionMatrix(lh_camera.getView());
    /*
    renderer->render(*bg);
    renderer->render(*robot);
    renderer->render(*zombie);
     */

    auto view = rh_camera.getView();
    renderer->setViewport({1024/2, 0, 1024/2, 768});
    renderer->setProjectionMatrix(view);
    /*
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

 private:
  int key_callback_id = -1; /**< Key Input Callback ID. */
  ASGE::Camera lh_camera{};
  ASGE::Camera rh_camera{};
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
