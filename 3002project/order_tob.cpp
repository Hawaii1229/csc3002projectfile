#include "order_tob.h"
#include "ui_order_tob.h"
#include "oneorder.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVBoxLayout>

// 店铺订单管理界面
order_tob::order_tob(QString t_strStorename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::order_tob)
{
    ui->setupUi(this);

    // 记录店铺名称
    setStorename(t_strStorename);

    // 实现动态添加订单模块的功能
    QWidget* widget = new QWidget;
    QVBoxLayout* m_Layout = new QVBoxLayout;
    widget->setLayout(m_Layout);

    ui->m_scrollArea->setWidget(widget);
    ui->m_scrollArea->setWidgetResizable(true);

    // 连接数据库
    m_dborder = QSqlDatabase::addDatabase("QSQLITE", "orderconnection");
    m_dborder.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dborder.open()) {
        qDebug() << "Database open error:" << m_dborder.lastError();
    }

    // 获取users表中所有顾客的用户名
    QSqlQuery query(m_dborder);
    if (!query.exec("SELECT username FROM users WHERE type='customer'")) {
        qDebug() << "Select error:" << query.lastError();
    } else {
        QSqlQuery query1(m_dborder);
        // 遍历顾客用户名去判断该顾客是否有在本店铺订餐
        while (query.next()) {
            query1.exec(QString("SELECT %1 FROM %2 WHERE %1 != 0").arg(query.value("username").toString()).arg(m_strStorename));
            if (query1.next()) {
                // 若订餐则实例化一个oneorder模块
                oneOrder *oneorder = new oneOrder(m_strStorename, query.value("username").toString(), this);
                // 将该oneorder模块加到布局上
                m_Layout->addWidget(oneorder);
            }
        }
    }
}

order_tob::~order_tob()
{
    delete ui;
    m_dborder.close();
}

// 记录店铺名的函数
void order_tob::setStorename(QString t_str)
{
    m_strStorename = t_str;
}