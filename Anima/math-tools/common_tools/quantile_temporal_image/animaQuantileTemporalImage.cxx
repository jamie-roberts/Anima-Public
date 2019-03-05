#include <itkImageRegionConstIterator.h>
#include <itkImageLinearConstIteratorWithIndex.h>
#include <itkImage.h>

#include <animaReadWriteFunctions.h>

#include <limits.h>
#include <iostream>
#include <fstream>
#include <tclap/CmdLine.h>

using namespace std;

int main(int argc, char **argv)
{
    TCLAP::CmdLine cmd("INRIA / IRISA - Visages Team", ' ',ANIMA_VERSION);
    
    TCLAP::ValueArg<std::string> inputArg("i","inputimage","Input image",true,"","Input image",cmd);
    TCLAP::ValueArg<std::string> outputArg("o","outputimage","Output image",true,"","Output image",cmd);
    TCLAP::ValueArg<std::string> outputArg2("O","outputimage2","Output image 2",true,"","Output image 2",cmd);
    TCLAP::ValueArg<double> quantileArg("q","quantile","quantile over temporal direction (between 0 and 1)",false,0.5,"quantile value",cmd);

    try
    {
        cmd.parse(argc,argv);
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "Error: " << e.error() << "for argument " << e.argId() << std::endl;
        return EXIT_FAILURE;
    }

    typedef itk::Image <double,4> Image4DType;
    typedef itk::Image <double,3> Image3DType;

    typedef itk::ImageLinearConstIteratorWithIndex <Image4DType> IteratorType;

    Image3DType::IndexType index3D;
    Image4DType::IndexType index4D;

    Image4DType::Pointer inputImage = anima::readImage <Image4DType> (inputArg.getValue());

    Image3DType::PointType outputOrigin;
    Image3DType::SpacingType outputSpacing;
    Image3DType::DirectionType outputDirection;
    Image3DType::SizeType outputSize;
    Image3DType::RegionType outputRegion;

    for (unsigned int d = 0; d < 3; ++d)
    {
        outputOrigin[d] = inputImage->GetOrigin()[d]; 
        outputSpacing[d] = inputImage->GetSpacing()[d]; 
        outputSize[d] = inputImage->GetLargestPossibleRegion().GetSize()[d];
        for(unsigned int i = 0; i < 3; ++i)
            outputDirection[d][i] = inputImage->GetDirection()[d][i];     
    } 
    outputRegion.SetSize(outputSize);

    Image3DType::Pointer outputImage = Image3DType::New();
    outputImage->Initialize();
    outputImage->SetOrigin(outputOrigin);
    outputImage->SetSpacing(outputSpacing);
    outputImage->SetDirection(outputDirection);
    outputImage->SetRegions(outputRegion);
    outputImage->Allocate();

    Image3DType::Pointer outputImage2 = Image3DType::New();
    outputImage2->Initialize();
    outputImage2->SetOrigin(outputOrigin);
    outputImage2->SetSpacing(outputSpacing);
    outputImage2->SetDirection(outputDirection);
    outputImage2->SetRegions(outputRegion);
    outputImage2->Allocate();

    Image4DType::RegionType tmpRegionInputImage = inputImage->GetLargestPossibleRegion();
    const unsigned int timeLength = tmpRegionInputImage.GetSize()[3];

    IteratorType it(inputImage,tmpRegionInputImage);
    it.SetDirection(3);
    it.GoToBegin();

    if ((quantileArg.getValue() < 0) || (quantileArg.getValue() > 1))
    {
        std::cerr << "Quantile value has to be included in the [0,1] interval" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector <double> tmpVec (timeLength);
    unsigned int quantileInd1 = (unsigned int)floor(quantileArg.getValue()*timeLength);
    unsigned int quantileInd2 = (unsigned int)floor((1-quantileArg.getValue())*timeLength);
    if (quantileInd1 == timeLength)
        quantileInd1 = timeLength - 1;
    if (quantileInd2 == timeLength)
        quantileInd2 = timeLength - 1;

    while( !it.IsAtEnd() )
    {
        it.GoToBeginOfLine();
        index4D = it.GetIndex();

        for (unsigned int i = 0;i < timeLength;++i)
        {
            tmpVec[i] = it.Get();
            ++it;
        }

        std::sort(tmpVec.begin(),tmpVec.end());
        double quantileV1 = tmpVec[quantileInd1];
        double quantileV2 = tmpVec[quantileInd2];
        double sum = 0;

        it.GoToBeginOfLine();

        for (unsigned int i = 0;i < timeLength;++i)
        {
            sum += std::max(std::min(it.Get(), max(quantileV1, quantileV2)), std::min(quantileV1, quantileV2)); 
            ++it;
        }
        sum /= timeLength;
        index3D[0] = index4D[0];
        index3D[1] = index4D[1];
        index3D[2] = index4D[2];

        outputImage->SetPixel( index3D, sum );
        outputImage2->SetPixel( index3D, quantileV1 );

        it.NextLine();
    }

    anima::writeImage <Image3DType> (outputArg.getValue(), outputImage);
    anima::writeImage <Image3DType> (outputArg2.getValue(), outputImage2);
    return EXIT_SUCCESS;
}
