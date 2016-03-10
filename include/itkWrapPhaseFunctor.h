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
#ifndef itkWrapPhaseFunctor_h
#define itkWrapPhaseFunctor_h

#include <vnl/vnl_math.h>
#include <math.h>

namespace itk
{
namespace Functor
{
/** \class WrapPhaseFunctor
 *  \ingroup ITKPhase
 *  \brief Unary functor that wraps the input value into the range -pi to pi.
 *
 * This functor is used by WrapPhaseImageFilter as well as filters that inherit
 * from PhaseImageToImageFilter.
 *
 */


template< typename TInputPixel, typename TOutputPixel = TInputPixel >
class WrapPhaseFunctor
{
public:
  WrapPhaseFunctor() {}
  ~WrapPhaseFunctor() {}
  
  bool operator!=(const WrapPhaseFunctor &) const
  {
    return false;
  }

  bool operator==(const WrapPhaseFunctor & other) const
  {
    return !( *this != other );
  }

  inline TOutputPixel operator()(const TInputPixel & pix) const
  {
    return static_cast<TOutputPixel>(vnl_math::angle_minuspi_to_pi(static_cast<double>(pix)));
  }
  
};


}  // end namespace functor
}  // end namespace itk

#endif
