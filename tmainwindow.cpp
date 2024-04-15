#include "tmainwindow.h"
#include "./ui_tmainwindow.h"
#include "tapi.h"
#include "thandler.h"

std::mutex taskMutex;

TMainWindow::TMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TMainWindow)
{
    ui->setupUi(this);
    readSettings();
}

TMainWindow::~TMainWindow()
{
    writeSettings();
    delete ui;
}

void TMainWindow::on_stoolButton_clicked()
{
    //open a dialog and read directionary to ui text
    QString DirName = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Directionary"), "", QFileDialog::ShowDirsOnly);
    sdir = QDir(DirName);
    //显示文件名
    ui->slineEdit->setText(DirName);
}


void TMainWindow::on_gtoolButton_clicked()
{
    //open a dialog and read directionary to ui text
    QString DirName = QFileDialog::getExistingDirectory(this,
                                                        tr("Open Directionary"), "", QFileDialog::ShowDirsOnly);
    gdir = QDir(DirName);
    //显示文件名
    ui->glineEdit->setText(DirName);
}


void TMainWindow::on_rbotton_clicked()
{
    //遍历并将结果输出至PlainText组件
    //保存为字符串列表
    sdir.setNameFilters(QStringList("*.yml"));
    sdir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    qDebug() << "Scanning: " << sdir.path();
    //read file list
    sfileList = sdir.entryInfoList();
    if(!sfileList.empty()){
        for (int i=0; i<sfileList.count(); i++)
        {
            //将读取结果输出
            ui->CommandBox->appendPlainText(">Found file: "+sfileList[i].fileName());
        }
        ui->CommandBox->appendPlainText(">Read success");
    }else{
        //提示目录为空
        informationMessage("源目录无可用资源文件");
    }

}


void TMainWindow::on_tbotton_clicked()
{
    if(!sfileList.empty()){
        writeFiles();
    }else{
        //提示目录为空
        informationMessage("源目录无可用资源文件");
    }

}

void TMainWindow::writeFiles(){
    // 解析用键值对
    std::map<std::string, std::string> source_map;
    std::map<std::string, std::string> goal_map;
    std::vector<std::map<std::string, std::string>> source_maps;
    //获取目标语言
    std::string sourceLanguage = ui->scomboBox->currentText().toStdString();
    std::string targetLanguage = ui->gcomboBox->currentText().toStdString();
    // 初始化curl库
    curl_global_init(CURL_GLOBAL_DEFAULT);
    //读取sfilelList 在gDir下生成对应.yml file
    int num = sfileList.count();
    int stages = 0;
    //empty the progress bar
    ui->downloadbar1->setValue(0);
    //read all files
    for (int i=0; i< num ; i++)
    {
        //english -> simp_chinese
        auto sname = sfileList[i].fileName();
        // generate gname
        // gDir + gname
        QDir sfpath = QDir(sdir.absolutePath()+"/"+sname);
        // parse_file_to_map sname->map
        source_map = parse_file_to_map(sfpath.absolutePath().toStdString());
        source_maps.push_back(source_map);
        stages += source_map.size();
        // read file
        // for second line (no blank line) key-value
        // generate a dictionary
    }
    //计算并设置步数
    ui->downloadbar1->setRange(0,stages);
    for (int i=0; i< num ; i++){
        auto gname = sfileList[i].fileName().replace("simp_chinese","english");
        std::string cstr = ">handing file: "+gname.toStdString();
        QDir gfpath = QDir(gdir.absolutePath()+"/"+gname);
        qDebug() << gfpath;
        progress_string(cstr);
        // translate map
        // parse_map_to_file map->gname
        // source_map -> goal_map BY translate API
        goal_map = translate_map(source_maps[i], sourceLanguage,targetLanguage);
        parse_map_to_file(goal_map,gfpath.absolutePath().toStdString());

    }

    curl_global_cleanup();
}

/// <summary>
/// parse a .yml file to map
/// which will be used as translate source
/// </summary>
/// <param name="filename">which file need to be translate</param>
/// <returns></returns>
std::map<std::string, std::string>
TMainWindow::parse_file_to_map(const std::string &filename) {
    // 解析文件 对每一行做特化
    // 注释正则表达式
    std::regex annotatePattern("^\\s*#.*\r?$");
    std::regex tokensPattern(
        "\\[[^\"\\]]*\\]|\\$[^$]+\\$|#[^$]+#!|\\\\n|@[^!]+!");
    std::string replace_token = "{@}";
    std::map<std::string, std::string> key_map;
    // 打开文件
    std::ifstream file(filename);
    // 第一行内容无视
    // 从第二行开始解析
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return key_map; // 返回空 map，表示发生错误
    }

    std::string line; // 行
    int line_num = 0;
    // 读取循环
    while (std::getline(file, line)) {
        if (line_num != 0) {
            // 无注释#的非空行视作合法
            if (line.size() > 1 && !std::regex_match(line, annotatePattern)) {
                ;
                // translate
                std::size_t pos = line.find("\"");
                std::string key = line.substr(0, pos - 1);
                std::string value = line.substr(pos);
                key_map[key] = value;
            }
        }
        line_num++;
    }
    // close file
    file.close();
    // 将非法字符替换为{@}
    /*
    for (const auto &pair : key_map) {
        // value 将 匹配模板 替换为 标记，之后替换回来
        // regex_search
        std::string value = pair.second;
        std::sregex_iterator words_begin(value.begin(), value.end(), tokensPattern);
        std::sregex_iterator words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            std::string match_str = match.str();
            tokens.push_back(match_str);
        }
        // regex_replace
        value = std::regex_replace(value, tokensPattern, replace_token);
        key_map[pair.first] = value;
    }*/

    return key_map;
}
/// <summary>
/// output a map to .yml file
/// </summary>
/// <param name="source_map">the source of map</param>
/// <param name="filename">the name of output file</param>
void TMainWindow::parse_map_to_file(const std::map<std::string, std::string> &source_map,
                                    const std::string &filename) {
    std::regex tokensPattern("\\{@\\}");
    std::ofstream ofs(filename);

    if (!ofs.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    // write UTF-8 BOM code
    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    ofs.write((char *)bom, sizeof(bom));
    // first line
    ofs << "l_english:" << std::endl;
    // other lines
    std::string value;
    for (const auto &pair : source_map) {
        value = pair.second;
        // remove all \n
        // found all {@} and replace them
        removeNewlines(value);
        size_t pos = value.find("{@}");

        // pair.second 回归替换
        ofs << pair.first << " " << value << std::endl;
    }
    // 计数置为0，清空数组
    tokens.clear();

    // close file
    ofs.close();
}

void write_to_map(std::map<std::string, std::string> &key_map, std::string key,
                  std::string tran_text) {
    std::lock_guard<std::mutex> guard(taskMutex);
    key_map[key] = tran_text;
}

std::map<std::string, std::string>
TMainWindow::translate_map(std::map<std::string, std::string> &source_map,
                           std::string source_language,std::string target_language) {
    // 隐式线程池
    std::vector<QFuture<void>> qt_pool;
    std::map<std::string, std::string> key_map;
    // 遍历map 翻译
    int number = 0;
    //获取api
    std::string key = getApiKey();
    for (auto &pair : source_map) {
        // key_map is shared data,which need mutex
        // id_translate_task(key_map,pair,target_language);
        // thread 传值不传递引用 使用std::ref解决
        QFuture<void> result = QtConcurrent::run(id_translate_task, std::ref(key_map), std::ref(pair),
                                                 source_language,target_language,key);
        qt_pool.push_back(std::move(result));
    }

    // 等待所有线程完成 - 异步条件
    for (auto &qt : qt_pool) {
        while(!qt.isFinished()){
            QApplication::processEvents();
            QThread::msleep(20);
        }
        //进度增加1
        ui->downloadbar1->setValue(ui->downloadbar1->value() + 1);
        number++;
    }

    ui->CommandBox->appendPlainText(">Completed!");

    return key_map;
}
void id_translate_task(std::map<std::string, std::string> &key_map,
                       std::pair<const std::string, std::string> &pair,
                       std::string source_language,std::string target_language,std::string apiKey) {
    // 过滤空格和{@}
    std::regex blankpattern("^\"[\\s]*\"$");
    std::regex specialPattern("\\\"[{@}]*\\\"");
    std::string text = pair.second;
    std::string tran_text = "\"\"";
    //translation task in background
    if (!std::regex_match(text, blankpattern) &&
        !std::regex_search(text, specialPattern)) {
        std::string tran_text = gptAPI(text,target_language,apiKey);
        write_to_map(key_map, pair.first, tran_text);
    } else {
        write_to_map(key_map, pair.first, text);
    }
    // 设置map
}
void TMainWindow::progress_string(std::string& str){
    QString qStr = QString::fromStdString(str);
    ui->CommandBox->appendPlainText(qStr);
}

void TMainWindow::on_spushButton_clicked()
{
    //获取源目录
    QString sdir = ui->slineEdit->text();
    QUrl url = QUrl::fromLocalFile(sdir);
    //打开源目录资源管理器视窗
    if(url.isValid()){
        QDesktopServices::openUrl(url);
    }else{
        //提示消息
        informationMessage("目录为空或不合法");
    }
}


void TMainWindow::on_gpushButton_clicked()
{
    //获取源目录
    QString gdir = ui->glineEdit->text();
    QUrl url = QUrl::fromLocalFile(gdir);
    //打开源目录资源管理器视窗
    if(url.isValid()){
        QDesktopServices::openUrl(url);
    }else{
        //提示消息
        informationMessage("目录为空或不合法");
    }
}

void TMainWindow::informationMessage(const QString& str){
    //提示消息
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示信息");
    msgBox.setText(str);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
}

std::string TMainWindow::getApiKey(){
    apiKey = ui->alineEdit->text().toStdString();
    return apiKey;
}

void TMainWindow::readSettings(){
    //int file setting
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "spanti", "translator");
    //load settings
    QString apiText = settings.value("ui/alineEdit","").toString();
    ui->alineEdit->setText(apiText);
    ui->slineEdit->setText(settings.value("ui/slineEdit","").toString());
    sdir = QDir(ui->slineEdit->text());
    ui->glineEdit->setText(settings.value("ui/glineEdit","").toString());
    gdir = QDir(ui->glineEdit->text());
    ui->scomboBox->setCurrentText(settings.value("ui/scomboBox","").toString());
    ui->gcomboBox->setCurrentText(settings.value("ui/gcomboBox","").toString());
}

void TMainWindow::writeSettings(){
    //int file setting
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "spanti", "translator");
    //save settings
    QString apiText = (ui->alineEdit) ? ui->alineEdit->text() : "";
    settings.setValue("ui/alineEdit",apiText);
    settings.setValue("ui/slineEdit",ui->slineEdit->text());
    settings.setValue("ui/glineEdit",ui->glineEdit->text());
    settings.setValue("ui/scomboBox",ui->scomboBox->currentText());
    settings.setValue("ui/gcomboBox",ui->gcomboBox->currentText());
}
