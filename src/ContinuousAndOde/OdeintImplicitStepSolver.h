#ifndef SRC_CONTINUOUS_AND_ODE_ODEINT_IMPLICIT_STEP_SOLVER
#define SRC_CONTINUOUS_AND_ODE_ODEINT_IMPLICIT_STEP_SOLVER


#include <tuple>
#include <vector>
#include <functional>
#include "IOdeStepSolver.h"
#include <boost/numeric/odeint.hpp>
#include <utility>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace PySysLinkBase
{
    template <typename T> 
    class OdeintImplicitStepSolver : public IOdeStepSolver
    {
        private:
            std::shared_ptr<T> controlledStepper;

            // Convert std::vector<double> to ublas::vector<double>
            static boost::numeric::ublas::vector<double> stdToUblas(const std::vector<double>& v) {
                boost::numeric::ublas::vector<double> u(v.size());
                for (size_t i = 0; i < v.size(); ++i) {
                    u(i) = v[i];
                }
                return u;
            }

            // Convert ublas::vector<double> to std::vector<double>
            static std::vector<double> ublasToStd(const boost::numeric::ublas::vector<double>& u) {
                std::vector<double> v(u.size());
                for (size_t i = 0; i < u.size(); ++i) {
                    v[i] = u(i);
                }
                return v;
            }

            boost::numeric::ublas::matrix<double> stdToUblasMatrix(const std::vector<std::vector<double>>& mat) {
                size_t rows = mat.size();
                size_t cols = (rows > 0 ? mat[0].size() : 0);
                boost::numeric::ublas::matrix<double> m(rows, cols);
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        m(i, j) = mat[i][j];
                    }
                }
                return m;
            }

        public:
            OdeintImplicitStepSolver(std::shared_ptr<T> controlledStepper)
            {
                this->controlledStepper = controlledStepper;
            }

            virtual bool IsJacobianNeeded() const 
            {
                return true;
            }

            virtual std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep)
            {
                throw std::runtime_error("Jacobian needed for implicit odeint method");
            }


            

            std::tuple<bool, std::vector<double>, double> SolveStep(std::function<std::vector<double>(std::vector<double>, double)> system, 
                                                                    std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobian, 
                                                                    std::vector<double> states_0, double currentTime, double timeStep)
            {
                auto systemFunction = [&](const boost::numeric::ublas::vector<double>& x_ublas, boost::numeric::ublas::vector<double>& dxdt_ublas, double t) -> void {
                    std::vector<double> x_std = OdeintImplicitStepSolver::ublasToStd(x_ublas);
                    std::vector<double> dxdt_std = system(x_std, t);
                    if(dxdt_std.size() != static_cast<size_t>(dxdt_ublas.size())) {
                        dxdt_ublas = boost::numeric::ublas::vector<double>(dxdt_std.size());
                    }
                    for (size_t i = 0; i < dxdt_std.size(); ++i) {
                        dxdt_ublas(i) = dxdt_std[i];
                    }
                };
                
                auto systemJacobianFunction = [&](const boost::numeric::ublas::vector<double>& x,
                    boost::numeric::ublas::matrix<double>& J,
                    double t,
                    boost::numeric::ublas::vector<double>& dfdt) -> void {
                    std::vector<double> x_std = OdeintImplicitStepSolver::ublasToStd(x);
                    std::vector<std::vector<double>> J_std = systemJacobian(x_std, t);
                    // Here you need to compute dfdt (the time derivative of f) or set it to zero if appropriate.
                    // For example, if dfdt is not provided by your original function, you might choose:
                    dfdt = boost::numeric::ublas::vector<double>(x_std.size(), 0.0);
                    J = OdeintImplicitStepSolver::stdToUblasMatrix(J_std);
                };
                
                typedef boost::numeric::ublas::vector<double> state_type;

                // Assuming state_type is boost::numeric::ublas::vector<double>
                std::function<void(const state_type&, state_type&, double)> boundSystem = systemFunction;
                std::function<void(const state_type&, boost::numeric::ublas::matrix<double>&, double, state_type&)> boundJacobian = systemJacobianFunction;

                // Create the stepper
                // Stepper stepper;

                // Integrate a single step
                std::vector<double> newStates = states_0; // Initial state
                double dt = timeStep;

                boost::numeric::ublas::vector<double> newStates_ublas(newStates.size());
                for (std::size_t i = 0; i < newStates.size(); ++i) {
                    newStates_ublas(i) = newStates[i];
                }

                boost::numeric::odeint::controlled_step_result result = this->controlledStepper->try_step(std::make_pair( boundSystem, boundJacobian ), newStates_ublas, currentTime, dt);
                
                newStates = OdeintImplicitStepSolver::ublasToStd(newStates_ublas);

                system(states_0, currentTime); // Set initial states again, may be optimized

                // Debug log output
                if (result == boost::numeric::odeint::success)
                {
                    return {true, newStates, dt};
                }
                else
                {
                    return {false, newStates, dt};
                }
            }
    };
} // namespace PySysLinkBase

#endif /* SRC_CONTINUOUS_AND_ODE_ODEINT_IMPLICIT_STEP_SOLVER */
