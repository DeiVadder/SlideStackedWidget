#ifndef CSTACKEDWIDGET_H
#define CSTACKEDWIDGET_H

#include <QStackedWidget>
#include <QEasingCurve>
#include <QDebug>

class cStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit cStackedWidget(QWidget *parent);
    ~cStackedWidget();

    enum slideDirection{
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop,
        Automatic
    };

signals:
    void AnimationFinished();
    void cancelAnimation();

public slots:
    void setVerticalMode(bool vertical = true){bVertical = vertical;}
    void setWrap(bool wrap = false){bWrap = wrap;}
    void setSpeed(int speed = 500){iSpeed = speed;}
    void setAnimation(enum QEasingCurve::Type type){slideAnimationtype = type;}

    void slideinIndex(int index, enum slideDirection direction = Automatic);
    void slideinNext(){if(bWrap || (currentIndex() < count()-1)) slideinIndex(currentIndex()+1);}
    void slideinPrev(){if(bWrap || (currentIndex() > 0)) slideinIndex(currentIndex()-1);}
    void slideinWidget(QWidget *widget, enum slideDirection direction = Automatic);

protected:

    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

    enum QEasingCurve::Type slideAnimationtype;
    int iSpeed, iNow, iNext;
    bool bWrap, bActive, bVertical;
    QPoint pNow;
    QList<QWidget*> blockedPageList;
};

#endif // CSTACKEDWIDGET_H
