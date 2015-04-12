#include "snd/generic.hpp"
#include "thr/dispatch.hpp"
#include "ui/ui_stack.hpp"
#include "ob_console_state.hpp"
#include "ob_game.hpp"
#include "ob_globals.hpp"
#include "ob_settings.hpp"

#include <boost/program_options.hpp>
#include <boost/exception/diagnostic_information.hpp>

//#include <iostream>

/** Turn off not to catch exceptions. */
#define CATCH_EXCEPTIONS 0

using namespace ob;
namespace po = boost::program_options;

/** Console output content. */
static const char *usage = ""
"Usage: orbital_bombardment <options>\n"
"Orbital Bombardment - a NajuEngine conceptual test game originally made for\n"
"Assembly 2010 game development competition.\n"
"\n"
"Copyright (c) Faemiyah. Distributed using Creative Commons and BSD licences.\n"
"\n";

int main(int argc, char *argv[])
{
#if (CATCH_EXCEPTIONS != 0)
  try
  {
#endif
    unsigned w;
    unsigned h;
    unsigned b;

    thr::thr_init();
    conf_init();

    if(argc > 0)
    {
      po::options_description desc("Options");
      desc.add_options()
        ("detail,d", po::value<std::string>(), "Detail level (laptop, desktop, bleeding, custom).")
        ("generate,g", "Generated procedural data will be saved for faster loading the next time around.\nOnly use this if you really know what you're doing.")
        ("fullscreen,f", "Full-screen mode instead of window.")
        ("help,h", "Print help text.")
        ("resolution,r", po::value<std::string>(), "Resolution to use.")
        ("window,w", "Window instead of full-screen mode.");

      po::variables_map vmap;
      po::store(po::parse_command_line(argc, argv, desc), vmap);
      po::notify(vmap);

      if(vmap.count("detail"))
      {
        conf->setDetail(vmap["detail"].as<std::string>());
      }
      if(vmap.count("generate"))
      {
        Globals::set_generate();
      }
      if(vmap.count("help"))
      {
        std::cout << usage << desc << std::endl;
        return 0;
      }
      if(vmap.count("fullscreen"))
      {
        conf->getFullscreen().set(1);
      }
      if(vmap.count("resolution"))
      {
        conf->setResolution(vmap["resolution"].as<std::string>());
      }
      if(vmap.count("window"))
      {
        conf->getFullscreen().set(0);
      }
    }

    boost::tie(w, h, b) = gfx::SurfaceScreen::parseResolution(conf->getResolution());
    gfx::SurfaceScreen scr(w, h, b, (conf->getFullscreen().get() != 0));

    snd::snd_init(16);
    glob_init(scr, conf->getDetail());
    SDL_SetCursor(glob->getCursorBlank());

    {
      ui::UiStack stack(scr, 100);

      stack.pushState(new ConsoleState(glob->getConsole()));
      stack.suspend();

      boost::thread precalc_thread(glob_precalc);

      thr::thr_main(2);

      precalc_thread.join();
    }

    // Deinitialize.
    SDL_SetCursor(glob->getCursorDefault());
    glob_quit();
    snd::snd_quit();
    conf_quit();
#if (CATCH_EXCEPTIONS != 0)
  }
  catch(const boost::exception & e)
  {
    std::cerr <<  boost::diagnostic_information(e) << std::endl;
    return EXIT_FAILURE;
  }
  catch(const std::exception & e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch(...)
  {
    std::cerr << "Unknown exception caught!" << std::endl;
    return EXIT_FAILURE;
  }
#endif

  return EXIT_SUCCESS;
}

