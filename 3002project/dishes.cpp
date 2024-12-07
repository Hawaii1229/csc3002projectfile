#include "dishes.h"
#include "ui_dishes.h"

#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError> 
#include <QtSql/QSqlDatabase>

// 单个菜品模块
dishes::dishes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dishes)
{
    ui->setupUi(this);

    // 连接数据库
    m_dbdish = QSqlDatabase::addDatabase("QSQLITE", "dish_connection");
    m_dbdish.setDatabaseName("C:\\Users\\86185\\Desktop\\CSC3170 project\\Project\\Project\\data\\waimai.db");
    if (!m_dbdish.open()) {
        qDebug() << "Database open error!" << m_dbdish.lastError();
    }
}

dishes::~dishes()
{
    delete ui;
    m_dbdish.close();
}

// 设置菜品的名称
void dishes::setName(QString t_str)
{
    m_strName = t_str;
    ui->name->setText(t_str);
}

// 设置菜品的介绍
void dishes::setIntroduction(QString t_str)
{
    m_strIntroduction = t_str;
    ui->introduction->setText(t_str);
}

// 设置菜品的单价
void dishes::setPrice(int t_i)
{
    m_iPrice = t_i;
    ui->price->setText(QString("%1 r").arg(m_iPrice));
}

// 记录店铺名称
void dishes::setStoreName(QString t_str)
{
    m_strStoreName = t_str;
}

// 设置菜品图片
void dishes::setPhoto(QString t_str)
{
    m_strPhoto = t_str;
    QPixmap pixmp(m_strPhoto); // 实例化一个QPixmap对象
    pixmp = pixmp.scaled(100, 100); // 将图片缩放适应QGraphicsView大小
    QGraphicsScene *scene = new QGraphicsScene; // 实例化一个QGraphicsScene对象
    scene->addPixmap(pixmp); // 设置该QGraphicsScene的图片路径
    ui->dishes_photo->setScene(scene); // 将QGraphicsScene显示在QGraphicsView控件上
}

// 当选餐数量发生改变时触发的槽函数
void dishes::on_quantity_valueChanged(int arg1)
{
    m_quantity = arg1;
    QSqlQuery query(m_dbdish);
    // 将实时选餐数据记录到对应店铺的quantity字段中
    if (!query.exec(QString("UPDATE %1 SET quantity=%2 WHERE name='%3'")
                    .arg(m_strStoreName).arg(arg1).arg(m_strName))) {
        qDebug() << "Update error" << m_dbdish.lastError();
    }
    // 发出让lcd number显示总价改变的信号
    emit signaltocalPayment();
}