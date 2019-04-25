## Install MASTODON

MASTODON is hosted on [GitHub](https://github.com/idaholab/mastodon) and should be cloned directly
from there using [git](https://git-scm.com/). We recommend creating a directory named projects to put
all of your MASTODON related work.

To install MASTODON run the following commands in a terminal.

```bash
mkdir ~/projects
cd ~/projects
git clone https://github.com/idaholab/mastodon.git
cd mastodon
git checkout master
git submodule update --init moose
```

## Compile libMesh

MASTODON directly relies on the [libMesh](http://libmesh.github.io/) finite-element
framework. Because of this strong tie MASTODON contains a particular version of libMesh that we have
vetted for our users. To pull down and compile this version of libMesh you simply need to run a
script in MOOSE:

```bash
cd ~/projects/mastodon/moose
./scripts/update_and_rebuild_libmesh.sh
```

## Compile and Test MASTODON

After libMesh has compiled the next step is to compile and test MASTODON.

```bash
cd ~/projects/mastodon
make -j 4
./run_tests -j 4
```

If the installation was successful you should see most of the tests passing (some tests will be
skipped depending on your system environment).

At this point you now have a working copy of MASTODON.

!alert note
The number 4 should be replaced with the number of processors you have available on your system.


## Update MASTODON

MASTODON does not use traditional versioning, is under heavy development, and is being updated
continuously. Therefore, it is critical that you continue to update MASTODON regularly to get the
latest features.

To update MASTODON, use the following commands:

```bash
cd ~/projects/mastodon
git fetch origin
git rebase origin/master
git submodule update
./moose/scripts/update_and_rebuild_libmesh.sh
```

To verify that the new version is working correctly is must be compiled and tested.

```bash
cd ~/projects/mastodon
make -j4
./run_tests -j4
```
