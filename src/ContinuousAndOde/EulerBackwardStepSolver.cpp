#include "EulerBackwardStepSolver.h"
#include <spdlog/spdlog.h>

namespace PySysLinkBase
{
    std::tuple<bool, std::vector<double>, double> EulerBackwardStepSolver::SolveStep(std::function<std::vector<double>(std::vector<double>, double)> systemDerivatives,
                                                                                std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobian,
                                                                                std::vector<double> states_0, double currentTime, double timeStep)
    {
        const double tol = 1e-6;
        const int maxIter = 50;
        double t_next = currentTime + timeStep;
        int n = states_0.size();
        std::vector<double> x = states_0;  // initial guess

        // Newton-Raphson iteration
        for (int iter = 0; iter < maxIter; ++iter) {
            // Evaluate function f(x, t_next)
            std::vector<double> f = systemDerivatives(x, t_next);
            // Compute F(x) = x - states_0 - timeStep * f(x, t_next)
            std::vector<double> F(n, 0.0);
            for (int i = 0; i < n; i++) {
                F[i] = x[i] - states_0[i] - timeStep * f[i];
            }
            // Compute norm of F(x)
            double normF = 0.0;
            for (double val : F) {
                normF += val * val;
            }
            normF = std::sqrt(normF);
            spdlog::get("default_pysyslink")->debug("Iteration {}: normF = {}", iter, normF);
            if (normF < tol) {
                return {true, x, timeStep};
            }
            // Compute Jacobian J(x, t_next)
            std::vector<std::vector<double>> J = systemJacobian(x, t_next);
            // Form matrix A = I - timeStep * J
            std::vector<std::vector<double>> A(n, std::vector<double>(n, 0.0));
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i == j) {
                        A[i][j] = 1.0 - timeStep * J[i][j];
                    } else {
                        A[i][j] = - timeStep * J[i][j];
                    }
                }
            }
            // Solve linear system A * delta = F
            std::vector<double> delta = this->SolveLinearSystem(A, F);
            // Update x: x = x - delta
            double normDelta = 0.0;
            for (int i = 0; i < n; i++) {
                x[i] -= delta[i];
                normDelta += delta[i] * delta[i];
            }
            normDelta = std::sqrt(normDelta);
            spdlog::get("default_pysyslink")->debug("Iteration {}: normDelta = {}", iter, normDelta);
            if (normDelta < tol) {
                return {true, x, timeStep};
            }
        }
        spdlog::get("default_pysyslink")->error("Newton-Raphson did not converge after {} iterations", maxIter);
        return {false, x, timeStep};
    }

} // namespace PySysLinkBase
