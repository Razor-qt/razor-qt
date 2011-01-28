#include "appswitcher.h"
#include "qxtglobalshortcut.h"

#include <QMessageBox>
#include <QEvent>



RazorAppSwitcher::AppSwitcher::AppSwitcher()
    : QWidget(0, Qt::FramelessWindowHint | Qt::Tool)
{
    setupUi(this);
    installEventFilter(this);

    m_layout = new QHBoxLayout();
    QWidget * background = new QWidget(this);
    scrollArea->setWidget(background);
    background->setLayout(m_layout);

    m_x = new XfitMan();
    Q_ASSERT(m_x);

    m_key = new QxtGlobalShortcut(this);

    //QKeySequence ks(Qt::CTRL + Qt::ALT + Qt::Key_T);
    QKeySequence ks(Qt::ALT + Qt::Key_Tab);
    if (! m_key->setShortcut(ks))
    {
        QMessageBox::information(this, tr("Global keyboard shortcut"),
                                 tr("Global shorcut: '%1' cannot be registered").arg(ks.toString()));
        qApp->exit(1);
    }

    m_localKey = new QShortcut(ks, this, SLOT(selectNextItem()), SLOT(selectNextItem()));

    connect(m_key, SIGNAL(activated()), this, SLOT(handleApps()));
}

void RazorAppSwitcher::AppSwitcher::handleApps()
{
    qDebug() << "AppSwitcher::handleApps() start";
    if (!isVisible())
    {
        QRect desktop = QApplication::desktop()->availableGeometry(m_x->getActiveDesktop());
        int x, y;
        x = desktop.width()/2 - width() / 2;
        y = desktop.height()/2 - height() / 2;
        move(x, y);
        qDebug() << "PARENT" << parent() << desktop << x << y;
        show();
    }

    if (m_layout->count())
    {
        QLayoutItem * item;
        while ((item = m_layout->takeAt(0)) != 0)
        {
            qDebug() << "Cleaning layout" << item;
            item->widget()->hide();
            delete item;
        }
    }
    m_list.clear();

    QList<Window> l = m_x->getClientList();
    Window actWin = m_x->getActiveWindow();
    bool setImplicitFocus = true;
    // setup new windows
    foreach (Window w, l)
    {
        if (!m_x->acceptWindow(w))
        {
            qDebug() << "Skipped due props:" << m_x->getName(w);
            continue;
        }
        qDebug() << "Append to layout:" << w << m_x->getName(w);
        QPixmap pm;
        if (! m_x->getClientIcon(w, pm))
            qDebug() << "No icon for:" << w << m_x->getName(w);

        SwitcherItem * item = new SwitcherItem(w, m_x->getName(w), pm, this);
        connect(item, SIGNAL(infoChanged(const QString&)),
                infoLabel, SLOT(setText(const QString&)));
        m_list.append(item);
        m_layout->addWidget(item);

        if (w == actWin)
        {
            item->setFocus(Qt::OtherFocusReason);
            setImplicitFocus = false;
        }
    }

    if (setImplicitFocus)
        selectNextItem();
}

void RazorAppSwitcher::AppSwitcher::hideEvent(QHideEvent *e)
{
    m_key->blockSignals(false);
    QWidget::hideEvent(e);
}

void RazorAppSwitcher::AppSwitcher::showEvent(QShowEvent *e)
{
    m_key->blockSignals(true);
    QWidget::showEvent(e);
}

void RazorAppSwitcher::AppSwitcher::keyPressEvent(QKeyEvent * e)
{
    qDebug() << "AppSwitcher::keyPressEvent" << e << e->key() << e->modifiers();;
    switch (e->key())
    {
        case Qt::Key_Escape:
            close();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            SwitcherItem * item = qobject_cast<SwitcherItem*>(QApplication::focusWidget());
            if (! item)
            {
                qDebug() << "AppSwitcher::keyPressEvent activation. Something is wrong - focus widget is not on SwitcherItem!";
                return;
            }
            m_x->raiseWindow(item->window());
            close();
            break;
        }
    };

    QWidget::keyPressEvent(e);
}

//void RazorAppSwitcher::AppSwitcher::keyReleaseEvent(QKeyEvent * e)
//{
//    qDebug() << "AppSwitcher::keyReleaseEvent" << e << e->key() << e->modifiers();;
//    QWidget::keyReleaseEvent(e);
//}

bool RazorAppSwitcher::AppSwitcher::eventFilter(QObject * o, QEvent * e)
{
    //qDebug() << "AppSwitcher::eventFilter" << e;
    if (e->type() == QEvent::WindowDeactivate)
    {
        close();
        return true;
    }
    return QWidget::eventFilter(o, e);
}

void RazorAppSwitcher::AppSwitcher::selectNextItem()
{
    if (m_list.count() == 0)
    {
        qDebug() << "AppSwitcher::selectItem m_list is empty. No action";
        return;
    }

    SwitcherItem * item = qobject_cast<SwitcherItem*>(QApplication::focusWidget());
    if (! item)
    {
        qDebug() << "AppSwitcher::selectItem implicit item (0) ";
        item = m_list.at(0);
    }
    else
    {
        qDebug() << "AppSwitcher::selectItem selected" << item << item->text();
        int index = m_list.indexOf(item);
        if (index == -1)
        {
            qDebug() << "AppSwitcher::selectItem item not found in list. And it's wrong!" << item << item->text();
            index = 0;
        }
        ++index;
        if (index > m_list.count() - 1)
            index = 0;
        item = m_list.at(index);
    }

    //infoLabel->setText(item->text());
    item->setFocus(Qt::OtherFocusReason);
}


RazorAppSwitcher::SwitcherItem::SwitcherItem(Window window, const QString & text, const QPixmap & pixmap, QWidget * parent)
    : QToolButton(parent),
      m_window(window)
{
    setObjectName("SwitcherItem '" + text + "'");
    setFocusPolicy(Qt::WheelFocus);
    setFocusProxy(0);
    setMaximumSize(64, 64);
    setMinimumSize(64, 64);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize(QSize(60, 60));
    setToolTip(text);

    setIcon(pixmap);
    setText(text);
}

void RazorAppSwitcher::SwitcherItem::focusInEvent(QFocusEvent * e)
{
    qDebug() << "SwitcherItem::focusInEvent" << e;
    emit infoChanged(text());
    QToolButton::focusInEvent(e);
}
