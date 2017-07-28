// CTK includes
#include "ctkWidgetsTestingUtilities.h"

// Qt includes
#include <QImage>

// STD includes
#include <iostream>

namespace ctkWidgetsTestingUtilities
{
//---------------------------------------------------------------------------- */
bool CheckImagesEqual(const QImage& current, const QImage& expected,
                      float percentThreshold)
{
  if (current.width() != expected.width() ||
      current.height() != expected.height() ||
      current.format() != expected.format())
    {
    return false;
    }

  if (current.format() != QImage::Format_RGB32)
    {
    std::cerr << "ERROR: CheckImagesEqual: Unsupported QImage::Format: "
              << static_cast<int>(current.format()) << std::endl;
    return false;
    }

  // Compute number of pixels that differ, masking out alpha channel.
  // Based on QImage::operator== implementation.
  unsigned long numDiffs = 0;
  for (int line = 0; line < current.height(); line++)
    {
    int w = current.width();
    const QRgb* p1 = reinterpret_cast<const QRgb*>(current.constScanLine(line));
    const QRgb* p2 = reinterpret_cast<const QRgb*>(expected.constScanLine(line));
    while (w--)
      {
      if ((*p1++ & 0x00ffffff) != (*p2++ & 0x00ffffff))
        {
        ++numDiffs;
        }
      }
    }

  const int numPixels = current.width() * current.height();
  const float percentDifferent = (numPixels > 0) ? ((100.f * numDiffs) / numPixels) : 0.f;
  return (percentDifferent <= percentThreshold);
}

} // namespace ctkWidgetsTestingUtilities

