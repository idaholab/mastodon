#ifndef MASTODONAPP_H
#define MASTODONAPP_H

#include "MooseApp.h"

class MastodonApp;

template <>
InputParameters validParams<MastodonApp>();

class MastodonApp : public MooseApp
{
public:
  MastodonApp(InputParameters parameters);
  static void registerApps();
  static void registerObjectDepends(Factory & factory);
  static void registerObjects(Factory & factory);
  static void associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void registerExecFlags(Factory & factory);
};

#endif /* MASTODONAPP_H */
