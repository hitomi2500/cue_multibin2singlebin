#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setFocus();
    activateWindow();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open multi-bin cue file"), tr("."), tr("Image Files (*.cue)"));
    if ((fileName.isEmpty() == false) && (fileName.isNull() == false) )
    {
        ui->lineEdit->setText(fileName);
        QString fileName2 = fileName;
        fileName2.truncate(fileName2.length()-4);
        fileName2.append("_mixed.cue");
        ui->lineEdit->setText(fileName);
        ui->lineEdit_2->setText(fileName2);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open multi-bin cue file"), ".", tr("Image Files (*.cue)"));
    if ((fileName.isEmpty() == false) && (fileName.isNull() == false) )
        ui->lineEdit_2->setText(fileName);
}

void MainWindow::on_pushButton_gen_clicked()
{
    int iPosStart,iPosSize;
    int iPosition = 0;
    int iPos;
    char c[2];

    c[1] = '\0';

    if ( (ui->lineEdit->text().isEmpty() == false) && (ui->lineEdit_2->text().isEmpty() == false) )
    {
        //getting output bin filename
        QString out_filename = ui->lineEdit_2->text();
        QString out_filename_bin = out_filename;
        QString out_filename_bin_nopath;
        QString in_filename_bin;
        QString in_path;
        out_filename_bin.truncate(out_filename.length()-3);
        out_filename_bin.append("bin");
        in_path = ui->lineEdit->text();
        in_path = in_path.left(in_path.lastIndexOf("/")+1);
        QFile f_in(ui->lineEdit->text());
        QFile f_in_bin;
        QFile f_out(out_filename);
        QFile f_out_bin(out_filename_bin);
        f_in.open(QIODevice::ReadOnly);
        f_out.open(QIODevice::Truncate|QIODevice::WriteOnly);
        f_out_bin.open(QIODevice::Truncate|QIODevice::WriteOnly);
        QByteArray inputData = f_in.readAll();
        QByteArray in2=inputData;
        QByteArray binary_data;
        //parsing first record
        iPosStart = in2.indexOf("FILE")+6;
        iPosSize = in2.mid(iPosStart).indexOf("\"");
        in_filename_bin = in2.mid(iPosStart,iPosSize);
        in_filename_bin.prepend(in_path);
        f_in_bin.setFileName(in_filename_bin);
        if (false == f_in_bin.open(QIODevice::ReadOnly))
        {
            QMessageBox msgBox;
            msgBox.setText(QString("Cannot open file %1").arg(in_filename_bin));
            int ret = msgBox.exec();
        }
        binary_data = f_in_bin.readAll();
        f_in_bin.close();
        iPosition += binary_data.size();
        f_out_bin.write(binary_data);
        f_out.write(in2.left(in2.indexOf("FILE")));
        out_filename_bin_nopath = out_filename_bin.mid(out_filename_bin.lastIndexOf("/")+1);
        f_out.write(QString("FILE \"%1\" BINARY\r\n").arg(out_filename_bin_nopath).toLatin1());
        in2 = in2.mid(in2.indexOf("BINARY")+8);
        //other records
        while (in2.contains("FILE"))
        {
            iPosStart = in2.indexOf("FILE")+6;
            iPosSize = in2.mid(iPosStart).indexOf("\"");
            in_filename_bin = in2.mid(iPosStart,iPosSize);
            in_filename_bin.prepend(in_path);

            f_in_bin.setFileName(in_filename_bin);
            if (false == f_in_bin.open(QIODevice::ReadOnly))
            {
                QMessageBox msgBox;
                msgBox.setText(QString("Cannot open file %1").arg(in_filename_bin));
                int ret = msgBox.exec();
            }
            binary_data = f_in_bin.readAll();
            f_in_bin.close();
            f_out_bin.write(binary_data);
            f_out.write(in2.left(in2.indexOf("FILE")));
            in2 = in2.mid(in2.indexOf("BINARY")+8);

            //patch index00
            iPos = in2.indexOf("INDEX 00");
            c[0] = ('0'+(((((iPosition/2352)/75)/60)%60)/10));
            in2.replace(iPos+9,1,c);
            c[0] = ('0'+(((((iPosition/2352)/75)/60)%60)%10));
            in2.replace(iPos+10,1,c);
            c[0] = ('0'+((((iPosition/2352)/75)%60)/10));
            in2.replace(iPos+12,1,c);
            c[0] = ('0'+((((iPosition/2352)/75)%60)%10));
            in2.replace(iPos+13,1,c);
            c[0] = ('0'+(((iPosition/2352)%75)/10));
            in2.replace(iPos+15,1,c);
            c[0] = ('0'+(((iPosition/2352)%75)%10));
            in2.replace(iPos+16,1,c);
            //get gap size
            iPos = in2.indexOf("INDEX 01");
            int iPositionGap = iPosition + (in2.at(iPos+13) - (char)'0')*2352*75;
            //patch gap
            c[0] = ('0'+(((((iPositionGap/2352)/75)/60)%60)/10));
            in2.replace(iPos+9,1,c);
            c[0] = ('0'+(((((iPositionGap/2352)/75)/60)%60)%10));
            in2.replace(iPos+10,1,c);
            c[0] = ('0'+((((iPositionGap/2352)/75)%60)/10));
            in2.replace(iPos+12,1,c);
            c[0] = ('0'+((((iPositionGap/2352)/75)%60)%10));
            in2.replace(iPos+13,1,c);
            c[0] = ('0'+(((iPositionGap/2352)%75)/10));
            in2.replace(iPos+15,1,c);
            c[0] = ('0'+(((iPositionGap/2352)%75)%10));
            in2.replace(iPos+16,1,c);

            iPosition += binary_data.size();

        }
/*
        //last patch
        //patch index00
        iPos = in2.indexOf("INDEX 00");
        c[0] = ('0'+(((((iPosition/2352)/75)/60)%60)/10));
        in2.replace(iPos+9,1,c);
        c[0] = ('0'+(((((iPosition/2352)/75)/60)%60)%10));
        in2.replace(iPos+10,1,c);
        c[0] = ('0'+((((iPosition/2352)/75)%60)/10));
        in2.replace(iPos+12,1,c);
        c[0] = ('0'+((((iPosition/2352)/75)%60)%10));
        in2.replace(iPos+13,1,c);
        c[0] = ('0'+(((iPosition/2352)%75)/10));
        in2.replace(iPos+15,1,c);
        c[0] = ('0'+(((iPosition/2352)%75)%10));
        in2.replace(iPos+16,1,c);
        //get gap size
        iPos = in2.indexOf("INDEX 01");
        int iPositionGap = iPosition + (in2.at(iPos+13) - (char)'0')*2352*75;
        //patch gap
        c[0] = ('0'+(((((iPositionGap/2352)/75)/60)%60)/10));
        in2.replace(iPos+9,1,c);
        c[0] = ('0'+(((((iPositionGap/2352)/75)/60)%60)%10));
        in2.replace(iPos+10,1,c);
        c[0] = ('0'+((((iPositionGap/2352)/75)%60)/10));
        in2.replace(iPos+12,1,c);
        c[0] = ('0'+((((iPositionGap/2352)/75)%60)%10));
        in2.replace(iPos+13,1,c);
        c[0] = ('0'+(((iPositionGap/2352)%75)/10));
        in2.replace(iPos+15,1,c);
        c[0] = ('0'+(((iPositionGap/2352)%75)%10));
        in2.replace(iPos+16,1,c);
*/

        f_out.write(in2);
        f_in.close();
        f_out.close();
        f_out_bin.close();
    }
}
