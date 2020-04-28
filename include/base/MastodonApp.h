#ifndef MASTODONAPP_H
#define MASTODONAPP_H

#include "MooseApp.h"



class MastodonApp : public MooseApp
{
public:
  static InputParameters validParams();
  MastodonApp(InputParameters parameters);
  static void registerApps();
  static void registerAll(Factory &, ActionFactory &, Syntax &);
};

#endif /* MASTODONAPP_H */
