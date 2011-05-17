/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#ifndef RAZORPANELAPPLICATION_H
#define RAZORPANELAPPLICATION_H

#include <QApplication>

class RazorPanel;

class RazorPanelApplication : public QApplication
{
    Q_OBJECT
public:
    explicit RazorPanelApplication(int& argc, char** argv);
    virtual bool x11EventFilter(XEvent* event);

    RazorPanel* panel() const { return mPanel; }
    void setPanel(RazorPanel* panel);

private:
    RazorPanel* mPanel;
};


#endif // RAZORPANELAPPLICATION_H
