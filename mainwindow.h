#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define WIDTH_DIVIDER 30
#define WIDTH_INDENT 0
#define HEIGHT_DIVIDER 16
#define FILE_PATH "D:/temp/"

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>

//#include <QtCore5Compat/QTextCodec>
//#include <QTextBrowser>
//#include <QScrollBar>
#include "searchform.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(QString input_filename, QWidget *parent = nullptr);
    ~MainWindow();

private:
        struct SearchData
        {
            int_least64_t start_addr=0;
            QString DATA_String="";
            int_least64_t integer_value=0;
            double float_value=0.0;
            SearchForm::Data_Type d_type=SearchForm::int_dec_type;
            bool char_register=true;
            bool accordance_flag=true;
            void initialize();
        };

protected:
   void resizeEvent(QResizeEvent *event) override;
   void keyReleaseEvent(QKeyEvent *event)override;
   void wheelEvent(QWheelEvent *event)override;
//    void QTextBrowser::wheelEvent(QMouseEvent *e) override;

private slots:
    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_verticalScrollBar_actionTriggered(int action);

    void on_action_6_triggered();

    void on_action_hex_triggered();

    void on_action_7_triggered();

    void on_action_4_triggered();

    void on_action_5_triggered();

public slots:
    void searchInFile(int_least64_t addr, QString value, SearchForm::Data_Type type, bool char_register,bool equal_or_not);
    void CloseSearchForm();
private:
    void FileOpening();
    void showDataHex();
    void showDataStr();
    void setHexFieldSize();
    void setStrFieldSize();
    void removeSpecialCharacters(QString &str);
    void ActivateAll();
    void ResetAll();
    bool DoSearch(SearchData& sd, QFile& file);
    int TakeDataSize(SearchData& sd);
    void PaintTextBlock(int begin, int end , QTextCursor cur, QBrush brash);
    void PaintRightBlock(int size , QTextCursor cur, QBrush brash);
    QString getHexString (const char in_str[],int str_size,int size_w);

private:
    Ui::MainWindow *ui;
    QString fname;
    int_least64_t dpos;
    uint SBscale;
    int size_w,size_h;
    QFile file;
    int_least64_t fsize;
    typedef void (MainWindow::*pf)();
    pf showData, setFieldSize;
    SearchForm *searchF;
    SearchData search_data;
};
#endif // MAINWINDOW_H
