/**
 * @file light_widget.cpp
 * @authors Peter Zdravecký, Eduard Frlička
 * @version 0.1
 * @date 2021-04-28
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "light_widget.h"

LightWidget::LightWidget(QWidget *parent, mqtt_client *client, QString name, QString topic, QString widgetID) : QWidget(parent) {
    setupUi(this);

    // asign variables
    this->name = name;
    this->topic = topic;
    this->widgetID = widgetID;
    this->client = client;
    this->name_text->setText(name);
    this->topic_text->setText(topic);

    // set default widget icon
    QPixmap p(":/lightbulb.png");
    int w = this->icon_label->width();
    int h = this->icon_label->height();
    this->icon_label->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));

    // connect signal, when new message arrive, function addMessage is called
    connect(client, SIGNAL(getMessage(QByteArray, QString)), this, SLOT(addMessage(QByteArray, QString)));
    // connect signal for message publishing
    connect(this, SIGNAL(sendMessage(QByteArray, QString, int)), parent, SLOT(addMessage(QByteArray, QString, int)));
    // connect signal, delete widget when topic is unsubscribed from explorer
    connect(parent, SIGNAL(unsubscribe_signal(QString)), this, SLOT(handle_unsubscribe(QString)));
    // connect signal, unsubscribe topic from explorer when widget is deleted
    connect(this, SIGNAL(widget_deleted_signal(QString)), parent, SLOT(unsubscribe_topic(QString)));
}

void LightWidget::addMessage(QByteArray msg, QString topicName) {
    QDateTime dateTime = dateTime.currentDateTime();
    QString currentTime = dateTime.toString("[HH:mm:ss] ");
    // check if message topic is same as widget topic
    if (topicName == this->topic) {
        this->status_text->setText(currentTime + msg);
    }
}

void LightWidget::on_switch_btn_clicked() {
    // toggle state
    if (this->state == "off") {
        this->state = "on";
    } else {
        this->state = "off";
    }
    client->sendMessage(this->state, this->topic);
    emit this->sendMessage(this->state, this->topic, 1);
}

void LightWidget::handle_unsubscribe(QString topicName) {
    if (topicName == this->topic) {
        this->deleteWidget();
    }
}

void LightWidget::on_delete_btn_clicked() {
    emit this->widget_deleted_signal(this->topic);
    deleteWidget();
}

void LightWidget::deleteWidget() {
    QSettings settings;
    settings.beginGroup("widget" + this->widgetID);
    settings.remove("");
    delete this;
}