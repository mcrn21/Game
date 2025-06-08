#include "game/first_state.h"

#include <ae/engine.h>
#include <ae/game_state_stack.h>
#include <ae/system/log.h>

#include <ae/audio/music.h>
#include <ae/audio/sound.h>
#include <ae/audio/sound_buffer.h>

using namespace ae;

int32_t main()
{
    l_debug_mode();

    auto maybe_config = Config::loadFromFile("config.toml");
    if (!maybe_config)
        return 0;

    Engine engine;
    // App app;
    engine.create(maybe_config.value());

    engine.getGameStateStack()->push(createShared<FirstState>(engine));

    //mixkit-little-cat-pain-meow-87.wav
    // test_music_1.mp3
    // auto audio_test = createShared<SoundBuffer>();
    // if (audio_test->loadFromFile("music_test.ogg")) {
    //     spdlog::debug("Load audio!!!");
    // }

    // Sound sound;
    // sound.setSoundBuffer(audio_test);
    // sound.setLoop(true);
    // sound.play();

    // auto music = createShared<Music>();
    // if (music->loadFromFile("music_test.ogg"))
    //     spdlog::debug("Load music!!!");

    // music->play();
    // // music->setPitch(2.1f);

    // app.getInput()->keyJustDown.connect([&](KeyCode key_code) {
    //     if (key_code == KeyCode::Q)
    //         music->play();

    //     if (key_code == KeyCode::W)
    //         music->pause();

    //     if (key_code == KeyCode::E)
    //         music->stop();
    // });

    return engine.exec();
}
