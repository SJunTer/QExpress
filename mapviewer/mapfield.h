#ifndef MAPFIELD_H
#define MAPFIELD_H

#include <QString>
#include <QList>

struct Field
{
    QString fieldName;
    QString fieldType;
    QString fieldContent;
};

class MapField
{
public:
    MapField();
    void addField(const QString name, const QString type, const QString content);
    int getFieldId(const QString name);
    QString getName(int index);
    QString getType(int index);
    QString getContent(int index);

private:
    QList<Field> fieldInfo;
    int fieldCount;
};

#endif // MAPFIELD_H
