#include "AppFactory.h"
#include "MastodonTestApp.h"
#include "Moose.h"
#include "MooseApp.h"
#include "MooseInit.h"

// Create a performance log
PerfLog Moose::perf_log("Mastodon");

// Begin the main program.
int
main(int argc, char * argv[])
{
  // Initialize MPI, solvers and MOOSE
  MooseInit init(argc, argv);

  // Register this application's MooseApp and any it depends on
  MastodonTestApp::registerApps();

  // This creates dynamic memory that we're responsible for deleting
  MooseApp * app = AppFactory::createApp("MastodonTestApp", argc, argv);

  // Execute the application
  app->run();

  // Free up the memory we created earlier
  delete app;

  return 0;
}
