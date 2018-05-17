#ifndef MASTODONTESTAPP_H
#define MASTODONTESTAPP_H

#include "MooseApp.h"

class MastodonTestApp;

template <>
InputParameters validParams<MastodonTestApp>();

class MastodonTestApp : public MooseApp
{
public:
  MastodonTestApp(InputParameters parameters);

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
  static void registerExecFlags(Factory & factory);
};

#endif /* MASTODONTESTAPP_H */
