/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

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

#ifndef RAZORPLUGINMANAGER_H
#define RAZORPLUGINMANAGER_H

#include <QList>

class RazorPanelPlugin;
class RazorPanel;


/*! \brief The RazorPluginManager class provides a list of RazorPanelPlugins.
 */

class RazorPluginManager : public QList<RazorPanelPlugin*>
{
public:
    /*! \brief Constructs an empty list.
    */
    explicit RazorPluginManager();

    /*! \brief Destroys the list. All plugins in the list become destroyed.
     */
    virtual ~RazorPluginManager();


    /*! \brief Constructs RazorPanelPlugin object from the library with the given libraryFileName and adds it in to plugins list.
        Returns the created plugin object if loading was success; otherwise retunrn NULL.
     */
    RazorPanelPlugin* loadPlugin(const QString& libraryFileName, const QString& configId, RazorPanel* panel);
};

typedef QListIterator<RazorPanelPlugin*> RazorPanelPluginIterator;
typedef QMutableListIterator<RazorPanelPlugin*> RazorPanelPluginMutableIterator;
#endif // RAZORPLUGINMANAGER_H
