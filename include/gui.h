/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#ifndef SOEM_GUI_GUI_H_
#define SOEM_GUI_GUI_H_

#include <QMainWindow>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include "ui.h"

class GUI : public QWidget, public UI
{
    Q_OBJECT

    public:
        GUI(std::shared_ptr<ZMQPublisher> zmq_pub);
        void selectSource(const std::string &src);
        void selectNetworkInterface(const std::string &iface);
        void setConfigFile(const std::string &path);
        void setPCAPFile(const std::string &path);
        void enableZMQ(bool enable);
        void start();
        void dataCallback(const std::vector<std::shared_ptr<EthercatSlave>> &slaves);
    protected:
        void closeEvent(QCloseEvent *event) override;

    private slots:
        void handleStart();
        void handleDiscoverButton();
        void handleSelectConfigFile();
        void handleSelectPCAPFile();
        void handleInputButtonChanged();
        void handleZMQCheckBox(int state);
        void handleWheelListChanged(QListWidgetItem *item);
        void fixSize();
        void handleDataChanged(const std::vector<std::shared_ptr<EthercatSlave>> &slaves);

    signals:
        void dataChanged(const std::vector<std::shared_ptr<EthercatSlave>> &slaves);


    private:
        QGridLayout *main_layout;
        QHBoxLayout *wheel_data_layout;

        QButtonGroup *input_data_button_group;
        QRadioButton *input_data_ethercat_button;
        QRadioButton *input_data_sniff_button;
        QRadioButton *input_data_file_button;

        QComboBox *if_combo_box;
        QPushButton *select_config_file_button;
        QLabel *config_file_name_lbl;
        QPushButton *select_pcap_file_button;
        QLabel *pcap_file_name_lbl;

        QPushButton *discover_button;

        QPushButton *start_button;

        QCheckBox *publish_zmq_checkbox;
        QCheckBox *show_units_checkbox;

        std::vector<QGroupBox *> wheel_group_boxes;

        QListWidget *wheel_list_widget;

        void populateNetworkInterfaces();
};

Q_DECLARE_METATYPE(std::vector<std::shared_ptr<EthercatSlave>>)
#endif
