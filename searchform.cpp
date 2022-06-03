#include "searchform.h"
#include "ui_searchform.h"

SearchForm::SearchForm(QWidget *parent,int_least64_t addr) :
    QWidget(parent),
    ui(new Ui::SearchForm)
{
    //this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    ui->plainTextEdit->setPlainText(QString::number(addr));

}

SearchForm::~SearchForm()
{
    emit CloseForm();
    delete ui;
}
// Нажатие кнопки "Искать"
void SearchForm::on_pushButton_clicked()
{
    DataInitialization();
    emit startSearch(dpos, ui->plainTextEdit_2->toPlainText(),data_type,ui->checkBox_4->isChecked(),ui->checkBox->isChecked());
}
// Нажатие кнопки "Искать далее"
void SearchForm::on_pushButton_2_clicked()
{
    DataInitialization();
    emit startSearch(dpos+1, ui->plainTextEdit_2->toPlainText(),data_type,ui->checkBox_4->isChecked(),ui->checkBox->isChecked());
}
// отработка смены значений radioButton и checkBox на формме поиска
void SearchForm::on_radioButton_clicked()
{
    ui->radioButton->setChecked(true);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setChecked(false);
    ui->checkBox_2->setEnabled(true);
    ui->checkBox_4->setEnabled(false);
    SearchForm::on_plainTextEdit_2_textChanged();
}
void SearchForm::on_radioButton_2_clicked()
{
    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(true);
    ui->radioButton_3->setChecked(false);
    ui->checkBox_2->setEnabled(false);
    ui->checkBox_4->setEnabled(true);
    SearchForm::on_plainTextEdit_2_textChanged();
}
void SearchForm::on_radioButton_3_clicked()
{
    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setChecked(true);
    ui->checkBox_2->setEnabled(false);
    ui->checkBox_4->setEnabled(false);
    SearchForm::on_plainTextEdit_2_textChanged();
}
void SearchForm::on_checkBox_3_stateChanged(int arg1)
{
    long long addr;
    QString str=ui->plainTextEdit->toPlainText();
    if (ui->checkBox_3->isChecked()) addr=str.toLongLong();
       else {str.remove(0,2); addr=str.toLongLong(NULL,16);}
    setAddr(addr,ui->plainTextEdit,ui->checkBox_3->isChecked());
}
void SearchForm::on_checkBox_2_stateChanged(int arg1)
{
    long long value;
    QString str=ui->plainTextEdit_2->toPlainText();
    if (ui->checkBox_2->isChecked()) value=str.toLongLong();
     else {str.remove(0,2);value=str.toLongLong(NULL,16); }
    setAddr(value,ui->plainTextEdit_2,ui->checkBox_2->isChecked());
}
void SearchForm::on_plainTextEdit_textChanged()
{
    if (ui->checkBox_3->isChecked())
    {
        QString str=checkHexString(ui->plainTextEdit);
        if (str!="") ui->plainTextEdit->setPlainText(str);
          else CheckMax(ui->plainTextEdit);
    } else {
        QString str=checkDecString(ui->plainTextEdit);
        if (str!="") ui->plainTextEdit->setPlainText(str);
          else CheckMax(ui->plainTextEdit);
    }
    ui->plainTextEdit->moveCursor(QTextCursor::End);
}

void SearchForm::on_plainTextEdit_2_textChanged()
{
    if (ui->radioButton->isChecked())
    {
        if (ui->checkBox_2->isChecked())
        {
            QString str=checkHexString(ui->plainTextEdit_2);
            if (str!="") ui->plainTextEdit_2->setPlainText(str);
              else CheckMax(ui->plainTextEdit_2);
        }
        else {
            QString str=checkDecString(ui->plainTextEdit_2);
            if (str!="") ui->plainTextEdit_2->setPlainText(str);
              else CheckMax(ui->plainTextEdit_2);
        }
    }
    if (ui->radioButton_3->isChecked())
    {
        QString str=checkFloatString(ui->plainTextEdit_2);
        if (str!="") ui->plainTextEdit_2->setPlainText(str);
          else CheckFloatMax(ui->plainTextEdit_2);
    }
    if (ui->radioButton_2->isChecked())
    {
        QString str=checTextStringMax(ui->plainTextEdit_2);
        if (str!="") ui->plainTextEdit_2->setPlainText(str);
    }
    ui->plainTextEdit_2->moveCursor(QTextCursor::End);
}
// инициализация значений адреса начала пойиска и типа данных для поиска
void SearchForm::DataInitialization()
{
    int_least64_t addr;
    if (ui->checkBox_3->isChecked()) addr=ui->plainTextEdit->toPlainText().toLongLong(NULL,16);
     else addr=ui->plainTextEdit->toPlainText().toLongLong();
    Data_Type d_type=Data_Type::int_dec_type;
    if (ui->radioButton->isChecked()&& ui->checkBox_2->isChecked()) d_type=Data_Type::int_hex_type;
    if (ui->radioButton_2->isChecked())  d_type=Data_Type::string_type;
    if (ui->radioButton_3->isChecked())  d_type=Data_Type::float_type;
    data_type= d_type;
    dpos=addr;
}
// запись адреса поиска в текстовое поле формы
void SearchForm::setAddr()
{
    setAddr(dpos,ui->plainTextEdit,ui->checkBox_3->isChecked());
}
// запись целого числа в указаное текстовое поле в шестнадцетеричном или десятичном формате
void SearchForm::setAddr(int_least64_t addr, QPlainTextEdit* TextBox, bool isHex)
{
    if (isHex) {TextBox->setPlainText("0x"+(QString::number(addr,16)).toUpper());}
    else TextBox->setPlainText(QString::number(addr));
}
// проверка того, что строка это шестнадцетеричное число
QString SearchForm::checkHexString(QPlainTextEdit *TextBox)
{
    QString str=TextBox->toPlainText();
    if (str.length()<3) return "0x0";
    if (str[2].toUpper()=='X') {QChar c=str[0];str.remove(0,1);str.insert(2,c);}
    for (int i=0; i<str.length();++i)
    {
        if (!(!(str[i].toUpper()<'A' ||str[i].toUpper()>'F') || !(str[i]<'0' || str[i]>'9')|| (str[i].toUpper()=='X'&& i==1 &&str[0]=='0')|| (str[i].toUpper()=='X' &&!i)))
           str.remove(i--,1);
    }
    if (str[0].toUpper()=='X') str.insert(0,'0');
    for(auto &elem:str) elem=elem.toUpper();
    if (str[1].toUpper()=='X') str[1]='x';
     else str.insert(0,"0x");
    if (TextBox->toPlainText()!=str) return str;
    else return "";
}
// проверка того, что строка это десятичное число
QString SearchForm::checkDecString(QPlainTextEdit *TextBox)
{
    QString str=TextBox->toPlainText();
    int i;
    for (i=0; i<str.length() && str[i]!='.';++i);
    if (i!=str.length()) str.remove(i,str.length()-i);
    for (int i=0; i<str.length();++i)
    {
        if (str[i]<'0' || str[i]>'9')
           str.remove(i--,1);
    }
    if (!str.length()) return "0";
    if (TextBox->toPlainText()!=str) return str;
    else return "";
}
// проверка того, что строка это число с плавающей точкой
QString SearchForm::checkFloatString(QPlainTextEdit *TextBox)
{   int i;
    QString str=TextBox->toPlainText();

    for (i=0; i<str.length();++i)
    {
        if ((str[i]<'0' || str[i]>'9')&& str[i]!='.'&&str[i]!=',')
           str.remove(i--,1);
    }
    if (!str.length())return "0.0";
    str.replace(',','.'); // замена запятых на точки
    if (str[0]=='.') str='0'+str;
    while (str.length()>1) { if (str[0]=='0'&& str[1]=='0') str.remove(0,1); else break; }// удаление лишних нулей в начале
    for (i=0; i<str.length() && str[i]!='.';++i);// поиск первой точки и
    if (i==str.length()) return str+".0";// превращение целого в дробное
    for (++i; i<str.length();++i) if (str[i]=='.')str.remove(i--,1);// удаление лишних точек
    if (TextBox->toPlainText()!=str) return str;
    else return "";
}
// проверка того, что целое число не превышает максимально возможное
void SearchForm::CheckMax(QPlainTextEdit *TextBox)
{
    QString old_str=TextBox->toPlainText();
    QString new_str=old_str;
    int_least64_t val;
    if (new_str.length()<2) return;
    if (new_str[0]=='0'&&new_str[1]=='x')
    {
        new_str.remove(0,2);
        val=new_str.toLongLong(NULL,16);
        new_str=QString::number(val,16).toUpper();
        new_str.insert(0,'x');
        new_str.insert(0,'0');
        if (new_str!=old_str) TextBox->setPlainText(old_str.remove(2,1));
    }
        else
        {
            val=new_str.toLongLong();
            new_str=QString::number(val);
            if (new_str!=old_str) TextBox->setPlainText(old_str.remove(0,1));
        }
}
// проверка того, что целое число с плавающей точкой не превышает максимально возможное
void SearchForm::CheckFloatMax(QPlainTextEdit *TextBox)
{
    QString old_str=TextBox->toPlainText();
    QString new_str=old_str;
    double val;
    int i,lenght,precision;
    lenght=new_str.length();
    for (i=0; i<lenght && new_str[i]!='.';++i);// поиск первой точки
    precision=lenght-i-1;
    val=new_str.toDouble();
    new_str=QString::number(val,'f',precision);
    if (new_str!=old_str)
    {
        if (old_str[lenght-2]=='.')
        {
           old_str[lenght-1]='0';
           TextBox->setPlainText(old_str.remove(lenght-3,1));
        }
    else
        {
            TextBox->setPlainText(old_str.remove(lenght-1,1));
        }
     }
}
// проверка того, что строка не превышает допустимую длину
QString SearchForm::checTextStringMax(QPlainTextEdit *TextBox)
{
    QString str=TextBox->toPlainText();
    if (str.length()> MAX_TEXT_LENGHT) return str.remove(str.length()-1,1);
    return "";
}
