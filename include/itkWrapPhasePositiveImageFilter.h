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

#ifndef itkWrapPhasePositiveImageFilter_h
#define itkWrapPhasePositiveImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkWrapPhasePositiveFunctor.h"

namespace itk
{

/** \class WrapPhasePositiveImageFilter
 *  \ingroup ITKPhase
 *  \brief Image filter that wraps the input value into the range [0 to 2pi).
 *
 * This image filter applies the itk::WrapPhasePositiveFunctor pixelwise
 * across the image.
 *
 */
template< class TInputImage, class TOutputImage = TInputImage >
class WrapPhasePositiveImageFilter:
  public
  UnaryFunctorImageFilter< TInputImage,
                           TOutputImage,
                           Functor::WrapPhasePositiveFunctor<
                             typename TInputImage::PixelType,
                             typename TOutputImage::PixelType > >
{
public:
  /** Standard class typedefs. */
  typedef WrapPhasePositiveImageFilter                                   Self;
  typedef UnaryFunctorImageFilter< TInputImage,
                                   TOutputImage,
                                   Functor::WrapPhasePositiveFunctor<
                                     typename TInputImage::PixelType,
                                     typename TOutputImage::PixelType > > Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(WrapPhasePositiveImageFilter,
               UnaryFunctorImageFilter);

  #ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro( SameDimensionCheck,
                   ( Concept::SameDimension< TInputImage::ImageDimension, TOutputImage::ImageDimension > ) );
  
  itkConceptMacro( InputFloatingPointCheck,
                   ( Concept::IsFloatingPoint< typename TInputImage::PixelType > ) );
                   
  itkConceptMacro( OutputFloatingPointCheck,
                   ( Concept::IsFloatingPoint< typename TOutputImage::PixelType > ) );
  // End concept checking
  #endif

protected:

  WrapPhasePositiveImageFilter() {}
  virtual ~WrapPhasePositiveImageFilter() {}

private:

  ITK_DISALLOW_COPY_AND_ASSIGN(WrapPhasePositiveImageFilter);

};
} // end namespace itk

#endif
