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
    RetrogradeGUI(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupConnections();
        context = rg_create();
        rg_set_verbose(context, 0);
    }

    ~RetrogradeGUI() {
        if (context) {
            rg_destroy(context);
        }
    }

private slots:
    void browseFile() {
        QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Select Binary File"),
            "",
            tr("Binary Files (*.bin *.exe *.so *.dylib *.o);;All Files (*)")
        );
        
        if (!fileName.isEmpty()) {
            fileEdit->setText(fileName);
        }
    }

    void decompileFile() {
        QString filePath = fileEdit->text();
        if (filePath.isEmpty()) {
            QMessageBox::warning(this, tr("Error"), tr("Please select a file first."));
            return;
        }

        statusBar()->showMessage("Decompiling...");
        progressBar->show();
        progressBar->setValue(0);
        outputEdit->setText("Loading binary...");

        progressBar->setValue(20);

        QByteArray filePathBytes = filePath.toUtf8();
        rg_result* result = rg_decompile_file(context, filePathBytes.constData());

        progressBar->setValue(60);

        if (rg_get_error(result) != RG_OK) {
            progressBar->setValue(0);
            progressBar->hide();
            QString errorMsg = QString("Decompilation failed: %1")
                .arg(rg_get_error_string(result));
            statusBar()->showMessage(errorMsg);
            outputEdit->setText(errorMsg);
            rg_result_free(result);
            return;
        }

        progressBar->setValue(80);

        QString output = QString("Successfully decompiled: %1\n\n").arg(filePath);
        
        size_t functionCount = rg_get_function_count(result);
        for (size_t i = 0; i < functionCount; ++i) {
            rg_function* func = rg_get_function(result, i);
            if (!func) continue;

            output += QString("// Function: %1 @ 0x%2\n")
                .arg(rg_function_get_name(func))
                .arg(rg_function_get_address(func), 0, 16);
            output += QString("// Blocks: %1\n")
                .arg(rg_function_get_block_count(func));
            output += rg_function_get_pseudocode(func);
            output += "\n\n";
        }

        progressBar->setValue(100);
        progressBar->hide();
        statusBar()->showMessage(QString("Done - %1 functions").arg(functionCount));
        outputEdit->setText(output);
        
        rg_result_free(result);
    }

    void clearOutput() {
        outputEdit->clear();
        statusBar()->showMessage("Ready");
    }

    void about() {
        QMessageBox::about(this, tr("About Retrograde"),
            tr("Retrograde - Binary Decompiler\n\n"
               "A simple C++ decompiler with GUI\n"
               "Version: %1").arg(rg_version()));
    }

private:
    void setupUI() {
        setWindowTitle("Retrograde");
        resize(800, 600);

        fileLabel = new QLabel(tr("Select binary file:"));
        fileEdit = new QLineEdit();
        fileEdit->setPlaceholderText(tr("Choose a file..."));
        
        browseBtn = new QPushButton(tr("Browse"));
        decompileBtn = new QPushButton(tr("Decompile"));
        decompileBtn->setDefault(true);
        
        clearBtn = new QPushButton(tr("Clear"));
        
        progressBar = new QProgressBar();
        progressBar->hide();
        
        outputEdit = new QTextEdit();
        outputEdit->setPlaceholderText(tr("Decompiled output will appear here..."));
        outputEdit->setAcceptRichText(false);

        QHBoxLayout* fileLayout = new QHBoxLayout();
        fileLayout->addWidget(fileLabel);
        fileLayout->addWidget(fileEdit);
        fileLayout->addWidget(browseBtn);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(decompileBtn);
        buttonLayout->addWidget(clearBtn);
        buttonLayout->addStretch();

        QVBoxLayout* mainLayout = new QVBoxLayout();
        mainLayout->addLayout(fileLayout);
        mainLayout->addLayout(buttonLayout);
        mainLayout->addWidget(progressBar);
        mainLayout->addWidget(outputEdit);

        QWidget* centralWidget = new QWidget();
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);

        QMenuBar* menuBar = this->menuBar();
        
        QMenu* fileMenu = menuBar->addMenu(tr("&File"));
        QAction* openAction = fileMenu->addAction(tr("&Open..."));
        openAction->setShortcut(QKeySequence::Open);
        connect(openAction, SIGNAL(triggered()), this, SLOT(browseFile()));
        
        fileMenu->addSeparator();
        QAction* exitAction = fileMenu->addAction(tr("E&xit"));
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

        QMenu* helpMenu = menuBar->addMenu(tr("&Help"));
        QAction* aboutAction = helpMenu->addAction(tr("&About"));
        connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

        statusBar()->showMessage("Ready");
    }

    void setupConnections() {
        connect(browseBtn, SIGNAL(clicked()), this, SLOT(browseFile()));
        connect(decompileBtn, SIGNAL(clicked()), this, SLOT(decompileFile()));
        connect(clearBtn, SIGNAL(clicked()), this, SLOT(clearOutput()));
    }

private:
    rg_context* context;
    QLabel* fileLabel;
    QLineEdit* fileEdit;
    QPushButton* browseBtn;
    QPushButton* decompileBtn;
    QPushButton* clearBtn;
    QProgressBar* progressBar;
    QTextEdit* outputEdit;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("Retrograde");
    app.setApplicationVersion("0.1.0");
    
    RetrogradeGUI window;
    window.show();
    
    return app.exec();
}

