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
  static void registerAll(Factory &, ActionFactory &, Syntax &);
};

#endif /* MASTODONAPP_H */
