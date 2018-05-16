#include "kbeventlistview.h"
#include <QKeyEvent>

LaunchListView::LaunchListView(QWidget *parent) :
    QListView(parent)
{
}

void LaunchListView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit enterPressed(currentIndex());
        break;
    default:
        QListView::keyPressEvent(event);
    }
}

ManipulateListView::ManipulateListView(QWidget *parent) :
    QListView(parent)
{
}

void ManipulateListView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
        emit deletePressed(currentIndex());
        break;
    case Qt::Key_PageUp:
        emit pageUpPressed(currentIndex());
        break;
    case Qt::Key_PageDown:
        emit pageDownPressed(currentIndex());
        break;
    default:
        QListView::keyPressEvent(event);
    }
}

