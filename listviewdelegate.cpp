#include "listviewdelegate.h"

ListviewDelegate::ListviewDelegate()
    {
    // TODO Auto-generated constructor stub

    }

ListviewDelegate::~ListviewDelegate()
    {
    // TODO Auto-generated destructor stub
    }

//alocate each item size in listview.
QSize ListviewDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex & index) const
{
//    QIcon icon = qvariant_cast<QIcon>(index.data(IconRole));
    QSize desiredSize(48, 48);
//    QSize iconsize = icon.actualSize(desiredSize);
//    QFont font = QApplication::font();
//    QFontMetrics fm(font);

    return(QSize(desiredSize.width(),desiredSize.height()+8 ));

}
void ListviewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
 {
    QStyledItemDelegate::paint(painter,option,index);

    painter->save();

    QFont font = QApplication::font();
    QFont SubFont = QApplication::font();
    font.setPixelSize(18);
    font.setBold(true);
    QFontMetrics fm(font);

    QIcon icon = qvariant_cast<QIcon>(index.data(IconRole));
    QString headerText = qvariant_cast<QString>(index.data(headerTextRole));
    QString subText = qvariant_cast<QString>(index.data(subHeaderTextrole));

    QSize desiredSize(48, 48);
    QSize iconsize = icon.actualSize(desiredSize);

    QRect headerRect = option.rect;
    QRect subheaderRect = option.rect;
    QRect iconRect = option.rect;

    iconRect.setRight(iconsize.width()+16);
    iconRect.setTop(iconRect.top()+4);

    headerRect.setLeft(64);
    headerRect.setTop(headerRect.top()+4);
    headerRect.setBottom(headerRect.top()+fm.height());

    subheaderRect.setLeft(64);
    subheaderRect.setTop(headerRect.bottom()+2);

//    if (index.row() % 2 && )
//    {
//        painter->fillRect(option.rect, QBrush("#ddd"));
//    }

    painter->drawPixmap(QPoint(iconRect.left()+8,iconRect.top()),icon.pixmap(iconsize.width(),iconsize.height()));

    if (option.state & QStyle::State_Selected)
    {
        painter->setPen(Qt::white);
    }

    painter->setFont(font);
    painter->drawText(headerRect,headerText);

    painter->setFont(SubFont);
    painter->drawText(subheaderRect,subText);

    painter->restore();

 }
