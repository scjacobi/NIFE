#ifndef KBEVENTLISTVIEW_H
#define KBEVENTLISTVIEW_H

#include <QListView>

class LaunchListView : public QListView
{
    Q_OBJECT

public:
    LaunchListView(QWidget *parent = 0);
    virtual ~LaunchListView() {};

signals:
    void enterPressed(QModelIndex index);

protected:
    void keyPressEvent(QKeyEvent *event);

};

class ManipulateListView : public QListView
{
    Q_OBJECT

public:
    ManipulateListView(QWidget *parent = 0);
    virtual ~ManipulateListView() {};

signals:
    void deletePressed(QModelIndex index);
    void pageUpPressed(QModelIndex index);
    void pageDownPressed(QModelIndex index);

protected:
    void keyPressEvent(QKeyEvent *event);

};

#endif // KBEVENTLISTVIEW_H
