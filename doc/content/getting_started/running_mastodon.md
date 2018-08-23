# Running MASTODON

The following information presented on this page is a general overview of the
MASTODON input file as well as the process for running a model.
For a more detailed walk through of a specific MASTODON input file, refer to
the [MASTODON User Manual](manuals/user/index.md).

## Input to MASTODON

MASTODON simulation models are defined by the user through a text file
that defines the parameters of the run.  This text file specifies the
set of code objects that are composed together to simulate a physical
problem, and provides parameters that control how those objects behave
and interact with each other.  This text file can be prepared using any
text editor. The free and opensource [Atom](http://atom.io) text editor has
several packages that help with creating MOOSE and MASTODON input files. Please
read this [article](development/atomio.md) about using Atom for writing
MASTODON input files.

In addition to the text file describing the model parameters, MASTODON also
requires a definition of the finite element mesh on which the physics
equations are solved. The mesh can be generated internally by MASTODON using
parameters defined in MASTODON's input file for very simple geometries, or can
be read from a file as defined in the MASTODON input file. MASTODON supports the
full set of mesh file formats supported by MOOSE, although the most common
mesh format is the ExodusII format.

## Basic Syntax

The input file used by MASTODON (or any MOOSE application) is broken into
sections or blocks identified with square brackets. The type of input block is
placed in the opening brackets, and empty brackets mark the end of the block.

```text
[BlockName]
  block_lines_and_subblocks
[]
```

Each block may contain an arbitrary number of line commands to define
parameters related to that block. They can also optionally contain one
or more subblocks, which may in turn contain their own nested subblocks.
Subblocks are opened and closed with the following syntax:

```text
[./subblock_name]
  line_command1
  line_command2
[../]
```

!alert note
All subblocks contained within a given block must have unique
names (within the opening [] brackets).

Parameters are defined with line commands, and are given as key/value
pairs separated by an equals sign (=). They specify parameters to be
used by the object being described.  The key is a string (no
whitespace), and the value may be a string, an integer, a real number,
or a list of strings, integers, or real numbers.  Lists are given
in single quotes and are separated by whitespace.

The following are examples of line commands for a single parameter and
for a list of parameters:

```text
single_parameter = 2.3
list_of_parameters = '1.0 2.3 3.7'
```

Blocks and subblocks at any level can contain line commands, which must
be appropriate for the scope of the block containing them. Most of the
time, blocks are used to create instances of MOOSE objects, and contain
a `type = ` parameter to specify the type of MOOSE object to be
created. The name of the MOOSE object specified in the parameter
corresponds to the name of the class in the C++ source code.

Each object type has a unique set of input parameters that are valid for
specifying the behavior of that object. Some parameters are required,
and some are optional, and revert to default behavior if they are not
specified.  An error message is generated if a line command does not
apply within the scope in which it is provided. Repeating a line within
a block also results in an error. A full list of object types and the
corresponding parameters valid for that object can be found in
[syntax](syntax/index.md).

The name of a subblock (`[./name]`) is arbitrary. However, these
names should be chosen to be meaningful because they can be used to
refer to those entities elsewhere in the input file. Not every created
entity is referenced elsewhere, but a name must be created for every
entity regardless.

## Summary of MOOSE Object Types

MOOSE is an objected-oriented system with well-defined interfaces for
applications to define their own physics-specific code modules. The
following is a listing of the major types of MOOSE objects used by MASTODON:

- Variable
- Kernel
- AuxVariable
- AuxKernel
- Material
- BoundaryCondition
- Function
- Postprocessor
- VectorPostprocessor
- Constraint
- Damper

Specialized versions of these object types are implemented to provide
the functionality needed to model physics of interest for MASTODON.

## Input Syntax Page

A complete listing of all input syntax options in MOOSE is available on
[the MOOSE Documentation page](http://mooseframework.org/documentation/).
See the section on Input File Documentation, you can run

```bash
./mastodon-opt --dump
```

to get a list of valid input options for MASTODON.

## Units

MASTODON can be run using any unit system preferred by the
user.

## Executing MASTODON

When first starting out with MASTODON, it is recommended to start from an
example problem similar to the problem that you are trying to solve.
Multiple examples can be found at mastodon/examples/.
It may be worth running the example problems to see how the code works
and modifying input parameters to see how the run time, results and
convergence behavior change.

To demonstrate running MASTODON, consider the simple shear example problem
included in the MASTODON example set:

```bash
cd ~/projects/mastodon/examples/ex01
# To run with one processor

~/projects/mastodon/mastodon-opt -i HYS_darendeli.i
# To run in parallel (2 processors)

mpiexec -n 2 ../../mastodon-opt -i HYS_darendeli.i
```

!alert note
The procedure for running this model in parallel is shown only
for illustrative purposes. This particular model is quite small, and would
not benefit from being run in parallel, although it can be run that way.

## Post Processing

MASTODON typically writes solution data to an ExodusII file. Data may also
be written in other formats, a simple comma separated file giving global
data being the most common.

Several options exist for viewing ExodusII results files. These include
commercial as well as open-source tools. One good choice is Paraview,
which is also open-source.

Paraview is available on a variety of platforms. It is capable of
displaying node and element data in several ways. It will also produce
line plots of global data or data from a particular node or element.

## Graphical User Interface

It is worth noting that a graphical user interface (GUI) exists for all
MOOSE-based applications. This GUI is named Peacock. Information about
Peacock and how to set it up for use may be found on
[the MOOSE wiki page](http://mooseframework.org/wiki/Peacock).

Peacock may be used to generate a text input file. It is also capable of
submitting the analysis. It also provides basic post processing
capabilities.
