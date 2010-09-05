////////////////////////////////////////
//  File      : settings.h            //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// Show system settings

////////////////////////////////////////

#ifndef SETTINGS_H
#define SETTINGS_H

#include "defs.h"

////////////////////////////////////////

class Settings : public QDialog
{
    Q_OBJECT

public:
    Settings(QWidget *parent=0);
    ~Settings();
    void read_settings();
    void init();
    void gui_tab();
    void display_tab();
    void system_tab();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:

public slots:
    void ok_pressed();
    void close_pressed();
    void update_style_combo(const QString &);
    void update_size_combo(const QString &);
    void update_sample_text(const QString &);

private:
    QString ok_button_pix_path;
    QString close_button_pix_path;
    QString system_pix_path;
    QString utility_pix_path;
    QString graphics_pix_path;
    QString stl_path;
    QString stl_name;
    QPoint mousepos;
    QTabWidget *tab;
    QSpinBox *hor_res_spin;
    QSpinBox *ver_res_spin;
    QSpinBox *cursor_flash_spin;
    QSpinBox *double_click_spin;
    QSpinBox *keyboard_input_spin;
    QSpinBox *wheel_scroll_spin;
    QComboBox *style_combo;
    QComboBox *font_family_combo;
    QComboBox *font_style_combo;
    QComboBox *font_size_combo;
    QLineEdit *sample_text;
    int scr_num;
};

#endif

