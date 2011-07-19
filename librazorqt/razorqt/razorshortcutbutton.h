/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef RAZORSHORTCUTBUTTON_H
#define RAZORSHORTCUTBUTTON_H

#include <QtGui/QToolButton>
#include <QtGui/QKeySequence>

class RazorShortcutButtonPrivate;


/**
 * @short A widget to input a QKeySequence.
 *
 * This widget lets the user choose a QKeySequence, which is usually used as a
 * shortcut key. The recording is initiated by the user clicking into the widget.
 *
 */
class RazorShortcutButton : public QToolButton
{
    Q_OBJECT
public:
    /// Constructor.
    explicit RazorShortcutButton(QWidget *parent = 0);

    /// Destructs the widget.
    virtual ~RazorShortcutButton();

    /// Return the currently selected key sequence.
    QKeySequence keySequence() const;

public slots:
    /// Set the key sequence.
    void setKeySequence(const QKeySequence &sequence);

    /// This function is provided for convenience. It's equivalent to calling setKeySequence(QKeySequence(sequence)).
    void setKeySequence(const QString &sequence);

signals:
    /**
     * This signal is emitted when the current key sequence has changed, be it by user
     * input or programmatically.
     */
    void keySequenceChanged(const QKeySequence &sequence);

    /**
     * This signal is emitted when the current key sequence has changed, be it by user
     * input or programmatically.
     */
    void keySequenceChanged(const QString &sequence);

protected:
    /// @reimp
    bool event(QEvent *event);

private:
    RazorShortcutButtonPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorShortcutButton)
};

#endif // RAZORSHORTCUTBUTTON_H
