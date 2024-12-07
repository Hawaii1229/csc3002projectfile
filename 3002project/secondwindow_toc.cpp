#include "secondwindow_toc.h"
#include "ui_secondwindow_toc.h"
#include "dishes.h"
#include "thirdwindow_toc.h"

#include <QVBoxLayout>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

// 顾客的第二页面，即顾客选餐页面
secondwindow_toc::secondwindow_toc(QString t_strUsername, QString t_strStoreName, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::secondwindow_toc)
{
    ui->setupUi(this);

    // 记录用户名和店铺名
    setUsername(t_strUsername);
    setStoreName(t_strStoreName);

    this->setWindowTitle(m_strStoreName);

    // 将返回按钮与关闭本窗口绑定
    connect(ui->menu, SIGNAL(aboutToShow()), this, SLOT(close()));

    // 连接数据库 waimai.db
    m_dbdishes = QSqlDatabase::addDatabase("QSQLITE", "dishes_connection");
    m_dbdishes.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dbdishes.open()) {
        qDebug() << "Database open error:" << m_dbdishes.lastError();
    }

    // 设置可以滚动的菜品待选框
    QWidget* widget = new QWidget;
    QVBoxLayout* m_Layout = new QVBoxLayout;
    widget->setLayout(m_Layout);

    ui->m_scrollArea->setWidget(widget);
    ui->m_scrollArea->setWidgetResizable(true);

    QSqlQuery query(m_dbdishes);

    // 从店铺表中选择数据
    if (!query.exec(QString("SELECT * FROM %1").arg(m_strStoreName))) {
        qDebug() << "Select error:" << m_dbdishes.lastError();
    } else {
        while (query.next()) {
            dishes* t_ptr = new dishes(this);
            // 设置菜品信息
            t_ptr->setName(query.value("name").toString());
            t_ptr->setIntroduction(query.value("introduction").toString());
            t_ptr->setPrice(query.value("price").toInt());
            t_ptr->setStoreName(this->m_strStoreName);
            t_ptr->setPhoto(query.value("photo").toString());
            m_Layout->addWidget(t_ptr);

            // 连接信号和槽以实时计算总账单
            QObject::connect(t_ptr, SIGNAL(signaltocalPayment()), this, SLOT(calPayment()));
        }
    }
}

secondwindow_toc::~secondwindow_toc()
{
    delete ui;
}

// 记录店铺名
void secondwindow_toc::setStoreName(QString t_str)
{
    m_strStoreName = t_str;
}

// 实时计算选择的餐品总价，并显示在 LCD number 上
void secondwindow_toc::calPayment()
{
    QSqlQuery query(m_dbdishes);
    if (!query.exec(QString("SELECT price, quantity FROM %1 WHERE quantity > 0").arg(m_strStoreName))) {
        qDebug() << "Select error:" << m_dbdishes.lastError();
    } else {
        m_iPayment = 0; // 清空总价
        while (query.next()) {
            m_iPayment += query.value("price").toInt() * query.value("quantity").toInt();
        }
        ui->payment->display(m_iPayment); // 在 LCD number 中展示
    }
}

// 按下确定按钮后触发的槽函数，进入 thirdwindow_toc 界面
void secondwindow_toc::on_pushButton_clicked()
{
    thirdwindow_toc *tw = new thirdwindow_toc(m_strUsername, m_strStoreName, this);
    connect(tw, SIGNAL(closeSecondwindow()), this, SLOT(close()));
    if (m_iPayment != 0) {
        tw->setPayment(m_iPayment); // 将订单总价传入订单付款界面
    }
    tw->show();
}

// 记录顾客用户名
void secondwindow_toc::setUsername(QString t_str)
{
    m_strUsername = t_str;
    qDebug() << "Username set to:" << m_strUsername;
}