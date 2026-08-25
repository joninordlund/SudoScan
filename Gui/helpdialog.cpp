#include "helpdialog.h"

#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QVBoxLayout>

HelpDialog::HelpDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle("SudoScan Application - User Guide");
    resize(600, 500);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QTextBrowser* browser = new QTextBrowser(this);

    browser->setOpenExternalLinks(true);
    browser->setHtml(getHelpHtmlContent());

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::accept);

    layout->addWidget(browser);
    layout->addWidget(buttonBox);
}

QString HelpDialog::getHelpHtmlContent()
{
    return R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<style>
    body {
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
        font-size: 14px;
        line-height: 1.6;
        color: #2c3e50;
        margin: 10px;
    }
    h1 {
        color: #1a252f;
        font-size: 20px;
        border-bottom: 2px solid #3498db;
        padding-bottom: 5px;
        margin-top: 0;
    }
    h2 {
        color: #2980b9;
        font-size: 16px;
        margin-top: 20px;
        margin-bottom: 8px;
    }
    p {
        margin-bottom: 12px;
    }
    ol {
        padding-left: 20px;
        margin-top: 5px;
        margin-bottom: 15px;
    }
    li {
        margin-bottom: 6px;
    }
    .highlight {
        background-color: #eef7fc;
        border-left: 4px solid #3498db;
        padding: 10px;
        margin-top: 15px;
        border-radius: 2px;
    }
    h3 {
        color: #2c3e50;
        font-size: 15px;
        margin-top: 18px;
        margin-bottom: 6px;
        border-bottom: 1px solid #ecf0f1;
        padding-bottom: 3px;
    }
    .controls-table {
        width: 100%;
        border-collapse: collapse;
        margin: 10px 0 15px 0;
        font-size: 13px;
    }
    .controls-table th, .controls-table td {
        border: 1px solid #dcdde1;
        padding: 6px 10px;
        text-align: left;
    }
    .controls-table th {
        background-color: #f5f6fa;
        color: #2f3640;
    }
    code {
        background-color: #f1f2f6;
        padding: 2px 5px;
        border-radius: 3px;
        font-family: monospace;
        color: #e84118;
    }
    .note {
        background-color: #fef9e7;
        border-left: 4px solid #f1c40f;
        padding: 8px 12px;
        margin: 10px 0;
        font-size: 13px;
    }
    .note ul {
        margin: 0;
        padding-left: 18px;
    }

</style>
</head>
<body>

<h1>SudoScan Application Guide</h1>

<h2>1. Overview</h2>
<p>Welcome to the Sudoku Application! This software is designed for solving, creating, and analyzing Sudoku puzzles with rich interactive tools.</p>

<p>You can import or create a Sudoku grid using <strong>four different methods</strong>:</p>
<ol>
    <li><strong>Generate:</strong> Create a new random Sudoku puzzle automatically.</li>
    <li><strong>Manual Creation:</strong> Enter your own clues directly onto the board in Edit mode.</li>
    <li><strong>Load Image File:</strong> Import an existing image file (e.g., JPEG/PNG) containing a Sudoku board from your local disk.</li>
    <li><strong>Scan QR Code & Camera Import:</strong> Scan a generated QR code with a mobile device, capture a physical Sudoku puzzle using your phone's camera, and sync it directly to the application.</li>
</ol>

<div class="highlight">
    <strong>Solver & Analysis Engine:</strong> Once a puzzle is loaded or locked, the application can find <em>all possible valid solutions</em>. You can easily browse and analyze multiple solutions for complex or custom-built puzzles.
</div>
<h2>2. Game Modes</h2>
<p>The application features three specialized modes to help you solve, design, and analyze Sudoku puzzles.</p>

<!-- 1. SOLVE MODE -->
<h3>2.1. Solve Mode</h3>
<p>Solve Mode is designed for playing and solving puzzles. In this mode, you can enter solution digits and mark candidate numbers without altering the original puzzle clues.</p>

<table class="controls-table">
    <tr>
        <th>Action</th>
        <th>Control / Shortcut</th>
    </tr>
    <tr>
        <td><strong>Enter Digits</strong></td>
        <td>Number keys <code>1–9</code></td>
    </tr>
    <tr>
        <td><strong>Center Marks (Candidates)</strong></td>
        <td>Hold <code>Ctrl</code> + Number key <code>1–9</code></td>
    </tr>
    <tr>
        <td><strong>Corner Marks (Candidates)</strong></td>
        <td>Hold <code>Alt</code> + Number key <code>1–9</code></td>
    </tr>
    <tr>
        <td><strong>Single Selection</strong></td>
        <td>Left-click cell or use <code>Arrow keys</code></td>
    </tr>
    <tr>
        <td><strong>Multi-Select (Individual)</strong></td>
        <td>Hold <code>Ctrl</code> + Left-click cells</td>
    </tr>
    <tr>
        <td><strong>Range Select (Drag)</strong></td>
        <td>Click &amp; drag mouse OR hold <code>Shift</code> + <code>Arrow keys</code></td>
    </tr>
    <tr>
        <td><strong>Clear Cell(s)</strong></td>
        <td><code>Delete</code> or <code>Backspace</code></td>
    </tr>
    <tr>
        <td><strong>Undo / Redo Action</strong></td>
        <td><code>Ctrl+Z</code> / <code>Ctrl+Shift+Z</code> (or <code>Ctrl+Y</code>)</td>
    </tr>
    <tr>
        <td><strong>Deselect All</strong></td>
        <td><code>Esc</code></td>
    </tr>
</table>

<div class="note">
    <ul>
        <li><strong>Check Button:</strong> Becomes active automatically once the grid is completely filled. Click it to verify if your solution is correct.</li>
        <li><strong>Peek Button:</strong> Press and hold the Peek button to temporarily reveal the correct solution.</li>
    </ul>
</div>

<!-- 2. EDIT MODE -->
<h3>2.2. Edit Mode</h3>
<p>Edit Mode is used for creating and constructing custom Sudoku puzzles by manually adding or removing given clues.</p>

<ul>
    <li><strong>Creating a Puzzle from Scratch:</strong> Start with an empty board and add numbers until the solution counter shows exactly <strong>1</strong> unique solution.</li>
    <li><strong>Creating a Puzzle from a Full Board:</strong> Click the <strong>Random</strong> button to populate the board with a complete valid grid, then remove numbers one by one while keeping the solution count at 1.</li>
    <li><strong>Real-time Solution Counter:</strong> Located in the bottom-left panel, this counter tracks the current number of valid solutions for your layout. For performance reasons, the display count is capped at <strong>25</strong> (even if the grid technically has more valid combinations).</li>
    <li><strong>Navigation:</strong> Cell selection, multi-selection, and navigation controls work identically to Solve Mode.</li>
</ul>

<!-- 3. LOCK MODE (ANALYSIS) -->
<h3>2.3. Lock / Analysis Mode</h3>
<p>Lock Mode can be entered directly from Edit Mode to freeze and analyze the puzzle's layout.</p>

<ul>
    <li><strong>Locked State:</strong> The current puzzle board and its generated solution set are locked on screen to prevent accidental editing.</li>
    <li><strong>Browsing Solutions:</strong> If the puzzle has multiple valid solutions (or fewer than 25), you can cycle through all possible solution grids using the <strong>navigation arrows</strong> in the bottom-left control panel.</li>
</ul>
<h2>3. Features &amp; Tools</h2>
<p>The application provides several built-in tools to assist in puzzle generation, image scanning, exporting, and managing your session history.</p>

<!-- GENERATION & SYMMETRY -->
<h3>3.1. Puzzle Generation &amp; Symmetry</h3>
<ul>
    <li><strong>Difficulty Level:</strong> Choose a difficulty rating from <strong>1</strong> (Easiest) to <strong>5</strong> (Hardest) using the star rating selection before clicking <em>Generate</em>.</li>
    <li><strong>Symmetry Modes:</strong> Select a layout symmetry from the <em>Symmetry</em> dropdown menu:
        <ul>
            <li><code>None</code> – Random clue placement without structural symmetry.</li>
            <li><code>Rotational</code> – Clues are placed symmetrically around the center point (180° rotation).</li>
            <li><code>Horizontal</code> – Clues mirror across the central horizontal axis.</li>
            <li><code>Vertical</code> – Clues mirror across the central vertical axis.</li>
        </ul>
    </li>
</ul>

<!-- UNDO / REDO -->
<h3>3.2. History (Undo / Redo)</h3>
<p>All grid operations (entering numbers, clearing cells, adding candidates) are tracked sequentially in the action history.</p>
<ul>
    <li>Use the <strong>Undo</strong> and <strong>Redo</strong> buttons (or keyboard shortcuts <code>Ctrl+Z</code> and <code>Ctrl+Shift+Z</code> / <code>Ctrl+Y</code>) to step backward or forward through your actions.</li>
    <li><em>Note:</em> Switching between game modes (e.g., from Edit to Solve) automatically resets the history stack to maintain puzzle integrity.</li>
</ul>

<!-- CAMERA & QR SCANNING -->
<h3>3.3. QR Code &amp; Mobile Camera Import</h3>
<p>You can instantly transfer a physical Sudoku puzzle from a newspaper or book to the application using your smartphone:</p>

<ol>
    <li>Scan the on-screen <strong>QR Code</strong> using your smartphone's camera or a QR scanner app.</li>
    <li>Your mobile browser will open a web page with two options: <strong>Use Camera</strong> and <strong>Send</strong>.</li>
    <li>Tap <strong>Use Camera</strong> to capture a photo of the Sudoku puzzle and review the preview image.</li>
    <li>If the photo is clear, tap <strong>Send</strong> to transmit the image directly to the desktop application.</li>
</ol>

<!-- IMAGE PROCESSING & F10 PREVIEW -->
<h3>3.4. Image Processing &amp; OCR Tools</h3>
<ul>
    <li><strong>Set Image:</strong> Analyzes the current loaded or captured image, interprets the Sudoku grid via Computer Vision, and populates the numbers onto the board.</li>
    <li><strong>Load / Remove Image:</strong> Use the <em>Load</em> button to open an existing image file from your disk, or <em>Remove</em> to clear the current image.</li>
    <li><strong>Processed Computer View (F10 Key):</strong> Press and hold <code>F10</code> at any time to inspect how the image processing algorithm sees the board (warp-corrected, thresholded binary image). This is useful for troubleshooting lighting or alignment issues during scanning.</li>
</ul>

<!-- FILE OPERATIONS & PDF -->
<h3>3.5. File Management &amp; PDF Export</h3>
<ul>
    <li><strong>Save / Load Game:</strong> Save your current puzzle session to disk (<code>.sdk</code> format). The save file preserves the full board state, including the entire <strong>Undo / Redo history</strong>.</li>
    <li><strong>To PDF:</strong> Opens a printable preview window. From there, you can export the formatted Sudoku puzzle directly to a <strong>PDF file</strong> or send it to a physical paper printer.</li>
</ul>

</body>
</html>
)rawhtml";
}
