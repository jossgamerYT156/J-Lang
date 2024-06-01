// jide.h
// definitions of custom vars and needed functions to save space in main files.

#ifndef JIDE_H
#define JIDE_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE
namespace Ui { class JIDE; }
QT_END_NAMESPACE

class JIDE : public QMainWindow
{
    Q_OBJECT

public:
    JIDE(QWidget *parent = nullptr);
    ~JIDE();

private slots:
    void on_executeButton_clicked();
    void on_buildButton_clicked();

private:
    Ui::JIDE *ui;
    QTextBrowser *outputShow;
    QPlainTextEdit *codeEditZone;
    QPushButton *executeButton;
    QPushButton *buildButton;

    void execute_jlang_code(const QString &jlang_code);
    QString execute_jlang_instructions(const QString &jlang_code);
    void execute_print(const QString &args, QString &output);
    void execute_browser(const QString &args, QString &output);
    void execute_notif(const QString &args, QString &output);
    void execute_warn(const QString &args, QString &output);
    void execute_window(const QString &args, QString &output);

    std::pair<QString, QString> parse_args(const QString &args);
    std::tuple<QStringList, QString, QString> parse_window_args(const QString &args);
};

#endif // JIDE_H
