#pragma once

#include <itkImageToImageFilter.h>

namespace anima
{

template< class TImage>
class SimuBlochIRGRE:public itk::ImageToImageFilter< TImage, TImage >
{
public:
    /** Standard class typedefs. */
    typedef SimuBlochIRGRE Self;
    typedef itk::ImageToImageFilter <TImage, TImage> Superclass;
    typedef itk::SmartPointer <Self> Pointer;

    typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Run-time type information (and related methods). */
    itkTypeMacro(SimuBlochIRGRE, ImageToImageFilter)

    itkSetMacro(TR, float)
    itkGetMacro(TR, float)

    itkSetMacro(TE, float)
    itkGetMacro(TE, float)

    itkSetMacro(TI, float)
    itkGetMacro(TI, float)

    /** T1 map */
    void SetInputT1(const TImage* T1);

    /** T2s map */
    void SetInputT2s(const TImage* T2s);

    /** M0 image / Rho map */
    void SetInputM0(const TImage* M0);

protected:
    SimuBlochIRGRE();
    virtual ~SimuBlochIRGRE() {}

    /** Does the real work. */
    virtual void DynamicThreadedGenerateData(const OutputImageRegionType &outputRegionForThread) ITK_OVERRIDE;

private:
    ITK_DISALLOW_COPY_AND_ASSIGN(SimuBlochIRGRE);

    float m_TR;
    float m_TE;
    float m_TI;
};

} // end of namespace anima

#include "animaSimuBlochIR-GRE.hxx"
