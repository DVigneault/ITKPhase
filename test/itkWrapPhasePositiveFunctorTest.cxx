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

#include "itkWrapPhasePositiveFunctor.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"
#include <iostream>

static bool different(double a, double b)
{
return std::fabs(a-b) > 10e-6;
}

int itkWrapPhasePositiveFunctorTest(int argc, char **argv)
{

  if (argc != 1)
    {
    std::cerr << "Usage: " << argv[0] << std::endl;
    return EXIT_FAILURE;
    }

  itk::Functor::WrapPhasePositiveFunctor< double > wrapFunc;
  
  if (different( wrapFunc( 3.0 ), 3.0) )
    {
    std::cerr << "Incorrect wrapping of 3.0." << std::endl;
    return EXIT_FAILURE;
    }

  if (different(wrapFunc( 0.0 ), 0.0) )
    {
    std::cerr << "Incorrect wrapping of 0.0." << std::endl;
    return EXIT_FAILURE;
    }

  if (different(wrapFunc( -3.0 ), -3.0 + 2*vnl_math::pi ) )
    {
    std::cerr << "Incorrect wrapping of -3.0." << std::endl;
    return EXIT_FAILURE;
    }

  if (different(wrapFunc( 1.0 + vnl_math::pi ), 1.0 + vnl_math::pi ) )
    {
    std::cerr << "Incorrect wrapping of 1.0 + pi." << std::endl;
    return EXIT_FAILURE;
    }

  if (different(wrapFunc( -vnl_math::pi - 1.0 ), ( +vnl_math::pi - 1.0 ) ) )
    {
    std::cerr << "Incorrect wrapping of -pi - 1.0." << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}
