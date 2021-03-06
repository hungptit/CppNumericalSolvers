// Copyright 2020, https://github.com/PatWie/CppNumericalSolvers

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <vector>

#include "Eigen/Core"
#include "function.h"
#include "solver/bfgs.h"
#include "solver/conjugated_gradient_descent.h"
#include "solver/gradient_descent.h"
#include "solver/lbfgs.h"
#include "solver/lbfgsb.h"
#include "solver/newton_descent.h"

using FunctionXd = cppoptlib::function::Function<double>;

// https://en.wikipedia.org/wiki/Rosenbrock_function
struct Rosenbrock : public FunctionXd {
  using FunctionXd::hessian_t;
  using FunctionXd::vector_t;

  scalar_t operator()(const vector_t& x) const override {
    const double t1 = (a - x[0]);
    const double t2 = (x[1] - x[0] * x[0]);
    return t1 * t1 + b * t2 * t2;
  }
  static constexpr double a = 1;
  static constexpr double b = 100;
};

template <typename Solver>
void solve(const std::vector<double>& values) {
  Rosenbrock f;
  Rosenbrock::vector_t x(2);
  x << values[0], values[1];

  auto state = f.Eval(x);

  std::cout << "f(x): " << f(x) << "\n"
            << "gradient: " << state.gradient << "\n"
            << "hessian: " << state.hessian << "\n"
            << "cppoptlib::utils::IsGradientCorrect(f, x): "
            << cppoptlib::utils::IsGradientCorrect(f, x) << "\n"
            << "cppoptlib::utils::IsHessianCorrect(f, x): "
            << cppoptlib::utils::IsHessianCorrect(f, x) << "\n";

  Solver solver;
  const auto [sol, final_state] = solver.Minimize(f, x);
  std::cout << "Solver status: " << final_state.status << "\n";
  std::cout << "Solution: " << sol.x.transpose() << "\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Need to provide the solver id (0-5).";
    return EXIT_FAILURE;
  }
  const uint32_t solver_id = std::atol(argv[1]);
  const std::vector<double> x{-100, 4};
  enum SolverId : int32_t {
    NEWTON_DESCENT = 0,
    GRADIENT_DESCENT = 1,
    CONJUGATE_GRADIENT_DESCENT = 2,
    BFGS = 3,
    L_BFGS = 4,
    L_BFGS_B = 5
  };

  switch (solver_id) {
    case NEWTON_DESCENT: {
      std::cout << "NewtonDescent\n";
      using Solver = cppoptlib::solver::NewtonDescent<Rosenbrock>;
      solve<Solver>(x);
      break;
    }
    case GRADIENT_DESCENT: {
      std::cout << "GradientDescent\n";
      using Solver = cppoptlib::solver::GradientDescent<Rosenbrock>;
      solve<Solver>(x);
      break;
    }

    case CONJUGATE_GRADIENT_DESCENT: {
      std::cout << "ConjugatedGradientDescent\n";
      using Solver = cppoptlib::solver::ConjugatedGradientDescent<Rosenbrock>;
      solve<Solver>(x);
      break;
    }

    case BFGS: {
      std::cout << "Bfgs\n";
      using Solver = cppoptlib::solver::Bfgs<Rosenbrock>;
      solve<Solver>(x);
      break;
    }

    case L_BFGS: {
      std::cout << "Lbfgs\n";
      using Solver = cppoptlib::solver::Lbfgs<Rosenbrock>;
      solve<Solver>(x);
      break;
    }

    case L_BFGS_B: {
      std::cout << "Lbfgsb\n";
      using Solver = cppoptlib::solver::Lbfgsb<Rosenbrock>;
      solve<Solver>(x);
      break;
    }

    default:
      std::cerr << "The solver id must be between 0-5\n";
      break;
  }

  return EXIT_SUCCESS;
}
