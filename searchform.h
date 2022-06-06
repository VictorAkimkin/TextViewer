#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QCheckBox>

#define MAX_TEXT_LENGHT 40

namespace Ui {
class SearchForm;
}

class SearchForm : public QWidget
{
    Q_OBJECT

public:
    explicit SearchForm(QWidget *parent=nullptr,int_least64_t addr=0);
    ~SearchForm();
    void setAddr();
private:
    Ui::SearchForm *ui;
    int cursor_pos_1{0},cursor_pos_2{0};


public:
   // Ui::SearchForm *ui;
   int_least64_t dpos;
   enum Data_Type {int_dec_type=0,int_hex_type=1,float_type=2,string_type=3} data_type;

signals:
    void startSearch(int_least64_t addr, QString value, Data_Type type, bool char_register,bool equal_or_not);
    void CloseForm();

private slots:
    void on_pushButton_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_checkBox_3_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_radioButton_3_clicked();
    void on_plainTextEdit_textChanged();

    void on_plainTextEdit_2_textChanged();

    void on_pushButton_2_clicked();

private:
    void setAddr(int_least64_t addr, QPlainTextEdit* TextBox, bool isHex);
    QString checkHexString(QPlainTextEdit* TextBox);
    QString checkDecString(QPlainTextEdit* TextBox);
    QString checkTextStringMax(QPlainTextEdit* TextBox);
    QString checkFloatString(QPlainTextEdit* TextBox);
    void DataInitialization();
    void checkFloatMax(QPlainTextEdit* TextBox);
    void checkMax(QPlainTextEdit* TextBox);

    class MyTextEdit : public QTextEdit
    {
        //Q_OBJECT
    public:
        void keyPressEvent(QKeyEvent *event) override
           {
             if (event->key() == Qt::Key_Return)
             {
             //   login(); // or rather emit submitted() or something along this way
              }
             else
             {
                 QTextEdit::keyPressEvent(event);
              }
           }

    };
    MyTextEdit* Mte;

    //void checkMaxValue(QPlainTextEdit* TextBox,int_least64_t max);
};

#endif // SEARCHFORM_H
