#ifndef DESKTOPSWITCH_H
#define DESKTOPSWITCH_H

#include "../panel/razorpanelplugin.h"

class QButtonGroup;

/**
 * @brief Desktop switcher. A very simple one...
 */
class DesktopSwitch : public RazorPanelPlugin
{
    Q_OBJECT
public:
    DesktopSwitch(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~DesktopSwitch();

    virtual Alignment preferredAlignment() const { return AlignRight; }


public slots:
    void handleEvent(XEvent* event);
    
private:
    QButtonGroup * m_buttons;

    void wheelEvent(QWheelEvent* e);

private slots:
    void setDesktop(int desktop);

};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
