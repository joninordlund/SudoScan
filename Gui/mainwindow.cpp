#include "mainwindow.h"
#include "grid.h"
#include "sudokuserializer.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <QPdfWriter>
#include <QScreen>
#include <QStyle>
#include <QVBoxLayout>
#include <QtPrintSupport/qprintpreviewdialog.h>
#include <helpdialog.h>

// HUOM: ei nimeä "Ui", koska uic generoi oman Ui-nimiavaruuden.
namespace Layout
{
constexpr int leftPanelW = 96;
constexpr int rightPanelW = 120;
constexpr int columnSpacing = 20;
constexpr int rowSpacing = 8;
constexpr int barH = 34;
constexpr int windowMargin = 11;
constexpr int maxAreaSize = 540;
constexpr int minAreaSize = 270;
constexpr int extraMargin = 40;
// Arvio ruutujen ulkopuolisesta korkeudesta (otsikkopalkki, alarivi, marginaalit)
constexpr int verticalChrome = 150;
} // namespace Layout

namespace
{

int computeAreaSize()
{
    const QRect available = QGuiApplication::primaryScreen()->availableGeometry();

    const int fixedWidth = Layout::leftPanelW + Layout::rightPanelW + 3 * Layout::columnSpacing;
    const int byWidth = (available.width() - fixedWidth - 2 * Layout::windowMargin - Layout::extraMargin) / 2;
    const int byHeight = available.height() - Layout::verticalChrome - Layout::extraMargin;

    int size = qMin(Layout::maxAreaSize, qMin(byWidth, byHeight));
    size = qMax(size, Layout::minAreaSize);
    return (size / 9) * 9; // jaollinen yhdeksällä
}

QLabel* makeSectionLabel(const QString& text)
{
    auto* l = new QLabel(text);
    l->setObjectName("sectionLabel");
    return l;
}

QPushButton* makeIconButton(const QString& icon, const QString& name)
{
    auto* b = new QPushButton;
    b->setIcon(QIcon(icon));
    b->setIconSize({24, 24});
    b->setFixedSize(35, 35);
    b->setObjectName(name);
    return b;
}

QWidget* makeToggleRow(QLabel* label, ToggleSwitch* toggle)
{
    auto* w = new QWidget;
    auto* l = new QHBoxLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(8);
    l->addWidget(label);
    l->addWidget(toggle);
    return w;
}

QWidget* makeButtonRow(QPushButton* a, QPushButton* b)
{
    auto* w = new QWidget;
    auto* l = new QHBoxLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(5);
    l->addWidget(a);
    l->addWidget(b);
    return w;
}

} // namespace

MainWindow::MainWindow(QWidget* parent) :
    QWidget{parent}, m_difficulty(1), m_grid(nullptr)
{
    m_areaSize = computeAreaSize();

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(Layout::windowMargin, Layout::windowMargin,
                                   Layout::windowMargin, Layout::windowMargin);

    // Ruudut rivillä 0 ja alarivit rivillä 1 samassa gridissä -> alarivit tasan.
    auto* centerArea = new QWidget;
    auto* grid = new QGridLayout(centerArea);
    grid->setHorizontalSpacing(Layout::columnSpacing);
    grid->setVerticalSpacing(Layout::rowSpacing);
    grid->setContentsMargins(0, 10, 0, 0);

    //                      widget                   rivi sar. rs cs  tasaus
    grid->addWidget(createLeftControls(),             0, 0, 3, 1, Qt::AlignTop);
    grid->addWidget(createSudokuArea(),               0, 1, Qt::AlignTop);
    grid->addWidget(createSudokuBar(),                1, 1);
    grid->addWidget(createImageArea(),                0, 2, Qt::AlignTop);
    grid->addWidget(createImageBar(),                 1, 2);
    grid->addWidget(createImageControls(),            0, 3, 3, 1, Qt::AlignTop);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 1);
    grid->setRowStretch(2, 1); // ylimääräinen tila menee rivin 2 (tyhjä) alle, ei ruutujen väliin

    mainLayout->addWidget(centerArea);

    onEditModeToggled(m_editModeToggle->isChecked());
    m_qrManager = new QrManager(this);
    connectSignals();
    m_qrManager->start();
    qApp->installEventFilter(this);
    updateUI(false);
    setupShortcuts();

    // Layout päättää koon -> toimii myös Windowsin 125 % / 150 % skaalauksella
    setFixedSize(sizeHint());
}

void MainWindow::setupShortcuts()
{
    auto* undoAction = new QAction(this);
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setShortcutContext(Qt::WindowShortcut);
    connect(undoAction, &QAction::triggered, m_grid, &GridView::onUndo);
    addAction(undoAction);

    auto* redoAction = new QAction(this);
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setShortcutContext(Qt::WindowShortcut);
    connect(redoAction, &QAction::triggered, m_grid, &GridView::onRedo);
    addAction(redoAction);
}

QWidget* MainWindow::createLeftControls()
{
    const int spacing = 4;
    QWidget* controlArea = new QWidget;
    controlArea->setFixedWidth(Layout::leftPanelW);
    QVBoxLayout* layout = new QVBoxLayout(controlArea);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    // File section
    m_loadBtn = new QPushButton("Load");
    m_saveBtn = new QPushButton("Save");
    m_toPDFBtn = new QPushButton("Print");

    layout->addWidget(makeSectionLabel("File"));
    layout->addWidget(m_loadBtn);
    layout->addWidget(m_saveBtn);
    layout->addWidget(m_toPDFBtn);
    layout->addSpacing(spacing);

    // Sudoku section
    layout->addWidget(makeSectionLabel("Sudoku"));

    // Undo/Redo arrows
    m_undoBtn = makeIconButton(":/icons/icons/undo.svg", "undo");
    m_redoBtn = makeIconButton(":/icons/icons/redo.svg", "redo");
    m_undoBtn->setEnabled(false);
    m_redoBtn->setEnabled(false);
    layout->addWidget(makeButtonRow(m_undoBtn, m_redoBtn));

    // Mode toggle
    m_editModeToggle = new ToggleSwitch(this);
    layout->addWidget(makeToggleRow(new QLabel("Mode:"), m_editModeToggle));

    // Clear and Random buttons
    m_clearBtn = new QPushButton("Clear");
    m_randomBtn = new QPushButton("Random");
    m_generateBtn = new QPushButton("Generate");
    m_generateBtn->setMinimumHeight(30);
    QHBoxLayout* starLayout = setupStars();
    starLayout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_clearBtn);
    layout->addWidget(m_randomBtn);

    QLabel* symmLbl = new QLabel("Symmetry:", this);
    m_symmetryCombo = new QComboBox(this);
    m_symmetryCombo->setMinimumHeight(30);
    m_symmetryCombo->addItem("None", static_cast<int>(Symmetry::None));
    m_symmetryCombo->addItem("Rotational", static_cast<int>(Symmetry::Rotate180));
    m_symmetryCombo->addItem("Horizontal", static_cast<int>(Symmetry::Horizontal));
    m_symmetryCombo->addItem("Vertical", static_cast<int>(Symmetry::Vertical));

    QVBoxLayout* genGroup = new QVBoxLayout();
    genGroup->setSpacing(4);
    genGroup->addWidget(m_generateBtn);
    genGroup->addLayout(starLayout);
    genGroup->addWidget(symmLbl);
    genGroup->addWidget(m_symmetryCombo);
    genGroup->setSizeConstraint(QLayout::SetMinimumSize);

    layout->addLayout(genGroup);
    updateStars(m_difficulty);

    // Solutions section
    m_peekBtn = new QPushButton("Peek");
    m_peekBtn->setObjectName("colorGreen");
    layout->addWidget(new QLabel("Solutions"));
    layout->addWidget(m_peekBtn);

    // Lock toggle
    m_lockLabel = new QLabel("Lock:");
    m_lockToggle = new ToggleSwitch(this);
    layout->addWidget(makeToggleRow(m_lockLabel, m_lockToggle));

    // Navigation arrows
    m_leftBrowseBtn = makeIconButton(":/icons/icons/chevron_left.svg", "leftBrowse");
    m_rightBrowseBtn = makeIconButton(":/icons/icons/chevron_right.svg", "rightBrowse");
    layout->addWidget(makeButtonRow(m_leftBrowseBtn, m_rightBrowseBtn));

    m_solIndexLabel = new QLabel("- / -");
    m_solIndexLabel->setAlignment(Qt::AlignCenter);
    m_solIndexLabel->setStyleSheet("color: #2c3e50; font-weight: bold; font-size: 14px;");
    layout->addWidget(m_solIndexLabel);

    layout->addStretch();

    return controlArea;
}

QWidget* MainWindow::createSudokuArea()
{
    QWidget* leftSquare = new QWidget;
    leftSquare->setFixedSize(m_areaSize, m_areaSize);

    QHBoxLayout* gridLayout = new QHBoxLayout(leftSquare);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setAlignment(Qt::AlignCenter);

    m_grid = new GridView(m_areaSize / 9, leftSquare);
    gridLayout->addWidget(m_grid, 0, Qt::AlignCenter);

    return leftSquare;
}

QWidget* MainWindow::createSudokuBar()
{
    QWidget* bar = new QWidget;
    bar->setFixedHeight(Layout::barH);
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(0, 0, 0, 0);

    // Check Button
    m_checkBtn = new QPushButton("Check");
    m_checkBtn->setFixedWidth(100);
    m_checkBtn->setEnabled(false);

    // Timer UI
    m_timerLabel = new QLabel("00:00");
    m_timerLabel->setStyleSheet("font-size: 16px; font-weight: bold; font-family: monospace; color: #2c3e50;");

    m_timerToggleBtn = new QPushButton("Start");
    m_timerToggleBtn->setFixedWidth(80);
    m_resetTimerBtn = new QPushButton("Reset");
    m_resetTimerBtn->setFixedWidth(80);

    m_qtimer = new QTimer(this);
    m_qtimer->setInterval(1000);

    l->addWidget(m_checkBtn);
    l->addStretch();
    l->addWidget(m_timerLabel);
    l->addWidget(m_timerToggleBtn);
    l->addWidget(m_resetTimerBtn);

    return bar;
}

QWidget* MainWindow::createImageArea()
{
    QFrame* rightSquare = new QFrame;
    m_rightSquare = rightSquare;

    rightSquare->setFixedSize(m_areaSize, m_areaSize);
    rightSquare->setFrameShape(QFrame::Box);
    rightSquare->setLineWidth(3);

    QHBoxLayout* squareLayout = new QHBoxLayout(rightSquare);
    squareLayout->setContentsMargins(8, 8, 8, 8);
    squareLayout->setAlignment(Qt::AlignCenter);

    m_image = new SudokuImage(rightSquare);
    squareLayout->addWidget(m_image);

    return rightSquare;
}

QWidget* MainWindow::createImageBar()
{
    QWidget* bar = new QWidget;
    bar->setFixedHeight(Layout::barH);
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(0, 0, 0, 0);

    m_helpBtn = new QPushButton("Help");

    l->addStretch();
    l->addWidget(m_helpBtn);

    return bar;
}

QWidget* MainWindow::createImageControls()
{
    QWidget* imageControlArea = new QWidget;
    imageControlArea->setFixedWidth(Layout::rightPanelW);
    QVBoxLayout* layout = new QVBoxLayout(imageControlArea);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    // Image section
    m_loadImgBtn = new QPushButton("Load");
    m_removeImgBtn = new QPushButton("Remove");
    m_setImgToGrid = new QPushButton("Import");
    m_setImgToGrid->setEnabled(false);

    layout->addWidget(makeSectionLabel("Image"));
    layout->addWidget(m_loadImgBtn);
    layout->addWidget(m_removeImgBtn);
    layout->addSpacing(10);
    layout->addWidget(m_setImgToGrid);

    layout->addSpacing(30);

    // QR-code
    QLabel* qrInfoLbl = new QLabel("or read\nQR-code");
    qrInfoLbl->setAlignment(Qt::AlignCenter);
    qrInfoLbl->setStyleSheet("color: #7f8c8d; font-size: 11px; font-weight: bold;");

    m_qrCodeLabel = new QLabel();
    m_qrCodeLabel->setFixedSize(120, 120);
    m_qrCodeLabel->setFrameShape(QFrame::Panel);
    m_qrCodeLabel->setLineWidth(1);
    m_qrCodeLabel->setScaledContents(true);
    m_qrCodeLabel->setStyleSheet("background-color: white;");
    m_qrCodeLabel->setText("QR");
    m_qrCodeLabel->setAlignment(Qt::AlignCenter);

    m_statusLabel = new QLabel("");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet("font-size: 10px; font-weight: bold;");

    layout->addWidget(qrInfoLbl);
    layout->addWidget(m_qrCodeLabel, 0, Qt::AlignCenter);
    layout->addSpacing(50);
    layout->addWidget(m_statusLabel);
    layout->addStretch();

    return imageControlArea;
}

QHBoxLayout* MainWindow::setupStars()
{
    QHBoxLayout* starLayout = new QHBoxLayout();
    for (int i = 1; i <= 5; i++)
    {
        QToolButton* star = new QToolButton();
        star->setText("\u2605");
        star->setCheckable(true);
        star->setObjectName("starButton");
        m_starButtons.append(star);
        starLayout->addWidget(star);

        connect(star, &QToolButton::clicked, this, [=]()
                { updateStars(i); });
    }
    updateStars(m_difficulty);
    return starLayout;
}

void MainWindow::updateStars(int level)
{
    m_difficulty = level;
    for (int i = 0; i < 5; i++)
    {
        m_starButtons[i]->setChecked(i < level);
    }

    if (m_grid)
    {
        m_grid->setDifficulty(level);
    }
}

void MainWindow::connectSignals()
{
    connect(m_image, &SudokuImage::newSudoku, m_grid, &GridView::newSudoku);
    connect(m_grid, &GridView::solutionCountChanged, this, &MainWindow::updateSolutionCount);
    connect(m_loadImgBtn, &QPushButton::clicked, this, &MainWindow::onLoadImage);
    connect(m_removeImgBtn, &QPushButton::clicked, m_image, &SudokuImage::onRemove);
    connect(m_peekBtn, &QPushButton::pressed, this, &MainWindow::onPeek);
    connect(m_peekBtn, &QPushButton::released, m_grid, &GridView::onHideSolution);
    connect(m_clearBtn, &QPushButton::clicked, m_grid, &GridView::onClearSolution);
    connect(m_randomBtn, &QPushButton::clicked, m_grid, &GridView::onRandom);
    connect(m_undoBtn, &QPushButton::clicked, m_grid, &GridView::onUndo);
    connect(m_redoBtn, &QPushButton::clicked, m_grid, &GridView::onRedo);
    connect(m_leftBrowseBtn, &QPushButton::clicked, m_grid, &GridView::onBrowseSolLeft);
    connect(m_rightBrowseBtn, &QPushButton::clicked, m_grid, &GridView::onBrowseSolRight);
    connect(m_lockToggle, &ToggleSwitch::toggled, this, &MainWindow::onLockToggled);
    connect(m_generateBtn, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(m_editModeToggle, &ToggleSwitch::toggled, this, &MainWindow::onEditModeToggled);
    connect(m_toPDFBtn, &QPushButton::clicked, this, &MainWindow::onExportToPdf);
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadRequested);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveRequested);
    connect(m_helpBtn, &QPushButton::clicked, this, [this]()
            {
                HelpDialog dlg(this);
                dlg.exec(); });
    connect(m_grid, &GridView::gridFullStatusChanged, m_checkBtn, &QPushButton::setEnabled);

    connect(m_timerToggleBtn, &QPushButton::clicked, this, &MainWindow::onToggleTimer);
    connect(m_resetTimerBtn, &QPushButton::clicked, this, &MainWindow::resetTimer);
    connect(m_checkBtn, &QPushButton::clicked, this, &MainWindow::onCheckSudoku);

    connect(m_qtimer, &QTimer::timeout, this, &MainWindow::updateTimerLabel);
    connect(m_symmetryCombo, &QComboBox::currentIndexChanged, this, [this](int index)
            {
        if (index < 0) return;

        Symmetry symmetry = static_cast<Symmetry>(m_symmetryCombo->currentData().toInt());
        m_grid->setSymmetry(symmetry); });

    connect(m_qrManager, &QrManager::qrCodeChanged, this, [this](const QPixmap& pix)
            { m_qrCodeLabel->setPixmap(pix); });

    connect(m_qrManager, &QrManager::imageReady, this, [this](const QByteArray& img)
            { m_image->loadFromData(img); });
    connect(m_image, &SudokuImage::imageOk, this, [this](const bool success)
            {
                if (success) {
                    onStatusChanged(true, "Sudoku found in the image!");
                } else {
                    onStatusChanged(false, "Sudoku not found in the image");
                } });
    connect(m_image, &SudokuImage::isImgLoadedChanged, this, [this](const bool isLoaded)
            { m_setImgToGrid->setEnabled(isLoaded); });
    connect(m_setImgToGrid, &QPushButton::clicked, m_image, &SudokuImage::onSetImage);
    connect(m_grid->getHistory(), &History::historyStateChanged,
            this, [this](bool canUndo, bool canRedo)
            {
                m_undoBtn->setEnabled(canUndo);
                m_redoBtn->setEnabled(canRedo); });
}

void MainWindow::onToggleTimer()
{
    if (m_isTimerRunning)
    {
        m_accumulatedTimeMs += m_elapsedTimer.elapsed();
        m_qtimer->stop();
        m_isTimerRunning = false;
        m_timerToggleBtn->setText("Resume");
    }
    else
    {
        m_elapsedTimer.start();
        m_qtimer->start();
        m_isTimerRunning = true;
        m_timerToggleBtn->setText("Pause");
    }
}

void MainWindow::resetTimer()
{
    m_qtimer->stop();
    m_accumulatedTimeMs = 0;
    m_isTimerRunning = false;
    m_timerLabel->setText("00:00");
    m_timerToggleBtn->setText("Start");
}

void MainWindow::updateTimerLabel()
{
    if (!m_isTimerRunning)
        return;

    qint64 totalMs = m_accumulatedTimeMs + m_elapsedTimer.elapsed();
    int totalSeconds = static_cast<int>(totalMs / 1000);

    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    m_timerLabel->setText(QString("%1:%2")
                              .arg(minutes, 2, 10, QChar('0'))
                              .arg(seconds, 2, 10, QChar('0')));
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_F10)
        {
            m_image->setDebugView(true);
            return true;
        }
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_F10)
        {
            m_image->setDebugView(false);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::onExportToPdf()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(30, 30, 30, 30));

    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle("Sudoku Print Preview");
    preview.resize(800, 900);

    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &MainWindow::renderSudokuPage);
    preview.exec();
}

void MainWindow::renderSudokuPage(QPrinter* printer)
{
    QPainter painter(printer);

    int pageWidth = printer->width();
    int headerHeight = pageWidth * 0.12;

    QFont titleFont("Arial", 28, QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    QRect titleRect(0, 0, pageWidth, headerHeight / 2);
    painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, "SUDOKU");

    QFont infoFont("Arial", 14, QFont::Normal);
    painter.setFont(infoFont);

    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy");
    QString difficultyText = "Difficulty: " + QString::number(m_difficulty);

    QRect dateRect(0, headerHeight / 2, pageWidth / 2, headerHeight / 2);
    QRect diffRect(pageWidth / 2, headerHeight / 2, pageWidth / 2, headerHeight / 2);

    painter.drawText(dateRect, Qt::AlignLeft | Qt::AlignVCenter, "Date: " + currentDate);
    painter.drawText(diffRect, Qt::AlignRight | Qt::AlignVCenter, difficultyText);

    int availableHeight = printer->height() - headerHeight;
    int side = qMin(pageWidth, availableHeight) * 0.85;
    int cellSize = side / 9;

    int offsetX = (pageWidth - side) / 2;
    int offsetY = headerHeight + (availableHeight - side) / 2;

    int thinPenWidth = qMax(1, cellSize / 30);
    int thickPenWidth = qMax(2, cellSize / 15);

    for (int i = 0; i <= 9; i++)
    {
        int currentWidth = (i % 3 == 0) ? thickPenWidth : thinPenWidth;
        painter.setPen(QPen(Qt::black, currentWidth, Qt::SolidLine, Qt::FlatCap));

        painter.drawLine(offsetX + i * cellSize, offsetY, offsetX + i * cellSize, offsetY + side);
        painter.drawLine(offsetX, offsetY + i * cellSize, offsetX + side, offsetY + i * cellSize);
    }

    vector<vector<CellData>> grid = m_grid->getBoard().toCellDataMatrix();
    int numFontSize = cellSize / 20;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i][j].digit != 0)
            {
                QFont digitFont("Arial", numFontSize, QFont::Normal);
                painter.setFont(digitFont);
                painter.setPen(grid[i][j].isGiven ? Qt::black : QColor(0, 102, 204));

                QRect rect(offsetX + j * cellSize, offsetY + i * cellSize, cellSize, cellSize);
                painter.drawText(rect, Qt::AlignCenter, QString::number(grid[i][j].digit));
            }
        }
    }
}

void MainWindow::onCheckSudoku()
{
    bool isOk = m_grid->getBoard().isSudokuSolutionValid();
    if (isOk)
    {
        QMessageBox::information(this,
                                 "Sudoku Checked",
                                 "Congratulations! The solution is 100% correct. 🎉");
    }
    else
    {
        QMessageBox::critical(this,
                              "Sudoku Checked",
                              "Oops! There is a mistake somewhere. Check the rows, columns, or 3x3 boxes again. ❌");
    }
}

void MainWindow::onPeek()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_grid->onShowSolution();
    QApplication::restoreOverrideCursor();
}

void MainWindow::onLoadImage()
{
    QSettings settings;
    QString defaultPath = QDir::homePath() + "/Documents";
    QString lastDir = settings.value("last_image_load_dir", defaultPath).toString();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Sudoku Image"),
                                                    lastDir,
                                                    tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (fileName.isEmpty())
    {
        return;
    };

    QFileInfo fileInfo(fileName);
    settings.setValue("last_image_load_dir", fileInfo.absolutePath());
    m_image->load(fileName);
}

void MainWindow::onGenerate()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_grid->onGenerate();
    QApplication::restoreOverrideCursor();
}

void MainWindow::onLockToggled(bool locked)
{
    updateUI(m_editModeToggle->isChecked());
    m_grid->onSolutionLocked(locked);
}

void MainWindow::onEditModeToggled(bool checked)
{
    m_grid->onEditModeChanged(checked);
    m_lockToggle->setChecked(checked && m_lockToggle->isChecked());
    updateUI(checked);
}

void MainWindow::updateSolutionCount(int current, int total)
{
    bool editMode = m_editModeToggle->isChecked();
    bool lockActive = m_lockToggle->isChecked();

    if (!editMode)
    {
        m_solIndexLabel->setText("- / -");
        return;
    }

    if (lockActive)
    {
        if (total == 0)
        {
            m_solIndexLabel->setText("0 / 0");
        }
        else
        {
            int displayIndex = (current <= 0) ? 1 : current;
            m_solIndexLabel->setText(QString("%1 / %2").arg(displayIndex).arg(total));
        }
    }
    else
    {
        if (total == 0)
        {
            m_solIndexLabel->setText("- / 0");
        }
        else
        {
            m_solIndexLabel->setText(QString("- / %1").arg(total));
        }
    }
}

void MainWindow::updateUI(bool editMode)
{
    bool lockActive = m_lockToggle->isChecked();
    m_leftBrowseBtn->setEnabled(editMode && lockActive);
    m_rightBrowseBtn->setEnabled(editMode && lockActive);

    m_randomBtn->setEnabled(editMode);
    m_lockToggle->setEnabled(editMode);
    m_lockLabel->setEnabled(editMode);

    if ((!lockActive && editMode) || !editMode)
    {
        m_peekBtn->setText("Peek");
    }
    else
    {
        m_peekBtn->setText("Reset");
    }
}

void MainWindow::updateUndoRedoButtons(bool canUndo, bool canRedo)
{
    m_undoBtn->setEnabled(canUndo);
    m_redoBtn->setEnabled(canRedo);
}

void MainWindow::onSaveRequested()
{
    QSettings settings;
    QString defaultPath = QDir::homePath() + "/Documents";
    QString lastDir = settings.value("last_save_dir", defaultPath).toString();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save Sudoku",
                                                    lastDir,
                                                    "Sudoku files (*.sdk);;All files (*)");

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith(".sdk"))
        {
            fileName += ".sdk";
        }

        QFileInfo fileInfo(fileName);
        settings.setValue("last_save_dir", fileInfo.absolutePath());

        if (!SudokuSerializer::saveToFile(fileName, m_grid->getBoard(), *m_grid->getHistory()))
        {
            QMessageBox::critical(this, "Error", "Saving file failed");
        }
    }
}

void MainWindow::onLoadRequested()
{
    QSettings settings;
    QString defaultPath = QDir::homePath() + "/Documents";
    QString lastDir = settings.value("last_save_dir", defaultPath).toString();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open Sudoku",
                                                    lastDir,
                                                    "Sudoku files (*.sdk);;All files (*)");

    if (!fileName.isEmpty())
    {
        if (SudokuSerializer::loadFromFile(fileName, m_grid->getBoard(), *m_grid->getHistory()))
        {
            m_grid->updateUI();
            m_grid->update();
        }
        else
        {
            QMessageBox::critical(this, "Error", "File load failed.");
        }
    }
}

void MainWindow::onStatusChanged(bool success, const QString& message)
{
    m_statusLabel->setText(message);
    if (success)
    {
        m_statusLabel->setStyleSheet("color: #27ae60; font-size: 14px; font-weight: bold;");
    }
    else
    {
        m_statusLabel->setStyleSheet("color: #e74c3c; font-size: 14px; font-weight: bold;");
    }

    QTimer::singleShot(12000, this, [this]()
                       { m_statusLabel->setText(""); });
}
