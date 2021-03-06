#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "animaSimuBlochSP-GRE.h"

int main(int argc, char *argv[] )
{
    TCLAP::CmdLine cmd("SimuBlochSP-GRE: Simulator for Spoiled Gradient Echo ( SPGR, FLASH, or T1-FFE )\nINRIA / IRISA - VisAGeS/Empenn Team", ' ',ANIMA_VERSION);
    
    TCLAP::ValueArg<std::string> t1MapArg("","t1","Input T1 map",true,"","T1 map",cmd);
    TCLAP::ValueArg<std::string> t2sMapArg("","t2s","Input T2* map",true,"","T2* map",cmd); //changed for SP-GRE
    TCLAP::ValueArg<std::string> m0ImageArg("","m0","Input M0 image",true,"","M0 image",cmd);
    TCLAP::ValueArg<std::string> b1ImageArg("","b1","Input B1 image",false,"","B1 image",cmd);
    TCLAP::ValueArg<std::string> resArg("o","output","Output simulated image for Spoiled Gradient Echo",true,"","output simulated image",cmd);
    
    TCLAP::ValueArg<float> trArg("r","tr","TR value (ms), default: 35ms",false, 35,"TR value",cmd);//changed for SP-GRE
    TCLAP::ValueArg<float> teArg("e","te","TE value (ms), default: 6ms",false, 6,"TE value",cmd);//changed for SP-GRE
    TCLAP::ValueArg<float> faArg("f","fa","Flip Angle (degree), default: 40degree", false, 40,"FA value",cmd);//changed for SP-GRE
    
    try
    {
        cmd.parse(argc,argv);
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "Error: " << e.error() << "for argument " << e.argId() << std::endl;
        return EXIT_FAILURE;//changed for SP-GRE
    }
    
    // Parse arguments
    std::string input1FileName = t1MapArg.getValue();
    std::string input2FileName = t2sMapArg.getValue();//changed for SP-GRE
    std::string input3FileName = m0ImageArg.getValue();
    std::string outputFileName = resArg.getValue();
    float TR = trArg.getValue();
    float TE = teArg.getValue();
    float FA = faArg.getValue();//changed for SP-GRE
    
    // Verify range of TR, TE
    if ((TR < 0) || (TE < 0))
    {
        std::cerr << "Error: " << std::endl;
        std::cerr << "TR(ms) < 0 or TE(ms)" << std::endl;
        std::cerr << "Please set TR(ms) >= 0 and TE(ms) >= 0." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Verify TE < TR
    if (TE >= TR)
    {
        std::cerr << "Error: " << std::endl;
        std::cerr << "TE(ms) >= TR(ms)" << std::endl;
        std::cerr << "Please set TE and TR values which satisfy TE(ms) < TR(ms)." << std::endl;
        return EXIT_FAILURE;
        
    }
    
    // Verify range of FA
    if ((FA > 180) || (FA < 0))
    {
        std::cerr << "Error: " << std::endl;
        std::cerr << "FA(degree) < 0 or FA(degree) > 180" << std::endl;
        std::cerr << "Please set FA(degree) in the range of [0, 180]." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Setup types
    typedef itk::Image<float, 3>   ImageType;
    typedef anima::SimuBlochSPGRE<ImageType>  FilterType;
    
    // Read T1 map
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader1 = ReaderType::New();
    reader1->SetFileName(input1FileName);
    reader1->Update();
    
    // Read T2s map
    ReaderType::Pointer reader2 = ReaderType::New();
    reader2->SetFileName(input2FileName);
    reader2->Update();
    
    // Read M0 map
    ReaderType::Pointer reader3 = ReaderType::New();
    reader3->SetFileName(input3FileName);
    reader3->Update();
    
    // Get the size of input image
    ReaderType::SizeType size1 = reader1->GetOutput()->GetLargestPossibleRegion().GetSize();
    ReaderType::SizeType size2 = reader2->GetOutput()->GetLargestPossibleRegion().GetSize();
    ReaderType::SizeType size3 = reader3->GetOutput()->GetLargestPossibleRegion().GetSize();
    
    // Verify the dimension of the input image
    if ( ((sizeof(size1)/sizeof(size1[0])) != (sizeof(size2)/sizeof(size2[0])))
        || ((sizeof(size1)/sizeof(size1[0])) != (sizeof(size3)/sizeof(size3[0]))) )
    {
        std::cerr << "Error: " << std::endl;
        std::cerr << "The dimensions of the input images are not the same." << std::endl;
        std::cerr << "Please input the images with the same dimension." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Verify the size of the input image
    if (((size1[0] != size2[0]) || (size1[0] != size3[0]))
        || ((size1[1] != size2[1]) || (size1[1] != size3[1]))
        || ((size1[2] != size2[2]) || (size1[2] != size3[2])) )
    {
        std::cerr << "Error: " << std::endl;
        std::cerr << "The sizes of the input images are not the same." << std::endl;
        std::cerr << "Please input the images with the same size." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Create the filter: estimate the sequence of SP-GRE
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(0,reader1->GetOutput());
    filter->SetInput(1,reader2->GetOutput());
    filter->SetInput(2,reader3->GetOutput());
    
    // Read B1 map
    if (b1ImageArg.getValue() != "")
    {
        ReaderType::Pointer readerB1 = ReaderType::New();
        readerB1->SetFileName(b1ImageArg.getValue().c_str());
        readerB1->Update();
        
        filter->SetInput(3,readerB1->GetOutput());
    }

    filter->SetTR(TR);
    filter->SetTE(TE);
    filter->SetFA(FA);//changed for SP-GRE
    filter->Update();
    
    // Save the output image
    typedef  itk::ImageFileWriter< ImageType  > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFileName);
    writer->SetInput(filter->GetOutput());
    writer->Update();
    
    // return
    return EXIT_SUCCESS;
}

