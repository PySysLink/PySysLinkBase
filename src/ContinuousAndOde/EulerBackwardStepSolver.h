#ifndef SRC_EULER_BACKWARD_STEP_SOLVER
#define SRC_EULER_BACKWARD_STEP_SOLVER


#include <tuple>
#include <vector>
#include <functional>
#include <sstream>
#include "IOdeStepSolver.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

namespace PySysLinkBase
{
    class EulerBackwardStepSolver : public IOdeStepSolver
    {
        public:
            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep)
            {
                throw std::runtime_error("Jacobian needed for implicit Euler method");
            }
            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> systemDerivatives,
                                                                    std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobian, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep);
            virtual bool IsJacobianNeeded() const 
            {
                return true;
            }

            // A simple solver for a linear system using Gaussian elimination
            std::vector<double> SolveLinearSystem(const std::vector<std::vector<double>>& A, const std::vector<double>& b)
            {
                int n = A.size();
                std::vector<std::vector<double>> M = A; // copy of A
                std::vector<double> x = b;              // copy of b (will hold modified RHS)

                // Gaussian elimination with partial pivoting
                for (int i = 0; i < n; i++) {
                    // find pivot
                    int pivot = i;
                    double maxVal = std::fabs(M[i][i]);
                    for (int j = i + 1; j < n; j++) {
                        if (std::fabs(M[j][i]) > maxVal) {
                            maxVal = std::fabs(M[j][i]);
                            pivot = j;
                        }
                    }
                    if (std::fabs(M[pivot][i]) < 1e-12) {
                        std::ostringstream oss;
                        oss << "Matrix is singular at row " << i;
                        throw std::runtime_error(oss.str());
                    }
                    if (pivot != i) {
                        std::swap(M[i], M[pivot]);
                        std::swap(x[i], x[pivot]);
                    }
                    // eliminate column i
                    for (int j = i + 1; j < n; j++) {
                        double factor = M[j][i] / M[i][i];
                        for (int k = i; k < n; k++) {
                            M[j][k] -= factor * M[i][k];
                        }
                        x[j] -= factor * x[i];
                    }
                }
                // Back substitution
                std::vector<double> solution(n, 0.0);
                for (int i = n - 1; i >= 0; i--) {
                    double sum = 0.0;
                    for (int j = i + 1; j < n; j++) {
                        sum += M[i][j] * solution[j];
                    }
                    solution[i] = (x[i] - sum) / M[i][i];
                }
                return solution;
            }
    };
} // namespace PySysLinkBase

#endif /* SRC_EULER_FORWARD_STEP_SOLVER */
