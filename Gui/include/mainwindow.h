#pragma once

#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>
#include <QWidget>
#include <QtPrintSupport/qprinter.h>

#include "countlabel.h"
#include "grid.h"
#include "qrmanager.h"
#include "sudokuimage.h"
#include "toggleswitch.h"

#include <QKeyEvent>

class MainWindow : public QWidget
{
    Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = nullptr);

  protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

  private slots:
    void updateUndoRedoButtons(bool canUndo, bool canRedo);
    void onExportToPdf();
    // void updateCountLabel(int count, int maxCount);
    void onGenerate();
    void onEditModeToggled(bool checked);
    void onLoadImage();
    void onPeek();
    // void onLockToggle(bool checked);

    void onLockToggled(bool checked);

    void onStatusChanged(bool success, const QString& message);
    void onLoadRequested();
    void onSaveRequested();
    void updateTimerLabel();
    void onToggleTimer();
    void resetTimer();

  private:
    QPushButton* m_solveBtn;
    QPushButton* m_showSolBtn;
    QPushButton* m_clearBtn;
    QPushButton* m_randomBtn;
    QPushButton* m_undoBtn;
    QPushButton* m_redoBtn;
    ToggleSwitch* m_lockToggle;
    ToggleSwitch* m_editModeToggle;
    CountLabel* m_puzzleCountLabel;
    GridView* m_grid;
    SudokuImage* m_image;
    QrManager* m_qrManager;
    QLabel* m_statusLabel;

    QPushButton* m_peekBtn;
    QPushButton* m_leftBrowseBtn;
    QPushButton* m_rightBrowseBtn;
    QLabel* m_solIndexLabel;
    QPushButton* m_loadBtn;
    QPushButton* m_saveBtn;
    QPushButton* m_toPDFBtn;
    QPushButton* m_loadImgBtn;
    QLabel* m_qrCodeLabel;
    QPushButton* m_removeImgBtn;
    QPushButton* m_setImgToGrid;
    QLabel* m_editModeValueLbl;
    QPushButton* m_generateBtn;
    QList<QToolButton*> m_starButtons;
    QLabel* m_lockLabel;
    QPushButton* m_checkBtn;
    int m_difficulty;
    QLabel* m_timerLabel;
    QPushButton* m_timerToggleBtn;
    QTimer* m_qtimer;
    QElapsedTimer m_elapsedTimer;
    QPushButton* m_resetTimerBtn;
    QComboBox* m_symmetryCombo;
    QPushButton* m_helpBtn;
    qint64 m_accumulatedTimeMs{0};
    bool m_isTimerRunning = false;

  private:
    QWidget* createLeftControls();
    QWidget* createSudokuArea();
    QWidget* createImageArea();
    QWidget* createImageControls();
    QHBoxLayout* createBottomBar();
    void connectSignals();

    QHBoxLayout* setupStars();
    void updateStars(int level);
    void updateUI(bool checked);
    void updateSolutionCount(int current, int total);
    void renderSudokuPage(QPrinter* printer);
    void onCheckSudoku();
};
