#include <QPainter>
#include "renderarea.h"
extern "C" {
#include "include/graph.h"
}

const int DEFAULT_MAX_ITERATIONS = 10;
const double DEFAULT_EPSILON = 0.1;

RenderArea::RenderArea(QWidget *parent)
    : QWidget{parent}
{
    _antialiased = true;
    _display_text = true;
    graph = NULL;
    _layout = FR;
    _circle_size = 12;
    _font_size = 12;
    _pen.setWidth(2);
    _pen.setColor(QColor::fromHsvF(0,0, 0.5));
    _main_color = QColor::fromRgb(190, 38, 76);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

//! [1]
QSize RenderArea::minimumSizeHint() const
{
    return QSize(400, 300);
}
//! [1]

//! [2]
QSize RenderArea::sizeHint() const
{
    return QSize(600, 450);
}
//! [2]

//! [4]
void RenderArea::setColor(int index, QColor color)
{
    vertexColors.at(index) = color;
    update();
}
//! [4]

void RenderArea::paintGraph(QPainter *painter)
{
    if (graph == NULL) return;

    ArrangeVertexPosition(graph, this->width(), this->height());

    if (_antialiased)
        painter->setRenderHint(QPainter::Antialiasing, true);

    double high_weight = graph->edges[0]->weight;
    double low_weight = graph->edges[0]->weight;
    for (int i = 1; i < graph->edges_num; ++i) {
        if (graph->edges[i]->weight > high_weight)
            high_weight = graph->edges[i]->weight;
        else if (graph->edges[i]->weight < low_weight)
            low_weight = graph->edges[i]->weight;
    }

    QColor curcolor = _pen.color();
    for (int i = 0; i < graph->edges_num; ++i) {
        QColor color = QColor::fromHsvF(0,0, curcolor.valueF() * (1 - (fabs(graph->edges[i]->weight) - low_weight) / (high_weight - low_weight)));
        _pen.setColor(color);
        painter->setPen(_pen);

        struct Vector v1 = graph->edges[i]->start->location;
        struct Vector v2 = graph->edges[i]->end->location;

        painter->drawLine(v1.x, v1.y, v2.x, v2.y);
    }
    _pen.setColor(curcolor);

    for (int i = 0; i < graph->vertexes_num; ++i) {
        struct Vector v = graph->vertexes[i]->location;
        char* name = graph->vertexes[i]->name;

        painter->setBrush(QBrush(vertexColors.at(i)));
        painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
        painter->drawEllipse(v.x-_circle_size, v.y-_circle_size, _circle_size*2, _circle_size*2);
        if (_display_text) {
            painter->setFont(QFont("Arial", _font_size));
            painter->drawText(v.x+_circle_size, v.y+_circle_size, QString(name));
        }
    }
}
//! [5]

//! [3]
void RenderArea::setPen(const QPen &pen)
{
    _pen = pen;
    update();
}
//! [3]

QPen RenderArea::pen() const
{
    return _pen;
}

void RenderArea::setMainColor(const QColor &newMainColor)
{
    _main_color = newMainColor;
    update();
}

QColor RenderArea::mainColor() const
{
    return _main_color;
}

void RenderArea::setCircleSize(int size)
{
    _circle_size = size;
    update();
}

int RenderArea::circleSize() const
{
    return _circle_size;
}

void RenderArea::setFontSize(int size)
{
    _font_size = size;
    update();
}
//! [5]

int RenderArea::fontSize() const
{
    return _font_size;
}
//!
//! [6]
void RenderArea::setAntialiased(bool antialiased)
{
    _antialiased = antialiased;
    update();
}
//! [6]

bool RenderArea::antialiased() const
{
    return _antialiased;
}

void RenderArea::setLayout(const Layout &newLayout)
{
    _layout = newLayout;

    if (graph == NULL) return;
    InitVerticesPosition(graph, false);
    if (_layout == FR)     //Fruchterman-Reingold
        ForceDirectedLayout(graph, DEFAULT_MAX_ITERATIONS);
    else                    //Kamanda-Kawaii
        LocalMinimum(graph, DEFAULT_EPSILON);

    update();
}

RenderArea::Layout RenderArea::layout() const
{
    return _layout;
}

//! [7]
void RenderArea::setGraph(struct Graph *graph)
{
    this->graph = graph;
    setLayout(_layout);
    vertexColors.assign(graph->vertexes_num, _main_color);
    update();
}
//! [7]
void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    paintGraph(&painter);
}

bool RenderArea::displayText() const
{
    return _display_text;
}

void RenderArea::setDisplayText(bool display)
{
    _display_text = display;
    update();
}
