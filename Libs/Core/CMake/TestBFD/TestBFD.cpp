
#include <bfd.h>

// STD includes
#include <cstdlib>

int main(int /*argc*/, char * /*argv*/[])
{
  bfd *abfd = 0;
  asymbol *symbol = 0;
  asection *p = 0;

  bfd_init();
  
  return EXIT_SUCCESS;
}
