#ifndef REGEXAPIENDPOINT_H
#define REGEXAPIENDPOINT_H

#include <QObject>

class RegExApiEndpoint : public QObject
{
    Q_OBJECT

    public:
        RegExApiEndpoint();

        Q_INVOKABLE QString fetch(const QVariant &pathParameter, const QVariant &requestParameter) const;
};

#endif // REGEXAPIENDPOINT_H
