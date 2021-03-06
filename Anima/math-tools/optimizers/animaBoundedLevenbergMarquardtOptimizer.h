#pragma once

#include <itkMultipleValuedNonLinearOptimizer.h>

#include <animaCholeskyDecomposition.h>
#include "AnimaOptimizersExport.h"

namespace anima
{

/**
 * @class BoundedLevenbergMarquardtOptimizer
 * @brief Levenberg-Marquardt optimizer with lower and upper bounds on parameters
 * Implementation of the original algorithmm, very well described in
 * K. Madsen, H.B. Nielsen and O. Tingleff. Methods for non-linear least squares problems. 2004
 * http://www2.imm.dtu.dk/pubdb/views/edoc_download.php/3215/pdf/imm3215.pdf
 * Bounded version by projection as suggested by Kanzow et al.
 * C. Kanzow, N. Yamashita and M. Fukushima. Levenberg-Marquardt methods with strong local convergence
 * properties for solving nonlinear equations with convex constraints. Journal of computational and
 * applied mathematics. 172:375-397, 2004.
 */
class ANIMAOPTIMIZERS_EXPORT BoundedLevenbergMarquardtOptimizer:
        public itk::MultipleValuedNonLinearOptimizer
{
public:
    /** Standard class typedefs. */
    typedef BoundedLevenbergMarquardtOptimizer Self;
    typedef itk::MultipleValuedNonLinearOptimizer Superclass;
    typedef itk::SmartPointer <Self> Pointer;
    typedef itk::SmartPointer <const Self> ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    typedef Superclass::CostFunctionType CostFunctionType;
    typedef Superclass::MeasureType MeasureType;

    /** Run-time type information (and related methods). */
    itkTypeMacro(BoundedLevenbergMarquardtOptimizer, MultipleValuedNonLinearOptimizer)

    /** Start optimization */
    void StartOptimization() ITK_OVERRIDE;

    itkSetMacro(NumberOfIterations, unsigned int)
    itkSetMacro(ValueTolerance, double)
    itkSetMacro(GradientTolerance, double)
    itkSetMacro(LambdaParameter, double)

    itkGetMacro(CurrentValue, double)

    itkSetMacro(LowerBounds, ParametersType)
    itkSetMacro(UpperBounds, ParametersType)

protected:
    BoundedLevenbergMarquardtOptimizer()
    {
        m_NumberOfIterations = 2000;
        m_ValueTolerance = 1e-8;
        m_GradientTolerance = 1e-5;
        m_LambdaParameter = 1.0e-8;

        m_CurrentValue = 0.0;
    }

    virtual ~BoundedLevenbergMarquardtOptimizer() ITK_OVERRIDE {}

    void GetDerivativeSquared(DerivativeType &derivativeMatrix, DerivativeType &derivativeSquared);

    double EvaluateCostFunctionAtParameters(ParametersType &parameters, ParametersType &scaledParameters,
                                            MeasureType &residualValues);

    bool CheckConditions(unsigned int numIterations, ParametersType &lastTestedParams,
                         ParametersType &newParams, DerivativeType &newDerivative);

private:
    ITK_DISALLOW_COPY_AND_ASSIGN(BoundedLevenbergMarquardtOptimizer);

    unsigned int m_NumberOfIterations;
    double m_ValueTolerance;
    double m_GradientTolerance;

    double m_LambdaParameter;
    double m_CurrentValue;

    anima::CholeskyDecomposition m_CholeskySolver;
    ParametersType m_LowerBounds, m_WorkLowerBounds;
    ParametersType m_UpperBounds, m_WorkUpperBounds;
    MeasureType m_ResidualValues;
};

} // end namespace anima
