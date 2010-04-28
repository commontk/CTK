// Qt includes
#include <QStyle>

// CTK includes
#include "ctkDirectoryButton.h"

//-----------------------------------------------------------------------------
class ctkDirectoryButtonPrivate: public ctkPrivate<ctkDirectoryButton>
{
public:
  ctkDirectoryButtonPrivate();
  void init();
  QString Caption;
  QFileDialog::Options Options;
};

//-----------------------------------------------------------------------------
ctkDirectoryButtonPrivate::ctkDirectoryButtonPrivate()
{
  this->Options = QFileDialog::ShowDirsOnly;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButtonPrivate::init()
{
  CTK_P(ctkDirectoryButton);
  QObject::connect(p, SIGNAL(clicked()), p, SLOT(browse()));
}

//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(QWidget * parentWidget)
  :QPushButton(parentWidget)
{
  CTK_INIT_PRIVATE(ctkDirectoryButton);
  this->setIcon(this->style()->standardIcon(QStyle::SP_DirIcon));
  ctk_d()->init();
}
    
//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(const QString & text, QWidget * parentWidget)
  :QPushButton(text, parentWidget)
{
  CTK_INIT_PRIVATE(ctkDirectoryButton);
  this->setIcon(this->style()->standardIcon(QStyle::SP_DirIcon));
  ctk_d()->init();
}

//-----------------------------------------------------------------------------
ctkDirectoryButton::ctkDirectoryButton(
  const QIcon & icon, const QString & text, QWidget * parentWidget)
  :QPushButton(icon, text, parentWidget)
{
  CTK_INIT_PRIVATE(ctkDirectoryButton);
  ctk_d()->init();
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::setCaption(const QString& captionTitle)
{
  CTK_D(ctkDirectoryButton);
  d->Caption = captionTitle;
}

//-----------------------------------------------------------------------------
const QString& ctkDirectoryButton::caption()const
{
  CTK_D(const ctkDirectoryButton);
  return d->Caption;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::ctkDirectoryButton::setOptions(const QFileDialog::Options& dialogOptions)
{
  CTK_D(ctkDirectoryButton);
  d->Options = dialogOptions;
}

//-----------------------------------------------------------------------------
const QFileDialog::Options& ctkDirectoryButton::options()const
{
  CTK_D(const ctkDirectoryButton);
  return d->Options;
}

//-----------------------------------------------------------------------------
void ctkDirectoryButton::browse()
{
  CTK_D(ctkDirectoryButton);
  QString directory = 
    QFileDialog::getExistingDirectory(
      this, 
      d->Caption.isEmpty() ? this->toolTip() : d->Caption, 
      this->text(), 
      d->Options);
  if (directory.isEmpty())
    {
    return;
    }
  this->setText(directory);
  emit directoryChanged(directory);
}
