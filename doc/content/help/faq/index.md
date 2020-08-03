# Frequently Asked Questions


### What if the `git clone` command does nothing?

Depending on your network configuration the HTTP access to the repository may not work. In this case
you must use [SSH](https://en.wikipedia.org/wiki/Secure_Shell) keys. First, follow the instructions
to
[generate a new SSH key](https://help.github.com/articles/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent/#generating-a-new-ssh-key)
and then run the following clone command.

```
git clone git@github.com:idaholab/mastodon.git
```

### How to `update_and_rebuild_libmesh.sh` in parallel?

In the MASTODON folder, run the following command.

```
JOBS=8 ./moose/scripts/update_and_rebuild_libmesh.sh
```

The number `8` above is the number of processors and can be changed to a different value.

### How to run MASTODON on an INL high performance computing account?

There are several steps in this process:

- Log in to your hpc account using `ssh USERNAME@HPC_NAME`. An example is,

```
ssh john@falcon1
```

- Install MASTODON on HPC

- Job scheduling is performed using the Portable Batch System (PBS). Batch script files should be created with a unique name using the `.sh` extension. For example, if you want to run [Example 03a](examples/example3a.md) on hpc, create a file named `Example03_hpc.sh`. The filename can be anything but it should end with a `.sh` extension. This file should contain the following lines.

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

- Finally, save this file `Example03_hpc.sh` in the folder in which the input file is located. Batch files can be submitted into the queue using the qsub command as shown below.

```
qsub Example03_hpc.sh
```

- For more information on PBS see [this](http://hpcweb.inl.gov/home/pbs).

### Some or all my tests are failing, what should I do?

There might be one of several reasons for this:

- You did not compile MASTODON. Compile MASTODON using `make` command.

- Your MOOSE environment may not be up to date. Update using the following commands.

```
conda activate moose
conda update --all
```

- You might have accidentally made some changes to MASTODON source code. To undo these changes use the below command. Then update and rebuild libmesh, compile MASTODON, and run the tests as described in the [Getting Started](getting_started/installation.md) page.  

```
git clean -dxf
```

!alert! warning
Running this command will completely erase all additional files and uncommitted changes to the MASTODON directory.
!alert-end!

### How to write an input file from scratch?

Generally, we do not recommend doing this. On the [Examples](examples/index.md) page, a variety of problems are discussed. We recommend downloading the input file from one of those problems and modifying it to suit your own problem. In this process, the MASTODON [User Manual](manuals/user/index.md) and [Syntax](syntax/index.md) pages might also help. There are also a number of input files checked into the `tests` directories within MASTODON. In addition, users are welcome to reach out to the MASTODON community through google groups and ask for files similar to what they are doing.

### How to input a mesh file from ABAQUS (i.e., a `.inp` file)?

MASTODON is capable of accepting mesh files in many formats, one of them being an ABAQUS mesh file. Below is an example on how to input an ABAQUS mesh file. Please refer to [`meshgenerators`](/meshgenerators/FileMeshGenerator.md) MOOSE page for more information.

```
[Mesh]
  [./file]
    type = FileMeshGenerator
    file = ABAQUS_input.inp
  [../]
[]
```

!alert! note
The .inp reader is currently limited to only one mesh block. If the .inp file contains multiple parts, they will all be moved into one block in MOOSE/MASTODON.
!alert-end!

### How to specify a velocity or a displacement as the boundary condition? Most example problems use an acceleration boundary condition.

Velocity and displacement can be specified as a boundary condition using [`PresetVelocity`](/bcs/PresetVelocity.md) and [`PresetDisplacement`](/bcs/PresetDisplacement.md), respectively.

### What is a bash profile and how can I access it?

Bash profile is a hidden file in your home directory. This file is named `.bash_profile` on Macs and `.bashrc` on Linux. The dot in the beginning of the file designates it as a hidden file on Unix systems. The bash profile contains the startup configuration and preferences for your terminal. To view or edit this file, open your terminal, go to your home directory, and run `nano .bash_profile`, to open it using the text editor, `nano`. You can also use other text editors such as `emacs` or `vim` to view or edit your bash profile.

### When I provide a displacement input at a boundary, the resulting displacement doesn't match the input. What's going on here?

MASTODON only applies displacement inputs to the mesh. If a velocity or acceleration input is provided using `PresetVelocity` or `PresetAcceleration`, it is integrated to the corresponding displacement history and applied to the mesh. This conversion requires an initial velocity or acceleration value, which, by default, is assumed to be zero. If an initial velocity and acceleration corresponding to the displacement history is not provided, MASTODON will input a displacement history that results in a zero initial velocity/acceleration. A temporary fix for this is to include a few time steps of constant zero displacement history in the beginning of the simulation, so that the initial velocity and acceleration are also zero for this displacement history. Alternatively, an initial velocity or acceleration can be provided through the `initial_condition` parameter in the velocity or acceleration [auxvariable definitions](/variables/MooseVariable.md) in the input file.
