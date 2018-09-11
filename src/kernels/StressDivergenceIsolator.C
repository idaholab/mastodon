// MASTODON includes
#include "StressDivergenceIsolator.h"

// MOOSE includes
#include "Assembly.h"
#include "Material.h"
#include "MooseVariable.h"
#include "SystemBase.h"
#include "NonlinearSystem.h"
#include "MooseMesh.h"

// libmesh includes
#include "libmesh/quadrature.h"

registerMooseObject("MastodonApp", StressDivergenceIsolator);

template <>
InputParameters
validParams<StressDivergenceIsolator>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Kernel for isolator element");
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction "
                                        "the variable this kernel acts in. (0 for x, "
                                        "1 for y, 2 for z, 3 for rot_x, 4 for rot_y and 5 for rot_z).");
  params.addRequiredCoupledVar("displacements", "The displacement variables for isolator.");
  params.addRequiredCoupledVar("rotations", "The rotation variables for the isolator.");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

StressDivergenceIsolator::StressDivergenceIsolator(const InputParameters & parameters)
  : Kernel(parameters),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _nrot(coupledComponents("rotations")),
    _rot_var(_nrot),
    _Fg(getMaterialPropertyByName<ColumnMajorMatrix>("global_forces")),
    _Kg(getMaterialPropertyByName<ColumnMajorMatrix>("global_stiffness_matrix"))
    // _kdr(getMaterialPropertyByName<ColumnMajorMatrix>("displacement_rotation_stiffness_matrix")),
    // _total_global_to_local_rotation(getMaterialPropertyByName<ColumnMajorMatrix>("total_global_to_local_rotation"))
{
  if (_component > 5)
    mooseError("Error in StressDivergenceIsolator block ", name(), ". Please enter an integer value between 0 and 5 for the 'component' parameter.");

  if (_ndisp != _nrot)
    mooseError("Error in StressDivergenceIsolator block ", name(), ". The number of displacement and rotation variables should be the same.");

  for (unsigned int i = 0; i < _ndisp; ++i)
    _disp_var[i] = coupled("displacements", i);

  for (unsigned int i = 0; i < _nrot; ++i)
    _rot_var[i] = coupled("rotations", i);
}

void
StressDivergenceIsolator::computeResidual()
{
  // Accessing residual vector, re, from MOOSE assembly
  DenseVector<Number> & re = _assembly.residualBlock(_var.number());
  mooseAssert(re.size() == 2, "Isolator element only has two nodes.");
  _local_re.resize(re.size());
  _local_re.zero();

  // Calculating residual for node 0 (external forces on node 0)
  _local_re(0) = _Fg[0](_component);

  // External force on node 1 (external forces on node 1)
  _local_re(1) = _Fg[0](_component + 6);

  re += _local_re;

  // std::cout << "******RESIDUAL = \n" << re << "\n";

  if (_has_save_in)
  {
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (unsigned int i = 0; i < _save_in.size(); ++i)
      _save_in[i]->sys().solution().add_vector(_local_re, _save_in[i]->dofIndices());
  }
}

void
StressDivergenceIsolator::computeJacobian()
{
  // std::cout << "$$$$$$$$$$$Executing ComputeJacobian\n";

  // Access Jacobian; size is n x n (n is number of nodes)
  DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), _var.number());
  _local_ke.resize(ke.m(), ke.n());
  _local_ke.zero();

  // i and j are looping over nodes
  for (unsigned int i = 0; i < _test.size(); ++i)
    for (unsigned int j = 0; j < _phi.size(); ++j)
    {
      // if (i == j)
      //   _local_ke(i, j) += _kg[0](i * 6 + _component, i * 6 + _component);
      // else
        _local_ke(i, j) += _Kg[0](i * 6 + _component, j * 6 + _component);
    }

  ke += _local_ke;

  // std::cout << "diag JACOBIAN *************** for component " << _component << "\n" << ke << "\n";

  if (_has_diag_save_in)
  {
    unsigned int rows = ke.m();
    DenseVector<Number> diag(rows);
    for (unsigned int i = 0; i < rows; ++i)
      diag(i) = _local_ke(i, i);

    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (unsigned int i = 0; i < _diag_save_in.size(); ++i)
      _diag_save_in[i]->sys().solution().add_vector(diag, _diag_save_in[i]->dofIndices());
  }
}

void
StressDivergenceIsolator::computeOffDiagJacobian(MooseVariableFEBase & jvar)
// coupling one variable to another (disp x to disp y, etc)
{
  // std::cout << "$$$$$$$$$$$Executing computeOffDiagJacobian\n";

  size_t jvar_num = jvar.number();
  if (jvar_num == _var.number())
  // jacobian calculation if jvar is the same as the current variable i.e.,
  // diagonal elements
    computeJacobian();

  else
  // jacobian calculation for off-diagonal elements
  {
    unsigned int coupled_component = 0;
    bool coupled = false;
    // finding which variable jvar is
    for (unsigned int i = 0; i < _ndisp; ++i)
    {
      if (jvar_num == _disp_var[i])
        {
          coupled_component = i;
          coupled = true;
          break;
        }
      else if (jvar_num == _rot_var[i])
        {
          coupled_component = i + 3;
          coupled = true;
          break;
        }
    }
    // getting the jacobian from assembly
    DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar_num);
    if (coupled)
    {
      for (unsigned int i = 0; i < _test.size(); ++i)
        for (unsigned int j = 0; j < _phi.size(); ++j)
          ke(i, j) += _Kg[0](i * 6 + _component, j * 6 + coupled_component);
    }
    // std::cout << "off diag JACOBIAN *************** for components " << _component << ", " << coupled_component << "\n" << ke << "\n";

  }
}
