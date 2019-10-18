# Frequently Asked Questions


#### What if the `git clone` command does nothing?

Depending on your network configuration the HTTP access to the repository may not work. In this case
you must use [SSH](https://en.wikipedia.org/wiki/Secure_Shell) keys. First, follow the instructions
to
[generate a new SSH key](https://help.github.com/articles/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent/#generating-a-new-ssh-key)
and then run the following clone command.

```
git clone git@github.com:idaholab/mastodon.git
```

#### How to `update_and_rebuild_libmesh.sh` in parallel?

In the MASTODON folder, run the following command.

```
JOBS=8 ./moose/scripts/update_and_rebuild_libmesh.sh
```

The number `8` above is the number of processors.

#### How to run MASTODON on an INL high performance computing account?

There are several steps in this process:

- Log in to your hpc account using `ssh USERNAME@HPC_NAME`. An example is,

```
ssh john@falcon1
```

- Repeat the MASTODON installation process explained in [Getting Started/MacOS (Mojave)](https://mooseframework.inl.gov/mastodon/getting_started/macos_mojave.html) or similar. That is, modify your bash profile, install MASTODON, update and rebuild libmesh, and compile and test MASTODON. Note that you have to setup you [github](https://github.com) on the hpc.

- Now you can run an example using a special file with a `.sh` extension. For example, if you want to run [Example 03](https://mooseframework.inl.gov/mastodon/examples/example3.html) on hpc, create a file named `Example03_hpc.sh`. The filename can be anything but it should end with a `.sh` extension. This file should contain the following lines.

```
#!/bin/bash
#PBS -M FIRSTNAME.LASTNAME@inl.gov
#PBS -m abe
#PBS -N Example03_hpc
#PBS -P moose
#PBS -l select=4:ncpus=36:mpiprocs=36
#PBS -l walltime=00:90:00

JOB_NUM=${PBS_JOBID%%\.*}

cd $PBS_O_WORKDIR

\rm -f out
date > out
MV2_ENABLE_AFFINITY=0 mpiexec ~/projects/mastodon/mastodon-opt -i shear_beam_Isoil_free_field.i >> out
date >> out
```

- You can re-use the above file with the following changes:    

  - `FIRSTNAME.LASTNAME@inl.gov` is your INL email ID.
  - `Example03_hpc` is the name of the job.
  - `select=4:ncpus=36:mpiprocs=36` means you requested 4 nodes, 36 cpus, and 36 processors. You can increase or decrease these numbers depending on the hpc machine.
  - `walltime=00:90:00` is the wall time above which the jobs gets terminated. The format is Days:Hours:Minutes. Larger the wall time, more the delay in getting the job started, in general.
  - `MV2_ENABLE_AFFINITY=0 mpiexec ~/projects/mastodon/mastodon-opt -i shear_beam_Isoil_free_field.i >> out` has the name of the input file. Use the input file name you are interested in.

- Finally, save this file `Example03_hpc.sh` in the folder in which the input file is located. Then, run the following.

```
qsub Example03_hpc.sh
```

#### Some or all my tests are failing, what should I do?

There might be one of several reasons for this:

- You did not compile MASTODON. Compile MASTODON using `make` command.


- If the errors while running tests are related to libmesh, your libmesh is out of date. Update and rebuild libmesh using the following command in the MASTDON folder. Then compile MASTODON and try running the tests.

```
./moose/scripts/update_and_rebuild_libmesh.sh
```

- You might have accidentally made some changes to MASTODON source code. To undo these changes use the below command. Then update and rebuild libmesh, compile MASTODON, and run the tests as described in [Getting Started/MacOS (Mojave)](https://mooseframework.inl.gov/mastodon/getting_started/macos_mojave.html).  

```
git clean -dxf
```
