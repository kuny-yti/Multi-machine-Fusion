#ifndef GLOBAL_SHORTCUT_H
#define GLOBAL_SHORTCUT_H

#include <QEvent>
#include <QKeySequence>
#include <QAbstractEventDispatcher>

class global_shortcut_p;
class global_shortcut : public QObject
{
    Q_OBJECT
public:
    global_shortcut(QObject *parent = 0);
    explicit global_shortcut(const QKeySequence& shortcut, QObject* parent = 0);
    ~global_shortcut();

    QKeySequence shortcut() const;
    bool setShortcut(const QKeySequence& shortcut);

    bool isEnabled() const;

public Q_SLOTS:
    void setEnabled(bool enabled = true);
    void setDisabled(bool disabled = true);

Q_SIGNALS:
    void activated();

private:
    global_shortcut_p *d;
};

#endif // GLOBAL_SHORTCUT_H
