#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dataCtrl.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDoubleValidator>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actLoadImage, SIGNAL(triggered()), this, SLOT(doLoadImage()));
  connect(this, SIGNAL(onLoadWorkImage(QImage)), ui->imageView, SLOT(doChangeImage(QImage)));
  connect(ui->actCloseImage, SIGNAL(triggered()), ui->imageView, SLOT(doCloseImage()));
  connect(ui->actModeEdit, SIGNAL(triggered(bool)), this, SLOT(doChangeMode(bool)));
  connect(ui->actModeView, SIGNAL(triggered(bool)), this, SLOT(doChangeMode(bool)));
  connect(ui->actResetView, SIGNAL(triggered()), ui->imageView, SLOT(doResetView()));
  connect(ui->actNew, SIGNAL(triggered()), this, SLOT(doNew()));
  connect(ui->actSave, SIGNAL(triggered()), this, SLOT(doSave()));
  connect(ui->actSaveAs, SIGNAL(triggered()), this, SLOT(doSaveAs()));
  connect(ui->actOpen, SIGNAL(triggered()), this, SLOT(doOpen()));
  connect(&ui->imageView->data(), SIGNAL(countChanged(int)), this, SLOT(doCellCountChanged(int)));
  connect(ui->actExport, SIGNAL(triggered()), this, SLOT(doExport()));
  connect(ui->actAbout, SIGNAL(triggered()), this, SLOT(doAbout()));

  ui->actModeView->blockSignals(true);
  ui->actModeView->trigger();
  ui->actModeView->blockSignals(false);
  lastModeAction = ui->actModeView;

  cellsLabel = new QLabel("");
  cellsLabel->setAlignment(Qt::AlignLeft);
  cellsLabel->setMinimumSize(cellsLabel->sizeHint());
  doCellCountChanged(0);
  statusBar()->addWidget(cellsLabel);

  QDoubleValidator *HeightValidator = new QDoubleValidator(ui->toolBar);
  HeightValidator->setDecimals(3);
  HeightValidator->setBottom(0.);
  QDoubleValidator *WidthValidator = new QDoubleValidator(ui->toolBar);
  WidthValidator->setDecimals(3);
  WidthValidator->setBottom(0.);

  ui->toolBar->addWidget(new QLabel("width:", ui->toolBar));
  imageWidth = new QLineEdit(ui->toolBar);
  imageWidth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  imageWidth->setValidator(WidthValidator);
  ui->toolBar->addWidget(imageWidth);
  ui->toolBar->addSeparator();

  ui->toolBar->addWidget(new QLabel("height:", ui->toolBar));
  imageHeight = new QLineEdit(ui->toolBar);
  imageHeight->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  imageHeight->setValidator(HeightValidator);
  ui->toolBar->addWidget(imageHeight);

  QLabel *Spacer = new QLabel("", ui->toolBar);
  Spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  ui->toolBar->addWidget(Spacer);

  connect(imageHeight,  SIGNAL(editingFinished()), this, SLOT(setImageHeight()));
  connect(imageWidth,   SIGNAL(editingFinished()), this, SLOT(setImageWidth()));
  connect(imageHeight,  SIGNAL(textChanged(QString)), this, SLOT(setImageHeight()));
  connect(imageWidth,   SIGNAL(textChanged(QString)), this, SLOT(setImageWidth()));

  imageWidth->setText(QString::number(1.));
  setImageWidth();
  ui->imageView->setFocus();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type())
  {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

bool MainWindow::askForUnsavedChanges(const QString &title)
{
  return (ui->imageView->data().isSaved() ||
          QMessageBox::question(this,
                                title,
                                tr("Le travail en cours comporte des modification non sauvegardées.\n"
                                   "Etes-vous sûr de vouloir continuer?"),
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes);
}

void MainWindow::doLoadImage()
{
  imageName.clear();
  QString Filename(QFileDialog::getOpenFileName(this, tr("Choisir une image"), tr("."), tr("Images (*.png *.jpg *.jpeg *.tif *.tiff);;Tous (*)")));
  if (Filename.isEmpty()) return;
  if (!QFileInfo(Filename).isReadable())
  {
    QMessageBox::warning(this, tr("Erreur"), tr("Impossible de lire le fichier."));
    return;
  }
  QImage Image(Filename);
  if (Image.isNull())
  {
    QMessageBox::warning(this, tr("Erreur"), tr("Format d'image incorrect."));
    return;
  }

  imageName = QFileInfo(Filename).baseName();

  emit onLoadWorkImage(Image);

  imageWidth->setText(QString::number(1.));
  setImageWidth();
  ui->imageView->setFocus();
}

void MainWindow::doChangeMode(bool activated)
{
  QAction *action = (QAction*)sender();
  if (activated)
  {
    if (lastModeAction)
    {
      lastModeAction->blockSignals(true);
      lastModeAction->trigger();
      lastModeAction->blockSignals(false);
    }
    if (action == ui->actModeView)
      ui->imageView->changeMode(DataCtrl::eModeView);
    if (action == ui->actModeEdit)
      ui->imageView->changeMode(DataCtrl::eModeEdit);
    lastModeAction = action;
  }
  else
  {
    action->blockSignals(true);
    action->trigger();
    action->blockSignals(false);
  }
}

void MainWindow::doNew()
{
  if (askForUnsavedChanges(tr("Nouveau document")))
  {
    ui->imageView->data().clear();
    fileName.clear();
  }
}

void MainWindow::doSave()
{
  if (fileName.isEmpty())
    doSaveAs();
  else
    ui->imageView->data().save(fileName);
}

void MainWindow::doSaveAs()
{
  QFileDialog FileDialog(this, tr("Enregistrer sous"), tr("."), tr("Documents (*.xml);;Tous (*)"));
  FileDialog.setAcceptMode(QFileDialog::AcceptSave);
  FileDialog.setFileMode(QFileDialog::AnyFile);
  FileDialog.selectFile(getDefaultFilename().append(".xml"));
  if (FileDialog.exec() == QDialog::Accepted)
  {
    const QString Filename(FileDialog.selectedFiles().at(0));
    if (!Filename.isEmpty())
    {
      fileName = Filename;
      doSave();
    }
  }
}

void MainWindow::doOpen()
{
  if (askForUnsavedChanges(tr("Ouvrir un document")))
  {
    const QString Filename(QFileDialog::getOpenFileName(this, tr("Ouvrir un document"), tr("."), tr("Documents (*.xml);;Tous (*)")));
    if (!Filename.isEmpty())
    {
      fileName = Filename;
      ui->imageView->data().load(fileName);
    }
  }
}

void MainWindow::doCellCountChanged(int count)
{
  cellsLabel->setText(QString("%1 point%2").arg(count).arg(count>1?"s":""));
}

void MainWindow::doExport()
{
  QFileDialog FileDialog(this, tr("Exporter sous"), tr("."), tr("Fichiers csv (*.csv)"));
  FileDialog.setAcceptMode(QFileDialog::AcceptSave);
  FileDialog.setFileMode(QFileDialog::AnyFile);
  FileDialog.selectFile(getDefaultFilename().append(".csv"));
  if (FileDialog.exec() == QDialog::Accepted)
  {
    const QString Filename(FileDialog.selectedFiles().at(0));

    if (!Filename.isEmpty())
      ui->imageView->data().exportCsv(Filename);
  }
}

void MainWindow::doAbout()
{
  #ifndef BT1_GIT_VERSION
  #  define BT1_GIT_VERSION ?
  #endif
  #define _XSTR(var)  _STR(var)
  #define _STR(var)   #var
  #define GIT_VERSION _XSTR(BT1_GIT_VERSION)

  QMessageBox::information(this, tr("A Propos"), tr("Biotool2\nversion 1-%1").arg(GIT_VERSION));
}

QString MainWindow::getDefaultFilename()
{
  if (fileName.isEmpty())
    return imageName;
  return QFileInfo(fileName).baseName();
}

void MainWindow::setImageWidth()
{
  imageHeight->blockSignals(true);
  ui->imageView->setImageRealWidth(imageWidth->text().toDouble());
  imageHeight->setText(QString::number(ui->imageView->imageRealHeight()));
  imageHeight->blockSignals(false);
}

void MainWindow::setImageHeight()
{
  imageWidth->blockSignals(true);
  ui->imageView->setImageRealHeight(imageHeight->text().toDouble());
  imageWidth->setText(QString::number(ui->imageView->imageRealWidth()));
  imageWidth->blockSignals(false);
}
