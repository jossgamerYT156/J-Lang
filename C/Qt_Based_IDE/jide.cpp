// the file that contains all the logic of J-Lang IDE (J-IDE)
// all the functions will be as better commented as Lilly can.

// we import our Libraries.
// custom jide.h library.
#include "jide.h"
#include "./ui_jide.h"
#include <QFile>
#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QDir>
#include <tuple>

// main function that draws the parent window.
JIDE::JIDE(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::JIDE)
{
  // we setup the UI
    ui->setupUi(this);

     // and it's contents.
    // Directly use the ui pointer to access widgets
    outputShow = ui->textBrowser;
    codeEditZone = ui->codeEdit;
    executeButton = ui->executeButton;
    buildButton = ui->buildButton;
    //we also connect the buttons
    connect(executeButton, &QPushButton::clicked, this, &JIDE::on_executeButton_clicked);
    connect(buildButton, &QPushButton::clicked, this, &JIDE::on_buildButton_clicked); // this button does nothing for now.
}

JIDE::~JIDE()
{
    delete ui;
}

// this calls the execute instructions/execute_jlang_code function, so we can execute our functions.
void JIDE::on_executeButton_clicked()
{
    QString jlang_code = codeEditZone->toPlainText();
    execute_jlang_code(jlang_code);
}

void JIDE::on_buildButton_clicked()
{
    QString jlang_code = codeEditZone->toPlainText();
    QString python_code = "import webbrowser\nfrom PySide6.QtWidgets import QApplication, QMessageBox\n\napp = QApplication([])\n\n";

    QStringList instructions = jlang_code.split("\n");
  // this is a map of our funcions.
    QMap<QString, QString> command_map {
        {"print", "print"},
        {"browser", "webbrowser.open"},
        {"notif", "self.show_notification"},
        {"warn", "self.show_warning"}
    };

    for (const QString &instruction : instructions) {
        QString instr = instruction.trimmed();
        if (instr.startsWith("*") || instr.startsWith("//")) {
            continue;
        }

        // this checks if the code we wrote contains valid J-Lang functions
        if (instr.contains("(") && instr.contains(")")) {
            QString command = instr.section('(', 0, 0).trimmed();
            QString args = instr.section('(', 1).section(')', 0, 0).trimmed();

            if (command_map.contains(command)) {
                python_code += command_map[command] + "(" + args + ")\n";
            } else {
                python_code += "# Unknown Instruction: " + command + "\n";
            }
        } else {
            python_code += "# Invalid instruction: " + instr + "\n";
        }
    }

    python_code += "app.exec()";

    QFile script_file("generated_script.py");
    if (script_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&script_file);
        out << python_code;
        script_file.close();

        QProcess process;
        process.setWorkingDirectory(QDir::currentPath());
        process.start("pyinstaller", QStringList() << "--onefile" << "generated_script.py");
        process.waitForFinished();

        if (process.exitCode() == 0) {
            outputShow->append("Build successful! Executable created.");
        } else {
            outputShow->append("Build failed. Please check the error messages.");
        }
    } else {
        outputShow->append("Failed to write the script file.");
    }
}


// here is the logic to execute the J-Lang code.
void JIDE::execute_jlang_code(const QString &jlang_code)
{
    QString result = execute_jlang_instructions(jlang_code);
    outputShow->append(result);
}

QString JIDE::execute_jlang_instructions(const QString &jlang_code)
{
    QStringList instructions = jlang_code.split("\n");
    QString output;

    // again, a map of our functions.
    QMap<QString, void (JIDE::*)(const QString&, QString&)> command_map {
        {"print", &JIDE::execute_print},
        {"browser", &JIDE::execute_browser},
        {"notif", &JIDE::execute_notif},
        {"warn", &JIDE::execute_warn},
        {"window", &JIDE::execute_window}
    };

    for (const QString &instruction : instructions) {
        QString instr = instruction.trimmed();
        if (instr.startsWith("*") || instr.startsWith("//")) {
            continue;
        }
      
        // we get the contents of the functions. 
        if (instr.contains("(") && instr.contains(")")) {
            QString command = instr.section('(', 0, 0).trimmed();
            QString args = instr.section('(', 1).section(')', 0, 0).trimmed();
            
            //if the function is valid, execute it
            if (command_map.contains(command)) {
                (this->*command_map[command])(args, output);
            } else {
              //if not, return an error.
                output += "Unknown Instruction: " + command + "\n";
            }
        } else {
            output += "Invalid instruction: " + instr + "\n";
        }
    }

    return output;
}

// the definitions of our custom J-Lang instructions.

void JIDE::execute_print(const QString &args, QString &output)
{
    QString text = args.trimmed().remove('"').remove("'");
    output += text + "\n";
}

void JIDE::execute_browser(const QString &args, QString &output)
{
    QString url = args.trimmed().remove('"').remove("'");
    QProcess::startDetached("xdg-open", QStringList() << url);
}

void JIDE::execute_notif(const QString &args, QString &output)
{
    auto [title, message] = parse_args(args);
    QMessageBox msg_box;
    msg_box.setWindowTitle(title);
    msg_box.setText(message);
    msg_box.setIcon(QMessageBox::Information);
    msg_box.exec();
}

void JIDE::execute_warn(const QString &args, QString &output)
{
    auto [title, message] = parse_args(args);
    QMessageBox msg_box;
    msg_box.setWindowTitle(title);
    msg_box.setText(message);
    msg_box.setIcon(QMessageBox::Critical);
    msg_box.exec();
}

void JIDE::execute_window(const QString &args, QString &output)
{
    auto [dimensions, title, content] = parse_window_args(args);
    QWidget *window = new QWidget();
    window->setGeometry(100, 100, dimensions[0].toInt(), dimensions[1].toInt());
    window->setWindowTitle(title);
    QVBoxLayout *layout = new QVBoxLayout();
    QLabel *label = new QLabel(content);
    layout->addWidget(label);
    window->setLayout(layout);
    window->show();
}

std::pair<QString, QString> JIDE::parse_args(const QString &args)
{
    QStringList parts = args.split(",");
    QString title = parts.size() > 0 ? parts[0].trimmed().remove('"').remove("'") : "";
    QString message = parts.size() > 1 ? parts[1].trimmed().remove('"').remove("'") : "";
    return {title, message};
}

std::tuple<QStringList, QString, QString> JIDE::parse_window_args(const QString &args)
{
    QStringList parts = args.split(",");
    QString dimensions_str = parts.size() > 0 ? parts[0].trimmed().remove("create([").remove("])") : "";
    QStringList dimensions = dimensions_str.split("-");
    QString title = parts.size() > 1 ? parts[1].trimmed().remove('"').remove("'") : "New Window";
    QString content = parts.size() > 2 ? parts[2].trimmed().remove('"').remove("'") : "";
    return {dimensions, title, content};
}

  // end of our custom functions!
 // this is extremely experimental, and MANY things won't work as expected.
// is LillyDev... can't expect much more.
