#ifndef HOME_TOB_H
#define HOME_TOB_H

#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui {
class home;
}

class home : public QMainWindow
{
    Q_OBJECT

public:
    explicit home(QString t_strUsername, QWidget *parent = nullptr);
    ~home();

    void setUsername(QString t_str);
    void setStorename(QString t_str);

signals:
    void closewindow(); // 发出关闭窗口的信号

private slots:
    void on_change_clicked(); // 修改个人信息的槽函数
    void refresh(); // 刷新显示函数

private:
    Ui::home *ui;
    QString m_strUsername;
    QString m_strStorename;
    QSqlDatabase m_dbhome;
};

#endif // HOME_TOB_H