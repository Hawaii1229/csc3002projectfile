#include "menu_tob.h"
#include "ui_menu_tob.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQueryModel>

// 店铺菜单管理页面
Menu::Menu(QString t_strStorename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    // 设置店铺名
    setStoreName(t_strStorename);
    // 展示菜单
    refresh();

    // 打开数据库
    m_dbMenu = QSqlDatabase::addDatabase("QSQLITE");
    m_dbMenu.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dbMenu.open()) {
        qDebug() << "Database open error:" << m_dbMenu.lastError();
    }
}

Menu::~Menu()
{
    delete ui;
    m_dbMenu.close();
}

void Menu::refresh()
{
    // 利用QSqlQueryModel对表格进行展示，节约了大量单独设置每一列数据来源的代码量
    QSqlQueryModel* model = new QSqlQueryModel(this);
    model->setQuery(QString("SELECT id, name, price, introduction, photo FROM %1").arg(m_strStoreName));
    ui->tableView->setModel(model);

    // 设置表头
    model->setHeaderData(1, Qt::Horizontal, "菜品");
    model->setHeaderData(2, Qt::Horizontal, "单价");
    model->setHeaderData(3, Qt::Horizontal, "介绍");
    model->setHeaderData(4, Qt::Horizontal, "图片地址");
}

// 记录店铺名
void Menu::setStoreName(QString t_str)
{
    m_strStoreName = t_str;
    m_dbMenu.close();
}

// 添加菜品按钮按下后触发的槽函数
void Menu::on_pushButton_clicked()
{
    // 判断是否输入了菜品的名称，单价，介绍（为强制要求）
    if (!ui->name->text().isEmpty() && !ui->introduction->toPlainText().isEmpty() && ui->price->value() != 0) {
        QSqlQuery query(m_dbMenu);
        // 判断是否输入了图片（图片为非强制要求）
        if (ui->photo->toPlainText().isEmpty()) {
            // 未输入图片信息则只添加(name, price, introduction, quantity, total)信息
            if (!query.exec(QString("INSERT INTO %1 (name, price, introduction, quantity, total) VALUES ('%2', %3, '%4', 0, 0)")
                            .arg(m_strStoreName)
                            .arg(ui->name->text())
                            .arg(ui->price->value())
                            .arg(ui->introduction->toPlainText()))) {
                qDebug() << "Insert error:" << query.lastError();
            }
        } else {
            // 输入了图片则添加(name, price, introduction, photo, quantity, total)信息
            if (!query.exec(QString("INSERT INTO %1 (name, price, introduction, photo, quantity, total) VALUES ('%2', %3, '%4', '%5', 0, 0)")
                            .arg(m_strStoreName)
                            .arg(ui->name->text())
                            .arg(ui->price->value())
                            .arg(ui->introduction->toPlainText())
                            .arg(ui->photo->toPlainText()))) {
                qDebug() << "Insert error:" << query.lastError();
            }
        }
    } else {
        // 未输入强制要求的信息，弹出警告
        QMessageBox::warning(this, "Warning", "请完整输入要添加的菜品的名称、单价、和介绍", QMessageBox::Ok);
    }
    // 菜单刷新
    refresh();
}

// 按下刷新按钮后触发的槽函数（应对bug）
void Menu::on_pushButton_2_clicked()
{
    refresh();
}

// 按下删除按钮后触发的槽函数
void Menu::on_pushButton_3_clicked()
{
    // 删除菜品以id为索引
    QSqlQuery query(m_dbMenu);
    // 判断是否输入id号
    if (ui->id->text().isEmpty()) {
        // 未输入id号，弹出警告
        QMessageBox::warning(this, "Warning", "请输入要删除的菜品id", QMessageBox::Ok);
        ui->id->setFocus(); // 移动光标至id号
    } else {
        // 利用select count(*)语句判断id号是否存在
        query.exec(QString("SELECT count(*) FROM %1 WHERE id = %2").arg(m_strStoreName).arg(ui->id->text().toInt()));
        query.next();
        // id号不存在时，弹出警告
        if (query.value(0).toInt() == 0) {
            QMessageBox::warning(this, "Warning", "查无此id", QMessageBox::Ok);
            ui->id->clear();
            ui->id->setFocus();
        } else {
            // id号存在时，将对应id号的菜品删除
            query.exec(QString("DELETE FROM %1 WHERE id = %2").arg(m_strStoreName).arg(ui->id->text().toInt()));
            refresh();
        }
    }
}

// 修改按钮按下后触发的槽函数
void Menu::on_pushButton_4_clicked()
{
    // 修改菜品以id号为索引
    QSqlQuery query(m_dbMenu);
    // 判断是否输入id号
    if (ui->id->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "请输入要修改的菜品id", QMessageBox::Ok);
        ui->id->setFocus();
    } else {
        // 判断id号是否存在
        query.exec(QString("SELECT count(*) FROM %1 WHERE id = %2").arg(m_strStoreName).arg(ui->id->text().toInt()));
        query.next();
        if (query.value(0).toInt() == 0) {
            QMessageBox::warning(this, "Warning", "查无此id", QMessageBox::Ok);
            ui->id->clear();
            ui->id->setFocus();
        } else {
            // id号存在，进行修改。因为修改时不确定哪些需要修改，对每个修改加了一个条件判断，如果修改框内有文字，则修改
            if (!ui->name->text().isEmpty()) {
                query.exec(QString("UPDATE %1 SET name = '%2' WHERE id = %3").arg(m_strStoreName).arg(ui->name->text()).arg(ui->id->text().toInt()));
            }
            if (ui->price->value() != 0) {
                query.exec(QString("UPDATE %1 SET price = %2 WHERE id = %3").arg(m_strStoreName).arg(ui->price->value()).arg(ui->id->text().toInt()));
            }
            if (!ui->introduction->toPlainText().isEmpty()) {
                query.exec(QString("UPDATE %1 SET introduction = '%2' WHERE id = %3").arg(m_strStoreName).arg(ui->introduction->toPlainText()).arg(ui->id->text().toInt()));
            }
            if (!ui->photo->toPlainText().isEmpty()) {
                query.exec(QString("UPDATE %1 SET photo = '%2' WHERE id = %3").arg(m_strStoreName).arg(ui->photo->toPlainText()).arg(ui->id->text().toInt()));
            }
            // 刷新显示
            refresh();
        }
    }
}