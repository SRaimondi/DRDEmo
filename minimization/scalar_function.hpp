//
// Created by Simone Raimondi on 02.08.17.
//

#ifndef DRDEMO_SCALAR_FUNCTION_HPP
#define DRDEMO_SCALAR_FUNCTION_HPP

#include "rad.hpp"

namespace drdemo {

    // Define interface for Scalar functions to be used in the minimization algorithms
    class ScalarFunctionInterface {
        // Evaluate the function at his current status
        virtual Float Evaluate() const = 0;

        // Compute gradient of the function
        virtual std::vector<float> ComputeGradient() const = 0;

        // Update the internal state of the function
        virtual void UpdateStatus(const std::vector<float> &deltas) = 0;

        // Set internal status
        virtual void SetStatus(const std::vector<float> &new_status) = 0;
    };

    // General utility function

    // Gradient norm squared
    float GradientNorm2(const std::vector<float> &gradient);

    // Gradient norm
    float GradientNorm(const std::vector<float> &gradient);

} // drdemo namespace

#endif //DRDEMO_SCALAR_FUNCTION_HPP
