
// STD includes
#include <iostream>
#include <cstdlib>

#ifdef WIN32
# define BFD_TEST_HELPER_EXPORT __declspec(dllexport)
#else
# define BFD_TEST_HELPER_EXPORT
#endif

extern "C" {

  BFD_TEST_HELPER_EXPORT int MtBlancElevationInMeters = 4810;
}

int main(int /*argc*/, char* /*argv*/[])
{
  std::cout << "Mt Blanc elevation is " << MtBlancElevationInMeters
            << " meters" << std::endl;
  return EXIT_SUCCESS;
}
