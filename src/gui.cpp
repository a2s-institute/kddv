/**
 * Copyright (c) 2021
 * Hochschule Bonn-Rhein-Sieg
 *
 * License: GPLv3
 */

#include <QtWidgets>
#include <QFileInfo>
#include "gui.h"
#include "ethercat_master.h"
#include "packet_sniffer.h"
#include <iostream>

GUI::GUI(std::shared_ptr<ZMQPublisher> zmq_pub) : UI(zmq_pub)
{
    QGridLayout *top_bar_layout = new QGridLayout;
    input_data_button_group = new QButtonGroup;
    input_data_ethercat_button = new QRadioButton("EtherCAT");
    input_data_sniff_button = new QRadioButton("Sniff Network Interface");
    input_data_file_button = new QRadioButton("PCAP File");
    input_data_button_group->addButton(input_data_ethercat_button);
    input_data_button_group->addButton(input_data_sniff_button);
    input_data_button_group->addButton(input_data_file_button);
    connect(input_data_button_group, SIGNAL(buttonClicked(int)), this, SLOT(handleInputButtonChanged()));
    QVBoxLayout *button_group_layout = new QVBoxLayout;
    button_group_layout->addWidget(input_data_ethercat_button);
    button_group_layout->addWidget(input_data_sniff_button);
    button_group_layout->addWidget(input_data_file_button);

    QVBoxLayout *source_config_layout = new QVBoxLayout;
    if_combo_box = new QComboBox;
    populateNetworkInterfaces();
    int width = if_combo_box->minimumSizeHint().width();
    if_combo_box->setMinimumWidth(width);
    if_combo_box->setEnabled(false);


    select_config_file_button = new QPushButton("Select Config File", this);
    connect(select_config_file_button, SIGNAL(clicked()), this, SLOT(handleSelectConfigFile()));
    config_file_name_lbl = new QLabel;
    QHBoxLayout *config_file_layout = new QHBoxLayout;
    config_file_layout->addWidget(select_config_file_button);
    config_file_layout->addWidget(config_file_name_lbl);
    select_config_file_button->setEnabled(false);

    select_pcap_file_button = new QPushButton("Select PCAP File", this);
    connect(select_pcap_file_button, SIGNAL(clicked()), this, SLOT(handleSelectPCAPFile()));
    pcap_file_name_lbl = new QLabel;
    QHBoxLayout *pcap_file_layout = new QHBoxLayout;
    pcap_file_layout->addWidget(select_pcap_file_button);
    pcap_file_layout->addWidget(pcap_file_name_lbl);
    select_pcap_file_button->setEnabled(false);

    source_config_layout->addWidget(if_combo_box);
    source_config_layout->addLayout(config_file_layout);
    source_config_layout->addLayout(pcap_file_layout);

    start_button = new QPushButton("Start", this);
    start_button->setEnabled(false);
    discover_button = new QPushButton("Discover Slaves", this);
    connect(discover_button, SIGNAL(clicked()), this, SLOT(handleDiscoverButton()));
    connect(start_button, SIGNAL(clicked()), this, SLOT(handleStart()));


    publish_zmq_checkbox = new QCheckBox("Publish via ZMQ");
    connect(publish_zmq_checkbox, SIGNAL(stateChanged(int)), this, SLOT(handleZMQCheckBox(int)));
    show_units_checkbox = new QCheckBox("Show Units");
    QVBoxLayout *checkbox_layout = new QVBoxLayout;
    checkbox_layout->addWidget(publish_zmq_checkbox);
    checkbox_layout->addWidget(show_units_checkbox);

    top_bar_layout->addLayout(button_group_layout, 0, 0);
    top_bar_layout->addLayout(source_config_layout, 0, 1);
    top_bar_layout->addWidget(discover_button, 0, 2);
    top_bar_layout->addWidget(start_button, 0, 3);
    top_bar_layout->addLayout(checkbox_layout, 0, 4);

    wheel_data_layout = new QHBoxLayout;
    wheel_list_widget = new QListWidget;
    wheel_list_widget->setMaximumWidth(100);
    connect(wheel_list_widget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(handleWheelListChanged(QListWidgetItem*)));

    qRegisterMetaType<std::vector<std::shared_ptr<EthercatSlave>>>();
    connect(this, &GUI::dataChanged, this, &GUI::handleDataChanged, Qt::QueuedConnection);

    main_layout = new QGridLayout;
    main_layout->addLayout(top_bar_layout, 0, 0, 1, 2);
    main_layout->addWidget(wheel_list_widget, 1, 0);
    main_layout->addLayout(wheel_data_layout, 1, 1);
    setLayout(main_layout);
}

void GUI::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void GUI::handleStart()
{
    if (start_button->text().toStdString() == "Start")
    {
        std::string error_msg;
        ecat_data_source->start(error_msg);
        if (!error_msg.empty())
        {
            QMessageBox::critical(this, tr("Error"), QString::fromStdString(error_msg));
        }
        else
        {
            discover_button->setEnabled(false);
            start_button->setText("Stop");
        }
    }
    else if (start_button->text().toStdString() == "Stop")
    {
        ecat_data_source->stop();
        discover_button->setEnabled(true);
        start_button->setText("Start");
    }
}

void GUI::handleDiscoverButton()
{
    start_button->setEnabled(false);
    wheel_group_boxes.clear();
    QLayoutItem *item;
    while((item = wheel_data_layout->takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }

    wheel_list_widget->clear();

    std::string interface = if_combo_box->currentText().toStdString();

    ecat_data_source.reset();
    if (input_data_ethercat_button->isChecked())
    {
        if (interface.empty() || interface == "Network Interface")
        {
            QMessageBox::critical(this, tr("Error"), tr("Network Interface not selected"));
            return;
        }
        ecat_data_source = std::make_shared<EthercatMaster>(interface, zmq_pub);
    }
    else if (input_data_sniff_button->isChecked())
    {
        if (interface.empty() || interface == "Network Interface")
        {
            QMessageBox::critical(this, tr("Error"), tr("Network Interface not selected"));
            ecat_data_source.reset();
            return;
        }
        std::string error_msg;
        ecat_data_source = std::make_shared<PacketSniffer>(interface, false, zmq_pub, error_msg);
        if (!error_msg.empty())
        {
            QMessageBox::critical(this, tr("Error"), QString::fromStdString(error_msg));
            ecat_data_source.reset();
            return;
        }
        if (!config_file_name.empty())
        {
            std::static_pointer_cast<PacketSniffer>(ecat_data_source)->setConfigFile(config_file_name, error_msg);
            if (!error_msg.empty())
            {
                QMessageBox::critical(this, tr("Error"), QString::fromStdString(error_msg));
                ecat_data_source.reset();
                return;
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Config file not specified"));
            ecat_data_source.reset();
            return;
        }
    }
    else if (input_data_file_button->isChecked())
    {
        if (config_file_name.empty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Config file not specified"));
            return;
        }

        if (pcap_file_name.empty())
        {
            QMessageBox::critical(this, tr("Error"), tr("PCAP file not specified"));
            return;
        }
        std::string error_msg;
        ecat_data_source = std::make_shared<PacketSniffer>(pcap_file_name, true, zmq_pub, error_msg);
        if (!error_msg.empty())
        {
            QMessageBox::critical(this, tr("Error"), QString::fromStdString(error_msg));
            ecat_data_source.reset();
            return;
        }
        std::static_pointer_cast<PacketSniffer>(ecat_data_source)->setConfigFile(config_file_name, error_msg);
        if (!error_msg.empty())
        {
            QMessageBox::critical(this, tr("Error"), QString::fromStdString(error_msg));
            ecat_data_source.reset();
            return;
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Select a data source first"));
        return;
    }
    if (publish_zmq_checkbox->isChecked())
    {
        ecat_data_source->setZMQPublish(true);
    }
    else
    {
        ecat_data_source->setZMQPublish(false);
    }
    ecat_data_source->setDataCallback(&UI::dataCallback, this);

    std::string error_msg;
    std::vector<std::shared_ptr<EthercatSlave>> slaves = ecat_data_source->getSlaves(error_msg);
    if (!error_msg.empty())
    {
        QMessageBox::critical(this, tr("Error"), QString::fromStdString(error_msg));
        ecat_data_source.reset();
        return;
    }
    else
    {
        for (int i = 0; i < slaves.size(); i++)
        {
            std::string wheel_id = slaves[i]->slave_info.name + " " + std::to_string(slaves[i]->slave_info.slave_number);
            QListWidgetItem *list_item = new QListWidgetItem(QString::fromStdString(wheel_id));
            list_item->setCheckState(Qt::Checked);
            wheel_list_widget->addItem(list_item);

            QGroupBox *box = new QGroupBox(QString::fromStdString(wheel_id));
            box->setObjectName(QString::fromStdString(wheel_id));
            QGridLayout *grid_layout = new QGridLayout;

            for (int j = 0; j < slaves[i]->getRxVariables().size(); j++)
            {
                QLabel *lbl = new QLabel(QString::fromStdString(slaves[i]->getRxVariables()[j]));
                grid_layout->addWidget(lbl, j, 0);
                QLabel *line = new QLabel;
                line->setObjectName(QString::fromStdString("RX" + slaves[i]->getRxVariables()[j]));
                line->setAlignment(Qt::AlignRight);
                grid_layout->addWidget(line, j, 1);
            }
            for (int j = 0; j < slaves[i]->getTxVariables().size(); j++)
            {
                QLabel *lbl = new QLabel(QString::fromStdString(slaves[i]->getTxVariables()[j]));
                grid_layout->addWidget(lbl, j, 2);
                QLabel *line = new QLabel;
                line->setObjectName(QString::fromStdString("TX" + slaves[i]->getTxVariables()[j]));
                line->setAlignment(Qt::AlignRight);
                grid_layout->addWidget(line, j, 3);
            }
            int last_row = std::max(slaves[i]->getRxVariables().size(), slaves[i]->getTxVariables().size());
            QSpacerItem * spacer = new QSpacerItem(1,1,QSizePolicy::Expanding, QSizePolicy::Expanding);
            grid_layout->addItem(spacer, last_row, 0, 1, 4);
            box->setLayout(grid_layout);
            box->setVisible(true);
            box->setMaximumWidth(600);
            wheel_data_layout->addWidget(box);
            wheel_group_boxes.push_back(box);
        }
        wheel_list_widget->setMinimumWidth(wheel_list_widget->sizeHintForColumn(0));
    }
    start_button->setEnabled(true);
}

void GUI::handleSelectConfigFile()
{
    QString qfilename = QFileDialog::getOpenFileName(this, tr("Open EtherCAT config file"), "../config", "JSON Files (*.json)");
    QFileInfo fileinfo(qfilename);

    config_file_name_lbl->setText(fileinfo.fileName());
    config_file_name = qfilename.toStdString();
}

void GUI::handleSelectPCAPFile()
{
    QString qfilename = QFileDialog::getOpenFileName(this, tr("Open PCAP file"), "../data", "PCAP Files (*.pcapng *.pcap)");
    pcap_file_name = qfilename.toStdString();
    if (pcap_file_name.empty())
    {
        return;
    }
    QFileInfo fileinfo(qfilename);
    pcap_file_name_lbl->setText(fileinfo.fileName());
}

void GUI::handleInputButtonChanged()
{

    if (input_data_ethercat_button->isChecked())
    {
        select_config_file_button->setEnabled(false);
        select_pcap_file_button->setEnabled(false);
        if_combo_box->setEnabled(true);
    }
    else if (input_data_sniff_button->isChecked())
    {
        select_config_file_button->setEnabled(true);
        select_pcap_file_button->setEnabled(false);
        if_combo_box->setEnabled(true);
    }
    else if (input_data_file_button->isChecked())
    {
        select_config_file_button->setEnabled(true);
        select_pcap_file_button->setEnabled(true);
        if_combo_box->setEnabled(false);
    }
    else
    {
        select_config_file_button->setEnabled(false);
        select_pcap_file_button->setEnabled(false);
        if_combo_box->setEnabled(false);
    }
}

void GUI::handleZMQCheckBox(int state)
{
    if (ecat_data_source != nullptr)
    {
        if (publish_zmq_checkbox->isChecked())
        {
            ecat_data_source->setZMQPublish(true);
        }
        else
        {
            ecat_data_source->setZMQPublish(false);
        }
    }
}

void GUI::fixSize()
{
    resize(sizeHint());
}

void GUI::handleWheelListChanged(QListWidgetItem *item)
{
    int box_idx = -1;
    for (int idx = 0; idx < wheel_group_boxes.size(); idx++)
    {
        if (wheel_group_boxes[idx]->objectName().compare(item->text()) == 0)
        {
            box_idx = idx;
            break;
        }
    }
    if (box_idx != -1)
    {
        if (item->checkState() == Qt::Unchecked)
        {
            wheel_group_boxes[box_idx]->setVisible(false);
        }
        else if (item->checkState() == Qt::Checked)
        {
            wheel_group_boxes[box_idx]->setVisible(true);
            wheel_group_boxes[box_idx]->setMaximumWidth(600);
        }
        // See https://stackoverflow.com/questions/13942616/qt-resize-window-after-widget-remove
        QTimer::singleShot(0, this, SLOT(fixSize()));
    }
}

void GUI::handleDataChanged(const std::vector<std::shared_ptr<EthercatSlave>> &slaves)
{
    for (int i = 0; i < slaves.size(); i++)
    {
        const std::vector<std::string> &rx_vars = slaves[i]->getRxVariables();
        const std::vector<std::string> &tx_vars = slaves[i]->getTxVariables();
        const std::vector<std::string> &rx_units = slaves[i]->getRxUnits();
        const std::vector<std::string> &tx_units = slaves[i]->getTxUnits();
        std::vector<std::string> rx_vals = slaves[i]->getRxValues();
        std::vector<std::string> tx_vals = slaves[i]->getTxValues();
        int rx_id = 0;
        int tx_id = 0;

        for (int child_idx = 0; child_idx < wheel_group_boxes[i]->children().count(); child_idx++)
        {
            std::string var_name = wheel_group_boxes[i]->children()[child_idx]->objectName().toStdString();
            if (var_name.empty()) continue;
            int var_type = 0;
            if (var_name.rfind("RX", 0) == 0)
            {
                var_type = 1;
                var_name = var_name.substr(2);
                std::stringstream ss;
                ss << rx_vals[rx_id];
                if (show_units_checkbox->isChecked())
                {
                    ss << std::setw(10) << rx_units[rx_id];
                }
                qobject_cast<QLabel *>(wheel_group_boxes[i]->children()[child_idx])->setText(QString::fromStdString(ss.str()));
            }
            else if (var_name.rfind("TX", 0) == 0)
            {
                var_type = 2;
                var_name = var_name.substr(2);
                std::stringstream ss;
                ss << tx_vals[tx_id];
                if (show_units_checkbox->isChecked())
                {
                    ss << std::setw(10) << tx_units[tx_id];
                }
                qobject_cast<QLabel *>(wheel_group_boxes[i]->children()[child_idx])->setText(QString::fromStdString(ss.str()));
            }
            // set tooltip for variables whose bits can be parsed
            if (slaves[i]->areBitsParsable(var_name))
            {
                std::vector<std::string> vars, vals;
                uint16_t val;
                if (var_type == 1)
                {
                    val = static_cast<uint16_t>(std::stoul(rx_vals[rx_id]));
                }
                else if (var_type == 2)
                {
                    val = static_cast<uint16_t>(std::stoul(tx_vals[tx_id]));
                }
                slaves[i]->parseBits(val, var_name, vars, vals);
                std::string tooltip = "";
                for (int cvar_idx = 0; cvar_idx < vars.size(); cvar_idx++)
                {
                    tooltip += vars[cvar_idx] + ":\t" + vals[cvar_idx];
                    if (cvar_idx != vars.size() - 1) tooltip += "\n";
                }
                qobject_cast<QLabel *>(wheel_group_boxes[i]->children()[child_idx])->setToolTip(QString::fromStdString(tooltip));
            }
            if (var_type == 1)
            {
                rx_id++;
            }
            else if (var_type == 2)
            {
                tx_id++;
            }
        }
    }
}

void GUI::dataCallback(const std::vector<std::shared_ptr<EthercatSlave>> &slaves)
{
    emit dataChanged(slaves);
}

void GUI::populateNetworkInterfaces()
{
    if_combo_box->addItem(tr("Network Interface"));
    std::vector<std::string> interfaces = getNetworkInterfaces();
    for (int i = 0; i < interfaces.size(); i++)
    {
        if_combo_box->addItem(QString::fromStdString(interfaces[i]));
    }
}

// functions to select options programmatically instead of via interaction
void GUI::selectSource(const std::string &src)
{
    if (src == "ecat")
    {
        input_data_ethercat_button->setChecked(true);
    }
    else if (src == "sniffer")
    {
        input_data_sniff_button->setChecked(true);
    }
    else if (src == "pcap")
    {
        input_data_file_button->setChecked(true);
    }
    else
    {
        return;
    }
    handleInputButtonChanged();
}

void GUI::selectNetworkInterface(const std::string &iface)
{
    for (int i = 0; i < if_combo_box->count(); i++)
    {
        if (if_combo_box->itemText(i).toStdString() == iface)
        {
            if_combo_box->setCurrentIndex(i);
        }
    }
}

void GUI::setConfigFile(const std::string &path)
{
    QFileInfo fileinfo(QString::fromStdString(path));
    config_file_name_lbl->setText(fileinfo.fileName());
    config_file_name = path;
}

void GUI::setPCAPFile(const std::string &path)
{
    QFileInfo fileinfo(QString::fromStdString(path));
    pcap_file_name_lbl->setText(fileinfo.fileName());
    pcap_file_name = path;
}

void GUI::enableZMQ(bool enable)
{
    publish_zmq_checkbox->setChecked(enable);
}

void GUI::start()
{
    discover_button->clicked();
    // TODO: might run into timing issues here
    if (start_button->isEnabled())
    {
        start_button->clicked();
    }
}
