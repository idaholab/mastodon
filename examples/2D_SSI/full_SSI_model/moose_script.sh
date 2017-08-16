#!/bin/bash
#PBS -M swetha.veeraraghavan@inl.gov
#PBS -m abe
#PBS -N SSI_1
#PBS -P moose
#PBS -l select=4:ncpus=36:mpiprocs=36:cputype=broadwell:mem=120gb
#PBS -l place=scatter:excl
#PBS -l walltime=200:00:00

JOB_NUM=${PBS_JOBID%%\.*}

cd $PBS_O_WORKDIR

module purge
module load pbs
module load use.moose
module load moose-dev-gcc
module load VTK/6.1.0-gmvolf-5.5.4-Python-2.7.8

\rm -f out_profile
date > out_profile
mpiexec ~/projects/mastodon_2/mastodon/mastodon-opt -i 2D_lanl_ssi_profile.i >> out_profile
date >> out_profile
