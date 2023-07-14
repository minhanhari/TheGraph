
#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <vector>

#include <QWidget>
#include <QBrush>
#include <QPen>
struct Graph;


class RenderArea : public QWidget
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = nullptr);

    enum Layout { FR, KK };

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void paintGraph(QPainter *painter);
    void setColor(int index, QColor color);

    void setPen(const QPen &pen);
    QPen pen() const;
    void setMainColor(const QColor &newMainColor);
    QColor mainColor() const;
    void setCircleSize(int size);
    int circleSize() const;
    void setFontSize(int size);
    int fontSize() const;
    void setAntialiased(bool antialiased);
    bool antialiased() const;
    Layout layout() const;
    void setLayout(const Layout &newLayout);

    bool displayText() const;
    void setDisplayText(bool display);

public slots:
    void setGraph(struct Graph *graph);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<QColor> vertexColors;
    struct Graph *graph;
    Layout _layout;
    QColor _main_color;
    QPen _pen;
    int _circle_size;
    int _font_size;
    bool _antialiased;
    bool _display_text;

};

#endif // RENDERAREA_H
