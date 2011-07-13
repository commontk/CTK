
#include <bfd.h>

// STD includes
#include <cstdlib>

int main(int /*argc*/, char * /*argv*/[])
{
  bfd *abfd = 0;
  asymbol *symbol = 0;
  asection *p = 0;
  bfd_init();
  abfd = bfd_openr("/path/to/library", 0);
  if (!abfd)
    {
    return false;
    }
  return EXIT_SUCCESS;
}
