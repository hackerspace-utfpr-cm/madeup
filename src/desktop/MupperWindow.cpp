#include <iostream>
#include <sstream>

#include <boost/property_tree/json_parser.hpp>

#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolBar>

#include "MupperWindow.h"

#include "madeup/ExpressionBlock.h"
#include "madeup/ExpressionClosure.h"
#include "madeup/GeometryMode.h"
#include "madeup/Lexer.h"
#include "madeup/Parser.h"
#include "twodee/Co.h"
#include "twodee/MessagedException.h"
#include "twodee/QVector3.h"

using namespace madeup;
namespace pt = boost::property_tree;

/* ------------------------------------------------------------------------- */

MupperWindow::MupperWindow(QWidget *parent) :
  QMainWindow(parent),
  editor(nullptr),
  canvas(nullptr) {

  config_path = (QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + "madeup_preferences.json").toStdString();
  std::cout << "config_path: " << config_path << std::endl;

  resize(1200, 600);

  QSplitter *horizontal_splitter = new QSplitter(this);
  horizontal_splitter->setOrientation(Qt::Horizontal);

  // Left pane
  QSplitter *vertical_splitter = new QSplitter(horizontal_splitter);
  vertical_splitter->setOrientation(Qt::Vertical);

  // - Editor
  editor = new QTextEdit(vertical_splitter);
  QFont font;
  font.setFamily(QStringLiteral("Courier New"));
  font.setPointSize(18);
  editor->setFont(font);
  editor->setLineWrapMode(QTextEdit::NoWrap);

  // - Console
  console = new QTextEdit(vertical_splitter);
  console->setFont(font);

  {
    QPalette palette;
    palette.setColor(QPalette::Base, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    editor->setPalette(palette);
    console->setPalette(palette);
  }

  QSizePolicy p1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  p1.setHorizontalStretch(0);
  p1.setVerticalStretch(2);
  QSizePolicy p2(QSizePolicy::Preferred, QSizePolicy::Preferred);
  p2.setHorizontalStretch(0);
  p2.setVerticalStretch(0);

  editor->setSizePolicy(p1);
  console->setSizePolicy(p2);

  vertical_splitter->addWidget(editor);
  vertical_splitter->addWidget(console);

  // Middle pane
  canvas = new MadeupCanvas(horizontal_splitter);

  // Right pane
  QWidget *settings_panel;
  settings_panel = new QWidget(horizontal_splitter);

  QComboBox *settings_picker = new QComboBox();
  settings_picker->addItem("Display");
  settings_picker->addItem("Editor");
  settings_picker->addItem("Camera");
  settings_picker->addItem("Lighting");

  QStackedWidget *settings_pager = new QStackedWidget();

  // Display page
  QWidget *display_page = new QWidget();

  // Display page widgets
  QGroupBox *cartesian_group = new QGroupBox("Cartesian");

  QLabel *axis_label = new QLabel("Axis");
  QLabel *grid_label = new QLabel("Grid");
  QLabel *size_label = new QLabel("Size");
  QLabel *spacing_label = new QLabel("Spacing");

  QLabel *x_label = new QLabel("X");
  QLabel *y_label = new QLabel("Y");
  QLabel *z_label = new QLabel("Z");

  QCheckBox *x_axis_checkbox = new QCheckBox();
  QCheckBox *y_axis_checkbox = new QCheckBox();
  QCheckBox *z_axis_checkbox = new QCheckBox();

  QCheckBox *x_plane_checkbox = new QCheckBox();
  QCheckBox *y_plane_checkbox = new QCheckBox();
  QCheckBox *z_plane_checkbox = new QCheckBox();

  QDoubleSpinBox *x_size_spinner = new QDoubleSpinBox();
  QDoubleSpinBox *y_size_spinner = new QDoubleSpinBox();
  QDoubleSpinBox *z_size_spinner = new QDoubleSpinBox();

  QDoubleSpinBox *x_space_spinner = new QDoubleSpinBox();
  QDoubleSpinBox *y_space_spinner = new QDoubleSpinBox();
  QDoubleSpinBox *z_space_spinner = new QDoubleSpinBox();

  QGroupBox *path_group = new QGroupBox("Path");
  QCheckBox *show_heading_checkbox = new QCheckBox("Show heading");
  show_heading_checkbox->setCheckState(canvas->getRenderer().showHeading() ? Qt::Checked : Qt::Unchecked);
  QCheckBox *show_stops_checkbox = new QCheckBox("Show stops");
  show_stops_checkbox->setCheckState(canvas->getRenderer().showStops() ? Qt::Checked : Qt::Unchecked);

  QLabel *path_stroke_width_label = new QLabel("Stroke width");
  path_stroke_width_spinner = new QDoubleSpinBox();
  path_stroke_width_spinner->setMinimum(1.0);

  QLabel *vertex_size_label = new QLabel("Vertex size");
  vertex_size_spinner = new QDoubleSpinBox();
  vertex_size_spinner->setMinimum(1.0);

  /* QPushButton *path_color_button = new QPushButton("Set path color"); */
  /* QPushButton *vertex_color_button = new QPushButton("Set vertex color"); */

  QLabel *path_color_label = new QLabel("Path color");
  QPushButton *path_color_button = new QPushButton();
  path_color_button->setFlat(true);
  path_color_button->setAutoFillBackground(true);

  QLabel *vertex_color_label = new QLabel("Vertex color");
  QPushButton *vertex_color_button = new QPushButton();
  vertex_color_button->setFlat(true);
  vertex_color_button->setAutoFillBackground(true);

  QLabel *face_label = new QLabel("Faces");
  QComboBox *face_picker = new QComboBox();
  face_picker->addItem("Counterclockwise");
  face_picker->addItem("Clockwise");
  face_picker->addItem("Both");
  face_picker->addItem("None");
  face_picker->setCurrentIndex(canvas->getRenderer().getFaceOrientation());

  QLabel *style_label = new QLabel("Style");
  QComboBox *style_picker = new QComboBox();
  style_picker->addItem("Filled");
  style_picker->addItem("Wireframe");
  style_picker->addItem("Vertices");
  style_picker->setCurrentIndex(canvas->getRenderer().getFaceStyle());

  QLabel *background_color_label = new QLabel("Background color");
  QPushButton *background_color_button = new QPushButton();
  background_color_button->setFlat(true);
  background_color_button->setAutoFillBackground(true);

  QSpacerItem *vertical_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  // Display page layout
  QGridLayout *cartesian_layout = new QGridLayout(cartesian_group);
  cartesian_layout->setHorizontalSpacing(4);
  cartesian_layout->setVerticalSpacing(0);
  cartesian_layout->setContentsMargins(0, 0, 0, 0);

  int row = 0;
  cartesian_layout->addWidget(axis_label, row, 1, Qt::AlignHCenter | Qt::AlignBottom);
  cartesian_layout->addWidget(grid_label, row, 2, Qt::AlignHCenter | Qt::AlignBottom);
  cartesian_layout->addWidget(size_label, row, 3, Qt::AlignBottom);
  cartesian_layout->addWidget(spacing_label, row, 4, Qt::AlignBottom);
  ++row;

  cartesian_layout->addWidget(x_label, row, 0);
  cartesian_layout->addWidget(x_axis_checkbox, row, 1, Qt::AlignCenter);
  cartesian_layout->addWidget(x_plane_checkbox, row, 2, Qt::AlignCenter);
  cartesian_layout->addWidget(x_size_spinner, row, 3);
  cartesian_layout->addWidget(x_space_spinner, row, 4);
  ++row;

  cartesian_layout->addWidget(y_label, row, 0);
  cartesian_layout->addWidget(y_axis_checkbox, row, 1, Qt::AlignCenter);
  cartesian_layout->addWidget(y_plane_checkbox, row, 2, Qt::AlignCenter);
  cartesian_layout->addWidget(y_size_spinner, row, 3);
  cartesian_layout->addWidget(y_space_spinner, row, 4);
  ++row;

  cartesian_layout->addWidget(z_label, row, 0);
  cartesian_layout->addWidget(z_axis_checkbox, row, 1, Qt::AlignCenter);
  cartesian_layout->addWidget(z_plane_checkbox, row, 2, Qt::AlignCenter);
  cartesian_layout->addWidget(z_size_spinner, row, 3);
  cartesian_layout->addWidget(z_space_spinner, row, 4);
  ++row;

  cartesian_layout->setColumnStretch(0, 0);
  cartesian_layout->setColumnStretch(1, 0);
  cartesian_layout->setColumnStretch(2, 0);
  cartesian_layout->setColumnStretch(3, 1);
  cartesian_layout->setColumnStretch(4, 1);

  QGridLayout *path_group_layout = new QGridLayout(path_group);
  path_group_layout->setHorizontalSpacing(3);
  path_group_layout->setVerticalSpacing(5);
  path_group_layout->setContentsMargins(0, 0, 0, 0);
  path_group_layout->addWidget(show_heading_checkbox, 0, 0, 1, 2);
  path_group_layout->addWidget(show_stops_checkbox, 1, 0, 1, 2);
  path_group_layout->addWidget(path_stroke_width_label, 2, 0, 1, 1);
  path_group_layout->addWidget(path_stroke_width_spinner, 2, 1, 1, 1);
  path_group_layout->addWidget(vertex_size_label, 3, 0, 1, 1);
  path_group_layout->addWidget(vertex_size_spinner, 3, 1, 1, 1);
  path_group_layout->addWidget(path_color_label, 4, 0, 1, 1);
  path_group_layout->addWidget(path_color_button, 4, 1, 1, 1);
  path_group_layout->addWidget(vertex_color_label, 5, 0, 1, 1);
  path_group_layout->addWidget(vertex_color_button, 5, 1, 1, 1);
  path_group_layout->setColumnStretch(0, 0);
  path_group_layout->setColumnStretch(1, 1);

  QGridLayout *display_page_layout = new QGridLayout(display_page);
  display_page_layout->setSpacing(-1);
  display_page_layout->setContentsMargins(0, 0, 0, 0);
  display_page_layout->addWidget(cartesian_group, 0, 0, 1, 2);
  display_page_layout->addWidget(path_group, 1, 0, 1, 2);
  display_page_layout->addWidget(face_label, 2, 0, 1, 1);
  display_page_layout->addWidget(face_picker, 2, 1, 1, 1);
  display_page_layout->addWidget(style_label, 3, 0, 1, 1);
  display_page_layout->addWidget(style_picker, 3, 1, 1, 1);
  display_page_layout->addWidget(background_color_label, 4, 0, 1, 1);
  display_page_layout->addWidget(background_color_button, 4, 1, 1, 1);
  display_page_layout->addItem(vertical_spacer, 5, 0, 1, 2);

  // Editor page
  QWidget *editor_page = new QWidget();

  QCheckBox *autopathify_checkbox = new QCheckBox("Autopathify");
  QCheckBox *show_console_checkbox = new QCheckBox("Show console");
  show_console_checkbox->setCheckState(Qt::Checked);
  QDoubleSpinBox *autopathify_delay_spinner = new QDoubleSpinBox();
  QPushButton *select_font_button = new QPushButton("Select Font");

  QSpacerItem *vertical_spacer3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  QGridLayout *editor_page_layout = new QGridLayout(editor_page);
  editor_page_layout->setSpacing(-1);
  editor_page_layout->setContentsMargins(0, 0, 0, 0);
  editor_page_layout->addWidget(autopathify_checkbox, 0, 0, 1, 2);

  editor_page_layout->addWidget(new QLabel("Autopathify delay"), 1, 0);
  editor_page_layout->addWidget(autopathify_delay_spinner, 1, 1);

  editor_page_layout->addWidget(show_console_checkbox, 2, 0, 1, 2);
  
  editor_page_layout->addWidget(select_font_button, 3, 0, 1, 2);

  editor_page_layout->addItem(vertical_spacer3, 4, 0, 1, 2);

  editor_page_layout->setColumnStretch(0, 0);
  editor_page_layout->setColumnStretch(1, 1);
 
  // Camera page
  QWidget *camera_page = new QWidget();

  QGroupBox *view_from_group = new QGroupBox("View From");
  QPushButton *negative_x_button = new QPushButton("-X");
  QPushButton *positive_x_button = new QPushButton("+X");
  QPushButton *negative_y_button = new QPushButton("-Y");
  QPushButton *positive_y_button = new QPushButton("+Y");
  QPushButton *negative_z_button = new QPushButton("-Z");
  QPushButton *positive_z_button = new QPushButton("+Z");

  QGridLayout *view_from_group_layout = new QGridLayout(view_from_group);
  view_from_group_layout->setSpacing(-1);
  view_from_group_layout->setContentsMargins(0, 0, 0, 0);
  view_from_group_layout->addWidget(negative_x_button, 0, 0);
  view_from_group_layout->addWidget(positive_x_button, 0, 1);
  view_from_group_layout->addWidget(negative_y_button, 1, 0);
  view_from_group_layout->addWidget(positive_y_button, 1, 1);
  view_from_group_layout->addWidget(negative_z_button, 2, 0);
  view_from_group_layout->addWidget(positive_z_button, 2, 1);

  QVBoxLayout *camera_page_layout = new QVBoxLayout(camera_page);
  camera_page_layout->setSpacing(-1);
  camera_page_layout->setContentsMargins(0, 0, 0, 0);
  camera_page_layout->addWidget(view_from_group);
  QSpacerItem *vertical_spacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  camera_page_layout->addItem(vertical_spacer2);

  // Add pages
  settings_pager->addWidget(display_page);
  settings_pager->addWidget(editor_page);
  settings_pager->addWidget(camera_page);

  // Layout
  QSizePolicy horizontal_stretch(QSizePolicy::Preferred, QSizePolicy::Preferred);
  horizontal_stretch.setHorizontalStretch(1);
  horizontal_stretch.setVerticalStretch(0);
  canvas->setSizePolicy(horizontal_stretch);

  QVBoxLayout *settings_panel_layout = new QVBoxLayout(settings_panel);
  settings_panel_layout->setSpacing(6);
  settings_panel_layout->setContentsMargins(0, 6, 0, 0);
  settings_panel_layout->addWidget(settings_picker);
  settings_panel_layout->addWidget(settings_pager);

  horizontal_splitter->addWidget(vertical_splitter);
  horizontal_splitter->addWidget(canvas);
  horizontal_splitter->addWidget(settings_panel);

  // Overall
 
  {
    QPalette palette;
    palette.setColor(QPalette::Button, toQColor(canvas->getRenderer().getBackgroundColor()));
    background_color_button->setPalette(palette);
    palette.setColor(QPalette::Button, toQColor(canvas->getRenderer().getPathColor()));
    path_color_button->setPalette(palette);
    palette.setColor(QPalette::Button, toQColor(canvas->getRenderer().getVertexColor()));
    vertex_color_button->setPalette(palette);
  }

  this->setCentralWidget(horizontal_splitter);

  // Actions
  QAction *action_solidify = new QAction(this);
  action_solidify->setText("Solidify");
  action_solidify->setShortcut(Qt::CTRL + Qt::Key_R);

  QAction *action_pathify = new QAction(this);
  action_pathify->setText("Pathify");
  action_pathify->setShortcut(Qt::CTRL + Qt::Key_E);

  QAction *action_settings = new QAction(this);
  action_settings->setText("Show settings");
  action_settings->setShortcut(Qt::CTRL + Qt::Key_P);
  action_settings->setCheckable(true);

  QAction *action_documentation = new QAction(this);
  action_documentation->setText("Documentation");

  QAction *action_fit = new QAction(this);
  action_fit->setText("Fit");
  action_fit->setShortcut(Qt::CTRL + Qt::Key_F);

  QAction *action_screenshot = new QAction(this);
  action_screenshot->setText("Take screenshot");
  action_screenshot->setShortcut(Qt::CTRL + Qt::Key_T);

  QAction *action_new = new QAction(this);
  action_new->setText("New window");
  action_new->setShortcut(Qt::CTRL + Qt::Key_N);

  QAction *action_open = new QAction(this);
  action_open->setText("Open");
  action_open->setShortcut(Qt::CTRL + Qt::Key_O);

  QAction *action_save = new QAction(this);
  action_save->setText("Save");
  action_save->setShortcut(Qt::CTRL + Qt::Key_S);

  QAction *action_save_as = new QAction(this);
  action_save_as->setText("Save as");
  action_save_as->setShortcut(Qt::SHIFT + Qt::CTRL + Qt::Key_S);

  QAction *action_close = new QAction(this);
  action_close->setText("Close");
  action_close->setShortcut(Qt::CTRL + Qt::Key_W);

  QAction *action_export = new QAction(this);
  action_export->setText("Export OBJ");

  // Toolbar
  QToolBar *toolbar = new QToolBar(this);
  toolbar->setMovable(false);
  addToolBar(Qt::TopToolBarArea, toolbar);
  toolbar->addAction(action_solidify);
  toolbar->addAction(action_pathify);
  toolbar->addAction(action_fit);

  // Status bar
  QStatusBar *statusBar = new QStatusBar(this);
  setStatusBar(statusBar);

  // Menubar
  QMenuBar *menuBar = new QMenuBar();
  setMenuBar(menuBar);

  QMenu *menuFile = new QMenu(menuBar);
  menuFile->setTitle("File");
  menuFile->addAction(action_open);
  menuFile->addAction(action_save);
  menuFile->addAction(action_save_as);
  menuFile->addAction(action_close);
  menuFile->addAction(action_new);
  menuFile->addSeparator();
  menuFile->addAction(action_export);

  QMenu *menuView = new QMenu(menuBar);
  menuView->setTitle("View");
  menuView->addAction(action_settings);
  menuView->addAction(action_screenshot);

  QMenu *menuHelp = new QMenu(menuBar);
  menuHelp->setTitle("Help");
  menuHelp->addAction(action_documentation);

  setWindowTitle("Madeup");
  menuFile->setTitle("File");
  menuView->setTitle("View");
  menuBar->addAction(menuFile->menuAction());
  menuBar->addAction(menuView->menuAction());
  menuBar->addAction(menuHelp->menuAction());

  // Events
  connect(settings_picker, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), settings_pager, &QStackedWidget::setCurrentIndex);
  connect(action_solidify, &QAction::triggered, this, &MupperWindow::onSolidify);
  connect(action_pathify, &QAction::triggered, this, &MupperWindow::onPathify);
  connect(action_fit, &QAction::triggered, this, &MupperWindow::onFit);
  connect(select_font_button, &QPushButton::clicked, this, &MupperWindow::selectFont);
  connect(editor, &QTextEdit::textChanged, this, &MupperWindow::onTextChanged);

  connect(face_picker, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
    canvas->makeCurrent(); // no effect without this
    canvas->getRenderer().setFaceOrientation(i);
    canvas->update();
  });

  connect(style_picker, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
    canvas->makeCurrent(); // no effect without this
    canvas->getRenderer().setFaceStyle(i);
    canvas->update();
  });

  connect(action_documentation, &QAction::triggered, [=]() {
    QDesktopServices::openUrl(QUrl("http://madeup.xyz/docs/introduction.html"));
  });

  connect(action_screenshot, &QAction::triggered, this, [=]() {
    canvas->makeCurrent();
    QString path = QFileDialog::getSaveFileName(this, "Save Screenshot", QDir::homePath(), "Images (*.png *.jpg *.gif)");
    if (!path.isEmpty()) {
      canvas->makeCurrent();
      canvas->getRenderer().takeScreenshot(path.toStdString());
    }
  });

  connect(action_open, &QAction::triggered, [=]() {
    QString path = QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "Madeup Programs (*.mup)");
    if (!path.isEmpty()) {
      mup_path = path;
      std::string source = td::Utilities::Slurp(path.toStdString());
      editor->setText(source.c_str());
      this->setWindowTitle(("Madeup: " + mup_path.toStdString()).c_str());
    }
  });

  connect(action_close, &QAction::triggered, [=]() {
    close();
  });

  connect(action_save, &QAction::triggered, [=]() {
    if (!mup_path.isEmpty()) {
      saveAs(mup_path); 
    } else {
      QString path = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Madeup Programs (*.mup)");
      if (!path.isEmpty()) {
        saveAs(path); 
      }
    }
  });

  connect(action_save_as, &QAction::triggered, [=]() {
    QString path = QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "Madeup Programs (*.mup)");
    if (!path.isEmpty()) {
      saveAs(path);
    }
  });

  connect(action_new, &QAction::triggered, [=]() {
    MupperWindow *new_window = new MupperWindow();
    // This window has no parent, so who's gonna delete it? Itself.
    new_window->setAttribute(Qt::WA_DeleteOnClose);
    new_window->show();
  });

  connect(action_export, &QAction::triggered, [=]() {
    QString path = QFileDialog::getSaveFileName(this, "Export File", QDir::homePath(), "Wavefront OBJ Files (*.obj)");
    if (!path.isEmpty()) {
      canvas->getRenderer().exportTrimesh(path.toStdString());
    }
  });

  connect(background_color_button, &QPushButton::clicked, [=](){
    selectColor(canvas->getRenderer().getBackgroundColor(), [=](const QColor &color) {
      canvas->makeCurrent(); // no effect without this
      canvas->getRenderer().setBackgroundColor(td::QVector4<float>(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f, color.alpha() / 255.0f));
      canvas->update();
      QPalette palette;
      palette.setColor(QPalette::Button, color);
      background_color_button->setPalette(palette);
    });
  });

  connect(path_color_button, &QPushButton::clicked, [=](){
    selectColor(canvas->getRenderer().getPathColor(), [=](const QColor &color) {
      this->canvas->makeCurrent(); // no effect without this
      this->canvas->getRenderer().setPathColor(td::QVector4<float>(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f, color.alpha() / 255.0f));
      this->canvas->update();
      QPalette palette;
      palette.setColor(QPalette::Button, color);
      path_color_button->setPalette(palette);
    });
  });

  connect(vertex_color_button, &QPushButton::clicked, [=](){
    selectColor(canvas->getRenderer().getVertexColor(), [=](const QColor &color) {
      this->canvas->makeCurrent(); // no effect without this
      this->canvas->getRenderer().setVertexColor(td::QVector4<float>(color.red() / 255.0f, color.green() / 255.0f, color.blue() / 255.0f, color.alpha() / 255.0f));
      this->canvas->update();
      QPalette palette;
      palette.setColor(QPalette::Button, color);
      vertex_color_button->setPalette(palette);
    });
  });

  connect(action_settings, &QAction::triggered, [=](bool is_checked) {
    settings_panel->setVisible(is_checked);
  });

  connect(show_console_checkbox, &QCheckBox::stateChanged, [=](int state) {
    console->setVisible(state == Qt::Checked);
  });

  connect(show_heading_checkbox, &QCheckBox::stateChanged, [=](int state) {
    canvas->getRenderer().showHeading(state == Qt::Checked);
    canvas->update();
  });

  connect(show_stops_checkbox, &QCheckBox::stateChanged, [=](int state) {
    canvas->getRenderer().showStops(state == Qt::Checked);
    canvas->update();
  });

  connect(path_stroke_width_spinner, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double d) {
    canvas->getRenderer().setPathStrokeWidth((float) d);
    canvas->update();
  });

  connect(vertex_size_spinner, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [=](double d) {
    canvas->getRenderer().setVertexSize((float) d);
    canvas->update();
  });

  // Tweaks
  editor->blockSignals(true);
  editor->setText("moveto 0, 0, 0\n"
                  "repeat 4\n"
                  "  move 10\n"
                  "  yaw 90\n"
                  "end\n"
                  "dowel\n");
  onTextChanged();
  setWindowTitle("Madeup");
  editor->blockSignals(false);
  /* settings_panel->setVisible(false); */
}

/* ------------------------------------------------------------------------- */

MupperWindow::~MupperWindow() {
}

/* ------------------------------------------------------------------------- */

void MupperWindow::onRun(GeometryMode::geometry_mode_t geometry_mode) {
  std::string source = editor->toPlainText().toStdString();

  std::stringstream cout_capturer;
  std::streambuf *old_cout_buffer = std::cout.rdbuf();
  std::cout.rdbuf(cout_capturer.rdbuf());

  std::stringstream in(source);
  Lexer lexer(in);
  try {
    const std::vector<Token> &tokens = lexer.lex();
    Parser parser(tokens, source);

    td::Co<ExpressionBlock> program = parser.program();

    Environment env;
    env.prime();
    env.setGeometryMode(geometry_mode);
    srand(time(NULL));
    rand();

    program->evaluate(env);

    if (geometry_mode == GeometryMode::PATH) {
      canvas->getRenderer().setPaths(env.getPaths());
    } else if (geometry_mode == GeometryMode::SURFACE) {
      td::Trimesh *trimesh = env.getMesh();
      trimesh->DisconnectFaces();
      if (trimesh->GetVertexCount() == 0) {
        std::cout << "Uh oh. You either didn't visit any locations or didn't invoke a solidifier. I can't make any models without more information from you." << std::endl;
      }
      canvas->getRenderer().setTrimesh(trimesh);
    }

  } catch (std::bad_alloc e) {
    std::cout << "Whoa! I don't have enough memory for that." << std::endl;
  } catch (td::MessagedException e) {
    std::cout << e.GetMessage() << std::endl;
  }

  std::cout.rdbuf(old_cout_buffer);
  console->setText(cout_capturer.str().c_str());

  canvas->update();
}

/* ------------------------------------------------------------------------- */

void MupperWindow::onTextChanged() {
  std::string source = editor->toPlainText().toStdString();

  if (mup_path.isEmpty()) {
    this->setWindowTitle("Madeup*");
  } else {
    this->setWindowTitle(("Madeup: " + mup_path.toStdString() + "*").c_str());
  }

  std::stringstream in(source);
  Lexer lexer(in);
  lexer.keepComments(true);

  try {
    lexer.lex();
  } catch (td::MessagedException e) {
    std::cerr << e.GetMessage() << std::endl;
  }

  const std::vector<Token> &tokens = lexer.getTokens();
  
  QTextCharFormat punctuation_format;
  punctuation_format.setForeground(Qt::blue);

  QTextCharFormat number_format;
  number_format.setForeground(QColor("#CF6A4C"));

  QTextCharFormat operator_format;
  operator_format.setForeground(QColor("#CDA869"));

  QTextCharFormat keyword_format;
  keyword_format.setForeground(QColor("#CDA869"));

  QTextCharFormat comment_format;
  comment_format.setForeground(QColor("#5F5A60"));

  QTextCharFormat identifier_format;
  identifier_format.setForeground(QColor("#7587A6"));

  QTextCharFormat string_format;
  string_format.setForeground(QColor("#8F9D6A"));

  QTextCharFormat comma_format;
  comma_format.setForeground(QColor("#FFFFFF"));

  editor->blockSignals(true);
  QTextCursor cursor(editor->document());

  // We skip the last two tokens (NEWLINE and END_OF_FILE) because they aren't
  // really in the original source.
  int ti = 0;
  for (auto token = tokens.begin(); ti + 2 < tokens.size(); ++token, ++ti) {
    SourceLocation location = token->getLocation();
    cursor.setPosition(location.getStartIndex(), QTextCursor::MoveAnchor);
    cursor.setPosition(location.getEndIndex() + 1, QTextCursor::KeepAnchor);

    switch (token->getType()) {
      case Token::COMMA:
      case Token::LEFT_PARENTHESIS:
      case Token::RIGHT_PARENTHESIS:
      case Token::LEFT_CURLY_BRACE:
      case Token::RIGHT_CURLY_BRACE:
      case Token::LEFT_BRACKET:
      case Token::RIGHT_BRACKET:
        cursor.setCharFormat(comma_format);
        break;
      case Token::INTEGER:
      case Token::REAL:
      case Token::TRUE:
      case Token::FALSE:
      case Token::NOTHING:
        cursor.setCharFormat(number_format);
        break;
      case Token::PLUS:
      case Token::COLON:
      case Token::MINUS:
      case Token::TIMES:
      case Token::DIVIDE:
      case Token::REAL_DIVIDE:
      case Token::CIRCUMFLEX:
      case Token::LESS_THAN:
      case Token::LESS_THAN_OR_EQUAL_TO:
      case Token::GREATER_THAN:
      case Token::GREATER_THAN_OR_EQUAL_TO:
      case Token::EQUAL_TO:
      case Token::NOT_EQUAL_TO:
      case Token::REMAINDER:
      case Token::ASSIGN:
      case Token::PIPE:
      case Token::RANGE:
        cursor.setCharFormat(operator_format);
        break;
      case Token::AND:
      case Token::IN:
      case Token::AROUND:
      case Token::THROUGH:
      case Token::OR:
      case Token::NOT:
      case Token::FOR:
      case Token::OF:
      case Token::BY:
      case Token::ELSE:
      case Token::IF:
      case Token::REPEAT:
      case Token::END:
      case Token::THEN:
      case Token::WHILE:
      case Token::TO:
        cursor.setCharFormat(keyword_format);
        break;
      case Token::COMMENT:
        cursor.setCharFormat(comment_format);
        break;
      case Token::ID:
        cursor.setCharFormat(identifier_format);
        break;
      case Token::STRING:
        cursor.setCharFormat(string_format);
        break;
      case Token::NEWLINE:
      case Token::END_OF_FILE:
        break;
    }
  }

  editor->blockSignals(false);

  if (canvas->isValid()) {
    onRun(GeometryMode::PATH);
  }
}

/* ------------------------------------------------------------------------- */

void MupperWindow::selectColor(const td::QVector4<float> &initial_color,
                               std::function<void(const QColor &)> onSelect) {
  QColorDialog *picker = new QColorDialog(toQColor(initial_color), this); 
  picker->setOption(QColorDialog::ShowAlphaChannel);
  picker->setOption(QColorDialog::NoButtons);
  connect(picker, &QColorDialog::currentColorChanged, onSelect);
  picker->setAttribute(Qt::WA_DeleteOnClose);
  picker->show();
}

/* ------------------------------------------------------------------------- */

void MupperWindow::selectFont() {
  QFontDialog *picker = new QFontDialog(editor->currentFont(), this); 
  picker->setOption(QFontDialog::NoButtons);
  connect(picker, &QFontDialog::currentFontChanged, [=](const QFont &font) {
    editor->setFont(font);
    console->setFont(font);
  });
  picker->setAttribute(Qt::WA_DeleteOnClose);
  picker->show();
}

/* ------------------------------------------------------------------------- */

void MupperWindow::onFit() {
  canvas->getRenderer().fit();
  canvas->update();
}

/* ------------------------------------------------------------------------- */

void MupperWindow::onSolidify() {
  onRun(GeometryMode::SURFACE);
}

/* ------------------------------------------------------------------------- */

void MupperWindow::onPathify() {
  onRun(GeometryMode::PATH);
}

/* ------------------------------------------------------------------------- */

QColor MupperWindow::toQColor(const td::QVector4<float> &color) {
  td::QVector4<int> tcolor(color * 255.0f);
  return QColor(tcolor[0], tcolor[1], tcolor[2], tcolor[3]);
}

/* ------------------------------------------------------------------------- */

void MupperWindow::saveAs(const QString &path) {
  mup_path = path;
  std::string source = editor->toPlainText().toStdString();
  std::ofstream out(path.toStdString());
  out << source;
  out.close();
  setWindowTitle(("Madeup: " + mup_path.toStdString()).c_str());
}

/* ------------------------------------------------------------------------- */

void MupperWindow::loadPreferences() {
  try {
    pt::ptree tree;
    pt::read_json(config_path, tree);
    loadPreferences(tree);
  } catch (boost::exception const &e) {
    std::cerr << "Couldn't load prefs." << std::endl;
  }
}

/* ------------------------------------------------------------------------- */

void MupperWindow::loadPreferences(const boost::property_tree::ptree &tree) {
  canvas->makeCurrent();

  float stroke_width = tree.get<float>("stroke_width", canvas->getRenderer().getPathStrokeWidth());
  canvas->getRenderer().setPathStrokeWidth(stroke_width);

  float vertex_size = tree.get<float>("vertex_size", canvas->getRenderer().getVertexSize());
  canvas->getRenderer().setVertexSize(vertex_size);

  string font_face = tree.get<string>("font.face", "Courier New");
  int font_size = tree.get<int>("font.size", 18);

  QFont font;
  font.setFamily(font_face.c_str());
  font.setPointSize(font_size);

  editor->setFont(font);
  console->setFont(font);

  canvas->update();
}

/* ------------------------------------------------------------------------- */

void MupperWindow::savePreferences() {
  try {
    pt::ptree tree;
    savePreferences(tree);
    pt::write_json(config_path, tree);
  } catch (boost::exception const &e) {
    std::cerr << "Couldn't save prefs." << std::endl;
  }
}

/* ------------------------------------------------------------------------- */

void MupperWindow::savePreferences(boost::property_tree::ptree &tree) {
   tree.put("font.size", editor->font().pointSize());
   tree.put("font.face", editor->font().family().toStdString());
   tree.put("stroke_width", canvas->getRenderer().getPathStrokeWidth());
   tree.put("vertex_size", canvas->getRenderer().getVertexSize());
}

/* ------------------------------------------------------------------------- */

void MupperWindow::showEvent(QShowEvent *event) {
  std::cout << "show" << std::endl;
  QMainWindow::showEvent(event);
  loadPreferences();
}

/* ------------------------------------------------------------------------- */

void MupperWindow::closeEvent(QCloseEvent *event) {
  std::cout << "close" << std::endl;
  savePreferences();
  QMainWindow::closeEvent(event);
}

/* ------------------------------------------------------------------------- */

