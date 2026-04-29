#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>

#include "../wrapper/wrapper.h"

class RetrogradeGUI : public QMainWindow {
    Q_OBJECT

public:
    RetrogradeGUI(QWidget *parent = nullptr);
    ~RetrogradeGUI();

private slots:
    void browseFile();
    void decompileFile();
    void clearOutput();
    void about();

private:
    void setupUI();
    void setupConnections();

    rg_context* context;
    QLabel* fileLabel;
    QLineEdit* fileEdit;
    QPushButton* browseBtn;
    QPushButton* decompileBtn;
    QPushButton* clearBtn;
    QProgressBar* progressBar;
    QTextEdit* outputEdit;
};
