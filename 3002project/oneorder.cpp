#include "oneorder.h"
#include "ui_oneorder.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

// 店铺订单的单个模块
oneOrder::oneOrder(QString t_strStorename, QString t_strUsername, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::oneOrder)
{
    ui->setupUi(this);

    // 记录店铺名和用户名
    setStorename(t_strStorename);
    setUsername(t_strUsername);

    // 打开数据库，并且拥有单独的连接，与order_tob的连接区分
    m_dboneorder = QSqlDatabase::addDatabase("QSQLITE", "oneorderconnection");
    m_dboneorder.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dboneorder.open()) {
        qDebug() << "Database open error:" << m_dboneorder.lastError();
    }

    // 从对应店铺的表中查询对应顾客的字段是否有菜品数量>0，即代表该顾客购买了该菜品
    QSqlQuery query(m_dboneorder);
    if (!query.exec(QString("SELECT name, \"%1\" FROM \"%2\" WHERE \"%1\" != 0").arg(m_strUsername).arg(m_strStorename))) {
        qDebug() << "Select error:" << query.lastError();
    } else {
        QString t_strShow = m_strUsername + "\n";
        while (query.next()) {
            t_strShow += QString("%1  %2份 \n").arg(query.value("name").toString()).arg(query.value(m_strUsername).toInt());
        }
        // 将订单字符串输出
        showOrder(t_strShow);
    }
}

oneOrder::~oneOrder()
{
    delete ui;
    m_dboneorder.close();
}

// 订单字符串显示到文本框的函数
void oneOrder::showOrder(QString t_str)
{
    ui->showorder->setPlainText(t_str);
}

// 记录店铺名
void oneOrder::setStorename(QString t_str)
{
    m_strStorename = t_str;
}

// 记录用户名
void oneOrder::setUsername(QString t_str)
{
    m_strUsername = t_str;
}

// 完成订单按钮按下后触发的槽函数
void oneOrder::on_complete_clicked()
{
    QSqlQuery query(m_dboneorder);
    query.exec(QString("SELECT * FROM %1").arg(m_strStorename));
    while (query.next()) {
        // 将各个菜品的total数据增加
        QSqlQuery query1(m_dboneorder);
        query1.exec(QString("UPDATE %1 SET total = %2 WHERE name = '%3'")
                    .arg(m_strStorename)
                    .arg(query.value("total").toInt() + query.value(m_strUsername).toInt())
                    .arg(query.value("name").toString()));
    }
    // 将顾客的订单数据清零
    query.exec(QString("UPDATE %1 SET %2 = 0").arg(m_strStorename).arg(m_strUsername));
    this->close();
}