#ifndef TMAINWINDOW_H
#define TMAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QFileInfoList>
#include <QMap>
#include <QVector>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>
#include <mutex>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>


QT_BEGIN_NAMESPACE
namespace Ui {
class TMainWindow;
}
QT_END_NAMESPACE

class TMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TMainWindow(QWidget *parent = nullptr);
    ~TMainWindow();

private slots:

    void on_stoolButton_clicked();

    void on_gtoolButton_clicked();

    void on_rbotton_clicked();

    void on_tbotton_clicked();

    void on_spushButton_clicked();

    void on_gpushButton_clicked();

private:
    Ui::TMainWindow *ui;
    //some member
    QDir sdir,gdir;
    QFileInfoList sfileList;
    std::vector<std::string> tokens;
    // apiKey
    std::string apiKey;
    // 互斥量
    std::mutex taskMutex;
    // 选项读取设置
    void writeFiles();
    std::map<std::string, std::string>
    parse_file_to_map(const std::string &filename);
    void parse_map_to_file(const std::map<std::string, std::string> &source_map,
                           const std::string &filename);
    std::map<std::string, std::string>
    translate_map(std::map<std::string, std::string> &source_map,
                  std::string source_language,std::string target_language);
    //the function concerned about member
    void progress_string(std::string& str);
    //提示框
    void informationMessage(const QString& str);
    std::string getApiKey();
    //save and load settings based on QSetting Class
    void readSettings();
    void writeSettings();
};
void write_to_map(std::map<std::string, std::string> &key_map, std::string key,
                  std::string tran_text);
void id_translate_task(std::map<std::string, std::string> &key_map,
                       std::pair<const std::string, std::string> &pair,
                       std::string source_language,std::string target_language,std::string apiKey);

#endif // TMAINWINDOW_H
