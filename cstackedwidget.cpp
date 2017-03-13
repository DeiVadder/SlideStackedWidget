#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QDebug>
#include <QMouseEvent>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QBitmap>
#include <QStyleOption>

#include "cstackedwidget.h"

cStackedWidget::cStackedWidget(QWidget *parent) : QStackedWidget(parent)
{
    bVertical   = false;
    bWrap       = false;
    bActive     = false;
    iSpeed      = 2000;
    iNow        = 0;
    iNext       = 0;
    pNow        = QPoint(0,0);
    slideAnimationtype = QEasingCurve::OutBack;
}

cStackedWidget::~cStackedWidget()
{

}

void cStackedWidget::slideinIndex(int index, cStackedWidget::slideDirection direction)
{
    if(index > count() -1){
        direction = bVertical ? TopToBottom : RightToLeft;
        index = index % count();
    }else if(index <0){
        direction = bVertical ? BottomToTop : LeftToRight;
        index = (index + count()) % count();
    }
    slideinWidget(widget(index),direction);

}

void cStackedWidget::slideinWidget(QWidget *nextWidget, cStackedWidget::slideDirection direction)
{
    if(bActive)
        return;
    bActive = true;

    int iCIndex = currentIndex();
    int slideTo = indexOf(nextWidget);

    if(iCIndex == slideTo){
        bActive = false;
        return;
    }

    if(direction == Automatic){
        if(iCIndex < slideTo)
            direction= bVertical ? TopToBottom : RightToLeft;
        else
            direction = bVertical ? BottomToTop : LeftToRight;
    }

    int iOffsetX = frameRect().width();
    int iOffsetY = frameRect().height();
    widget(slideTo)->setGeometry(0,0,iOffsetX, iOffsetY);

    switch (direction){
    case Automatic:break;
    case BottomToTop:   iOffsetX = 0; iOffsetY = -iOffsetY;break;
    case TopToBottom:   iOffsetX = 0;break;
    case RightToLeft:   iOffsetX = -iOffsetX; iOffsetY = 0;break;
    case LeftToRight:   iOffsetY = 0;
    }

    QPoint pNext = widget(slideTo)->pos();
    pNow = widget(iCIndex)->pos();

    QPixmap pmCurrent = QPixmap(widget(iCIndex)->size());
    widget(iCIndex)->render(&pmCurrent);
    QPixmap pmSlideTo = QPixmap(widget(slideTo)->size());
    widget(slideTo)->render(&pmSlideTo);

    QLabel *lblCurrent = new QLabel(this);
    QLabel *lblSlideTo = new QLabel(this);
    lblCurrent->resize(widget(iCIndex)->size());
    lblSlideTo->resize(widget(slideTo)->size());
    lblCurrent->show(); lblSlideTo->show();
    lblCurrent->setPixmap(pmCurrent);
    lblSlideTo->setPixmap(pmSlideTo);
    lblCurrent->raise();
    lblSlideTo->raise();

    lblCurrent->move(0,0);
    lblSlideTo->move(pNext.x() - iOffsetX, pNext.y()-iOffsetY);
//    widget(slideTo)->move(pNext.x() - iOffsetX, pNext.y()-iOffsetY);
//    widget(slideTo)->show();
//    widget(slideTo)->raise();

//    QPropertyAnimation *anCurrent = new QPropertyAnimation(widget(iCIndex),"pos");
//    QPropertyAnimation *anSlideto = new QPropertyAnimation(widget(slideTo),"pos");
    QPropertyAnimation *anCurrent = new QPropertyAnimation(lblCurrent,"pos");
    QPropertyAnimation *anSlideto = new QPropertyAnimation(lblSlideTo,"pos");
    QParallelAnimationGroup *anGroup = new QParallelAnimationGroup;

    anCurrent->setDuration(iSpeed);
    anSlideto->setDuration(iSpeed);
    anCurrent->setEasingCurve(slideAnimationtype);
    anSlideto->setEasingCurve(slideAnimationtype);

    anCurrent->setStartValue(pNow);
    anCurrent->setEndValue(QPoint(iOffsetX + pNow.x(), iOffsetY + pNow.y()));

    anSlideto->setStartValue(QPoint(-iOffsetX+pNext.x(), -iOffsetY +pNext.y()));
    anSlideto->setEndValue(pNext);

    anGroup->addAnimation(anCurrent);
    anGroup->addAnimation(anSlideto);

    iNext = slideTo;
    iNow = iCIndex;
    connect(anGroup, &QParallelAnimationGroup::finished, lblCurrent, &QLabel::deleteLater);
    connect(anGroup, &QParallelAnimationGroup::finished, lblSlideTo, &QLabel::deleteLater);

    connect(anGroup, &QParallelAnimationGroup::finished, [=]{
       setCurrentIndex(iNext);
       widget(iNow)->hide();
       widget(iNow)->move(pNow);
       bActive = false;
       emit AnimationFinished();
    });
    anGroup->start();
    connect(this, &cStackedWidget::cancelAnimation, [=]{
        if(anGroup->currentTime() > 50)
            anGroup->setCurrentTime(iSpeed);});
}

void cStackedWidget::mousePressEvent(QMouseEvent *event)
{
    emit cancelAnimation();
    event->ignore();
}

void cStackedWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


