/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef RAZORCPULOAD_H
#define RAZORCPULOAD_H
#include <QLabel>

class IRazorPanelPlugin;

class RazorCpuLoad: public QFrame
{
	Q_OBJECT
public:
    /**
      Describes orientation of cpu load bar
     **/
    enum BarOrientation {
        BottomUpBar,    //! Bar begins at bottom and grows up
        TopDownBar,     //! Bar begins at top and grows down
        RightToLeftBar, //! Bar begins at right edge and grows to the left
        LeftToRightBar  //! Bar begins at left edge and grows to the right
    };

    RazorCpuLoad(IRazorPanelPlugin *plugin, QWidget* parent = 0);
	~RazorCpuLoad();


    void settingsChanged();

protected:
	void virtual timerEvent(QTimerEvent *event);
	void virtual paintEvent ( QPaintEvent * event );
	void virtual resizeEvent(QResizeEvent *);

private:
	double getLoadCpu() const;

    IRazorPanelPlugin *mPlugin;
	QWidget m_stuff;

	//! average load
	int m_avg;

	bool m_showText;
    BarOrientation m_barOrientation;
    int m_updateInterval;
    int m_timerID;

	QFont m_font;
};


#endif // RAZORCPULOAD_H


