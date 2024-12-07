#include "signup.h"
#include "ui_signup.h"

#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QIntValidator>

// 注册界面
Signup::Signup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Signup)
{
    ui->setupUi(this);
    setWindowTitle("Registry Interface");

    // 设置密码输入框的显示模式，提示文字，输入限制为整数
    ui->password->setValidator(new QIntValidator(ui->password));
    ui->password->setPlaceholderText("Only numbers");
    ui->username->setPlaceholderText("Cannot be pure numbers");

    ui->password->setEchoMode(QLineEdit::Password);
    ui->ensure_password->setValidator(new QIntValidator(ui->password));
    ui->ensure_password->setPlaceholderText("Only numbers");
    ui->ensure_password->setEchoMode(QLineEdit::Password);

    // 商家设置店名的文本框，并在初始时设置为隐藏
    ui->storename->setPlaceholderText("Enter store name");
    ui->storename->hide();
    ui->label_4->hide();

    // 连接数据库
    m_dbSignup = QSqlDatabase::addDatabase("QSQLITE");
    m_dbSignup.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dbSignup.open()) {
        qDebug() << "Database open error:" << m_dbSignup.lastError();
    }
}

Signup::~Signup()
{
    delete ui;
    m_dbSignup.close();
}

// 确认注册按钮按下触发的槽函数
void Signup::on_signup_clicked()
{
    // 用户名，密码，确认密码框均有输入时
    if (ui->password->text() == ui->ensure_password->text() && !ui->password->text().isEmpty() && !ui->username->text().isEmpty()) {
        QSqlQuery t_query(m_dbSignup);
        t_query.exec(QString("SELECT * FROM users WHERE username = '%1'").arg(ui->username->text()));
        if (t_query.next()) {
            QMessageBox::warning(this, "Warning", "ID already exists");
            ui->username->clear();
            ui->password->clear();
            ui->ensure_password->clear();
            ui->username->setFocus();
        } else {
            // 选择注册商家账号
            if (ui->checktype->isChecked()) {
                // 判断是否输入店名，默认在注册时必输入
                if (!ui->storename->text().isEmpty()) {
                    QMessageBox::information(this, nullptr, "Register successfully");
                    QSqlQuery query(m_dbSignup);
                    // 将账号，密码，账号类型记录进users表
                    query.prepare("INSERT INTO users (username, password, type) VALUES (:username, :password, :type);");
                    query.bindValue(":username", ui->username->text());
                    query.bindValue(":password", ui->password->text().toInt());
                    query.bindValue(":type", "business");
                    if (!query.exec()) {
                        qDebug() << "Insert error:" << m_dbSignup.lastError();
                    }

                    // 将用户名，店铺名，店铺地址相关信息记录进store表
                    query.prepare("INSERT INTO store (username, storename, address) VALUES (:username, :storename, :address)");
                    query.bindValue(":username", ui->username->text());
                    query.bindValue(":storename", ui->storename->text());
                    query.bindValue(":address", "cuhksz");
                    if (!query.exec()) {
                        qDebug() << "Insert error!";
                    }

                    // 创建以店铺名为表头的表，用来记录该店中的菜品和销量
                    query.prepare(QString("CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY, name TEXT, price INTEGER, introduction TEXT, photo TEXT DEFAULT './photo/暂无图片.jpg', quantity INTEGER, total INTEGER)").arg(ui->storename->text()));
                    if (!query.exec()) {
                        qDebug() << "Create error:" << m_dbSignup.lastError();
                    }
                    this->close();
                } else {
                    QMessageBox::warning(this, "Warning", "Please enter store name");
                    ui->username->clear();
                    ui->password->clear();
                    ui->ensure_password->clear();
                    ui->username->setFocus();
                }
            } else {
                // 选择注册顾客账号
                QMessageBox::information(this, nullptr, "Register successfully");
                QSqlQuery query(m_dbSignup);
                // 将账号，密码记录进users表
                query.prepare("INSERT INTO users (username, password, type) VALUES (:username, :password, :type);");
                query.bindValue(":username", ui->username->text());
                query.bindValue(":password", ui->password->text().toInt());
                query.bindValue(":type", "customer");

                if (!query.exec()) {
                    qDebug() << "Insert error!";
                }
                this->close();
            }
        }
    } else {
        // 账号，密码，确认密码输入有误的情况，进行警告
        if (ui->username->text().isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please enter ID");
        } else if (ui->password->text().isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please enter password");
        } else if (ui->ensure_password->text().isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please confirm password");
        } else if (ui->password->text() != ui->ensure_password->text()) {
            QMessageBox::warning(this, "Warning", "Passwords do not match");
        }
        ui->username->clear();
        ui->password->clear();
        ui->ensure_password->clear();
        ui->username->setFocus();
    }
}

// 返回按钮按下后执行的槽函数。关闭注册页面，返回登录界面
void Signup::on_back_clicked()
{
    m_dbSignup.close();
    this->close();
}

// 设置密码显示方式
void Signup::on_password_show_stateChanged(int arg1)
{
    ui->password->setEchoMode(arg1 == Qt::Checked ? QLineEdit::Normal : QLineEdit::Password);
}

void Signup::on_ensure_password_show_stateChanged(int arg1)
{
    ui->ensure_password->setEchoMode(arg1 == Qt::Checked ? QLineEdit::Normal : QLineEdit::Password);
}

// 设置勾选“我是商家”后店铺名输入框的显示和隐藏
void Signup::on_checktype_stateChanged(int arg1)
{
    bool isChecked = (arg1 == Qt::Checked);
    ui->label_4->setVisible(isChecked);
    ui->storename->setVisible(isChecked);
}