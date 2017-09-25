#include "MastodonTestApp.h"
#include "MastodonApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<MastodonTestApp>()
{
  InputParameters params = validParams<MastodonApp>();
  return params;
}

MastodonTestApp::MastodonTestApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  MastodonApp::registerObjectDepends(_factory);
  MastodonApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  MastodonApp::associateSyntaxDepends(_syntax, _action_factory);
  MastodonApp::associateSyntax(_syntax, _action_factory);

  bool use_test_objs = getParam<bool>("allow_test_objects");
  if (use_test_objs)
  {
    MastodonTestApp::registerObjects(_factory);
    MastodonTestApp::associateSyntax(_syntax, _action_factory);
  }
}

MastodonTestApp::~MastodonTestApp() {}

// External entry point for dynamic application loading
extern "C" void
MastodonTestApp__registerApps()
{
  MastodonTestApp::registerApps();
}
void
MastodonTestApp::registerApps()
{
  registerApp(MastodonApp);
  registerApp(MastodonTestApp);
}

// External entry point for dynamic object registration
extern "C" void
MastodonTestApp__registerObjects(Factory & factory)
{
  MastodonTestApp::registerObjects(factory);
}
void
MastodonTestApp::registerObjects(Factory & /*factory*/)
{
}

// External entry point for dynamic syntax association
extern "C" void
MastodonTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  MastodonTestApp::associateSyntax(syntax, action_factory);
}
void
MastodonTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
