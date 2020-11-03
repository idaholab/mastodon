## Clone MASTODON

MASTODON is hosted on [GitHub](https://github.com/idaholab/mastodon) and should be cloned directly
from there using [git](https://git-scm.com/). We recommend creating the directory, `~/projects`, to put
all of your MASTODON related work.

MASTODON can be installed with or without [BlackBear](https://mooseframework.inl.gov/blackbear).
BlackBear is a MOOSE-based structural material degradation simulation code and
can be included in MASTODON as a submodule to run multi-physics simulations of
degraded structures. Below are the instructions to install MASTODON with
or without BlackBear.

To clone MASTODON +=without BlackBear=+, run the following commands in a terminal.

```bash
mkdir ~/projects
cd ~/projects
git clone https://github.com/idaholab/mastodon.git
cd mastodon
git checkout master
git submodule update --init moose
```

To clone MASTODON +=with BlackBear=+, run the following commands in a terminal.

```bash
mkdir ~/projects
cd ~/projects
git clone https://github.com/idaholab/mastodon.git
cd mastodon
git checkout master
git submodule update --init moose
git submodule update --init blackbear
cd blackbear
git submodule update --init contrib/neml
```

## Compile and Test MASTODON

After cloning, MASTODON should be compiled and tested before use. This step is the same
with or without BlackBear.

```bash
conda activate moose
cd ~/projects/mastodon
make -j 4
./run_tests -j 4
```

!alert note
The number 4 should be replaced with the number of processors you have available on your system.

If the installation was successful you should see most of the tests passing (some tests will be
skipped depending on your system environment).

You now have a working copy of MASTODON.

## Update MASTODON

MASTODON does not use traditional versioning, is under heavy development, and is being updated
continuously. Therefore, it is critical that you continue to update MASTODON regularly to get the
latest features. Following are the steps to update MASTODON.

First update the MOOSE environment:

```bash
conda activate moose
conda update --all
```

Then, update MASTODON using the following commands:

```bash
cd ~/projects/mastodon
git fetch origin
git rebase origin/master
git submodule update
```

Compile and test your fresh copy.

```bash
cd ~/projects/mastodon
make -j4
./run_tests -j4
```
