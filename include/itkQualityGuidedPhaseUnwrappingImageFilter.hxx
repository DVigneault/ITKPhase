/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkQualityGuidedPhaseUnwrappingImageFilter_hxx
#define itkQualityGuidedPhaseUnwrappingImageFilter_hxx

#include "itkQualityGuidedPhaseUnwrappingImageFilter.h"
 
/** Standard headers */
#include <set>
#include <vector>

/** VNL headers */
#include "vnl/vnl_math.h"

/** ITK headers */
#include "itkImageAlgorithm.h"
#include "itkProgressReporter.h"

/** Contributed headers */
#include "itkIndexValuePair.h"

namespace itk {

template< typename TInputImage, typename TOutputImage >
QualityGuidedPhaseUnwrappingImageFilter< TInputImage, TOutputImage >
::QualityGuidedPhaseUnwrappingImageFilter()
{

  /** There are two required inputs for this filter: the phase and the quality. */
  this->SetNumberOfRequiredInputs(2);

}

template< class TInputImage, class TOutputImage>
void
QualityGuidedPhaseUnwrappingImageFilter<TInputImage, TOutputImage>
::SetPhaseImage(const TInputImage* image)
{
  this->SetNthInput(0, const_cast<TInputImage*>(image));
}
 
template< class TInputImage, class TOutputImage>
void
QualityGuidedPhaseUnwrappingImageFilter<TInputImage, TOutputImage>
::SetQualityImage(const TInputImage* image)
{
  this->SetNthInput(1, const_cast<TInputImage*>(image));
}

template< typename TInputImage, typename TOutputImage >
void
QualityGuidedPhaseUnwrappingImageFilter< TInputImage, TOutputImage >
::GenerateData()
{

  /**
   *
   * Allocate input/output and temporary images.
   * Input 1: Wrapped phase image.
   * Input 2: Phase quality image.
   * Output: Unwrapped phase image.
   * Temporary: Binary unwrapped image (indicates which pixels have been unwrapped).
   *
   */

  /** Create input/output images.  */
  typename TInputImage::ConstPointer input = this->GetInput(0); // wrapped phase
  typename TInputImage::ConstPointer quality = this->GetInput(1); // phase quality
  typename TOutputImage::Pointer unwrapped = this->GetOutput();

  this->AllocateOutputs();
 
  /** Fill unwrapped data with input. */
  ImageAlgorithm::Copy(input.GetPointer(),
                       unwrapped.GetPointer(),
                       unwrapped->GetLargestPossibleRegion(),
                       unwrapped->GetLargestPossibleRegion() );

  /** Create a binary image to keep track of pixels that need to be unwrapped. */
  typename TBinaryImage::Pointer binaryUnwrapped = TBinaryImage::New();
  binaryUnwrapped->SetRegions( input->GetLargestPossibleRegion() );
  binaryUnwrapped->Allocate();
  binaryUnwrapped->FillBuffer( false );
 
  /**
   *
   * Create neighborhood iterators for the unwrapped, binary, and quality images.
   * Set the initial location of the iterators to m_TruePhase.
   * Set the center pixel to be "unwrapped."
   *
   */

  typename WorkNItType::RadiusType radius;
  radius.Fill( 1 );

  WorkNItType   outIt(       radius, unwrapped,       unwrapped->GetLargestPossibleRegion() );
  BinaryNItType binUnwrapIt( radius, binaryUnwrapped, unwrapped->GetLargestPossibleRegion() );
  WorkCNItType  qualIt(      radius, quality,         unwrapped->GetLargestPossibleRegion() );

  outIt.SetLocation( m_TruePhase );
  binUnwrapIt.SetLocation( m_TruePhase );
  qualIt.SetLocation( m_TruePhase );

  binUnwrapIt.SetCenterPixel( true );

  typedef std::vector< typename WorkNItType::SizeValueType > OffsetVectorType;
  OffsetVectorType offsetVector;
  typename WorkNItType::SizeValueType center = outIt.Size() / 2;

  for (unsigned int i = 0; i < TInputImage::ImageDimension; ++i)
    {
    offsetVector.push_back( center - outIt.GetStride( i ) );
    offsetVector.push_back( center + outIt.GetStride( i ) );
    }
 
  /**
   *
   * Create a list to keep track of those pixels which are adjoining and their quality.
   * 
   * The data type used here is the "IndexValuePair" templated itk class.
   * This holds the index and value of a pixel.
   * Pair1 == Pair2 if Pair1.index == Pair2.index
   * Pair1 < Pair2 if Pair1.value < Pair2.value
   * 
   * This allows pairs to be stored inside a std::set, with two important consequences.
   *
   * First, no two pixels with the same index can be present.  If you try to add
   * a pixel that already exists, it will have no effect.
   *
   * Second, the pairs will be ordered according to the quality.  Therefore, the first
   * element in the set will always the the pixel with the lowest phase derivative
   * variance, aka highest quality.
   *
   */
  
  typedef IndexValuePair< IndexType, PixelType > PairType;
  std::set< PairType > AdjoiningPixels;
  
  for (typename OffsetVectorType::iterator it = offsetVector.begin();
       it != offsetVector.end();
       ++it)
    {

    if ( input->GetLargestPossibleRegion().IsInside( qualIt.GetIndex( *it ) ) )
      {
  
      PairType pixel;
      pixel.SetIndex(qualIt.GetIndex( *it ));
      pixel.SetValue(qualIt.GetPixel( *it ));
      AdjoiningPixels.insert( pixel );

      }

    }

  // Valid region; use validRegion.IsInside( pixelIndex )
  
  // Setup progress reporter
  ProgressReporter progress(this, 0, input->GetLargestPossibleRegion().GetNumberOfPixels(), 100);
  
  while ( !AdjoiningPixels.empty() )
    {
  
    // Find the active index
    // The active index is the adjoining pixel with the highest quality (lowest derivative variance)
    // Because the indices are stored in a set ordered according to value,
    // the first element in the set is the highest quality.
    // The first time through the loop, it should be one pixel away from m_TruePhase
    
    typename TInputImage::IndexType activeIndex = (--AdjoiningPixels.end())->GetIndex();
    
    // Move the iterators to the active index
    outIt.SetLocation( activeIndex );
    binUnwrapIt.SetLocation( activeIndex );
    qualIt.SetLocation( activeIndex );
    
    // Find the adjoining offset (an adjoining pixel with unwrapped phase)
    typename WorkNItType::SizeValueType adjoiningIndex(0);
    
    for (typename OffsetVectorType::iterator it = offsetVector.begin();
         it != offsetVector.end();
         ++it)
      {

      if (binUnwrapIt.GetPixel( *it ) )
        {
        adjoiningIndex = *it;
        break;
        }
      }

    // Unwrap active pixel and label it as unwrapped in the binary image
    typename TInputImage::PixelType unwrappedValue = outIt.GetCenterPixel();
    typename TInputImage::PixelType adjoiningValue = outIt.GetPixel( adjoiningIndex );
    
    // unwrap unwrappedValue relative to adjoiningValue
    unwrappedValue = this->Unwrap( unwrappedValue, adjoiningValue );
    
    outIt.SetCenterPixel( unwrappedValue );
    binUnwrapIt.SetCenterPixel( true );
    progress.CompletedPixel();
    
    AdjoiningPixels.erase( --AdjoiningPixels.end() );
    
    for (typename OffsetVectorType::iterator it = offsetVector.begin();
         it != offsetVector.end();
         ++it)
      {
      if (!binUnwrapIt.GetPixel( *it ) &&
          input->GetLargestPossibleRegion().IsInside( qualIt.GetIndex( *it ) ) )
        {
    
        PairType pixel;
        pixel.SetIndex(qualIt.GetIndex( *it ));
        pixel.SetValue(qualIt.GetPixel( *it ));
        AdjoiningPixels.insert( pixel );

        }

      }
  
    }
 
} // end GenerateData()

template < typename TInputImage, typename TOutputImage > 
void 
QualityGuidedPhaseUnwrappingImageFilter< TInputImage, TOutputImage >
::PrintSelf( std::ostream& os, Indent indent ) const 
{

  Superclass::PrintSelf(os,indent); 

  os << indent << "TruePhase: " << this->m_TruePhase << std::endl;
  
}

} // end namespace itk
 
#endif
