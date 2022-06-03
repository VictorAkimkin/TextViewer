#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    ui->setupUi(this);
    // отключаем события мыши на textBrowser, в том числе (ctrl+mouse wheel)
    ui->textBrowser->setAttribute( Qt::WA_TransparentForMouseEvents );
    fsize=0;SBscale=1;searchF=NULL;
    showData=&MainWindow::showDataHex;
    setFieldSize=&MainWindow::setHexFieldSize;
}

MainWindow::~MainWindow()
{
    ResetAll();
    delete ui;
}
// Вызов вывода данных в текстовое поле при изменении размеров формы
void MainWindow::resizeEvent(QResizeEvent *event)
{
  if (!file.isOpen()) return;
  if (fsize)  (this->*showData)();
}
// обработка события нажатия клавиш
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!file.isOpen()) return;
    switch (event->key())
    {
    case Qt::Key_Down:
        on_verticalScrollBar_actionTriggered(QAbstractSlider::SliderSingleStepAdd);
    break;
    case Qt::Key_Up:
        on_verticalScrollBar_actionTriggered(QAbstractSlider::SliderSingleStepSub);
    break;
    case Qt::Key_PageDown:
        on_verticalScrollBar_actionTriggered(QAbstractSlider::SliderPageStepAdd);
    break;
    case Qt::Key_PageUp:
        on_verticalScrollBar_actionTriggered(QAbstractSlider::SliderPageStepSub);
    break;
    }
}
// Обработка события колеса прокрутки
void MainWindow::wheelEvent(QWheelEvent *event)
{
  if (!file.isOpen()) return;
  (this->*setFieldSize)();
  if (event->angleDelta().y()>0)  dpos-=size_w;
    else dpos+=size_w;
  if (dpos<0) dpos=0;
  // если условие dpos>=(fsize-size_w) поставить первым, то оно сработает как сравнение беззнаковых чисел из-за fsize
  if (dpos>=(fsize-size_w)) dpos=fsize-size_w;
  ui->verticalScrollBar->setSliderPosition(dpos/SBscale);
  (this->*showData)();
}
// Открываем файл
void MainWindow::on_action_triggered()
{
   // QDir::setCurrent("D:/tmp/");
    QString str=QFileDialog::getOpenFileName(this, "Открыть файл", FILE_PATH);
    if (str!="")
    {
        ResetAll();
        fname=str;
        file.setFileName(fname);
        if (file.open(QFile::ReadOnly))
        {
            fsize=file.size();
            if (fsize<0)
            {
             file.flush(); file.close();fsize=0;
             QMessageBox::warning(this, "Ошибка", "Не удается определить размер файла", QMessageBox::Ok);
             return;
            }
            ActivateAll();
            (this->*showData)();
            return;
        }
    }
    QMessageBox::warning(this, "Ошибка", "Не удалось открыть выбранный файл", QMessageBox::Ok);
}
// Закрываем файл
void MainWindow::on_action_2_triggered()
{
     ResetAll();
}
// Закрываем приложени
void MainWindow::on_action_3_triggered()
{
    close();
}

// Отработка перемещения вертикальной полосы прокрутки текстового поля
void MainWindow::on_verticalScrollBar_actionTriggered(int action)
{
    (this->*setFieldSize)();
    switch (action) {
    case QAbstractSlider::SliderSingleStepAdd:
        dpos+=size_w;
        if (dpos>=(fsize-size_w)) dpos=fsize-size_w;
        ui->verticalScrollBar->setSliderPosition(dpos/SBscale);

        break;
    case QAbstractSlider::SliderSingleStepSub:
        dpos-=size_w;
        if (dpos<0) dpos=0;
        ui->verticalScrollBar->setSliderPosition(dpos/SBscale);

        break;
    case QAbstractSlider::SliderPageStepAdd:
        dpos+=size_w*size_h;
        if (dpos>=(fsize-size_w)) dpos=fsize-size_w;
        ui->verticalScrollBar->setSliderPosition(dpos/SBscale);

        break;
    case QAbstractSlider::SliderPageStepSub:
        dpos-=size_w*size_h;
        if (dpos<0) dpos=0;
        ui->verticalScrollBar->setSliderPosition(dpos/SBscale);

        break;
    case QAbstractSlider::SliderMove:
        dpos=ui->verticalScrollBar->sliderPosition()*SBscale;
        if (dpos>=(fsize-size_w)) dpos=fsize-size_w;
        //ui->verticalScrollBar->setSliderPosition(dpos/SBscale);

        break;
    }
    (this->*showData)();
}
// Вывод надписи о программе
void MainWindow::on_action_6_triggered()
{
    QMessageBox::about(this, "О программе", "Программа для просмотра кода или текста любого файла\n(c) Акимкин Виктор");
}
// Поменять режим просмотра на шестнадцетеричный код
void MainWindow::on_action_hex_triggered()
{
    ui->action_hex->setChecked(true);
    ui->action_7->setChecked(false);
    showData=&MainWindow::showDataHex;
    setFieldSize=&MainWindow::setHexFieldSize;
    if (file.isOpen()) (this->*showData)();
}
// Поменять режим просмотра на строковый
void MainWindow::on_action_7_triggered()
{
    ui->action_7->setChecked(true);
    ui->action_hex->setChecked(false);
    showData=&MainWindow::showDataStr;
    setFieldSize=&MainWindow::setStrFieldSize;
    if (file.isOpen()) (this->*showData)();
}
// Открыть окно данны поискового запроса
void MainWindow::on_action_4_triggered()
{
    if (searchF) {searchF->dpos=this->dpos; searchF->setAddr(); searchF->show();searchF->activateWindow(); return;}
    searchF= new SearchForm(nullptr,this->dpos);
    connect (searchF, &SearchForm::startSearch, this, &MainWindow::searchInFile);
    connect (searchF, &SearchForm::CloseForm, this, &MainWindow::CloseSearchForm);
    searchF->show();
}

// Инициализация целочисленных данных и числа с плавающей точкой в зависимости от типа данных
void MainWindow::SearchData::initialize()
{
    switch (SearchData::d_type)
    {
        case SearchForm::Data_Type::int_dec_type:
             SearchData::integer_value=SearchData::DATA_String.toLongLong();
        break;
        case SearchForm::Data_Type::int_hex_type:
             SearchData::integer_value=SearchData::DATA_String.toLongLong(NULL,16);
        break;
        case SearchForm::Data_Type::float_type:
             SearchData::float_value=SearchData::DATA_String.toDouble();
        break;

    }
}

// Установка нулевого указателя на динамическую поисковую форму при закрытии этой формы
void MainWindow::CloseSearchForm()
{
    searchF=nullptr;
}

// Получаем строку шестнадцетеричных кодов входящего массива разделенных пробелами
QString MainWindow::getHexString(const char in_str[],int str_size,int field_size)
{
    QString add_str;
    QString tmp_str;
    tmp_str=in_str;
    char new_str [str_size+1];
    new_str[str_size]=0;
    for (int i=0;i<str_size;++i)
    {
        if ((unsigned char)in_str[i]<16)add_str+='0'; // добавление 0 к числу с одним разярдом
        add_str+= (QString::number((unsigned char)in_str[i], 16).toUpper()+" ");
    }
    for (int i=add_str.size();i< field_size*3;++i)add_str+=' ';// добавление пробелов в короткую строку
    add_str+=" | ";
    removeSpecialCharacters(tmp_str); // убираем служебные символы
    return add_str+tmp_str;//+QChar::Null;
}

// Вариант заполнения текстового поля шестнадцетеричными кодами
void MainWindow::showDataHex()
{
    (this->*setFieldSize)();
    int readnum=size_w;
    char buff[size_w+1];
    file.seek(dpos);// seek
    ui->statusbar->showMessage("Отображение данных с адреса: "+QString::number(dpos));
    ui->textBrowser->clear();
    for (int raw=0;(raw<size_h) && (readnum==size_w);++raw)
    {
        memset(buff,0,size_w+1);
        if(readnum=file.read(buff,size_w))
        {
           QString str= getHexString(buff, readnum,size_w);
           ui->textBrowser->append(str);
        }
    }
}

// Вариант заполнения текстового поля только строками
void MainWindow::showDataStr()
{
    (this->*setFieldSize)();
    int readnum=size_w;
    char buff[size_w+1];
    file.seek(dpos);// seek
    ui->statusbar->showMessage("Отображение данных с адреса: "+QString::number(dpos));
    ui->textBrowser->clear();
    for (int raw=0;(raw<size_h) && (readnum==size_w);++raw)
    {
        memset(buff,0,size_w+1);
        if(readnum=file.read(buff,size_w))
        {
           QString str(buff);
           removeSpecialCharacters(str);
           ui->textBrowser->append(str);
        }
    }

}
// Расчет размеров текстового поля при выводе строк
void MainWindow::setStrFieldSize()
{
    size_w=(ui->textBrowser->width()/WIDTH_DIVIDER-WIDTH_INDENT)*4;
    size_h=ui->textBrowser->height()/HEIGHT_DIVIDER;
}
// Расчет размеров тектового поля при выводе кодов символов
void MainWindow::setHexFieldSize()
{
    size_w=ui->textBrowser->width()/WIDTH_DIVIDER-WIDTH_INDENT;
    size_h=ui->textBrowser->height()/HEIGHT_DIVIDER;
}
// Удаление из строки спец символов
void MainWindow::removeSpecialCharacters(QString &str)
{
    for (auto& elem :str)
    {
        if (elem==QChar::Null || elem==QChar::Tabulation || elem==QChar::LineFeed || elem==QChar::FormFeed ||
            elem==QChar::CarriageReturn || elem==QChar::Space || elem==QChar::Nbsp || elem==QChar::SoftHyphen ||
            elem==QChar::ReplacementCharacter || elem==QChar::ObjectReplacementCharacter || elem==QChar::ByteOrderMark ||
            elem==QChar::ByteOrderSwapped || elem==QChar::ParagraphSeparator || elem==QChar::LineSeparator ||
            elem==QChar::LastValidCodePoint || elem<' ') elem=' ';
    }

}
// Проинициализировать все переменные и виджеты для работы с новым файлом данных
void MainWindow::ActivateAll()
{
    ui->verticalScrollBar->setSliderPosition(dpos/SBscale);
    ui->statusbar->showMessage("Размер файла "+QString::number(fsize));
    SBscale=fsize/1000000;
    if (!SBscale) SBscale=1;
    ui->verticalScrollBar->setMaximum(fsize/SBscale);
    ui->verticalScrollBar->setEnabled(true);
    ui->action_4->setEnabled(true);
    ui->action_2->setEnabled(true);
    ui->action_2->setText("Закрыть файл "+fname);
    setWindowTitle("Text Viewer - "+fname);
}
// Сбросить все переменные и виджеты в неактивное состояние
void MainWindow::ResetAll()
{
    if (file.isOpen()){ file.flush(); file.close();fsize=0;}
    ui->textBrowser->clear();
    if (searchF) {searchF->close();searchF=nullptr;}
    dpos=0;
    SBscale=1;
    ui->statusbar->clearMessage();
    ui->verticalScrollBar->setSliderPosition(0);
    ui->verticalScrollBar->setMaximum(1000000);
    ui->verticalScrollBar->setEnabled(false);
    ui->action_4->setEnabled(false);
    ui->action_2->setEnabled(false);
    ui->action_2->setText("Закрыть файл");
    setWindowTitle("Text Viewer");
}
// подкрасить блок текста
void MainWindow::PaintTextBlock(int begin, int end, QTextCursor cur, QBrush brash)
{
    QTextCharFormat  format;

    if (begin<0 ||end<0 || begin>=end) return;
    cur.setPosition(begin,QTextCursor::QTextCursor::MoveAnchor);
    cur.setPosition(end,QTextCursor::KeepAnchor);
    format.setBackground(brash);
    cur.setCharFormat(format);
}
// подкрасить блок текста справа при отображении кода в шестнадцетеричном фоомате
void MainWindow::PaintRightBlock(int size, QTextCursor cur, QBrush brash)
{
    int margin=size_w*3+3;
    if (size<size_w)
        PaintTextBlock(margin,margin+size,cur,brash);
       else
       {
         int odd_size=size-size_w;
          PaintTextBlock(margin,margin+size_w,cur,brash);
          margin=margin*2+size_w+1;
          PaintTextBlock(margin,margin+odd_size,cur,brash);
       }
}
// Возвращает количество байт для хранения данных
int MainWindow::TakeDataSize(SearchData &sd)
{
    int size=0;
    if (sd.d_type==SearchForm::Data_Type::string_type) size=sd.DATA_String.length()*sizeof(QChar);
    if (sd.d_type==SearchForm::Data_Type::int_dec_type || sd.d_type==SearchForm::Data_Type::int_hex_type)
        {
            QString num=QString::number(sd.integer_value,16);
            size=num.length()/2+num.length()%2;
         }
     if (sd.d_type==SearchForm::Data_Type::float_type)
        {
            float fl= sd.float_value;
            double dbl=sd.float_value;
            if (fl== sd.float_value) size=sizeof(float);// если число помещается в тип float, то размер числа как у float
            if (dbl==sd.float_value) size=sizeof(double);// иначе как у double
        }
     return size;
}

// Инициализация данных для поиска и запуск поиска данных в файле
void MainWindow::searchInFile(int_least64_t addr, QString value,  SearchForm::Data_Type type,bool char_register, bool equal_or_not)
{
    search_data.start_addr=addr;
    search_data.DATA_String=value;
    search_data.d_type=type;
    search_data.char_register=char_register;
    search_data.accordance_flag=equal_or_not;
    search_data.initialize();
    QBrush brush;
    if (search_data.accordance_flag) brush=Qt::red; else brush=Qt::yellow;
    if (DoSearch(search_data, file))
    {
        searchF->dpos=this->dpos; searchF->setAddr();(this->*showData)();
        //        подсветка найденого текста
        if (search_data.d_type==SearchForm::Data_Type::string_type)
        {
            if (ui->action_hex->isChecked()) PaintRightBlock(search_data.DATA_String.length(),ui->textBrowser->textCursor(),brush);
            else PaintTextBlock(0,search_data.DATA_String.length(),ui->textBrowser->textCursor(),brush);
        }
        else
        {
            int paint_block_size;
            if (ui->action_hex->isChecked()) paint_block_size=TakeDataSize(search_data)*3;
                else paint_block_size=TakeDataSize(search_data);
            PaintTextBlock(0,paint_block_size,ui->textBrowser->textCursor(),brush);
        }
    }
        else QMessageBox::about(this, "Ошибка поиска", "Значение не найдено");
}

// Выполнить поиск
bool MainWindow::DoSearch(SearchData& sd, QFile& targetfile)
{
  int_least64_t tmp_val=0;
  double tmp_float_val=0.0;
  int qchar_size=sizeof (QChar);
  int size=TakeDataSize(sd);
  if (!size) return true;
  int readnum=size;
  char buff[size];

  file.seek(sd.start_addr);// seek
// поиск целого числа
  if (sd.d_type==SearchForm::Data_Type::int_dec_type || sd.d_type==SearchForm::Data_Type::int_hex_type )
  {
      for (int_least64_t count=0;readnum==size;++count)
      {
          memset(buff,0,size);
          readnum=file.read(buff,size);
          if(readnum)
          {
              tmp_val=0;memcpy (&tmp_val,buff,readnum);
              if ((sd.accordance_flag^(tmp_val==sd.integer_value)))
              {
                  dpos=sd.start_addr+count;
                  return true;
              }
              file.seek(sd.start_addr+count+1);
          }
      }
  }
// поиск строки
  if (sd.d_type==SearchForm::Data_Type::string_type)
  for (int count=0;readnum==size;++count)
  {
      memset(buff,0,size+1);
      readnum=file.read(buff,size);
      if(readnum)
      {
         QString tmp_str(buff);
         // обрезаем строку на тот случай, когда считанные из файла символы имеют размер меньше чем QChar
         tmp_str.resize(sd.DATA_String.size());
         if (sd.char_register)
             {if ((sd.accordance_flag^(tmp_str==sd.DATA_String))) {dpos=sd.start_addr+count;return true;}}
             else
             {if ((sd.accordance_flag^(tmp_str.toUpper()==sd.DATA_String.toUpper()))) {dpos=sd.start_addr+count;return true;}}
         file.seek(sd.start_addr+count+1);
      }
  }
// поиск числа с плавающей точкой
    if (sd.d_type==SearchForm::Data_Type::float_type)
    {
        for (int_least64_t count=0;readnum==size;++count)
        {
            memset(buff,0,size);
            readnum=file.read(buff,size);
            if(readnum)
            {
                tmp_float_val=0.0;memcpy (&tmp_float_val,buff,readnum);
                if ((sd.accordance_flag^(tmp_val==sd.float_value)))
                {
                    dpos=sd.start_addr+count;
                    return true;
                }
                file.seek(sd.start_addr+count+1);
            }
        }
    }
  return false;
}
