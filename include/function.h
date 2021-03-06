// Copyright https://github.com/PatWie/CppNumericalSolvers, MIT license
#ifndef INCLUDE_CPPOPTLIB_FUNCTION_H_
#define INCLUDE_CPPOPTLIB_FUNCTION_H_

#include "Eigen/Core"
#include "utils/derivatives.h"

namespace cppoptlib {
namespace function {

// Specifies a current function state.
template <typename scalar_t, typename vector_t, typename matrix_t>
struct State {
  int dim = -1;
  int order = -1;

  scalar_t value = 0;  // The objective value.
  vector_t x;          // The current input value in x.
  vector_t gradient;   // The gradient in x.
  matrix_t hessian;    // The Hessian in x;

  State() = default;
  State(const int dim, const int order)
      : dim(dim),
        order(order),
        x(vector_t::Zero(dim)),
        gradient(vector_t::Zero(dim)),
        hessian(matrix_t::Zero(dim, dim)) {}

  State operator=(const State<scalar_t, vector_t, matrix_t>& rhs) {
    assert(rhs.order > -1);
    dim = rhs.dim;
    order = rhs.order;
    value = rhs.value;
    x = rhs.x.eval();
    if (order >= 1) {
      gradient = rhs.gradient.eval();
    }
    if (order >= 2) {
      hessian = rhs.hessian.eval();
    }
    return *this;
  }
};

template <class TScalar, int TDim = Eigen::Dynamic>
class Function {
 public:
  static constexpr int Dim = TDim;

  using scalar_t = TScalar;
  using vector_t = Eigen::Matrix<TScalar, Dim, 1>;
  using hessian_t = Eigen::Matrix<TScalar, Dim, Dim>;
  using matrix_t = Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic>;
  using index_t = typename vector_t::Index;

  using state_t = function::State<scalar_t, vector_t, hessian_t>;

 public:
  Function() = default;
  virtual ~Function() = default;

  // Computes the value of a function.
  virtual scalar_t operator()(const vector_t& x) const = 0;

  // Computes the gradient of a function.
  virtual void Gradient(const vector_t& x, vector_t* grad) const {
    utils::ComputeFiniteGradient(*this, x, grad);
  }
  // Computes the Hessian of a function.
  virtual void Hessian(const vector_t& x, hessian_t* hessian) const {
    utils::ComputeFiniteHessian(*this, x, hessian);
  }

  virtual int Order() const { return 1; }

  // For improved performance, this function will return the state directly.
  // Override this method if you can compute the objective value, gradient and
  // Hessian simultaneously.
  virtual State<scalar_t, vector_t, hessian_t> Eval(const vector_t& x,
                                                    const int order = 2) const {
    State<scalar_t, vector_t, hessian_t> state(x.rows(), order);
    state.value = this->operator()(x);
    state.x = x;
    if (order >= 1) {
      this->Gradient(x, &state.gradient);
    }
    if (order >= 2) {
      this->Hessian(x, &state.hessian);
    }
    return state;
  }
};

}  // namespace function
}  // namespace cppoptlib

#endif  // INCLUDE_CPPOPTLIB_FUNCTION_H_
