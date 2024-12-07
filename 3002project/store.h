#ifndef STORE_H
#define STORE_H

#include <QWidget>
#include <QSqlDatabase>

// 店铺的子模块，用于显示店铺信息
namespace Ui {
class store;
}

class store : public QWidget
{
    Q_OBJECT

public:
    explicit store(QString username, QWidget *parent = nullptr);
    void setName(QString name = "unknown"); // 记录店铺的名字并显示在窗口上
    void setScore(double score = 5.0); // 记录店铺的评分并显示在窗口上
    void setPrice(double price = 0.0); // 记录店铺的人均价格并显示在窗口上
    void setAddress(QString address = "Genshin, start!"); // 记录店铺的地址并显示在窗口上
    void setPhoto(QString photo); // 设置店铺的图片
    ~store();
    void setUsername(QString username); // 记录顾客用户名并向下传递

private slots:
    void on_select_clicked(); // 每个店铺子模块有一个按钮，按了这个按钮后触发的槽函数代表顾客选择了这个店铺

private:
    Ui::store *ui;
    QString m_strName;
    QString m_strAddress;
    QString m_strUsername;
    QString m_strPhoto; // 记录QString的变量，包括店铺的名称，地址，和顾客的用户名
    double m_dScore;
    double m_dPrice; // 记录double的成员变量，包括店铺的评分和人均售价
    QSqlDatabase m_dbStore;
};

#endif // STORE_H