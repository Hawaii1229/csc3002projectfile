#include "mainwindow_toc.h"
#include "ui_mainwindow_toc.h"
#include "store.h"
#include "login.h"

#include <QVBoxLayout>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

// 顾客的主页面，即顾客选择店铺的页面
MainWindow_toc::MainWindow_toc(QString t_strUsername, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow_toc)
{
    ui->setupUi(this);
    setUsername(t_strUsername);

    // 设置工具栏的退出登录按钮与关闭该页面，打开上一页面的槽函数连接
    connect(ui->action, SIGNAL(triggered(bool)), new Login, SLOT(show()));
    connect(ui->action, SIGNAL(triggered(bool)), this, SLOT(close()));

    // 设置可以滚动的店铺待选框
    QWidget* widget = new QWidget;
    ui->m_scrollArea->setWidget(widget);
    ui->m_scrollArea->setWidgetResizable(true);

    QVBoxLayout* m_Layout = new QVBoxLayout;
    widget->setLayout(m_Layout);

    // 连接数据库到waimai.db
    m_dbstore = QSqlDatabase::addDatabase("QSQLITE", "mainwindow_toc_connection");
    m_dbstore.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dbstore.open()) {
        qDebug() << "Database open error:" << m_dbstore.lastError();
    }

    // 获取store表中的数据
    QSqlQuery query(m_dbstore);
    QString select_all = "SELECT * FROM store";
    if (!query.exec(select_all)) {
        qDebug() << "Select error:" << m_dbstore.lastError();
    } else {
        while (query.next()) {
            store* t_ptrStore = new store(m_strUsername, this);
            t_ptrStore->setName(query.value("storename").toString());
            t_ptrStore->setAddress(query.value("address").toString());
            t_ptrStore->setPhoto(query.value("photo").toString());
            t_ptrStore->setPrice(18);
            t_ptrStore->setScore(4.7);
            m_Layout->addWidget(t_ptrStore);
        }
    }
}

MainWindow_toc::~MainWindow_toc()
{
    delete ui;
    m_dbstore.close();
}

// 记录用户名的函数
void MainWindow_toc::setUsername(QString t_str)
{
    m_strUsername = t_str;
    qDebug() << "Username set to:" << m_strUsername;
}