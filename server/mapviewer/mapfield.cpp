#include "mapfield.h"

MapField::MapField()
: fieldCount(0)
{
}

void MapField::addField(const QString name, const QString type, const QString content)
{
    Field field;
    field.fieldName = name;
    field.fieldType = type;
    field.fieldContent = content;
    fieldInfo.append(field);
    ++fieldCount;
}

int MapField::getFieldId(const QString name)
{
    int index = 0;
    for(auto field : fieldInfo)
    {
        if(field.fieldName.compare(name, Qt::CaseInsensitive) == 0)
            return index;
        ++index;
    }
    return -1;
}

QString MapField::getName(int index)
{
    if(index == -1)
        return NULL;
    return fieldInfo.at(index).fieldName;
}

QString MapField::getType(int index)
{
    if(index == -1)
        return NULL;
    return fieldInfo.at(index).fieldType;
}

QString MapField::getContent(int index)
{
    if(index == -1)
        return NULL;
    return fieldInfo.at(index).fieldContent;
}

