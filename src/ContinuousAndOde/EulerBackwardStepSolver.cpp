#include "EulerBackwardStepSolver.h"
#include <spdlog/spdlog.h>
#include <Eigen/Dense>

namespace PySysLinkBase
{
    std::tuple<bool, std::vector<double>, double> EulerBackwardStepSolver::SolveStep(std::function<std::vector<double>(std::vector<double>, double)> systemDerivatives,
                                                                                std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobian,
                                                                                std::vector<double> states_0, double currentTime, double timeStep)
    {
        std::vector<double> statesEnd = states_0;

        for (int i = 0; i < this->maximumIterations; i++)
        {
            std::vector<double> statesEndOld = statesEnd;
            std::vector<double> systemDerivativesEnd = systemDerivatives(statesEnd, currentTime + timeStep);
            std::vector<std::vector<double>> systemJacobianEnd = systemJacobian(statesEnd, currentTime + timeStep);

       
            std::vector<double> delta = this->ComputeNewtonStep(systemJacobianEnd, systemDerivativesEnd, states_0, statesEnd, timeStep);

            for (size_t j = 0; j < statesEnd.size(); j++) {
                statesEnd[j] += delta[j];
            }

            // Check convergence
            double maxError = 0.0;
            for (size_t j = 0; j < statesEnd.size(); j++)
            {
                maxError = std::max(maxError, std::abs(statesEnd[j] - statesEndOld[j]));
            }
            
            if (maxError < this->tolerance)
            {
                systemDerivatives(states_0, currentTime);
                return {true, statesEnd, timeStep};
            }
        }

        systemDerivatives(states_0, currentTime);
        return {true, statesEnd, timeStep};
    }

    std::vector<double> EulerBackwardStepSolver::ComputeNewtonStep(const std::vector<std::vector<double>>& systemJacobianEnd,
        const std::vector<double>& systemDerivativesEnd, const std::vector<double>& states_0, const std::vector<double>& statesEnd,
         double timeStep) 
    {
        int rows = systemJacobianEnd.size();
        if (rows == 0 || systemJacobianEnd[0].size() != rows) {
            throw std::runtime_error("Jacobian must be a non-empty square matrix.");
        }

        Eigen::MatrixXd eigenJacobian(rows, rows);
        for (int i = 0; i < rows; i++) {
            if (systemJacobianEnd[i].size() != rows) {
                throw std::runtime_error("Jacobian must be a square matrix.");
            }
            for (int j = 0; j < rows; j++) {
                eigenJacobian(i, j) = systemJacobianEnd[i][j];
            }
        }

        Eigen::VectorXd eigenDerivatives(rows);
        for (int i = 0; i < rows; i++) {
            eigenDerivatives(i) = systemDerivativesEnd[i];
        }

        Eigen::VectorXd eigenStatesEnd(rows);
        for (int i = 0; i < rows; i++) {
            eigenStatesEnd(i) = statesEnd[i];
        }
        Eigen::VectorXd eigenStates_0(rows);
        for (int i = 0; i < rows; i++) {
            eigenStates_0(i) = states_0[i];
        }

        Eigen::VectorXd F = eigenStatesEnd - eigenStates_0 - timeStep * eigenDerivatives;
        Eigen::MatrixXd dF = Eigen::MatrixXd::Identity(rows, rows) - timeStep * eigenJacobian;

        Eigen::VectorXd delta = -dF.inverse() * F;

        // Convert the result back to std::vector<double>
        std::vector<double> deltaStd(delta.data(), delta.data() + delta.size());
        return deltaStd;
    }

} // namespace PySysLinkBase
