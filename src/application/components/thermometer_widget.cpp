#include "thermometer_widget.h"

ThermometerWidget::ThermometerWidget(QWidget *parent, mqtt_client *client, QString name, QString topic) : QWidget(parent) {
    setupUi(this);
    this->name = name;
    this->topic = topic;

    this->name_text->setText(name);
    this->topic_text->setText(topic);

    connect(client, SIGNAL(getMessage(QByteArray, QString)), this, SLOT(addMessage(QByteArray, QString)));
}

void ThermometerWidget::addMessage(QByteArray msg, QString topicName) {
    QDateTime dateTime = dateTime.currentDateTime();
    QString currentTime = dateTime.toString("[HH:mm:ss] ");
    if (topicName == this->topic) {
        this->status_text->setText(currentTime + msg);
    }
}

void ThermometerWidget::on_delete_btn_clicked() {
    delete this;
}