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
  virtual ~MastodonApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* MASTODONAPP_H */
