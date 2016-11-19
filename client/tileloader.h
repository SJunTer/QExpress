#ifndef TILELOADER_H
#define TILELOADER_H

#include <QObject>
#include <QString>
#include <QRectF>
#include <QVector>

class TileLoader : public QObject
{
    Q_OBJECT
public:
    explicit TileLoader(int sock, int lv, QRectF sr,QRectF vr, QVector<QString> &t);

private:
    int sockfd;
    int zoomLevel;
    QRectF sceneRect;
    QRectF viewRect;
    QVector<QString> tiles;
    bool stopped;

signals:
    void taskFinished();
    bool tileLoaded(QString fileName);
    void drawPixmap(QString fileName, int zoomLevel, int x, int y);
    void drawSymbol(QString symbolName, int zoomLevel, int x, int y);

public slots:
    void doWork();
    void stop() { stopped = true; }
};

#endif // TILELOADER_H
