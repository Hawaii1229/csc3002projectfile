#include "login.h"
#include "ui_login.h"
#include "mainwindow_toc.h"
#include "mainwindow_tob.h"
#include "signup.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QIntValidator>

// 登录界面
Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("Login");

    // 设置password输入框只能输入数字
    ui->password->setValidator(new QIntValidator(ui->password));
    ui->password->setPlaceholderText("Only numbers"); // 设置提示
    ui->username->setPlaceholderText("Can't be pure numbers");
    ui->password->setEchoMode(QLineEdit::Password); // 设置输入的显示方式

    // 连接数据库
    m_dbLogin = QSqlDatabase::addDatabase("QSQLITE");
    m_dbLogin.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dbLogin.open()) {
        qDebug() << "Database open error:" << m_dbLogin.lastError();
    }
}

Login::~Login()
{
    delete ui;
    m_dbLogin.close();
}

// 按下登录按钮后触发的槽函数
void Login::on_login_clicked()
{
    QSqlQuery query(m_dbLogin);
    query.prepare("SELECT username, password, type FROM users WHERE username = :username");
    query.bindValue(":username", ui->username->text());
    query.exec();

    bool t_bExisted = query.first();
    // 用户名不存在时
    if (!t_bExisted) {
        QMessageBox::warning(this, "Fail", "ID does not exist!", QMessageBox::Ok);
        ui->username->clear();
        ui->password->clear();
        ui->username->setFocus();
    }
    // 用户名存在且密码正确时
    else if (query.value("password") == ui->password->text().toInt()) {
        this->close();
        if (query.value("type") == "customer") {
            MainWindow_toc *t_ptrMainWindow = new MainWindow_toc(ui->username->text(), this);
            t_ptrMainWindow->show();
        } else if (query.value("type") == "business") {
            MainWindow_tob *t_ptrMainWindow = new MainWindow_tob(ui->username->text(), this);
            t_ptrMainWindow->show();
        }
    }
    // 用户名存在但密码错误
    else {
        QMessageBox::warning(this, "Fail", "Wrong ID or password!", QMessageBox::Ok);
        ui->username->clear();
        ui->password->clear();
        ui->username->setFocus();
    }
}

// 注册按钮被按时触发的槽函数
void Login::on_signup_clicked()
{
    Signup *s = new Signup;
    s->show();
}

// 用来调整密码的显示还是隐藏
void Login::on_password_show_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        ui->password->setEchoMode(QLineEdit::Normal);
    } else {
        ui->password->setEchoMode(QLineEdit::Password);
    }
}