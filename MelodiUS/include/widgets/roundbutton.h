#pragma once
#include "globaldef.h"

class RoundButton : public QPushButton
{
    Q_DISABLE_COPY(RoundButton)
private:
    QPixmap image;

public:
    using QPushButton::QPushButton;
    void SetImage(QPixmap newImage)
    {
        image = newImage;
    }

public slots:
    void paintEvent(QPaintEvent*) override
    {
        QColor background = isDown() ? QColor("grey") : QColor("lightgrey");
        background = isEnabled() ? background : QColor("grey");
        int    diameter   = qMin(height(), width()) - 4;

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.translate(width() / 2, height() / 2);

        painter.setPen(QPen(QColor("grey"), 1));
        painter.setBrush(QBrush(background));
        painter.drawEllipse(QRect(-diameter / 2, -diameter / 2, diameter, diameter));

        if(!image.isNull())
        {
            int xOff = (-diameter / 2) + (image.width() * 3 / 2) - 1;
            int yOff = (-diameter / 2) + (image.height() * 3 / 2) - 1;
            painter.drawPixmap(QRect(xOff, yOff, image.width(), image.height()), image);
        }
    }

    void resizeEvent(QResizeEvent* event) override
    {
        QPushButton::resizeEvent(event);

        int diameter = qMin(height(), width()) + 4;
        int xOff     = (width() - diameter) / 2;
        int yOff     = (height() - diameter) / 2;
        setMask(QRegion(xOff, yOff, diameter, diameter, QRegion::Ellipse));
    }
};